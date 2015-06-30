/**
 * @file example_tensor.c
 */

#include "rock.h"

int main(int argc, char **argv)
{
    srand(time(NULL));

    /* Initialize a tensor of arbitrary size. */
    rock_uint_t order = 3;
    rock_uint_t dim_size[] = {10, 300000, 500};
    rock_uint_t nnz = 20; /* Number of non-zero elements. */
    rock_tensor_t *tensor = rock_tensor_init(order, dim_size, nnz);

    /* Sample and shuffle the tensor. */
    rock_tensor_sample(tensor);
    rock_tensor_shuffle(tensor);

    /* Sort it according to dimension 0. */
    rock_tensor_sort(tensor, 0);

    /* Print and cleanup. */
    rock_tensor_print(tensor, "tensor", 4, 2);
    rock_tensor_free(tensor);
}
