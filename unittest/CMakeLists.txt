function(add_unittest_target target sources test_name)
    add_executable(${target} ${sources})
    target_include_directories(${target} PRIVATE "${PROJECT_SOURCE_DIR}/include")
    target_link_libraries(${target} PRIVATE OptSuite gtest_main)
    add_test(NAME ${test_name} COMMAND "./${target}")
endfunction()

add_unittest_target(grad_unittest grad_unittest.cpp gradient)