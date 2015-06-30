set(INDX_1 indx_example_core.hdf5)
set(ELEM_1 elem_example_core.hdf5)

configure_file(${INDX_1} ${CMAKE_CURRENT_BINARY_DIR}/${INDX_1} COPYONLY)
configure_file(${ELEM_1} ${CMAKE_CURRENT_BINARY_DIR}/${ELEM_1} COPYONLY)
