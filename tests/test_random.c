/**
 * @file test_random.c
 * @author timoteus <mail@timoteus.se>
 */

#include "core.h"
#include "random.h"

/**
  * Unit test of rock_indx_sample().
  *
  * This is a very simple test, we _could_ spend all year verifying
  * the results of this function if we wanted to.
  *
  * We're mostly going to make sure generated indices are within
  * the bounds of our tensor, we're not going to test the actual
  * distribution of indices.
  */
void
test_rock_indx_sample()
{
    rock_uint_t order = 3;
    rock_uint_t dim_size[] = {6, 40, 2};
    rock_desc_t *desc = rock_desc_init(order, dim_size);

    /* Make sure we're not able to try and do the impossible. */
    rock_indx_t *indx_err = rock_indx_init(desc->total_size + 1);
    assert(rock_indx_sample(desc, indx_err) == ROCK_BAD_INPUT);

    /*
     * This should be rare (the reason for this library is _sparse_
     * tensors and it can't get denser than this) although it should
     * be supported.
     */
    rock_indx_t *indx_ok = rock_indx_init(desc->total_size);
    assert(rock_indx_sample(desc, indx_ok) == ROCK_OK);

    /* Make sure indices are within bounds. */
    for (rock_uint_t i = 0; i < indx_ok->len; i++) {
        for (rock_uint_t k = 0; k < desc->order; k++) {
            assert(rock_indx_extract(desc, indx_ok, i, k)
                    < desc->dim_size[k]);
        }
    }

    /* Make sure mutli-indices are distinct (naively without sorting them). */
    for (rock_uint_t i = 0; i < indx_ok->len; i++) {
        for (rock_uint_t k = 0; k < indx_ok->len; k++) {
            if (i != k) {
                assert(rock_indx_get(indx_ok, i) !=
                        rock_indx_get(indx_ok, k));
            }
        }
    }

    /* Free memory. */
    rock_desc_free(desc);
    rock_indx_free(indx_err);
    rock_indx_free(indx_ok);
}

int
main()
{
    srand(time(NULL));

    test_rock_indx_sample();

    return ROCK_OK;
}
