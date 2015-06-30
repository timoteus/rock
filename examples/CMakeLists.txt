cmake_minimum_required(VERSION 2.8)

add_subdirectory(data)

add_executable(example_tensor example_tensor.c)
target_link_libraries(example_tensor rock)

add_executable(example_core example_core.c)
target_link_libraries(example_core rock)

add_executable(example_distribute example_distribute.c)
target_link_libraries(example_distribute rock)
