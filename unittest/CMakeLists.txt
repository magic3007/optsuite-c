function(add_unittest_target target sources test_name)
    add_executable(${target} ${sources})
    target_include_directories(${target} PRIVATE "${PROJECT_SOURCE_DIR}/include")
    target_link_libraries(${target} PRIVATE OptSuite gtest_main)
    add_test(NAME ${test_name} COMMAND "./${target}")
endfunction()

add_unittest_target(grad_unittest grad_unittest.cpp gradient)

add_executable(lasso lasso.cpp)
target_include_directories(lasso PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(lasso PRIVATE OptSuite)

