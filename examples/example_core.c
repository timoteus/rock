/**
 * @file example_core.c
 */

#include "rock.h"

int main(int argc, char **argv)
{
    srand(time(NULL));

    /* Create a descriptor for the tensor described in core.h. */
    rock_uint_t order = 3;
    rock_uint_t dim_size[] = {12, 5, 2};
    rock_desc_t *desc = rock_desc_init(order, dim_size);

    /* Load the test data described in core.h. */
    rock_uint_t nnz = 4; /* Number of non-zero elements. */
    rock_indx_t *indx = rock_indx_init(nnz);
    rock_elem_t *elem = rock_elem_init(nnz);
    rock_indx_load(indx, "examples/data/indx_example_core.hdf5");
    rock_elem_load(elem, "examples/data/elem_example_core.hdf5");

    /* Print loaded data. */
    rock_desc_print("descriptor", desc);
    rock_indx_print_with_elem("loaded data", desc, indx, elem);

    /* Sort the packed indices according to dimensions 0 and 1. */
    rock_uint_t num_dims = 2;
    rock_uint_t dims[] = {0,1};
    rock_perm_t *perm = rock_perm_init(nnz);
    rock_indx_sort(desc, num_dims, dims, perm, indx);

    /* Permute the data elements using the achieved permutation. */
    rock_elem_permute(elem, perm);

    /* Print the sorted data. */
    rock_indx_print_with_elem("sorted data", desc, indx, elem);

    /* Cleanup. */
    rock_desc_free(desc);
    rock_indx_free(indx);
    rock_elem_free(elem);
    rock_perm_free(perm);
}
