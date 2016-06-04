/**
 * @file random.c
 * @author timoteus <mail@timoteus.se>
 */

#include "random.h"
#include "sort.h"

rock_uint_t
rock_uint_random(rock_uint_t range)
{
#ifdef ROCK_WORD_SIZE_64
    return ROCK_ERR;
#else
    if (range > RAND_MAX) {
        return ROCK_BAD_INPUT;
    }
    uint32_t threshold = -range % range;
    for (;;) {
        uint32_t r = rand();
        if (r >= threshold) {
            return r % range;
        }
    }
#endif
}

double
rock_double_random()
{
    return (double) rand() / RAND_MAX;
}

/*
 * Use a custom algorithm based on the Fisher-Yates shuffle to shuffle
 * a yet not represented array full of integers between 0 and range - 1.
 *
 * By doing it this way, we don't have to iterate the whole range.
 *
 * Here are three iterations of the custom algorithm:
 *
 *     i=0, k=2, pos_z=2
 *
 *     20*         <- a
 *        0000000  <- b
 *     ^
 *
 *     i=1, k=2, pos_z=1
 *
 *     2*1         <- a
 *        0000000  <- b
 *      ^
 *
 *     i=2, k=0, pos_z=1
 *
 *     2*1         <- a
 *        0000000  <- b
 *       ^
 */
void
rock_uint_shuffle_take(rock_uint_t *a,
                       rock_uint_t len,
                       rock_uint_t range)
{
    rock_uint_t *b = calloc(range - len, sizeof(rock_uint_t));
    rock_uint_t k = rock_uint_random(range);
    rock_uint_t pos_z = k;

    a[0] = k;

    for (rock_uint_t i = 1; i < len; i++) {
        rock_uint_t ival;
     /* rock_uint_t *c = (k < len) ? a : b; */
        k = rock_uint_random(range - i) + i;

        if (a[i] == 0) {
            if (i == pos_z) {
                ival = 0;
                pos_z = k;
            } else {
                ival = i;
            }
        } else {
            ival = a[i];
        }

        if ((k < len && a[k] == 0) ||
                (k >= len && b[k-len] == 0)) {

            if (k == pos_z) {
                a[i] = 0;
                pos_z = ival;
            } else {
                a[i] = k;
            }

            if (k < len) {
                a[k] = ival;
            } else {
                b[k-len] = ival;
            }

        } else {
            if (k < len) {
                a[i] = a[k];
                a[k] = ival;
            } else {
                a[i] = b[k-len];
                b[k-len] = ival;
            }
        }
    }

    free(b);
}

static inline void
rock_val_to_indx(rock_desc_t *desc, rock_indx_t *indx)
{
    for (rock_uint_t i = 0; i < indx->len; i++) {
        rock_uint_t v = indx->v[i];

        for (rock_uint_t k = 0; k < desc->order; k++) {
            rock_uint_t n_k = desc->dim_size[k];
            rock_uint_t v_k = v % n_k;

            v = (v - v_k) / n_k;

            rock_indx_insert(desc, indx, i, k, v_k);
        }
    }
}

static inline int
rock_indx_distinct_step(rock_desc_t *desc,
                        rock_indx_t *indx,
                        rock_uint_t num_dims,
                        rock_uint_t *dims)
{
    int distinct = 1;

    rock_indx_sort(desc, num_dims, dims, NULL, indx);

    for (rock_uint_t i = 0; i < indx->len - 1; i++) {
        while (indx->v[i] == indx->v[i+1]) {
            rock_indx_set(indx, i, rock_uint_random(desc->total_size));
            if (distinct) {
                distinct = 0;
            }
        }
    }

    return distinct;
}

int
rock_indx_sample_shuffle_take(rock_desc_t *desc, rock_indx_t *indx)
{
    /* Are we able to sample indices for this tensor? */
    if (indx->len > desc->total_size) {
        return ROCK_BAD_INPUT;
    }

    rock_uint_shuffle_take(indx->v, indx->len, desc->total_size);

    rock_val_to_indx(desc, indx);

    return ROCK_OK;
}

int
rock_indx_sample_sort_discard(rock_desc_t *desc, rock_indx_t *indx)
{
    /* Are we able to sample indices for this tensor? */
    if (indx->len > desc->total_size) {
        return ROCK_BAD_INPUT;
    }

    rock_uint_t num_dims = desc->order;
    rock_uint_t *dims = calloc(desc->order, sizeof(rock_uint_t));

    for (rock_uint_t i = 0; i < desc->order; i++) {
        dims[i] = i;
    }

    /* Initial sample. */
    for (rock_uint_t i = 0; i < indx->len; i++) {
        rock_indx_set(indx, i, rock_uint_random(desc->total_size));
    }

    /* Until distinct, resample duplicates. */
    int distinct = 0;
    while (!distinct) {
        distinct = rock_indx_distinct_step(desc, indx, num_dims, dims);
    }

    /* When distinct, convert values to proper multi-indices. */
    rock_val_to_indx(desc, indx);

    free(dims);

    return ROCK_OK;
}

int
rock_indx_sample(rock_desc_t *desc, rock_indx_t *indx)
{

    /* TODO Dynamically select the faster algorithm based on input. */

    /* Fast but whole range needs to fit in memory. */
    /* rock_indx_sample_shuffle_take(desc, indx); */

    /* Does not require the whole range to fit in memory. */
    return rock_indx_sample_sort_discard(desc, indx);
}

void
rock_elem_sample(rock_desc_t *desc, rock_elem_t *elem)
{
    for (rock_uint_t i = 0; i < elem->len; i++) {
        elem->v[i] = rock_double_random();
    }
}

void
rock_perm_sample(rock_perm_t *perm)
{
    for (rock_uint_t i = 0; i < perm->len; i++) {
        perm->v[i] = i;
    }

    rock_perm_shuffle(perm);
}

void
rock_perm_shuffle(rock_perm_t *perm)
{
    for (rock_uint_t i = perm->len - 1; i > 0; i--) {
        rock_uint_t k = rock_uint_random(i + 1); /* Due to exclusive rand. */
        rock_uint_swap(&perm->v[k], &perm->v[i]);
    }
}
