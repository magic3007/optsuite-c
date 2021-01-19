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

add_executable(group_lasso group_lasso.cpp)
target_include_directories(group_lasso PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(group_lasso PRIVATE OptSuite)

add_executable(Axmb_Nuclear Axmb_Nuclear.cpp)
target_include_directories(Axmb_Nuclear PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(Axmb_Nuclear PRIVATE OptSuite)

add_executable(Axmb_L2 Axmb_L2.cpp)
target_include_directories(Axmb_L2 PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(Axmb_L2 PRIVATE OptSuite)