/**
 * @file core.c
 * @author timoteus <mail@timoteus.se>
 */

#include "core.h"
#include "sort.h"

rock_desc_t *
rock_desc_init(rock_uint_t order,
               rock_uint_t *dim_size)
{
    rock_desc_t *desc = calloc(1, sizeof(rock_desc_t));
    rock_uint_t total_size = 1;

    for (rock_uint_t i = 0; i < order; i++) {
        total_size *= dim_size[i];
        desc->dim_size[i] = dim_size[i];
        desc->bit_width[i] = ceil(log2(desc->dim_size[i]));
        desc->bit_offset[i] = (i == 0) ? 0
                : desc->bit_offset[i-1] + desc->bit_width[i-1];
        if (desc->bit_width[i] > ROCK_MAX_SHIFT) {
            desc->bit_mask[i] = ROCK_UINT_MAX;
        } else {
            desc->bit_mask[i] = (~(~(rock_uint_t)0
                    << desc->bit_width[i]) << desc->bit_offset[i]);
        }
    }

    desc->order = order;
    desc->total_size = total_size;

    return desc;
}

rock_indx_t *
rock_indx_init(rock_uint_t len)
{
    rock_indx_t *indx = calloc(1, sizeof(rock_indx_t));
    indx->v = calloc(len, sizeof(rock_indx_t));

    indx->len = len;

    return indx;
}

rock_elem_t *
rock_elem_init(rock_uint_t len)
{
    rock_elem_t *elem = calloc(1, sizeof(rock_elem_t));
    elem->v = calloc(len, sizeof(rock_elem_t));

    elem->len = len;

    return elem;
}

rock_upkd_t *
rock_upkd_init(rock_desc_t *desc, rock_uint_t len)
{
    rock_upkd_t *upkd = calloc(1, sizeof(rock_upkd_t));
    rock_uint_t tot_len = len * desc->order;

    upkd->v = calloc(tot_len, sizeof(rock_uint_t));

    upkd->len = tot_len;

    return upkd;
}

rock_perm_t *
rock_perm_init(rock_uint_t len)
{
    rock_perm_t *perm = calloc(1, sizeof(rock_perm_t));
    perm->v = calloc(len, sizeof(rock_uint_t));

    perm->len = len;

    return perm;
}

rock_part_t *
rock_part_init(rock_uint_t num_parts)
{
    rock_part_t *part = calloc(1, sizeof(rock_part_t));
    part->offset = calloc(num_parts + 1, sizeof(rock_uint_t));

    part->num_parts = num_parts;

    return part;
}

rock_freq_t *
rock_freq_init(rock_desc_t *desc, rock_indx_t *indx)
{
    rock_freq_t *freq = calloc(1, sizeof(rock_freq_t));

    for (rock_uint_t i = 0; i < desc->order; i++) {
        freq->dim_freq[i] =
                calloc(desc->dim_size[i], sizeof(rock_uint_t));
    }

    /* Count frequency of indices. */
    for (rock_uint_t i = 0; i < indx->len; i++) {
        for (rock_uint_t k = 0; k < desc->order; k++) {
            rock_uint_t val = rock_indx_extract(desc, indx, i, k);
            freq->dim_freq[k][val]++;
        }
    }

    freq->desc = desc;
    freq->indx = indx;

    return freq;
}

rock_view_t *
rock_view_init(rock_desc_t *desc,
               rock_indx_t *indx,
               rock_uint_t sorted_dim)
{
    rock_view_t *view = calloc(1, sizeof(rock_view_t));

    /* Copy to avoid permuting the index array. */
    rock_indx_t *tmp_indx = rock_indx_copy(indx);

    for (rock_uint_t i = 0; i < desc->order; i++) {
        rock_uint_t dims[] = {i};

        view->dim_perm[i] = rock_perm_init(tmp_indx->len);

        /* Don't unnecessarily compute perm for already sorted dim. */
        if (sorted_dim == i) {
            for (rock_uint_t k = 0; k < view->dim_perm[i]->len; k++) {
                view->dim_perm[i]->v[k] = k;
            }
        } else {
            rock_indx_sort(desc, 1, dims,
                    view->dim_perm[i], tmp_indx);
        }
    }

    rock_indx_free(tmp_indx);

    view->desc = desc;
    view->indx = indx;

    return view;
}

void
rock_desc_free(rock_desc_t *desc)
{
    free(desc);
}

void
rock_indx_free(rock_indx_t *indx)
{
    free(indx->v);
    free(indx);
}

void
rock_elem_free(rock_elem_t *elem)
{
    free(elem->v);
    free(elem);
}

void
rock_upkd_free(rock_upkd_t *upkd)
{
   free(upkd->v);
   free(upkd);
}

void
rock_perm_free(rock_perm_t *perm)
{
    free(perm->v);
    free(perm);
}

void
rock_part_free(rock_part_t *part)
{
    free(part->offset);
    free(part);
}

void
rock_freq_free(rock_freq_t *freq)
{
    for (rock_uint_t i = 0; i < freq->desc->order; i++) {
        free(freq->dim_freq[i]);
    }

    free(freq);
}

void
rock_view_free(rock_view_t *view)
{
    for (rock_uint_t i = 0; i < view->desc->order; i++) {
        rock_perm_free(view->dim_perm[i]);
    }

    free(view);
}

rock_indx_t *
rock_indx_copy(rock_indx_t *indx)
{
    rock_indx_t *copy = rock_indx_init(indx->len);
    memcpy(copy->v, indx->v, sizeof(indx->v)*indx->len);

    return copy;
}

rock_elem_t *
rock_elem_copy(rock_elem_t *elem)
{
    rock_elem_t *copy = rock_elem_init(elem->len);
    memcpy(copy->v, elem->v, sizeof(elem->v)*elem->len);

    return copy;
}

bool
rock_indx_eq(rock_indx_t *p1, rock_indx_t *p2)
{
    if (p1->len != p2->len) {
        return false;
    }

    for (rock_uint_t i = 0; i < p1->len; i++) {
        if (p1->v[i] != p2->v[i]) {
            return false;
        }
    }

    return true;
}

bool
rock_elem_eq(rock_elem_t *p1, rock_elem_t *p2)
{
    if (p1->len != p2->len) {
        return false;
    }

    for (rock_uint_t i = 0; i < p1->len; i++) {
        if (p1->v[i] != p2->v[i]) {
            return false;
        }
    }

    return true;
}

bool
rock_upkd_eq(rock_upkd_t *p1, rock_upkd_t *p2)
{
    if (p1->len != p2->len) {
        return false;
    }

    for (rock_uint_t i = 0; i < p1->len; i++) {
        if (p1->v[i] != p2->v[i]) {
            return false;
        }
    }

    return true;
}

bool
rock_perm_eq(rock_perm_t *p1, rock_perm_t *p2)
{
    if (p1->len != p2->len) {
        return false;
    }

    for (rock_uint_t i = 0; i < p1->len; i++) {
        if (p1->v[i] != p2->v[i]) {
            return false;
        }
    }

    return true;
}

void
rock_uint_swap(rock_uint_t *p1, rock_uint_t *p2)
{
    rock_uint_t tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
}

void
rock_indx_swap(rock_indx_t **p1, rock_indx_t **p2)
{
    void *tmp = *(void **)p1;
    *(void **)p1 = *(void **)p2;
    *(void **)p2 = tmp;
}

void
rock_perm_swap(rock_perm_t **p1, rock_perm_t **p2)
{
    void *tmp = *(void **)p1;
    *(void **)p1 = *(void **)p2;
    *(void **)p2 = tmp;
}

int
rock_indx_permute(rock_indx_t *indx, rock_perm_t *perm)
{
    return rock_indx_permute_alt(indx, perm, NULL);
}

int
rock_indx_permute_alt(rock_indx_t *indx,
                      rock_perm_t *perm,
                      rock_indx_t *out)
{
    if (out) {
        if (indx->len != out->len) {
            return ROCK_BAD_INPUT;
        }
        for (rock_uint_t i = 0; i < indx->len; i++) {
            out->v[i] = indx->v[perm->v[i]];
        }
    } else {
        rock_indx_t *tmp = rock_indx_init(indx->len);

        for (rock_uint_t i = 0; i < indx->len; i++) {
            tmp->v[i] = indx->v[perm->v[i]];
        }
        memcpy(indx->v, tmp->v, sizeof(rock_indx_t)*indx->len);

        rock_indx_free(tmp);
    }

    return ROCK_OK;
}

int
rock_elem_permute(rock_elem_t *elem, rock_perm_t *perm)
{
    return rock_elem_permute_alt(elem, perm, NULL);
}

int
rock_elem_permute_alt(rock_elem_t *elem,
                      rock_perm_t *perm,
                      rock_elem_t *out)
{
    if (out) {
        if (elem->len != out->len) {
            return ROCK_BAD_INPUT;
        }
        for (rock_uint_t i = 0; i < elem->len; i++) {
            out->v[i] = elem->v[perm->v[i]];
        }
    } else {
        rock_elem_t *tmp = rock_elem_init(elem->len);

        for (rock_uint_t i = 0; i < elem->len; i++) {
            tmp->v[i] = elem->v[perm->v[i]];
        }

        memcpy(elem->v, tmp->v, sizeof(rock_elem_t)*elem->len);

        rock_elem_free(tmp);
    }

    return ROCK_OK;
}

int
rock_upkd_pack(rock_desc_t *desc,
               rock_upkd_t *upkd,
               rock_indx_t *indx)
{
    for (rock_uint_t i = 0; i < upkd->len; i++) {
        for (rock_uint_t k = 0; k < desc->order; k++) {
            indx->v[i] |= upkd->v[i*desc->order+k] << desc->bit_offset[k];
        }
    }

    return ROCK_OK;
}


int
rock_indx_unpack(rock_desc_t *desc,
                 rock_indx_t *indx,
                 rock_upkd_t *upkd)
{
    for (rock_uint_t i = 0; i < indx->len; i++) {
        rock_indx_unpack_one(desc, indx->v[i],
                &(upkd->v[i*desc->order]));
    }

    return ROCK_OK;
}

int
rock_indx_unpack_one(rock_desc_t *desc,
                     rock_uint_t indx,
                     rock_uint_t *uint)
{
    for (rock_uint_t i = 0; i < desc->order; i++) {
        uint[i] = (indx & desc->bit_mask[i]) >> desc->bit_offset[i];
    }

    return ROCK_OK;
}

int
rock_part_indx_based(rock_desc_t *desc,
                     rock_part_t *part,
                     rock_indx_t *indx,
                     rock_uint_t dim_num)
{
    if (dim_num >= desc->order) {
        return ROCK_BAD_INPUT;
    }
    if (indx->len < part->num_parts) {
        return ROCK_BAD_INPUT;
    }
    if (desc->dim_size[dim_num] < part->num_parts) {
        return ROCK_BAD_INPUT;
    }

    rock_uint_t proc_total = 0;
    rock_uint_t proc_part = 0;

    rock_uint_t prev, curr;

    rock_uint_t chunk = indx->len / part->num_parts;
    rock_uint_t curr_part = 0;

    part->offset[curr_part] = proc_total;

    while (proc_total < indx->len) {
        curr = rock_indx_extract(desc, indx, proc_total, dim_num);

        if (proc_part >= chunk && prev != curr) {
            part->offset[++curr_part] = proc_total;
            proc_part = 0;
        } else {
            proc_part++;
        }

        proc_total++;
        prev = curr;
    }

    part->offset[++curr_part] = proc_total;

    return ROCK_OK;
}

int
rock_part_desc_based(rock_desc_t *desc,
                     rock_part_t *part,
                     rock_uint_t dim_num)
{
    if (dim_num >= desc->order) {
        return ROCK_BAD_INPUT;
    }
    if (desc->dim_size[dim_num] < part->num_parts) {
        return ROCK_BAD_INPUT;
    }

    rock_uint_t block = desc->dim_size[dim_num] / part->num_parts;
    rock_uint_t overflow = desc->dim_size[dim_num] % part->num_parts;

    for (rock_uint_t i = 0; i < part->num_parts; i++) {
        part->offset[i] = i * block;
        if (i == part->num_parts - 1) {
            part->offset[i+1] = ((i+1) * block) + overflow;
        }
    }

    return ROCK_OK;
}
