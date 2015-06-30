/**
 * @file test_sort.c
 * @author timoteus <mail@timoteus.se>
 */

#include "core.h"
#include "disk.h"
#include "sort.h"
#include "random.h"
#include "print.h"

extern int rock_radix_bits;
extern int rock_num_threads;

void
test(rock_desc_t *desc,
     rock_uint_t num_dims,
     rock_uint_t *dims,
     rock_indx_t *indx_test,
     rock_indx_t *indx_correct,
     rock_perm_t *perm_correct)
{
    /* Not using alternate buffers. */
    rock_indx_t *indx_1 = rock_indx_copy(indx_test);
    rock_perm_t *perm_1 = rock_perm_init(indx_test->len);
    rock_indx_sort(desc, num_dims, dims, perm_1, indx_1);
    assert(rock_indx_eq(indx_1, indx_correct));
    assert(rock_perm_eq(perm_1, perm_correct));
    rock_indx_free(indx_1);
    rock_perm_free(perm_1);

    /* Using alternate buffers. */
    rock_indx_t *indx_2 = rock_indx_copy(indx_test);
    rock_indx_t *indx_alt_2 = rock_indx_init(indx_test->len);
    rock_perm_t *perm_2 = rock_perm_init(indx_test->len);
    rock_perm_t *perm_alt_2 = rock_perm_init(indx_test->len);
    bool swapped_2;
    rock_indx_sort_alt(desc, num_dims, dims,
            perm_2, perm_alt_2, indx_2, indx_alt_2, &swapped_2);
    if (swapped_2) {
        rock_indx_swap(&indx_2, &indx_alt_2);
        rock_perm_swap(&perm_2, &perm_alt_2);
    }
    assert(rock_indx_eq(indx_2, indx_correct));
    assert(rock_perm_eq(perm_2, perm_correct));
    rock_indx_free(indx_2);
    rock_indx_free(indx_alt_2);
    rock_perm_free(perm_2);
    rock_perm_free(perm_alt_2);
}

/**
 * Unit test of rock_indx_sort().
 */
void
test_rock_indx_sort()
{
    /* Setup descriptor and arrays according to test data (see filename). */
    rock_uint_t order = 4;
    rock_uint_t dim_size[] = {512, 4, 8, 2};
    rock_uint_t nnz = 5000; /* Number of non-zero elements. */
    rock_desc_t *desc = rock_desc_init(order, dim_size);

    /* Load unsorted data. */
    rock_indx_t *indx_test = rock_indx_init(nnz);
    assert(rock_indx_load(indx_test,
            "data/indx_32bit_512x4x8x2_5000_sample.hdf5") == ROCK_OK);

    /* Load correctly sorted data. */
    rock_indx_t *indx_correct = rock_indx_init(nnz);
    assert(rock_indx_load(indx_correct,
            "data/indx_32bit_512x4x8x2_5000_sorted_3210.hdf5") == ROCK_OK);

    /* Load correctly generated perm info. */
    rock_perm_t *perm_correct = rock_perm_init(nnz);
    assert(rock_perm_load(perm_correct,
            "data/perm_32bit_512x4x8x2_5000_sorted_3210.hdf5") == ROCK_OK);

    /* Sort according to same dims as loaded data. */
    rock_uint_t num_dims = 4;
    rock_uint_t dims[] = {3, 2, 1, 0};

    /* Run tests. */
    for (int radix = 1; radix <= 10; radix++) {
        rock_radix_bits = radix;
        for (int np = 1; np <= 10; np++) {
            rock_num_threads = np;
            test(desc, num_dims, dims,
                indx_test, indx_correct, perm_correct);
        }
    }

    rock_desc_free(desc);
    rock_indx_free(indx_test);
    rock_indx_free(indx_correct);
    rock_perm_free(perm_correct);
}

int
main()
{
    srand(time(NULL));

    test_rock_indx_sort();

    return ROCK_OK;
}
