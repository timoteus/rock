/**
 * @file disk.h
 * @author timoteus <mail@timoteus.se>
 */

#ifndef _DISK_H
#define _DISK_H

#include "core.h"

/**
 * Save an array of packed multi-indices to disk.
 *
 * @param [in] indx
 * @param [in] fname
 * @return                  ROCK_OK or ROCK_ERR.
 */
int
rock_indx_save(rock_indx_t *indx, char *fname);

/**
 * Load an array of previously saved packed multi-indices.
 *
 * @param [out] indx        An already initialized index object.
 * @param [in] fname
 * @return                  ROCK_OK or ROCK_ERR.
 */
int
rock_indx_load(rock_indx_t *indx, char *fname);

/**
 * Save an array of data elements.
 *
 * @param [in] elem
 * @param [in] fname
 * @return                  ROCK_OK or ROCK_ERR.
 */
int
rock_elem_save(rock_elem_t *elem, char *fname);

/**
 * Load an array of previously saved data elements.
 *
 * @param [out] elem        An already initialized elem object.
 * @param [in] fname
 * @return                  ROCK_OK or ROCK_ERR.
 */
int
rock_elem_load(rock_elem_t *elem, char *fname);

/**
 * Save an array of unpacked unsigned integers.
 *
 * @param [in] upkd
 * @param [in] fname
 * @return                  ROCK_OK or ROCK_ERR.
 */
int
rock_upkd_save(rock_upkd_t *upkd, char *fname);

/**
 * Load an array of previously saved unpacked unsigned integers.
 *
 * @param [out] upkd        An already initialized upkd object.
 * @param [in] fname
 * @return                  ROCK_OK or ROCK_ERR.
 */
int
rock_upkd_load(rock_upkd_t *upkd, char *fname);

/**
 * Save a permutation object.
 *
 * @param [in] perm
 * @param [in] fname
 * @return                  ROCK_OK or ROCK_ERR.
 */
int
rock_perm_save(rock_perm_t *perm, char *fname);

/**
 * Load a previously saved permutation object
 *
 * @param [out] perm        An already initialized perm object.
 * @param [in] fname
 * @return                  ROCK_OK or ROCK_ERR.
 */
int
rock_perm_load(rock_perm_t *perm, char *fname);

#endif
