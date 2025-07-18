# executable for tests of class Bar
add_executable(test_bar test_bar.cpp)
target_link_libraries(test_bar lob_lib utils_lib ${Boost_LIBRARIES})

# executable for tests of class LOB
add_executable(test_lob test_lob.cpp)
target_link_libraries(test_lob lob_lib utils_lib ${Boost_LIBRARIES})

# executable for tests of class Random
add_executable(test_random test_random.cpp)
target_link_libraries(test_random utils_lib ${Boost_LIBRARIES})

# executable for tests of class Option
add_executable(test_option test_option.cpp)
target_link_libraries(test_option option_lib ${Boost_LIBRARIES})

# executable for tests of class DeltaHedger
add_executable(test_deltahedger test_deltahedger.cpp)
target_link_libraries(test_deltahedger players_lib ${Boost_LIBRARIES})

# executable for tests of class PathCollection
add_executable(test_pathcollection test_pathcollection.cpp)
target_link_libraries(test_pathcollection path_lib ${Boost_LIBRARIES})

# executable for tests of utility function - sortPairedVectors
add_executable(test_paired_vector_sort test_paired_vector_sort.cpp)
target_link_libraries(test_paired_vector_sort utils_lib ${Boost_LIBRARIES})


enable_testing()

add_test(NAME BarTests COMMAND test_bar)
add_test(NAME LOBTests COMMAND test_lob)
add_test(NAME RandomTests COMMAND test_random)
add_test(NAME OptionTests COMMAND test_option)
add_test(NAME DeltaHedgerTests COMMAND test_deltahedger)
add_test(NAME PathCollectionTest COMMAND test_pathcollection)
add_test(NAME PairedVectorSortTest COMMAND test_paired_vector_sort)

# customised target and run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS test_bar test_lob test_random test_option test_deltahedger test_pathcollection test_paired_vector_sort
    COMMENT "Running all unit tests"
)

# set output directories
set_target_properties(test_bar test_lob test_random test_option test_deltahedger test_pathcollection test_paired_vector_sort
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# alternatively we can use a single executable for all tests
# add_executable(all_tests test_bar.cpp test_lob.cpp test_random.cpp test_option.cpp test_deltahedger.cpp test_pathcollection.cpp test_paired_vector_sort.cpp)
# target_compile_definitions(all_tests PRIVATE BOOST_TEST_DYN_LINK)
# target_link_libraries(all_tests lob_lib utils_lib ${Boost_LIBRARIES})

# set output directories
# set_target_properties(all_tests
#     PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
#)