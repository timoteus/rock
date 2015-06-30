/**
 * @file core.h
 * @author timoteus <mail@timoteus.se>
 *
 * Sparse and incomplete tensors are represented using a coordinate format
 * where each non-zero element is stored with its index and value in
 * separate arrays (@c rock_indx_t and @c rock_elem_t):
 *
 *         "indx"       elem
 *     +---+---+---+  +------+
 *     | 8 | 3 | 0 |  | 0.10 |
 *     +---+---+---+  +------+
 *     | 4 | 1 | 1 |  | 0.20 |
 *     +---+---+---+  +------+
 *     | 0 | 5 | 1 |  | 0.30 |
 *     +---+---+---+  +------+
 *     | 9 | 1 | 0 |  | 0.40 |
 *     +---+---+---+  +------+
 *       0   1   2
 *
 * An element with value 0.10 is located at index 8,3,0 in a corresponding
 * tensor described by a descriptor object (@c rock_desc_t). The descriptor
 * object includes information about the number of dimensions, their sizes
 * and bit-packing information:
 *
 *              tensor               desc
 *
 *      2  +---------------+
 *        /|              /|     order       3
 *       +---------------+ |     dim_size    12, 5, 2
 *       |               | |     bit_width   4, 3, 1
 *     5 |               |-+     bit_offset  0, 4, 7
 *       |               |/
 *       +---------------+
 *              12
 *
 * Each multi-index in the index array are packed using bit fields of
 * either 32 or 64 bits (depending on @c ROCK_WORD_SIZE).
 *
 * Multi-indices can be translated between the packed format and a more
 * traditional format (@c rock_upkd_t) using @c rock_indx_unpack() and
 * @c rock_upkd_pack():
 *
 *               indx                upkd        elem
 *     +---------------------+  +---+---+---+  +------+
 *     | 00000000 0 011 0100 |  | 4 | 3 | 0 |  | 0.10 |
 *     +---------------------+  +---+---+---+  +------+
 *     | 00000000 1 001 0100 |  | 4 | 1 | 1 |  | 0.20 |
 *     +---------------------+  +---+---+---+  +------+
 *     | 00000000 1 101 0000 |  | 0 | 5 | 1 |  | 0.30 |
 *     +---------------------+  +---+---+---+  +------+
 *     | 00000000 0 001 1001 |  | 9 | 1 | 0 |  | 0.40 |
 *     +---------------------+  +---+---+---+  +------+
 *                2  1    0       0   1   2
 */

#ifndef _CORE_H
#define _CORE_H

#include "common.h"

#ifdef ROCK_WORD_SIZE_32
typedef uint32_t rock_uint_t;
#endif
#ifdef ROCK_WORD_SIZE_64
typedef uint64_t rock_uint_t;
#endif

/** A descriptor of a sparse or incomplete tensor. */
typedef struct rock_desc_s
{
    /** The number of dimensions. */
    rock_uint_t order;

    /** The size of each dimension. */
    rock_uint_t dim_size[ROCK_MAX_ORDER];

    /**
     * The product of dim_size. The number of non-zero elements should
     * naturally be less than this.
     */
    rock_uint_t total_size;

    /** The bit field width of each dimension in the bit-packing scheme. */
    rock_uint_t bit_width[ROCK_MAX_ORDER];

    /** The bit field offset of each dimension in the bit-packing scheme. */
    rock_uint_t bit_offset[ROCK_MAX_ORDER];

    /** The bit field masks of each dimension in the bit-packing scheme. */
    rock_uint_t bit_mask[ROCK_MAX_ORDER];

} rock_desc_t;

/** An array of bit-packed multi-indices. */
typedef struct rock_indx_s
{
    /** The length of the array. */
    rock_uint_t len;

    /** The array of packed multi-indices. */
    rock_uint_t *v;

} rock_indx_t;

/** An array of elements (that is, their values). */
typedef struct rock_elem_s
{
    /** The length of the array. */
    rock_uint_t len;

    /** The array of elements. */
#ifdef ROCK_ELEM_DOUBLE
    double *v;
#else
    float *v;
#endif

} rock_elem_t;

/** An array of unpacked unsigned integers (indices). */
typedef struct rock_upkd_s
{
    /** The length of the array. */
    rock_uint_t len;

    /** The array of unsigned integers. */
    rock_uint_t *v;

} rock_upkd_t;

/**
 * Permutation object.
 *
 * Represents a permutation of an index array as a mapping from
 * output to input.
 *
 * Used to permute the element array of an indx/elem array pair
 * after having sorted the index array. Can also be used to
 * "overload" the index array of an indx/elem array pair, see
 * rock_view_t for more information.
 */
typedef struct rock_perm_s
{
    /** The length of the permutation. */
    rock_uint_t len;

    /** The mapping from output to input location. */
    rock_uint_t *v;

} rock_perm_t;

/**
 * Partition object.
 *
 * Represents a partitioning of an array into a number of parts.
 *
 * An array split into three parts:
 *
 *     +-------------+    +-----+-----+-----+
 *     | 0 1 4 5 8 9 | -> | 0 1 | 4 5 | 8 9 |
 *     +-------------+    +-----+-----+-----+
 *                           0     1     2
 */
typedef struct rock_part_s
{
    /** The number of parts. */
    rock_uint_t num_parts;

    /**
     * The start and end of each part. The k-th part starts at offset[k] and
     * ends at offset[k+1]-1 and contains offset[k+1]-offset[k] elements.
     */
    rock_uint_t *offset;

} rock_part_t;

/**
 * Frequency object.
 *
 * Represents the frequency of each index for each dimensions of a tensor.
 */
typedef struct rock_freq_s
{
    /** Reference of a tensor descriptor (not owned). */
    rock_desc_t *desc;

    /** Reference of an index array (not owned). */
    rock_indx_t *indx;

    /** The frequency arrays (one for each dimension). */
    rock_uint_t *dim_freq[ROCK_MAX_ORDER];

} rock_freq_t;

/**
 * View object.
 *
 * Represents multiple views (permutations) into the same tensor
 * (@c desc, @c indx and @c elem).
 *
 * Used to store multiple access-patterns of the same tensor
 * inside one convenient type. For each dimension, the permutation
 * for accessing the tensor sorted according to said dimension is stored.
 */
typedef struct rock_view_s
{
    /** Reference of a tensor descriptor (not owned). */
    rock_desc_t *desc;

    /** Reference of an index array (not owned). */
    rock_indx_t *indx;

    /** The already sorted dimension (equal to @c order if none). */
    rock_uint_t sorted_dim;

    /** One permutation for each dimension of the tensor. */
    rock_perm_t *dim_perm[ROCK_MAX_ORDER];

} rock_view_t;

/**
 * Initialize a tensor descriptor object.
 *
 * @param [in] order        The dimension of the described tensor.
 * @param [in] dim_size     The size of each dimension (values copied).
 * @return                  A pointer to the initialized descriptor object.
 */
rock_desc_t *
rock_desc_init(rock_uint_t order, rock_uint_t *dim_size);

/**
 * Initialize an array of packed multi-indices.
 *
 * @param [in] len          Desired length.
 * @return                  Initialized index array.
 */
rock_indx_t *
rock_indx_init(rock_uint_t len);

/**
 * Initialize an array of data elements.
 *
 * @param [in] len          Desired length.
 * @return                  Initialized elem array.
 */
rock_elem_t *
rock_elem_init(rock_uint_t len);

/**
 * Initialize an array of unpacked unsigned integers.
 *
 * @param [in] desc         A tensor descriptor object.
 * @param [in] len          Desired length.
 * @return                  Initialized upkd array.
 */
rock_upkd_t *
rock_upkd_init(rock_desc_t *desc, rock_uint_t len);

/**
 * Initialize an empty permutation object.
 *
 * @param [in] len          Desired length.
 * @return                  Initialized empty perm object.
 */
rock_perm_t *
rock_perm_init(rock_uint_t len);

/**
 * Initialize an empty partition object.
 *
 * Use @c rock_part_indx_based or @c rock_part_desc_based
 * to populate the initialized partition object.
 *
 * @param [in] num_parts   The number of parts.
 * @return                 Initialized empty part object.
 */
rock_part_t *
rock_part_init(rock_uint_t num_parts);

/**
 * Initialize and populate a frequency object.
 *
 * Populate frequency object with frequencies of each index for each
 * dimension of an index array.
 *
 * @param [in] desc         A tensor descriptor object.
 * @param [in] indx         An index array.
 * @return                  Intialized and populated freq object.
 */
rock_freq_t *
rock_freq_init(rock_desc_t *desc, rock_indx_t *indx);

/**
 * Initialize and populate a view object.
 *
 * Populate view object by creating permutations for each dimension.
 *
 * The descriptor and index objects are referenced, not copied. The
 * passed index array is not permuted.
 *
 * For each dimension, the permutation for accessing the tensor sorted
 * according to said dimension is calculated and stored in @c dim_perm.
 *
 * @param [in] desc         Descriptor object to associate with view.
 * @param [in] indx         Index array to associate with view.
 * @param [in] sorted_dim   Already sorted dimension, equal to $c order
 *                          if none (or unknown).
 * @return                  Initialized and populated view object.
 */
rock_view_t *
rock_view_init(rock_desc_t *desc,
               rock_indx_t *indx,
               rock_uint_t sorted_dim);

/**
 * Free a tensor descriptor object.
 *
 * @param [in] desc
 */
void
rock_desc_free(rock_desc_t *desc);

/**
 * Free an array of packed indices.
 *
 * @param [in] indx
 */
void
rock_indx_free(rock_indx_t *indx);

/**
 * Free an array of data elements.
 *
 * @param [in] elem
 */
void
rock_elem_free(rock_elem_t *elem);

/**
 * Free an array of unpacked unsigned integers.
 *
 * @param [in] upkd
 */
void
rock_upkd_free(rock_upkd_t *upkd);

/**
 * Free a permutation object.
 *
 * @param [in] perm
 */
void
rock_perm_free(rock_perm_t *perm);

/**
 * Free a partition object.
 *
 * @param [in] part
 */
void
rock_part_free(rock_part_t *part);

/**
 * Free a frequency object.
 *
 * Associated objects should be free'd separately.
 *
 * @param [in] freq
 */
void
rock_freq_free(rock_freq_t *freq);

/**
 * Free a view object.
 *
 * Associated objects should be free'd separately.
 *
 * @param [in] view
 */
void
rock_view_free(rock_view_t *view);

/**
 * Duplicate an index array.
 *
 * @param [in] indx
 * @return                  Duplicated index array.
 */
rock_indx_t *
rock_indx_copy(rock_indx_t *indx);

/**
 * Duplicate an elem array.
 *
 * @param [in] elem
 * @return                  Duplicated elem array.
 */
rock_elem_t *
rock_elem_copy(rock_elem_t *elem);

/**
 * Compare two index arrays for equality.
 *
 * @param [in] p1
 * @param [in] p2
 * @return                  @c true if equal, @c false if not.
 */
bool
rock_indx_eq(rock_indx_t *p1, rock_indx_t *p2);

/**
 * Compare two elem arrays for equality.
 *
 * @param [in] p1
 * @param [in] p2
 * @return                  @c true if equal, @c false if not.
 */
bool
rock_elem_eq(rock_elem_t *p1, rock_elem_t *p2);

/**
 * Compare two upkd arrays for equality.
 *
 * @param [in] p1
 * @param [in] p2
 * @return                  @c true if equal, @c false if not.
 */
bool
rock_upkd_eq(rock_upkd_t *p1, rock_upkd_t *p2);

/**
 * Compare two perm objects for equality.
 *
 * @param [in] p1
 * @param [in] p2
 * @return                  @c true if equal, @c false if not.
 */
bool
rock_perm_eq(rock_perm_t *p1, rock_perm_t *p2);

/**
 * Swap two unsigned integers.
 *
 * @param [in,out] p1
 * @param [in,out] p2
 */
void
rock_uint_swap(rock_uint_t *p1, rock_uint_t *p2);

/**
 * Swap the references of two arrays of multi-indices.
 *
 * @param [in,out] p1
 * @param [in,out] p2
 */
void
rock_indx_swap(rock_indx_t **p1, rock_indx_t **p2);

/**
 * Swap the references of two arrays of permutations.
 *
 * @param [in,out] p1
 * @param [in,out] p2
 */
void
rock_perm_swap(rock_perm_t **p1, rock_perm_t **p2);

/**
 * Apply a permutation to an index array.
 *
 * @param [in,out] indx     The index array to permute.
 * @param [in] perm         The permutation to apply.
 */
int
rock_indx_permute(rock_indx_t *indx, rock_perm_t *perm);

/**
 * Apply a permutation to an index array.
 *
 * @param [in] indx         The index array to permute.
 * @param [in] perm         The permutation to apply.
 * @param [out] out         The permuted index array.
 */
int
rock_indx_permute_alt(rock_indx_t *indx,
                      rock_perm_t *perm,
                      rock_indx_t *out);

/**
 * Applies a permutation to an element array.
 *
 * @param [in,out] elem     The elem array to permute.
 * @param [in] perm         The permutation to apply.
 */
int
rock_elem_permute(rock_elem_t *elem, rock_perm_t *perm);

/**
 * Apply a permutation to an element array.
 *
 * @param [in] elem         The elem array to permute.
 * @param [in] perm         The permutation to apply.
 * @param [out] out         The permuted elem array.
 */
int
rock_elem_permute_alt(rock_elem_t *elem,
                      rock_perm_t *perm,
                      rock_elem_t *out);

/**
 * Turn an unpacked array of multi-indices into its packed representation.
 *
 * @param [in] desc         A tensor descriptor object.
 * @param [in] upkd         An unpacked array of multi-indices.
 * @param [out] indx        The resulting packed array of multi-indices.
 */
int
rock_upkd_pack(rock_desc_t *desc,
               rock_upkd_t *upkd,
               rock_indx_t *indx);

/**
 * Unpack an array of packed multi-indices.
 *
 * @param [in] desc         A tensor descriptor object.
 * @param [in] indx         A packed array of multi-indices.
 * @param [out] upkd        The resulting unpacked array of multi-indices.
 */
int
rock_indx_unpack(rock_desc_t *desc,
                 rock_indx_t *indx,
                 rock_upkd_t *upkd);

/**
 * Unpack a single packed multi-index into a order-sized array.
 *
 * @param [in] desc         A tensor descriptor object.
 * @param [in] indx         A packed multi-index.
 * @param [out] uint        An array of size @c desc->order where
 *                          the unpacked indices are stored.
 */
int
rock_indx_unpack_one(rock_desc_t *desc,
                     rock_uint_t indx,
                     rock_uint_t *uint);

/**
 * Partition one dimension of a tensor based on an index array.
 *
 * Each specific index is guaranteed to not be split among
 * different parts. This means the resulting parts may not
 * be of equal length.
 *
 *                indx
 *     +-----+--------+-----+----+
 *     |*****|********|*****|****|   part->num_parts = 4
 *     +-----+--------+-----+----+
 *
 * The number of parts of the partition object must be less
 * than the size of the dimension and less than the length
 * of the index array.
 *
 * @param [in] desc         A tensor descriptor ojbect.
 * @param [in,out] part     The partition object to fill.
 * @param [in] indx         The index array to base partition on.
 * @param [in] dim_num      The dimension to partition.
 */
int
rock_part_indx_based(rock_desc_t *desc,
                     rock_part_t *part,
                     rock_indx_t *indx,
                     rock_uint_t dim_num);

/**
 * Partition a dimension of a tensor based on its descriptor.
 *
 * Note that this routine partitions solely on tensor *space* compared
 * to actual indices, it may result in a skewed distribution (if non-zero
 * elements aren't evenly distributed in the tensor).
 *
 *         tensor space
 *     +----+----+----+----+
 *     | *  |    |    | *  |
 *     |    |    |    |    |     desc->order = 2
 *     |    |    |    |   *|     part->num_pars = 4
 *     |    |    | *  |    |
 *     |    |    |    |*   |     * denotes multi-indices visualized
 *     |    | *  |    |    |     in the tensor space described by desc
 *     |    |    |    |    |
 *     +----+----+----+----+
 *
 * @param [in] desc         Tensor space to base partition on.
 * @param [in,out] part     An already initialized partition object.
 * @param [in] dim_num      The dimension to partition.
 */
int
rock_part_desc_based(rock_desc_t *desc,
                     rock_part_t *part,
                     rock_uint_t dim_num);

/**
 * Get the @c ith multi-index from a multi-index array.
 *
 * @param [in] indx
 * @param [in] i
 * @return                  @c ith multi-index.
 */
static inline rock_uint_t
rock_indx_get(rock_indx_t *indx, rock_uint_t i)
{
    return indx->v[i];
}

/**
 * Get the @c ith integer from an array of unsigned integers.
 *
 * @param [in] upkd
 * @param [in] i
 * @return                  @c ith unsigned integer.
 */
static inline rock_uint_t
rock_upkd_get(rock_upkd_t *upkd, rock_uint_t i)
{
    return upkd->v[i];
}

/**
 * Get the @c ith element from an array of elements.
 *
 * @param [in] elem
 * @param [in] i
 * @return                  @c ith element.
 */
#ifdef ROCK_ELEM_DOUBLE
static inline double
#else
static inline float
#endif
rock_elem_get(rock_elem_t *elem, rock_uint_t i)
{
    return elem->v[i];
}

/**
 * Set the @c ith multi-index of a multi-index array.
 *
 * @param [in,out] indx
 * @param [in] i
 * @param [in] val          Bit-packed formatted value
 */
static inline void
rock_indx_set(rock_indx_t *indx,
              rock_uint_t i,
              rock_uint_t val)
{
    indx->v[i] = val;
}

/**
 * Set the @c ith integer of an array of integers.
 *
 * @param [in,out] upkd
 * @param [in] i
 * @param [in] val
 */
static inline void
rock_upkd_set(rock_upkd_t *upkd,
              rock_uint_t i,
              rock_uint_t val)
{
    upkd->v[i] = val;
}

/**
 * Set the @c ith element of an array of elements.
 *
 * @param [in,out] elem
 * @param [in] i
 * @param [in] val
 */
static inline void
rock_elem_set(rock_elem_t *elem,
              rock_uint_t i,
#ifdef ROCK_ELEM_DOUBLE
              double val
#else
              float val
#endif
              )
{
    elem->v[i] = val;
}

/**
 * Extract value of a specific dimension of a packed multi-index from
 * an array of multi-indices.
 *
 * @param [in] desc         Tensor descriptor object.
 * @param [in] indx         An array of multi-indices.
 * @param [in] i            The index to fetch.
 * @param [in] dim          The dimension to extract.
 * @return                  Specific index of a multi-index tuple.
 */
static inline rock_uint_t
rock_indx_extract(rock_desc_t *desc,
                  rock_indx_t *indx,
                  rock_uint_t i,
                  rock_uint_t dim)
{
    return (indx->v[i] & desc->bit_mask[dim]) >> desc->bit_offset[dim];
}

/**
 * Extract value of a specific dimension of an integer-tuple from an array
 * of unpacked integers.
 *
 * @param [in] desc         Tensor descriptor object.
 * @param [in] upkd         An array of unpacked integers.
 * @param [in] i            The index to fetch.
 * @param [in] dim          The dimension to extract.
 * @return                  Specific integer of an integer-tuple.
 */
static inline rock_uint_t
rock_upkd_extract(rock_desc_t *desc,
                  rock_upkd_t *upkd,
                  rock_uint_t i,
                  rock_uint_t dim)
{
    return upkd->v[i * desc->order + dim];
}

/**
 * Insert value for specific dimension of a packed multi-index from
 * an array of multi-indices.
 *
 * @param [in] desc         Tensor descriptor object.
 * @param [in,out] indx     Array to manipulate.
 * @param [in] i            Multi-index to insert to.
 * @param [in] dim          Dimension to overwrite.
 * @param [in] val          Value to overwrite with.
 */
static inline void
rock_indx_insert(rock_desc_t *desc,
                 rock_indx_t *indx,
                 rock_uint_t i,
                 rock_uint_t dim,
                 rock_uint_t val)
{
    rock_indx_set(indx, i, ((indx->v[i] & (~desc->bit_mask[dim])) |
            (val << desc->bit_offset[dim])));
}

/**
 * Insert value for specific dimension of an integer-tuple from
 * an array of unpacked integers.
 *
 * @param [in] desc         Tensor descriptor object.
 * @param [in,out] upkd     Array to manipulate.
 * @param [in] i            Integer-tuple to insert to.
 * @param [in] dim          Dimension to overwrite.
 * @param [in] val          Value to overwrite with.
 */
static inline void
rock_upkd_insert(rock_desc_t *desc,
                 rock_upkd_t *upkd,
                 rock_uint_t i,
                 rock_uint_t dim,
                 rock_uint_t val)
{
    upkd->v[i * desc->order + dim] = val;
}

#endif
