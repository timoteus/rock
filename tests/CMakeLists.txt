cmake_minimum_required(VERSION 2.8)

add_subdirectory(data)

add_executable(test_core test_core.c)
target_link_libraries(test_core rock)
add_test(test_core test_core)

add_executable(test_disk test_disk.c)
target_link_libraries(test_disk rock)
add_test(test_disk test_disk)

add_executable(test_random test_random.c)
target_link_libraries(test_random rock)
add_test(test_random test_random)

add_executable(test_sort test_sort.c)
target_link_libraries(test_sort rock)
add_test(test_sort test_sort)

add_executable(test_distribute test_distribute.c)
target_link_libraries(test_distribute rock)
add_test(test_distribute mpirun -np 16 test_distribute)

configure_file(valgrind.supp ${CMAKE_CURRENT_BINARY_DIR}/valgrind.supp COPYONLY)
