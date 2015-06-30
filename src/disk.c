/**
 * @file disk.c
 * @author timoteus <mail@timoteus.se>
 */

#include "disk.h"
#include "hdf5.h"
#include "hdf5_hl.h"

int
rock_indx_save(rock_indx_t *indx, char *fname)
{
    hid_t file_id;
    herr_t status;
    hsize_t dims[] = {indx->len};

    file_id = H5Fcreate (fname, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    status = H5LTmake_dataset(file_id, "/indx", 1, dims, ROCK_UINT_H5T, indx->v);
    if (status) {
        return ROCK_ERR;
    }
    status = H5Fclose (file_id);
    if (status) {
        return ROCK_ERR;
    }

    return ROCK_OK;
}

int
rock_indx_load(rock_indx_t *indx, char *fname)
{
    hid_t file_id;
    herr_t status;

    file_id = H5Fopen(fname, H5F_ACC_RDONLY, H5P_DEFAULT);
#ifdef ROCK_WORD_SIZE_64
    status = H5LTread_dataset_long(file_id, "/indx", (long *)indx->v);
#else
    status = H5LTread_dataset_int(file_id, "/indx", (int *)indx->v);
#endif
    if (status) {
        return ROCK_ERR;
    }
    status = H5Fclose (file_id);
    if (status) {
        return ROCK_ERR;
    }

    return ROCK_OK;
}

int
rock_elem_save(rock_elem_t *elem, char *fname)
{
    hid_t file_id;
    herr_t status;
    hsize_t dims[] = {elem->len};

    file_id = H5Fcreate (fname, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    status = H5LTmake_dataset(file_id, "/elem", 1, dims, ROCK_ELEM_H5T, elem->v);
    if (status) {
        return ROCK_ERR;
    }
    status = H5Fclose (file_id);
    if (status) {
        return ROCK_ERR;
    }

    return ROCK_OK;
}

int
rock_elem_load(rock_elem_t *elem, char *fname)
{
    hid_t file_id;
    herr_t status;

    file_id = H5Fopen(fname, H5F_ACC_RDONLY, H5P_DEFAULT);
#ifdef ROCK_ELEM_FLOAT
    status = H5LTread_dataset_float(file_id, "/elem", elem->v);
#else
    status = H5LTread_dataset_double(file_id, "/elem", elem->v);
#endif
    if (status) {
        return ROCK_ERR;
    }
    status = H5Fclose (file_id);
    if (status) {
        return ROCK_ERR;
    }

    return ROCK_OK;
}

int
rock_upkd_save(rock_upkd_t *upkd, char *fname)
{
    hid_t file_id;
    herr_t status;
    hsize_t dims[] = {upkd->len};

    file_id = H5Fcreate (fname, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    status = H5LTmake_dataset(file_id, "/upkd", 1, dims, ROCK_UINT_H5T, upkd->v);
    if (status) {
        return ROCK_ERR;
    }
    status = H5Fclose (file_id);
    if (status) {
        return ROCK_ERR;
    }

    return ROCK_OK;
}

int
rock_upkd_load(rock_upkd_t *upkd, char *fname)
{
    hid_t file_id;
    herr_t status;

    file_id = H5Fopen(fname, H5F_ACC_RDONLY, H5P_DEFAULT);
#ifdef ROCK_WORD_SIZE_64
    status = H5LTread_dataset_long(file_id, "/upkd", (long *)upkd->v);
#else
    status = H5LTread_dataset_int(file_id, "/upkd", (int *)upkd->v);
#endif
    if (status) {
        return ROCK_ERR;
    }
    status = H5Fclose (file_id);
    if (status) {
        return ROCK_ERR;
    }

    return ROCK_OK;
}

int
rock_perm_save(rock_perm_t *perm, char *fname)
{
    hid_t file_id;
    herr_t status;
    hsize_t dims[] = {perm->len};

    file_id = H5Fcreate (fname, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    status = H5LTmake_dataset(file_id, "/perm", 1, dims, ROCK_UINT_H5T, perm->v);
    if (status) {
        return ROCK_ERR;
    }
    status = H5Fclose (file_id);
    if (status) {
        return ROCK_ERR;
    }

    return ROCK_OK;
}

int
rock_perm_load(rock_perm_t *perm, char *fname)
{
    hid_t file_id;
    herr_t status;

    file_id = H5Fopen(fname, H5F_ACC_RDONLY, H5P_DEFAULT);
#ifdef ROCK_WORD_SIZE_64
    status = H5LTread_dataset_long(file_id, "/perm", (long *)perm->v);
#else
    status = H5LTread_dataset_int(file_id, "/perm", (int *)perm->v);
#endif
    if (status) {
        return ROCK_ERR;
    }
    status = H5Fclose (file_id);
    if (status) {
        return ROCK_ERR;
    }

    return ROCK_OK;
}
