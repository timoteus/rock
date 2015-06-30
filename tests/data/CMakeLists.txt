set(SAMPLE_1 indx_32bit_100x10x1000_50000_sample.hdf5)
set(SORTED_1 indx_32bit_100x10x1000_50000_sorted_102.hdf5)

set(SAMPLE_2 indx_32bit_512x4x8x2_5000_sample.hdf5)
set(SORTED_2 indx_32bit_512x4x8x2_5000_sorted_3210.hdf5)
set(PERM_2 perm_32bit_512x4x8x2_5000_sorted_3210.hdf5)

set(SAMPLE_3 indx_32bit_32max_100000_sample.hdf5)
set(SORTED_3 indx_32bit_32max_100000_sorted_0.hdf5)

configure_file(${SAMPLE_1} ${CMAKE_CURRENT_BINARY_DIR}/${SAMPLE_1} COPYONLY)
configure_file(${SORTED_1} ${CMAKE_CURRENT_BINARY_DIR}/${SORTED_1} COPYONLY)

configure_file(${SAMPLE_2} ${CMAKE_CURRENT_BINARY_DIR}/${SAMPLE_2} COPYONLY)
configure_file(${SORTED_2} ${CMAKE_CURRENT_BINARY_DIR}/${SORTED_2} COPYONLY)
configure_file(${PERM_2} ${CMAKE_CURRENT_BINARY_DIR}/${PERM_2} COPYONLY)

configure_file(${SAMPLE_3} ${CMAKE_CURRENT_BINARY_DIR}/${SAMPLE_3} COPYONLY)
configure_file(${SORTED_3} ${CMAKE_CURRENT_BINARY_DIR}/${SORTED_3} COPYONLY)
