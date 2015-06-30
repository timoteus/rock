cmake_minimum_required(VERSION 2.8)
project(rock C)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")

include(CheckFunctionExists)
find_program(MEMORYCHECK_COMMAND valgrind)
set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --suppressions=valgrind.supp --leak-check=full --error-exitcode=1" )
include(CTest)

if(NOT DEFINED CMAKE_MACOSX_RPATH)
    set(CMAKE_MACOSX_RPATH 0)
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DROCK_DEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -funroll-all-loops -DROCK_RELEASE")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

mark_as_advanced(HDF5_DIR)

# Configuration options
set(ROCK_WORD_SIZE_64 OFF CACHE BOOL
    "Use 64-bit tensor indices (here be dragons, highly experimental).")
set(ROCK_ELEM_FLOAT OFF CACHE BOOL
    "Use single precision for tensor elements.")

set(BUILD_EXAMPLES ON CACHE BOOL "Build examples")
set(BUILD_BENCHMARKS ON CACHE BOOL "Build benchmarks")

# Generate a configuration header (included by common.h), as opposed
# to preprocessing rock.h directly. This avoids having to include rock.h
# internally (and makes it easier for non cmake users to manually edit).
configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)

# Find and incorporate dependecies

# MPI
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

# OpenMP
find_package(OpenMP REQUIRED)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")

# HDF5
find_package(HDF5 COMPONENTS HL REQUIRED)
include_directories(${HDF5_INCLUDE_DIRS})
link_directories(${HDF5_LIBRARY_DIRS})

# Add rock build (manually add source files as per CMake recommendation)
add_library(rock SHARED src/core.c src/random.c src/print.c src/sort.c 
                        src/tensor.c src/disk.c src/distribute.c config.h)

# Add custom target (check) that prints output for failed tests
if(CMAKE_CONFIGURATION_TYPES)
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
        --force-new-ctest-process --output-on-failure
        --build-config "$<CONFIGURATION>")
else()
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
        --force-new-ctest-process --output-on-failure)
endif()

# Link and set library flags
target_link_libraries(rock ${HDF5_LIBRARIES})
target_link_libraries(rock ${MPI_LIBRARIES})
if(MPI_COMPILE_FLAGS)
  set_target_properties(rock PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()
if(MPI_LINK_FLAGS)
  set_target_properties(rock PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

# Tests
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
    add_custom_target(memcheck ctest --output-on-failure -R 'core|disk|random|sort' -D ExperimentalMemCheck)
endif ()

# Benchmarks
if(BUILD_BENCHMARKS)
    # TODO
endif()
