/**
 * @file sort.c
 * @author timoteus <mail@timoteus.se>
 */

#include "sort.h"
#include "print.h"

/* Use default values if not manually overridden. */
int rock_radix_bits = ROCK_USE_DEFAULT;
int rock_num_threads = ROCK_USE_DEFAULT;

static inline void
indx_sort_thread(rock_desc_t *desc,
                 rock_uint_t num_dims,
                 rock_uint_t *dims,
                 rock_perm_t *perm,
                 rock_perm_t *perm_alt,
                 rock_indx_t *indx,
                 rock_indx_t *indx_alt,
                 bool *swapped,
                 rock_uint_t size,
                 rock_uint_t num_bins,
                 rock_uint_t *bins,
                 rock_uint_t bins_offset,
                 rock_uint_t indx_offset,
                 int *num_passes)
{
    int num_threads = omp_get_num_threads();

    int dim_num = num_dims - 1;
    int dim = dims[dim_num];
    int offset = 0;
    int dim_offset = 0;
    int dim_bits = 0;
    int num_bits = 0;

    if (swapped != NULL) {
        *swapped = false;
    }

    bool first_pass = true;
    int total_bits = 0;
    int processed_bits = 0;
    for (rock_uint_t i = 0; i < num_dims; i++) {
        total_bits += desc->bit_width[dims[i]];
    }

    while (processed_bits < total_bits) {

        (*num_passes)++;

        /* Update extraction information. */
        if (first_pass || offset == dim_offset + dim_bits) {
            dim = dims[dim_num];
            dim_offset = desc->bit_offset[dim];
            dim_bits = desc->bit_width[dim];

            num_bits = rock_radix_bits;
            offset = dim_offset;
            dim_num--;
        }

        if (offset + rock_radix_bits > dim_offset + dim_bits) {
            num_bits = dim_offset + dim_bits - offset;
        }

        if (!first_pass) {
            rock_indx_swap(&indx, &indx_alt);
            if (perm != NULL) {
                rock_perm_swap(&perm, &perm_alt);
            }
        }

        rock_uint_t mask = (~(rock_uint_t)(~(rock_uint_t)
                    0 << num_bits) << offset);

        #pragma omp barrier

        /* Phase 1: Histogram. */
        for (rock_uint_t i = 0; i < size; i++) {
            if (perm != NULL && first_pass) {
                perm->v[indx_offset+i] = indx_offset+i;
            }
            rock_uint_t val = (indx->v[indx_offset+i] & mask) >> offset;
            bins[bins_offset+val]++;
        }

        #pragma omp barrier

        #pragma omp master
        {
            /* Phase 2: Prefix sum. */
            rock_uint_t total = 0;
            for (rock_uint_t i = 0; i < num_bins; i++) {
                for (rock_uint_t k = 0; k < num_threads; k++) {
                    rock_uint_t old = bins[(k*num_bins) + i];
                    bins[(k*num_bins) + i] = total;
                    total += old;
                }
            }
        }

        #pragma omp barrier

        /* Phase 3: Movement. */
        for (rock_uint_t i = 0; i < size; i++) {
            rock_uint_t ele = indx->v[indx_offset+i];
            int val = (ele & mask) >> offset;
            int pos = bins[bins_offset+val]++;
            indx_alt->v[pos] = ele;
            if (perm != NULL) {
                perm_alt->v[pos] = perm->v[indx_offset+i];
            }
        }

        offset += num_bits;
        processed_bits += num_bits;
        if (first_pass) {
            first_pass = false;
        }

        #pragma omp barrier

        #pragma omp master
            memset(bins, 0, num_threads * num_bins * sizeof(rock_uint_t));
    }
}

static inline void
indx_sort(rock_desc_t *desc,
          rock_uint_t num_dims,
          rock_uint_t *dims,
          rock_perm_t *perm,
          rock_perm_t *perm_alt,
          rock_indx_t *indx,
          rock_indx_t *indx_alt,
          bool *swapped)
{
    /* Buffer setup. */

    rock_uint_t num_bins =
            (rock_radix_bits > ROCK_MAX_SHIFT) ?  ROCK_UINT_MAX :
            (rock_uint_t) 1 << rock_radix_bits;
    rock_uint_t *bins = NULL;

    bool indx_alt_passed = true;
    bool perm_alt_passed = true;
    if (indx_alt == NULL) {
        indx_alt_passed = false;
        indx_alt = rock_indx_init(indx->len);
    }
    if (perm != NULL && perm_alt == NULL) {
        perm_alt_passed = false;
        perm_alt = rock_perm_init(perm->len);
    }
    if (swapped != NULL) {
        *swapped = false;
    }

    #pragma omp parallel shared(indx, indx_alt, perm, perm_alt, bins)
    {
        /* Parallel setup. */

        int id, num_threads;
        int num_passes = 0;
        id = omp_get_thread_num();
        num_threads = omp_get_num_threads();

        #pragma omp master
            bins = calloc(num_threads * num_bins, sizeof(rock_uint_t));

        #pragma omp barrier

        rock_uint_t size, chunk, indx_offset, bins_offset;
        chunk = indx->len / num_threads;
        indx_offset = id * chunk;
        bins_offset = id * num_bins;

        if (id == num_threads - 1) {
            size = indx->len - indx_offset;
        } else {
            size = chunk;
        }

        /* Sort. */

        indx_sort_thread(desc, num_dims, dims, perm, perm_alt,
                indx, indx_alt, swapped, size, num_bins, bins,
                bins_offset, indx_offset, &num_passes);


        #pragma omp master
        {
            /* Rearrange buffers and cleanup. */

            bool odd_passes = num_passes % 2 != 0;

            if (odd_passes && swapped != NULL) {
                *swapped = true;
            }

            if (!indx_alt_passed) {
                if (odd_passes) {
                    indx->len = indx_alt->len;
                    memcpy(indx->v, indx_alt->v,
                            indx_alt->len * sizeof(rock_uint_t));
                }
                rock_indx_free(indx_alt);
            }

            if (perm != NULL && !perm_alt_passed) {
                if (odd_passes) {
                    perm->len = perm_alt->len;
                    memcpy(perm->v, perm_alt->v,
                            perm_alt->len * sizeof(rock_uint_t));
                }
                rock_perm_free(perm_alt);
            }

            free(bins);
        }
    }
}

void
rock_indx_sort(rock_desc_t *desc,
               rock_uint_t num_dims,
               rock_uint_t *dims,
               rock_perm_t *perm,
               rock_indx_t *indx)
{
    rock_indx_sort_alt(desc, num_dims, dims, perm, NULL, indx, NULL, NULL);
}

void
rock_indx_sort_alt(rock_desc_t *desc,
                   rock_uint_t num_dims,
                   rock_uint_t *dims,
                   rock_perm_t *perm,
                   rock_perm_t *perm_alt,
                   rock_indx_t *indx,
                   rock_indx_t *indx_alt,
                   bool *swapped)
{
    if (rock_radix_bits == ROCK_USE_DEFAULT) {
        rock_radix_bits = ROCK_DEFAULT_RADIX_BITS;
    }

    /* Use single thread below threshold. */
    if (indx->len <= ROCK_PARALLEL_THRESHOLD) {
        omp_set_num_threads(1);
    }

    /* Allow manual override. */
    if (rock_num_threads != ROCK_USE_DEFAULT) {
        omp_set_num_threads(rock_num_threads);
    }

    indx_sort(desc, num_dims, dims, perm, perm_alt, indx, indx_alt, swapped);
}
