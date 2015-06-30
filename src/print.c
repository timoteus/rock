/**
 * @file print.c
 * @author timoteus <mail@timoteus.se>
 */

#include "print.h"

void
rock_byte_print(int s, void *p)
{
    for (int i = s - 1; i >= 0; i--) {
        for (int k = 7; k >= 0; k--) {
            printf("%u", (*((unsigned char *)p+i) & (1 << k)) >> k);
        }
    }
}

void
rock_desc_print(char *name, rock_desc_t *desc)
{
    printf("%s\n", name);
    printf("    order       %" PRIu32 "\n", desc->order);
    printf("    total_size  %" PRIu32 "\n", desc->total_size);
    printf("    dim_size    ");
    for (rock_uint_t i = 0; i < desc->order; i++) {
        printf("%" PRIu32 " ", desc->dim_size[i]);
    }
    printf("\n");

    printf("    bit_width   ");
    for (rock_uint_t i = 0; i < desc->order; i++) {
        printf("%" PRIu32 " ", desc->bit_width[i]);
    }
    printf("\n");

    printf("    bit_offset  ");
    for (rock_uint_t i = 0; i < desc->order; i++) {
        printf("%" PRIu32 " ", desc->bit_offset[i]);
    }
    printf("\n");

    printf("    bit_mask");
    for (rock_uint_t i = 0; i < desc->order; i++) {
        if (i == 0) {
            printf("    ");
        } else {
            printf("                ");
        }
        if (desc->bit_mask[i]) {
            rock_byte_print(sizeof(desc->bit_mask[i]),
                    &(desc->bit_mask[i]));
        } else {
            printf(" NULL");
        }
        printf("\n");
    }
}

void
rock_indx_print_one(rock_desc_t *desc,
                    rock_indx_t *indx,
                    rock_uint_t i)
{
    void *p = &(indx->v[i]);
    rock_uint_t s = sizeof(rock_uint_t);
    rock_uint_t c = s*8 - 1;

    for (int j = s - 1; j >= 0; j--) {
        for (int k = 7; k >= 0; k--) {
            printf("%" PRIu32 "", (*((unsigned char *)p+j) & (1 << k)) >> k);
            for (int l = 0; l < desc->order; l++) {
                if (c == desc->bit_offset[l]) {
                    printf(" ");
                }
            }
            if (c == ROCK_MAX_ORDER) {
                printf(" | ");
            }
            if (c == (desc->bit_offset[desc->order-1]
                        + desc->bit_width[desc->order-1])) {
                printf(" ");
            }
            c--;
        }
    }
}

void
rock_indx_print(char *name,
                rock_desc_t *desc, 
                rock_indx_t *indx)
{
    rock_upkd_t *upkd = rock_upkd_init(desc, indx->len);
    rock_indx_unpack(desc, indx, upkd);

    printf("%s\n", name);
    for (rock_uint_t i = 0; i < indx->len; i++) {
        rock_indx_print_one(desc, indx, i);
        printf("= ");
        rock_upkd_print_one(desc, upkd, i);
        printf("\n");
    }

    rock_upkd_free(upkd);
}

void
rock_indx_print_with_elem(char *name,
                          rock_desc_t *desc,
                          rock_indx_t *indx,
                          rock_elem_t *elem)
{
    rock_upkd_t *upkd = rock_upkd_init(desc, indx->len);
    rock_indx_unpack(desc, indx, upkd);

    printf("%s\n", name);
    for (rock_uint_t i = 0; i < indx->len; i++) {
        rock_indx_print_one(desc, indx, i);
        printf("= ");
        rock_upkd_print_one(desc, upkd, i);
        printf(" -> ");
        rock_elem_print_one(elem, i);
        printf("\n");
    }

    rock_upkd_free(upkd);
}

void
rock_elem_print_one(rock_elem_t *elem, rock_uint_t i)
{
    printf("%f ", elem->v[i]);
}

void
rock_upkd_print_one(rock_desc_t *desc,
                    rock_upkd_t *upkd,
                    rock_uint_t i)
{
    printf("(");
    for (rock_uint_t k = 0; k < desc->order; k++) {
        if (k > 0) {
            printf(", ");
        }
        printf("%llu", (long long unsigned int)upkd->v[(i * desc->order) + k]);
    }
    printf(")");
}

void
rock_perm_print_one(rock_perm_t *perm, rock_uint_t i)
{
    printf("%d ", perm->v[i]);
}

void
rock_part_print(char *name, rock_part_t *part)
{
    printf("%s\n", name);
    printf("    num_parts   %" PRIu32 "\n", part->num_parts);
    printf("    offset      ");

    for (rock_uint_t i = 0; i < part->num_parts + 1; i++) {
        printf("%" PRIu32 " ", part->offset[i]);
    }
    printf("\n");
}
