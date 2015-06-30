/**
 * @file distribute.c
 * @author timoteus <mail@timoteus.se>
 */

#include "distribute.h"
#include "sort.h"

rock_mesh_t *
rock_mesh_init(MPI_Comm comm,
               rock_uint_t order,
               rock_uint_t *dim_size)
{
    rock_mesh_t *mesh = calloc(1, sizeof(rock_mesh_t));

    MPI_Comm_size(comm, &mesh->np);
    MPI_Comm_rank(comm, &mesh->rank);

    mesh->comm = comm;
    mesh->order = order;
    for (rock_uint_t i = 0; i < order; i++) {
        mesh->dim_size[i] = dim_size[i];
    }

    return mesh;
}

rock_mpart_t *
rock_mpart_init(rock_uint_t order, rock_uint_t *dim_num_parts)
{
    rock_mpart_t *mpart = calloc(1, sizeof(rock_mpart_t));

    for (rock_uint_t i = 0; i < order; i++) {
        mpart->dim_num[i] = ROCK_UINT_MAX;
        mpart->dim_part[i] = rock_part_init(dim_num_parts[i]);
    }

    mpart->order = order;

    return mpart;
}

rock_dist_t *
rock_dist_init(rock_indx_t *indx,
               rock_elem_t *elem,
               rock_mpart_t *mpart,
               rock_mesh_t *mesh)
{
    rock_dist_t *dist = calloc(1, sizeof(rock_dist_t));
    dist->count = calloc(mesh->np, sizeof(rock_uint_t));
    dist->offset = calloc(mesh->np + 1, sizeof(rock_uint_t));

    /*
     * Find out what process each index belongs to using the supplied
     * multi-partition object. Calculate counts and offsets for each 
     * processor.
     */
    if (mesh->rank == ROCK_MASTER) {

        /* Create new index array to hold processor indices. */
        rock_indx_t *proc_indx = rock_indx_init(indx->len);

        /* Populate proc array and counts. */
        for (rock_uint_t i = 0; i < indx->len; i++) {
            rock_uint_t part_num = rock_part_num_from_mpart(mpart, indx, i);
            rock_indx_set(proc_indx, i, part_num);
            dist->count[part_num]++;
        }

        /* Sort proc indices and permute indx and elem. */
        rock_uint_t order = 1;
        rock_uint_t dim_size[] = {mesh->np};
        rock_desc_t *desc = rock_desc_init(order, dim_size);
        rock_perm_t *perm = rock_perm_init(indx->len);
        rock_uint_t num_dims = 1;
        rock_uint_t dims[] = {0};

        rock_indx_sort(desc, num_dims, dims, perm, proc_indx);

        rock_indx_permute(indx, perm);
        rock_elem_permute(elem, perm);

        rock_indx_free(proc_indx);
        rock_desc_free(desc);
    }

    MPI_Bcast(dist->count, mesh->np, ROCK_UINT_MPI, ROCK_MASTER, mesh->comm);

    /* Calculate offsets from counts. */
    for (rock_uint_t i = 1; i <= mesh->np; i++) {
        dist->offset[i] = dist->offset[i-1] + dist->count[i-1];
    }

    /* Calculate sum from offset. */
    dist->sum = dist->offset[mesh->np];

    dist->mesh = mesh;

    return dist;
}

void
rock_mesh_free(rock_mesh_t *mesh)
{
    free(mesh);
}

void
rock_mpart_free(rock_mpart_t *mpart)
{
    for (rock_uint_t i = 0; i < mpart->order; i++) {
        rock_part_free(mpart->dim_part[i]);
    }

    free(mpart);
}

void
rock_dist_free(rock_dist_t *dist)
{
    free(dist->count);
    free(dist->offset);
    free(dist);
}

void
rock_mesh_print(rock_mesh_t *mesh)
{

    printf("mesh\n");
    printf("    np          %" PRIu32 "\n", mesh->np);
    printf("    rank        %" PRIu32 "\n", mesh->rank);
    printf("    order       %" PRIu32 "\n", mesh->order);
    printf("    dim_size    ");
    for (rock_uint_t i = 0; i < mesh->order; i++) {
        printf("%" PRIu32 " ", mesh->dim_size[i]);
    }
    printf("\n");
    printf("    comm        ");
    if (mesh->comm)  {
        printf("obj\n");
    } else {
        printf("NULL\n");
    }
    printf("\n");
}

void
rock_mpart_print(rock_mpart_t *mpart)
{

    printf("mpart\n");
    printf("    desc        ");
    if (mpart->desc)  {
        printf("obj\n");
    } else {
        printf("NULL\n");
    }
    printf("    order       %" PRIu32 "\n", mpart->order);
    printf("    dim_num     ");
    for (rock_uint_t i = 0; i < mpart->order; i++) {
        printf("%" PRIu32 " ", mpart->dim_num[i]);
    }
    printf("\n");
    printf("    dim_part    ");
    for (rock_uint_t i = 0; i < mpart->order; i++) {
        if (mpart->dim_part[i]) {
            printf("obj ");
        }
    }
    printf("\n");
}

void
rock_indx_scatter(rock_indx_t **indx, rock_dist_t *dist)
{
    MPI_Barrier(dist->mesh->comm);

    /* Allocate memory from counts for everyone. */
    rock_indx_t *recv = rock_indx_init(dist->count[dist->mesh->rank]);

    rock_uint_t *v = NULL;
    if (dist->mesh->rank == ROCK_MASTER) {
        v = (*indx)->v;
    }

    /* Scatter the index array. */
    MPI_Scatterv(v, dist->count, dist->offset, ROCK_UINT_MPI,
            recv->v, recv->len, ROCK_UINT_MPI,
            ROCK_MASTER, dist->mesh->comm);

    /* Free source buffer at master. */
    if (dist->mesh->rank == ROCK_MASTER) {
        free(*indx);
    }

    *indx = recv;
}

void
rock_indx_gather(rock_indx_t **indx, rock_dist_t *dist)
{
    MPI_Barrier(dist->mesh->comm);

    rock_indx_t *recv = NULL;
    rock_uint_t *v = NULL;

    /* Allocate memory for recv buffer at root. */
    if (dist->mesh->rank == ROCK_MASTER) {
        recv = rock_indx_init(dist->sum);
        v = recv->v;
    }

    MPI_Gatherv((*indx)->v, dist->count[dist->mesh->rank], ROCK_UINT_MPI,
            v, dist->count, dist->offset, ROCK_UINT_MPI,
            ROCK_MASTER, dist->mesh->comm);

    /* Free old buffers at all processes and point root to new buffer. */
    free(*indx);
    if (dist->mesh->rank == ROCK_MASTER) {
        *indx = recv;
    }
}

void
rock_elem_scatter(rock_elem_t **elem, rock_dist_t *dist)
{
    MPI_Barrier(dist->mesh->comm);

    /* Allocate memory from counts for everyone. */
    rock_elem_t *recv = rock_elem_init(dist->count[dist->mesh->rank]);

#ifdef ROCK_ELEM_FLOAT
    float *v = NULL;
#else
    double *v = NULL;
#endif

    if (dist->mesh->rank == ROCK_MASTER) {
        v = (*elem)->v;
    }

    /* Scatter the index array. */
    MPI_Scatterv(v, dist->count, dist->offset, ROCK_ELEM_MPI,
            recv->v, recv->len, ROCK_ELEM_MPI,
            ROCK_MASTER, dist->mesh->comm);

    /*
     * Deallocate the source array and use smaller recv arrays
     * for all processes.
     */
    if (dist->mesh->rank == ROCK_MASTER) {
        free(*elem);
    }
    *elem = recv;
}

void
rock_elem_gather(rock_elem_t **elem, rock_dist_t *dist)
{
    MPI_Barrier(dist->mesh->comm);

    /* Allocate memory for recv buffer at root. */
    rock_elem_t *recv = NULL;

#ifdef ROCK_ELEM_FLOAT
    float *v = NULL;
#else
    double *v = NULL;
#endif

    if (dist->mesh->rank == ROCK_MASTER) {
        recv = rock_elem_init(dist->sum);
        v = recv->v;
    }

    MPI_Gatherv((*elem)->v, dist->count[dist->mesh->rank], ROCK_ELEM_MPI,
            v, dist->count, dist->offset, ROCK_ELEM_MPI,
            ROCK_MASTER, dist->mesh->comm);

    /* Free old buffers at all processes and point root to new buffer. */
    free(*elem);
    if (dist->mesh->rank == ROCK_MASTER) {
        *elem = recv;
    }
}

rock_uint_t
rock_part_num_from_mpart(rock_mpart_t *mpart,
                         rock_indx_t *indx,
                         rock_uint_t i)
{
    rock_uint_t part_indx[mpart->order];

    for (rock_uint_t j = 0; j < mpart->order; j++) {
        part_indx[j] = 0;
        rock_uint_t dim_indx =
                rock_indx_extract(mpart->desc, indx, i, mpart->dim_num[j]);

        while (dim_indx >= mpart->dim_part[j]->offset[part_indx[j] + 1]) {
            part_indx[j]++;
        }
    }

    rock_uint_t part_num = 0;

    for (rock_uint_t j = 0; j < mpart->order; j++) {
        rock_uint_t prev_prod = 1;
        for (rock_uint_t k = 0; k < j; k++) {
            prev_prod *= mpart->dim_part[k]->num_parts;
        }
        part_num += prev_prod * part_indx[j];
    }

    return part_num;
}

rock_uint_t
rock_get_largest_remaining_dim(rock_desc_t *desc, rock_mpart_t *mpart)
{
    rock_uint_t dim_num = ROCK_UINT_MAX;

    for (rock_uint_t i = 0; i < desc->order; i++) {
        bool already_partitioned = false;

        /* Did we already partition this dimension? */
        if (mpart->dim_num[i] == ROCK_UINT_MAX) {
            already_partitioned = false;
        } else {
            for (rock_uint_t k = 0; k < mpart->order; k++) {
                if (i == mpart->dim_num[k]) {
                    already_partitioned = true;
                }
            }
        }

        if (!already_partitioned) {

            /* We want the largest remaining. */
            if (dim_num == ROCK_UINT_MAX) {
                dim_num = i;
            } else {
                if (desc->dim_size[dim_num] < desc->dim_size[i]) {
                    dim_num = i;
                }
            }
        }
    }

    return dim_num;
}

int
rock_mpart_desc_based(rock_desc_t *desc,
                     rock_mpart_t *mpart,
                     rock_uint_t num_prio_dims,
                     rock_uint_t *prio_dims)
{
    for (rock_uint_t i = 0; i < mpart->order; i++) {

        rock_uint_t dim_num;

        /*
         * Start partitioning specified dimensions, use largest remaining
         * if not enough are specified (i.e. if mpart->order > num_prio_dims).
         */
        if (i < num_prio_dims) {
            dim_num = prio_dims[i];
        } else {
            dim_num = rock_get_largest_remaining_dim(desc, mpart);
        }

        /* Create partition and store it in the mpartribution object. */
        rock_part_desc_based(desc, mpart->dim_part[i], dim_num);

        mpart->dim_num[i] = dim_num;
    }

    mpart->desc = desc;

    return ROCK_OK;
}
