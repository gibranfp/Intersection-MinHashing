set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
include_directories( ${PROJECT_SOURCE_DIR}/include/imh )
add_executable( test_iminhash test_iminhash )
target_link_libraries( test_iminhash iminhash listdb array_lists rng mt19937-64 m)
