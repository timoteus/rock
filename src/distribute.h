/**
 * @file distribute.h
 * @author timoteus <mail@timoteus.se>
 */

#ifndef _DISTRIBUTE_H
#define _DISTRIBUTE_H

#include "core.h"
#include <mpi.h>

/**
 * Processor mesh object.
 *
 * Represents a logical mesh of processors.
 *
 * Can be used in combination with @c rock_mpart_t to distribute
 * tensors on a distributed memory machine.
 */
typedef struct rock_mesh_s
{
    /** The number of processes (for convenience). */
    rock_uint_t np;

    /** Processor rank (i.e. processor "number"). */
    rock_uint_t rank;

    /** The number of dimensions in the mesh (0 < n <= ROCK_MAX_MESH_ORDER). */
    rock_uint_t order;

    /** The size of each dimension. */
    rock_uint_t dim_size[ROCK_MAX_MESH_ORDER];

    /** The MPI communicator object. */
    MPI_Comm comm;

} rock_mesh_t;

/**
 * Multi-partition object.
 *
 * Represents a tensor partitioned along multiple dimensions using
 * partition objects (@c rock_part_t).
 *
 * Can be used together with @c rock_mesh_t to distribute tensors
 * on a distributed memory machine.
 */
typedef struct rock_mpart_s
{
    /* Reference to a tensor descriptor (not owned). */
    rock_desc_t *desc;

    /** The number of partitions i.e. partitioned dimensions. */
    rock_uint_t order;

    /** The dimension number of @c dim_part in the tensor descriptor. */
    rock_uint_t dim_num[ROCK_MAX_ORDER];

    /** Partition objects for partitioned dimensions. */
    rock_part_t *dim_part[ROCK_MAX_ORDER];

} rock_mpart_t;

/**
 * Distribution object.
 *
 * Containts data needed to distribute a tensor over a mesh object
 * on a distributed memory machine using collective MPI operations.
 *
 * Produced using the $c rock_mesh_t and @c rock_mpart_t objects.
 */
typedef struct rock_dist_s
{
    /* Reference to mesh object (not owned). */
    rock_mesh_t *mesh;

    /* The number of items that belong to each processor. */
    rock_uint_t *count;

    /* The offset of those items in the index and element array. */
    rock_uint_t *offset;

    /* The sum for convenience, same as offset[np]. */
    rock_uint_t sum;

} rock_dist_t;

/**
 * Initialize a processor mesh object.
 *
 * Represent a logical mesh of processors.
 *
 * You'll probably want the number of processors of the mesh object
 * to match the number of processors you run mpi with, e.g., using
 * mpirun -np num_procs.
 *
 * @param [in] comm         MPI Comm object to associate mesh with.
 * @param [in] num_dims     The number of dimensions of the processor mesh.
 * @param [in] dim_size     The size of each dimension.
 * @return                  A pointer to the initialized mesh object.
 */
rock_mesh_t *
rock_mesh_init(MPI_Comm comm,
               rock_uint_t num_dims,
               rock_uint_t *dim_size);

/**
 * Initialize a multi-partition object.
 *
 * Match @c order and @c dim_num_parts with a mesh's @c order
 * and @c dim_size to create a object suitable for creating a
 * tensor distribution object (@c rock_dist_t).

 * @param [in] order            The number of partitions.
 * @param [in] dim_num_parts    The size (number of parts) of each dimension.
 * @return                      A pointer to the initialized object.
 */
rock_mpart_t *
rock_mpart_init(rock_uint_t order, rock_uint_t *dim_num_parts);

/**
 * Initialize and populate a distribution object.
 *
 * Calculate information needed to distribute a tensor over a mesh of
 * processors based on a mesh and a multi-partition object.
 *
 * Computes what part of the index and elem arrays that corresponds
 * to what processor in the processor mesh.
 *
 * Index and element arrays are both permuted based on what processor their
 * elements corresponds to.
 *
 * @param [in,out] indx     The tensor indices.
 * @param [in,out] elem     The tensor elements.
 * @param [in] mpart        The partitions to use for distribution.
 * @param [in] mesh         The mesh to distribute the tensor over.
 * @return                  Initialized and populated distribution object.
 */
rock_dist_t *
rock_dist_init(rock_indx_t *indx,
               rock_elem_t *elem,
               rock_mpart_t *mpart,
               rock_mesh_t *mesh);

/**
 * Free a processor mesh object.
 *
 * @param [in] mesh
 */
void
rock_mesh_free(rock_mesh_t *mesh);

/**
 * Free a multi-partition object.
 *
 * Associated objects should be free'd separately.
 *
 * @param [in] mpart
 */
void
rock_mpart_free(rock_mpart_t *mpart);

/**
 * Free a collective counts object.
 *
 * Associated objects should be free'd separately.
 *
 * @param [in] dist
 */
void
rock_dist_free(rock_dist_t *dist);

/**
 * Print a whole processor mesh object.
 *
 * @param [in] mesh
 */
void
rock_mesh_print(rock_mesh_t *mesh);

/**
 * Print a whole multi-partition object.
 *
 * @param [in] mpart
 */
void
rock_mpart_print(rock_mpart_t *mpart);

/**
 * Scatter an index array among a mesh of processors using
 * a distribution object.
 *
 * This function allocates enough memory for all processors to fit
 * their respective chunk of the index array. The supplied index
 * array at the master processor will be deallocated after being
 * scattered.
 *
 * - Master: buffer deallocated
 * - All: buffer allocated
 *
 * @param [in,out] indx     The index array to scatter from and to.
 *                          Only master processor should supply this.
 * @param [in] dist
 */
void
rock_indx_scatter(rock_indx_t **indx, rock_dist_t *dist);

/**
 * Gather an index array among a mesh of processors using a
 * distribution object.
 *
 * This function allocates enough memory at the master processor to fit
 * all data being gathered from all processors. After the gather has been
 * completed, the buffers that were sent from are all deallocated.
 *
 * - Master: buffer allocated
 * - All: buffer deallocated
 *
 * @param [in,out] indx     The index array to gather to and from.
 *                          All processors in the comm group must supply this.
 * @param [in] dist
 */
void
rock_indx_gather(rock_indx_t **indx, rock_dist_t *dist);

/**

 * Scatter an element array among a mesh of processors using
 * a distribution object.
 *
 * This function allocates enough memory for all processors to fit
 * their respective chunk of the element array. The supplied elem
 * array at the master processor will be deallocated after being
 * scattered.
 *
 * - Master: buffer deallocated
 * - All: buffer allocated
 *
 * @param [in,out] elem     The element array to scatter from and to.
 *                          Only master processor should supply this.
 * @param [in] dist
 */
void
rock_elem_scatter(rock_elem_t **elem, rock_dist_t *dist);

/**
 * Gather an element array among a mesh of processors using a
 * distribution object.
 *
 * This function allocates enough memory at the master processor to fit
 * all data being gathered from all processors. After the gather has been
 * completed, the buffers that were sent from are all deallocated.
 *
 * - Master: buffer allocated
 * - All: buffer deallocated
 *
 * @param [in,out] elem     The element array to gather to and from.
 *                          All processors in the comm group must supply this.
 * @param [in] dist
 */
void
rock_elem_gather(rock_elem_t **elem, rock_dist_t *dist);

/**
 * Calculate what part of a multi-partition object a specific
 * multi-index corresponds to.
 *
 *         tensor space            mpart
 *     +------------------+ +------------------+
 *     |                  | |      part 0      |
 *     |                  | +------------------+
 *     |  * <- which part | |      part 1      |
 *     |  does this index | +------------------+
 *     |  correspond to?  | |                  |
 *     |  => part 1       | |      part 2      |
 *     |                  | |                  |
 *     +------------------+ +------------------+
 *
 * @param [in] mpart
 * @param [in] indx
 * @param [in] i
 * @return                  Number of the corresponding part.
 */
rock_uint_t
rock_part_num_from_mpart(rock_mpart_t *mpart,
                         rock_indx_t *indx,
                         rock_uint_t i);

/**
 * Create a multi-partition based on a tensor descriptor.
 *
 * Partition tensor space along a pre-defined number of dimensions
 * (@c mpart->order). Use arguments to specify what dimensions of
 * the tensor to partition. If not enough dimensions are specified,
 * the largest remaining dimensions of the tensor (described by @c
 * desc) will be used.
 *
 * @param [in] desc             Tensor space to base partitions on.
 * @param [in,out] mpart        An already initialized multi-partition object.
 * @param [in] num_prio_dims    Number of dimensions in @c desc to partition,
 *                              uses largest remaining if < @c mpart->order.
 * @param [in] prio_dims        Indices of dimensions in @c desc to partition.
 */
int
rock_mpart_desc_based(rock_desc_t *desc,
                      rock_mpart_t *mpart,
                      rock_uint_t num_prio_dims,
                      rock_uint_t *prio_dims);

#endif
