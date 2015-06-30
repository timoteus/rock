/**
 * @file test_disk.c
 * @author timoteus <mail@timoteus.se>
 */

#include "core.h"
#include "sort.h"
#include "random.h"
#include "disk.h"

/**
 * Unit test of rock_indx_save() and rock_indx_load().
 */
void
test_rock_indx_save_load()
{
    char *fname = "test_rock_indx_save_load.hdf5";

    /* Setup arbitrary tensor. */
    rock_uint_t order = 4;
    rock_uint_t dim_size[] = {20, 500, 4, 1000};
    rock_desc_t *desc = rock_desc_init(order, dim_size);
    rock_uint_t nnz = 500; /* Number of non-zero elements. */

    /* Sample some test data. */
    rock_indx_t *indx = rock_indx_init(nnz);
    rock_indx_sample(desc, indx);

    /* Save sampled data. */
    assert(rock_indx_save(indx, fname) == ROCK_OK);

    /* Load previously saved data. */
    rock_indx_t *indx_loaded = rock_indx_init(nnz);
    assert(rock_indx_load(indx_loaded, fname) == ROCK_OK);

    /* Compare loaded with original. */
    assert(rock_indx_eq(indx, indx_loaded));

    /* Free memory. */
    rock_desc_free(desc);
    rock_indx_free(indx);
    rock_indx_free(indx_loaded);
}

int
main()
{
    srand(time(NULL));

    test_rock_indx_save_load();

    return ROCK_OK;
}
