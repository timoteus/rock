/**
 * @file sort.h
 * @author timoteus <mail@timoteus.se>
 */

#ifndef _SORT_H
#define _SORT_H

#include "core.h"

/** The number of threads to use for parallel sort. */
extern int rock_num_threads;

/** The number of bits to maximally process each pass of radix sort. */
extern int rock_radix_bits;

/**
 * Sorts an index array of packed multi-indices according to one or
 * more dimensions.
 *
 * Returns the resulting permutation and sorted indices in the same
 * buffers as were used to pass them. This may result in a superfluous
 * copy, use @c rock_indx_sort_alt if you would like to avoid this.
 *
 * @param [in] desc         A tensor descriptor object.
 * @param [in] num_dims     The number of dimensions to sort.
 * @param [in] dims         The dimensions to sort, highest priority first.
 * @param [in,out] perm     The permutation applied to the index array.
 * @param [in,out] indx     The sorted index array.
 */
void
rock_indx_sort(rock_desc_t *desc,
               rock_uint_t num_dims,
               rock_uint_t *dims,
               rock_perm_t *perm,
               rock_indx_t *indx);

/**
 * Sorts an index array of packed multi-indices according to one or
 * more dimensions.
 *
 * The output is stored in @c perm and @c indx if @c swapped is @c false
 * and in @c perm_alt and @c indx_alt if @c swapped is @c true.
 *
 * @param [in] desc         A tensor descriptor object.
 * @param [in] num_dims     The number of dimensions to sort.
 * @param [in] dims         The dimensions to sort, highest priority first.
 * @param [in,out] perm     The permutation applied to the index array.
 * @param [out] perm_alt    A permutation array same length as perm.
 * @param [in,out] indx     The sorted index array.
 * @param [out] indx_alt    An index array same length as indx.
 * @param [out] swapped     @c true if output is located in alt arrays.
 */
void
rock_indx_sort_alt(rock_desc_t *desc,
                   rock_uint_t num_dims,
                   rock_uint_t *dims,
                   rock_perm_t *perm,
                   rock_perm_t *perm_alt,
                   rock_indx_t *indx,
                   rock_indx_t *indx_alt,
                   bool *swapped);

#endif
