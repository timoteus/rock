/**
 * @file test_distribute.c
 * @author timoteus <mail@timoteus.se>
 */

#include "core.h"
#include "distribute.h"
#include "sort.h"
#include "print.h"
#include "random.h"

/*
 * Unit test of rock_indx_scatter(), rock_elem_scatter(),
 * rock_indx_gather() and rock_elem_gather().
 */
int
test_scatter_gather()
{
    rock_indx_t *indx = NULL;
    rock_elem_t *elem = NULL;
    rock_indx_t *indx_sample = NULL;
    rock_elem_t *elem_sample = NULL;
    rock_desc_t *desc = NULL;
    rock_mpart_t *mpart = NULL;

    /* Mesh setup. */
    rock_uint_t proc_order = 3; /* 3D mesh. */
    rock_uint_t proc_dims[] = {2, 4, 2}; /* 16 processors. */
    rock_mesh_t *mesh = rock_mesh_init(MPI_COMM_WORLD, proc_order, proc_dims);

    if (mesh->rank == ROCK_MASTER) {

        /* Tensor setup */
        rock_uint_t order = 3;
        rock_uint_t nnz = 1e6; /* Number of non-zero elements. */
        rock_uint_t dim_size[] = {300, 2, 25000};
        desc = rock_desc_init(order, dim_size);

        /** Initialize multi-partition that matches processor mesh. */
        mpart = rock_mpart_init(mesh->order, mesh->dim_size);

        /* Create partition based on tensor space. */
        rock_uint_t num_part_dims = 1;
        rock_uint_t part_dims[] = {1}; /* Start with 1 then use largest. */
        rock_mpart_desc_based(desc, mpart, num_part_dims, part_dims);

        /* Allocate index and elem arrays at master. */
        elem_sample = rock_elem_init(nnz);
        indx_sample = rock_indx_init(nnz);

        /* Sample index and elements at master. */
        rock_elem_sample(desc, elem_sample);
        rock_indx_sample(desc, indx_sample);

        /* Copy data to working buffers (keep original for comparison). */
        elem = rock_elem_copy(elem_sample);
        indx = rock_indx_copy(indx_sample);
    }

    /* SCATTER */

    /* Use multi-partition to distribute tensor over processor mesh. */
    rock_dist_t *dist = rock_dist_init(indx, elem, mpart, mesh);

    /* Scatter indices. */
    rock_indx_scatter(&indx, dist);

    /* Scatter elements. */
    rock_elem_scatter(&elem, dist);

    /* GATHER */

    /* Gather indices. */
    rock_indx_gather(&indx, dist);

    /* Gather elements. */
    rock_elem_gather(&elem, dist);

    /* VERIFY */

    MPI_Barrier(mesh->comm);

    if (mesh->rank == ROCK_MASTER) {

        /* Sort data prior to comparison. */

        rock_uint_t num_dims = 3;
        rock_uint_t dims[] = {0, 1, 2};
        rock_perm_t *perm_sort = rock_perm_init(indx->len);

        rock_indx_sort(desc, num_dims, dims, perm_sort, indx_sample);
        rock_elem_permute(elem_sample, perm_sort);

        rock_indx_sort(desc, num_dims, dims, perm_sort, indx);
        rock_elem_permute(elem, perm_sort);

        assert(rock_indx_eq(indx, indx_sample));
        assert(rock_elem_eq(elem, elem_sample));

        rock_perm_free(perm_sort);
    }

    /* CLEANUP */

    MPI_Barrier(mesh->comm);

    if (mesh->rank == ROCK_MASTER) {
        rock_desc_free(desc);
        rock_mpart_free(mpart);
        rock_indx_free(indx);
        rock_elem_free(elem);
        rock_indx_free(indx_sample);
        rock_elem_free(elem_sample);
    }

    rock_mesh_free(mesh);
    rock_dist_free(dist);

    return ROCK_OK;
}

int
main(int argc, char **argv)
{
    srand(time(NULL));

    MPI_Init(&argc, &argv);
    assert(test_scatter_gather() == ROCK_OK);
    MPI_Finalize();

    return ROCK_OK;
}
