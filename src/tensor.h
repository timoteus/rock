/**
 * @file tensor.h
 * @author timoteus <mail@timoteus.se>
 */

#ifndef _TENSOR_H
#define _TENSOR_H

#include "core.h"

/**
 * A tensor object using the coordinate format to repesent a sparse
 * or incomplete tensor (multidimensional array).
 *
 * The index (@c indx) array represents the coordinates (indices) of
 * the elements stored in the element (@c elem) array.
 *
 * See core.h for further details.
 */
typedef struct rock_tensor_s
{
    /**
     * A tensor descriptor object.
     *
     * This object includes the order, dimension sizes and
     * bit-packing information of the tensor.
     */

    rock_desc_t *desc;
    /**
     * An array of packed multi-indices.
     *
     * These are the indices of the non-zero elements of the tensor.
     */
    rock_indx_t *indx;

    /**
     * An array of elements.
     *
     * These are the values of the non-zero elements of the tensor.
     */
    rock_elem_t *elem;

} rock_tensor_t;

/**
 * Initialize a tensor object.
 *
 * This routine allocates and initializes the descriptor object,
 * the index and the element array.
 *
 * See core.h for further details.
 *
 * @param [in] order        The order (or number of dimensions).
 * @param [in] dim_size     The size of each dimension (order).
 * @param [in] num_elem     The number of non-zero elements.
 */
rock_tensor_t *
rock_tensor_init(rock_uint_t order,
                 rock_uint_t *dim_size,
                 rock_uint_t num_elem);

/**
 * Free a tensor object.
 *
 * This routine will also free the indx and elem arrays.
 *
 * @param [in] tensor
 */
void
rock_tensor_free(rock_tensor_t *tensor);

/**
 * Print the tensor's non-zero elements and their respective indices
 * using a specified name, width, and precision.
 *
 * See print.h for further details.
 *
 * @param [in] tensor
 * @param [in] name
 * @param [in] width
 * @param [in] precision
 */
void
rock_tensor_print(rock_tensor_t *tensor,
                  char *name,
                  int width,
                  int precision);

/**
 * Populate a tensor with sample data.
 *
 * The index array is populated with distinct indices based on the tensor's
 * descriptor object. The element array is populated with randomly drawn
 * values between 0 and 1.
 *
 * A seed (using @c srand()) should be set prior to sampling.
 *
 * See random.h for further details.
 *
 * @param [in] tensor       Initialized tensor object.
 */
void
rock_tensor_sample(rock_tensor_t *tensor);

/**
 * Shuffle the tensor's index and element arrays.
 *
 * See random.h for further details.
 *
 * @param [in] tensor       Initialized and populated tensor object.
 */
void
rock_tensor_shuffle(rock_tensor_t *tensor);

/**
 * Sort a tensor according to a specified dimension (mode).
 *
 * The element array will be permuted using the permutation retrieved
 * by sorting the index array.
 *
 * High performance is achieved by using tailored algorithms and a
 * compact (bit-packed) representation scheme.
 *
 * This is the heart of the library, expect this routine to be many
 * times faster than using, e.g., @c qsort() from the C standard library.
 *
 * See sort.h for more information.
 *
 * @c rock_indx_sort and friends can be used for more advanced and
 * performant sorting operations (be sure to permute @c elem post sort).
 *
 * @param [in] tensor       Initialized and populated tensor object.
 * @param [in] dimension    The dimension of the tensor to sort.
 */
void
rock_tensor_sort(rock_tensor_t *tensor, rock_uint_t dimension);

#endif
