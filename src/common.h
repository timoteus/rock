/**
 * @file common.h
 * @author timoteus <mail@timoteus.se>
 */

#ifndef _COMMON_H
#define _COMMON_H

#include "config.h"

#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>
#include <omp.h>

#ifdef ROCK_WORD_SIZE_64
    #define ROCK_MAX_ORDER 64
    #define ROCK_UINT_MAX ULONG_MAX
    #define ROCK_UINT_H5T H5T_NATIVE_LONG
    #define ROCK_UINT_MPI MPI_UNSIGNED_LONG
#else
    #ifndef ROCK_WORD_SIZE_32
        #define ROCK_WORD_SIZE_32
    #endif
    #define ROCK_MAX_ORDER 32
    #define ROCK_UINT_MAX UINT_MAX
    #define ROCK_UINT_H5T H5T_NATIVE_INT
    #define ROCK_UINT_MPI MPI_UNSIGNED
#endif

#ifdef ROCK_ELEM_FLOAT
    #define ROCK_ELEM_H5T H5T_NATIVE_FLOAT
    #define ROCK_ELEM_MPI MPI_FLOAT
#else
    #ifndef ROCK_ELEM_DOUBLE
        #define ROCK_ELEM_DOUBLE
    #endif
    #define ROCK_ELEM_H5T H5T_NATIVE_DOUBLE
    #define ROCK_ELEM_MPI MPI_DOUBLE
#endif

#define ROCK_MAX_SHIFT ROCK_MAX_ORDER - 1

#define ROCK_MAX_MESH_ORDER 3
#define ROCK_MASTER 0

#define ROCK_PARALLEL_THRESHOLD 1e5

#define ROCK_USE_DEFAULT -1

#define ROCK_DEFAULT_RADIX_BITS 8

#include "error_codes.h"

#endif
