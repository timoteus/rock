/**
 * @file test_core.c
 * @author timoteus <mail@timoteus.se>
 */

#include "core.h"
#include "print.h"

/**
 * Unit test of rock_desc_init(), rock_desc_free().
 */
void
test_rock_desc(rock_uint_t order, rock_uint_t *dim_size)
{
    rock_desc_t *desc = rock_desc_init(order, dim_size);

    assert(desc != NULL);

    assert(desc->order == 3);
    assert(desc->total_size == 1000 * 20 * 500);
    assert(desc->dim_size[0] == 1000);
    assert(desc->dim_size[1] == 20);
    assert(desc->dim_size[2] == 500);

    assert(desc->bit_width[0] == 10);       /* 2^10 == 1024 */
    assert(desc->bit_width[1] == 5);        /* 2^5 == 32 */
    assert(desc->bit_width[2] == 9);        /* 2^9 == 512 */

    assert(desc->bit_offset[0] == 0);       /* Should always be 0. */
    assert(desc->bit_offset[1] == 10);      /* Prev width. */
    assert(desc->bit_offset[2] == 10 + 5);  /* Prev widths. */

    /* This is not standard (but oh so convenient, requires gcc) */
    rock_uint_t mask1 = 0b00000000000000000000001111111111;
    rock_uint_t mask2 = 0b00000000000000000111110000000000;
    rock_uint_t mask3 = 0b00000000111111111000000000000000;

    assert(desc->bit_mask[0] == mask1);
    assert(desc->bit_mask[1] == mask2);
    assert(desc->bit_mask[2] == mask3);

    rock_desc_print("desc", desc);

    rock_desc_free(desc);
}

/**
 * Unit test of rock_indx_init(), rock_indx_free().
 */
void
test_rock_indx(rock_uint_t nnz)
{
    rock_indx_t *indx = rock_indx_init(nnz);

    assert(indx != NULL);

    rock_indx_free(indx);
}

/**
 * Unit test of rock_elem_init(), rock_elem_free().
 */
void
test_rock_elem(rock_uint_t nnz)
{
    rock_elem_t *elem = rock_elem_init(nnz);

    assert(elem != NULL);

    rock_elem_free(elem);
}

/**
 * Unit test of rock_upkd_init(), rock_upkd_free().
 */
void
test_rock_upkd(rock_desc_t *desc, rock_uint_t nnz)
{
    rock_upkd_t *upkd = rock_upkd_init(desc, nnz);

    assert(upkd != NULL);

    rock_upkd_free(upkd);
}

/**
 * Unit test of rock_perm_init(), rock_perm_free().
 */
void
test_rock_perm(rock_uint_t nnz)
{
    rock_perm_t *perm = rock_perm_init(nnz);

    assert(perm != NULL);

    rock_perm_free(perm);
}

/**
 * Unit test of rock_part_init(), rock_part_free(),
 * rock_part_indx_based(), rock_part_desc_based().
 */
void
test_rock_part(rock_desc_t *desc,
               rock_indx_t *indx,
               rock_uint_t num_parts)
{
    rock_part_t *part_1 = rock_part_init(num_parts);
    rock_part_t *part_2 = rock_part_init(num_parts);

    assert(part_1 != NULL);
    assert(part_2 != NULL);

    rock_part_indx_based(desc, part_1, indx, 0);
    rock_part_desc_based(desc, part_2, 0);

    rock_part_free(part_1);
    rock_part_free(part_2);
}

/**
 * Unit test of rock_freq_init(), rock_freq_free().
 */
void
test_rock_freq(rock_desc_t *desc, rock_indx_t *indx)
{
    rock_freq_t *freq = rock_freq_init(desc, indx);

    assert(freq != NULL);

    rock_freq_free(freq);
}

/**
 * Unit test of rock_view_init(), rock_view_free().
 */
void
test_rock_view(rock_desc_t *desc,
               rock_indx_t *indx,
               rock_uint_t sorted_dim)
{
    rock_view_t *view = rock_view_init(desc, indx, sorted_dim);

    assert(view != NULL);

    rock_view_free(view);
}

int
main()
{
    srand(time(NULL));

    rock_uint_t order = 3;
    rock_uint_t dim_size[] = {1000, 20, 500};
    rock_uint_t nnz = 1e4; /* Number of non-zero elements. */

    test_rock_desc(order, dim_size);

    rock_desc_t *desc = rock_desc_init(order, dim_size);

    test_rock_indx(nnz);
    test_rock_elem(nnz);
    test_rock_upkd(desc, nnz);
    test_rock_perm(nnz);

    rock_indx_t *indx = rock_indx_init(nnz);

    test_rock_part(desc, indx, 4);
    test_rock_freq(desc, indx);
    test_rock_view(desc, indx, 2);

    rock_desc_free(desc);
    rock_indx_free(indx);

    return ROCK_OK;
}
