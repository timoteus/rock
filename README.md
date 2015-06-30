ROCK
====

[![Build Status](https://travis-ci.org/timoteus/rock.svg?branch=master)](https://travis-ci.org/timoteus/rock)

**rock** is a high-performance library for representing and manipulating *sparse tensors*, i.e., multi-dimensional numerical arrays where most elements are equal to zero. A compact bit field representation of the tensor indices reduces the memory requirements and speeds up sorting of the tensor elements. A special purpose radix sorting algorithm allows for up to 20 times faster sorting compared to `qsort` in the C standard library. The primary features of the **rock** library is to sort, partition, distribute, and store sparse and incomplete tensors.

Introduction
------------

#### History

The precursor of the library was written by Timoteus Dahlberg as part of a Bachelor Thesis in Computing Science at Umeå University called [*Compact Representation and Efficient Manipulation of Sparse Multidimensional Arrays*](http://www8.cs.umu.se/education/examina/Rapporter/TimoteusDahlberg.pdf). The thesis covers the sorting operation and the compact representation scheme in detail together with performance evaluations of different implementation techniques. The **rock** library is an improved version with more functionality and a refined API.


#### Purpose

High-performance tensor computations is a relatively new field. Applications and algorithms exist but few are implemented with high performance and parallel scalability in mind. And those that are tend to use ad hoc implementations instead of promoting a reusable software stack similar to BLAS, LAPACK, and ScaLAPACK in the field of matrix computations.

We think that the field of high-performance tensor computations can benefit by thinking in terms of layers of separate concerns:

1. The *application* layer: Unique features of the application such as pre- and post-processing.
2. The *algorithm* layer: Tensor algoritms such as alternating least squares.
3. The *kernel* layer: Tensor kernels such as contractions.
4. The *distribution* layer: Distribution of tensors on a distributed memory machine.
5. The *storage* layer: Tensor storage formats, conversions, and file I/O.

The **rock** library provides a comprehensive solution to the *storage* layer for sparse and incomplete tensors as well as a currently partial solution to the *distribution* layer for sparse and incomplete tensors.

#### Design

Sparse tensors are represented in *coordinate format*, which means that every non-zero element is represented together with its index tuple. Furthermore, the index tuples are packed into either 32 or 64 bits using bit fields to compress the data and greatly speed up sorting operations.

The library is primarily designed for high performance and flexibility, which can make it a little bit difficult to get used to. In particular, the tensor elements are represented separately from the tensor indices. This makes it possible, for example, to share one tensor index array across several sparse tensors that have the same sparsity pattern, which saves a significant amount of memory as well as reduces the time spent sorting indices.

#### Main features

Two of the main features of **rock** are its capabilities for *sorting* and *partitioning*.

*Sorting* means to permute the tensor indices and/or elements in lexicographic ordering across all or a subset of the tensor dimensions. By arranging the tensor indices in a particular order, tensor algorithms such as alternating least squares can be made more efficient by, for example, requiring less memory and/or using the memory hierarchy more effectively. Sorting the tensor indices also makes it possible to partition tensors for parallelism, see below.

*Partitioning* means to split the tensor indices and/or elements into disjoint subsets. A primary use for partitioning is to enable parallelism in tensor algorithms.

#### Why is it called "rock"?

Since the library is all about sparse and incomplete tensors, it was originally called **sten** for **s**parse **ten**sors. However, since the author is Swedish, and *sten* is a proper Swedish word, the name felt awkward to use in an English context. The solution was to just translate it into English, hence the name **rock**.

#### References

* [Compact Representation and Efficient Manipulation of Sparse Multidimensional Arrays](http://www8.cs.umu.se/education/examina/Rapporter/TimoteusDahlberg.pdf) (Bachelor Thesis in Computing Science)
* [Tensor Decompositions and Applications](http://epubs.siam.org/doi/abs/10.1137/07070111X)
* [Efficient MATLAB Computations with Sparse and Factored Tensors](http://epubs.siam.org/doi/abs/10.1137/060676489)

Getting started
---------------

An incomplete list of dependencies:

- [OpenMP](http://openmp.org/wp/) for shared memory parallelization.
- An implementation of the [Message Passing Interface](http://en.wikipedia.org/wiki/Message_Passing_Interface), e.g., [Open MPI](http://www.open-mpi.org/) for distributed memory parallelization.
- [HDF5](http://www.hdfgroup.org/HDF5/) for saving and loading data to disk.

An out-of-source build can be generated using [`cmake`](http://www.cmake.org/) as follows:

    $ mkdir build
    $ cd build
    $ cmake ..

Use `ccmake` instead of `cmake` if you want the opportunity to configure the build using a number of options.

Continue by running the generated Makefile:

    $ make

Run tests using:

    $ make test

Examples are by default generated and put under `examples/` in the build directory. Run the most convoluted example using, e.g., `mpirun` (depending on your installation) with 16 threads:

    $ mpirun -np 16 examples/example_distribute

Or a simpler one (that replicates the tensor described in [`core.h`](src/core.h)):

    $ ./examples/example_core

Documentation can be generated using [`doxygen`](http://www.stack.nl/~dimitri/doxygen/) and the provided configuration:

    $ doxygen doxygen.config

Point your browser at `doc/html/index.html` and go crazy.

Configuration
-------------

**rock** offers a number of configuration options.

#### Number of threads and radix bits

Use external variables to manually specify the number of threads to use for shared memory parallelization and the number of bits to maximally process each pass by the custom radix sort:

    extern int rock_num_threads;
    extern int rock_radix_bits;

These variables are declared in [`sort.h`](src/sort.h) and can be overridden by defining them (e.g., see [`test_sort.c`](tests/test_sort.c)).

#### Elemental precision
Double precision of tensor elements can be switched off to save memory using `ccmake`.

#### 32/64-bit tensor indices
Support for 64-bit tensor indices, in order to handle larger tensors than 32-bits are able to represent, is partially supported but still highly experimental. It can be switched on using `ccmake` but beware of dragons.

Compatibility
-------------

**rock** should be able to run on most Unix-based systems including OS X without too much hassle.

Tested on Ubuntu 64-bit 12.04 LTS and 14.04 LTS using gcc version 4.6.3 and 4.8.4 respectively.

Examples
--------

Using the high-level tensor type ([`rock_tensor_t`](src/tensor.h)):

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

Using the lower-level types directly (see [`core.h`](src/core.h) for more information):

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

Distributing a tensor among a mesh of processors:

    /**
     * @file example_distribute.c
     */

    #include "rock.h"

    int main(int argc, char **argv)
    {
        srand(time(NULL));

        MPI_Init(&argc, &argv);

        rock_indx_t *indx = NULL;
        rock_elem_t *elem = NULL;
        rock_desc_t *desc = NULL;
        rock_mpart_t *mpart = NULL;

        /* Mesh setup. */
        rock_uint_t proc_order = 3; /* 3D mesh. */
        rock_uint_t proc_dims[] = {2, 4, 2}; /* 16 processors. */
        rock_mesh_t *mesh = rock_mesh_init(MPI_COMM_WORLD, proc_order, proc_dims);

        if (mesh->rank == ROCK_MASTER) {

            /* Tensor setup */
            rock_uint_t order = 3;
            rock_uint_t nnz = 1e6; /* Number of non-zero elements. */
            rock_uint_t dim_size[] = {300, 2, 25000};
            desc = rock_desc_init(order, dim_size);

            /** Initialize multi-partition that matches processor mesh. */
            mpart = rock_mpart_init(mesh->order, mesh->dim_size);

            /* Create partition based on tensor space. */
            rock_uint_t num_part_dims = 1;
            rock_uint_t part_dims[] = {1}; /* Start with 1 then use largest. */
            rock_mpart_desc_based(desc, mpart, num_part_dims, part_dims);

            /* Allocate index and elem arrays at master. */
            elem = rock_elem_init(nnz);
            indx = rock_indx_init(nnz);

            /* Sample index and elements at master. */
            rock_elem_sample(desc, elem);
            rock_indx_sample(desc, indx);
        }

        /* SCATTER */

        /* Use multi-partition to distribute tensor over processor mesh. */
        rock_dist_t *dist = rock_dist_init(indx, elem, mpart, mesh);

        /* Scatter indices. */
        rock_indx_scatter(&indx, dist);

        /* Scatter elements. */
        rock_elem_scatter(&elem, dist);

        /* COMPUTE */

        /* <Perform distributed computations> */

        /* GATHER */

        /* Gather indices. */
        rock_indx_gather(&indx, dist);

        /* Gather elements. */
        rock_elem_gather(&elem, dist);

        /* CLEANUP */

        MPI_Barrier(mesh->comm);

        if (mesh->rank == ROCK_MASTER) {
            rock_desc_free(desc);
            rock_mpart_free(mpart);
            rock_indx_free(indx);
            rock_elem_free(elem);
        }

        rock_mesh_free(mesh);
        rock_dist_free(dist);

        MPI_Finalize();
    }

License
-------
This library is free software and may be redistributed and modified under the terms of the MIT-license. See [LICENSE](LICENSE) for details.
