/**
 * @file random.h
 * @author timoteus <mail@timoteus.se>
 */

#ifndef _RANDOM_H
#define _RANDOM_H

#include "core.h"

/**
 * Generate a pseudo-random number within a certain range.
 *
 * @param range             Upper limit (exclusive).
 * @return                  A number between 0 and @c range.
 */
rock_uint_t
rock_uint_random(rock_uint_t range);

/**
 * Generate a pseudo-random double between 0 and 1.
 *
 * @return                  A double between 0 and 1.
 */
double
rock_double_random();

/**
 * Generate a pseudo-random index array based on a tensor descriptor.
 *
 * The generated set of indices is distinct (we can't have two indices
 * referencing the same position in the same tensor).
 *
 * Due to this, the length of the index array must, obviously, be less
 * than or equal to the total_size of the tensor descriptor.
 *
 * This routine should ideally select the faster algorithm based on input.
 * Until that time, you can manually select between algorithms yourself,
 * see @c rock_indx_sample_shuffle_take and $c rock_indx_sample_sort_discard.
 *
 * @param [in] desc         A tensor descriptor object.
 * @param [in,out] indx     The index array to sample indices for.
 * @return                  ROCK_OK or ROCK_BAD_INPUT.
 */
int
rock_indx_sample(rock_desc_t *desc, rock_indx_t *indx);

/**
 * Generate a pseudo-random index array based on a tensor descriptor.
 *
 * This algorithm is expected to be fast, but requires the whole range
 * to fit in memory.
 *
 * @param [in] desc         A tensor descriptor object.
 * @param [in,out] indx     The index array to sample indices for.
 * @return                  ROCK_OK or ROCK_BAD_INPUT.
 */
int
rock_indx_sample_shuffle_take(rock_desc_t *desc, rock_indx_t *indx);

/**
 * Generate a pseudo-random index array based on a tensor descriptor.
 *
 * This algorithm does not require the whole range to fit in memory.
 *
 * @param [in] desc         A tensor descriptor object.
 * @param [in,out] indx     The index array to sample indices for.
 * @return                  ROCK_OK or ROCK_BAD_INPUT.
 */
int
rock_indx_sample_sort_discard(rock_desc_t *desc, rock_indx_t *indx);

/**
 * Generate a pseudo-random element array based on a tensor descriptor.
 *
 * @param [in] desc         A tensor descriptor object.
 * @param [in,out] elem     The element array to sample elements for.
 */
void
rock_elem_sample(rock_desc_t *desc, rock_elem_t *elem);

/**
 * Sample a random permutation.
 *
 * @param [in,out] perm
 */
void
rock_perm_sample(rock_perm_t *perm);

/**
 * Shuffle an existing permutation.
 *
 * @param [in,out] perm
 */
void
rock_perm_shuffle(rock_perm_t *perm);

#endif
