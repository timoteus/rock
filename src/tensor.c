/**
 * @file tensor.c
 * @author timoteus <mail@timoteus.se>
 */

#include "tensor.h"
#include "core.h"
#include "sort.h"
#include "disk.h"
#include "random.h"
#include "print.h"

rock_tensor_t *
rock_tensor_init(rock_uint_t order,
                 rock_uint_t *dim_size,
                 rock_uint_t num_elem)
{
    rock_tensor_t *tensor = calloc(1, sizeof(rock_tensor_t));

    tensor->desc = rock_desc_init(order, dim_size);
    tensor->indx = rock_indx_init(num_elem);
    tensor->elem = rock_elem_init(num_elem);

    return tensor;
}

void
rock_tensor_free(rock_tensor_t *tensor)
{
    rock_desc_free(tensor->desc);
    rock_indx_free(tensor->indx);
    rock_elem_free(tensor->elem);
    free(tensor);
}

void
rock_tensor_print(rock_tensor_t *tensor,
                  char *name,
                  int width,
                  int precision)
{
    rock_upkd_t *upkd = rock_upkd_init(tensor->desc, tensor->indx->len);
    rock_indx_unpack(tensor->desc, tensor->indx, upkd);

    for (rock_uint_t i = 0; i < tensor->indx->len; i++) {
        printf("%s", name);
        rock_upkd_print_one(tensor->desc, upkd, i);
        printf(" = %*.*lf;\n", width, precision,
                rock_elem_get(tensor->elem, i));
    }

    rock_upkd_free(upkd);
}

void
rock_tensor_sample(rock_tensor_t *tensor)
{
    rock_indx_sample(tensor->desc, tensor->indx);
    rock_elem_sample(tensor->desc, tensor->elem);
}

void
rock_tensor_shuffle(rock_tensor_t *tensor)
{
    rock_perm_t *perm = rock_perm_init(tensor->indx->len);

    rock_perm_sample(perm);
    rock_indx_permute(tensor->indx, perm);
    rock_elem_permute(tensor->elem, perm);

    rock_perm_free(perm);
}

void
rock_tensor_sort(rock_tensor_t *tensor, rock_uint_t dimension)
{
    rock_perm_t *perm;
    rock_uint_t dims[1] = {dimension};

    perm = rock_perm_init(tensor->indx->len);
    rock_indx_sort(tensor->desc, 1, (rock_uint_t *)dims, perm, tensor->indx);
    rock_elem_permute(tensor->elem, perm);

    rock_perm_free(perm);
}
