/**
 * @file print.h
 * @author timoteus <mail@timoteus.se>
 */

#ifndef _PRINT_H
#define _PRINT_H

#include "core.h"

/**
 * Print a whole tensor descriptor object.
 *
 * @param [in] name
 * @param [in] desc
 */
void
rock_desc_print(char *name, rock_desc_t *desc);

/**
 * Print the binary representation of the @c ith packed multi-index
 * from an array of multi-indices.
 *
 * @param [in] desc
 * @param [in] indx
 * @param [in] i
 */
void
rock_indx_print_one(rock_desc_t *desc,
                    rock_indx_t *indx,
                    rock_uint_t i);

/**
 * Print the binary along with non-binary representation
 * of an index array.
 *
 *
 * @param [in] name
 * @param [in] desc
 * @param [in] indx
 */
void
rock_indx_print(char *name,
                rock_desc_t *desc, 
                rock_indx_t *indx);

/**
 * Print the packed and unpacked indices together with associated
 * element data.
 *
 * This routine unpacks the packed indices in order to print them.
 *
 * @param [in] name
 * @param [in] desc
 * @param [in] indx
 * @param [in] elem
 */
void
rock_indx_print_with_elem(char *name,
                          rock_desc_t *desc,
                          rock_indx_t *indx,
                          rock_elem_t *elem);

/**
 * Print the @c ith element of an array of elements.
 *
 * @param [in] desc
 * @param [in] elem
 * @param [in] i
 */
void
rock_elem_print_one(rock_elem_t *elem, rock_uint_t i);

/**
 * Print the @c ith integer value of an array of unpacked integers.
 *
 * @param [in] desc
 * @param [in] upkd
 * @param [in] i
 */
void
rock_upkd_print_one(rock_desc_t *desc,
                    rock_upkd_t *upkd,
                    rock_uint_t i);

/**
 * Print the @c ith permutation of a permutation object.
 *
 * @param [in] perm
 * @param [in] i
 */
void
rock_perm_print_one(rock_perm_t *perm, rock_uint_t i);

/**
 * Print a whole partition object.
 *
 * @param [in] name
 * @param [in] part
 */
void
rock_part_print(char *name, rock_part_t *part);

#endif
