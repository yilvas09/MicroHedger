# Bar类测试可执行文件
add_executable(test_bar test_bar.cpp)
target_link_libraries(test_bar lob_lib ${Boost_LIBRARIES})

# LOB类测试可执行文件  
add_executable(test_lob test_lob.cpp)
target_link_libraries(test_lob lob_lib ${Boost_LIBRARIES})

# 启用测试
enable_testing()

# 添加测试
add_test(NAME BarTests COMMAND test_bar)
add_test(NAME LOBTests COMMAND test_lob)

# 自定义目标，运行所有测试
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS test_bar test_lob
    COMMENT "Running all unit tests"
)

# 可选：创建一个包含所有测试的单一可执行文件
add_executable(all_tests test_bar.cpp test_lob.cpp)
target_compile_definitions(all_tests PRIVATE BOOST_TEST_DYN_LINK)
target_link_libraries(all_tests lob_lib ${Boost_LIBRARIES})

# 设置输出目录
set_target_properties(test_bar test_lob all_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)