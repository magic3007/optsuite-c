configure_file(__init__.py.in __init__.py)

pybind11_add_module(mat_op mat_op_pybind.cpp)
target_link_libraries(mat_op PRIVATE OptSuite)

pybind11_add_module(functional functional_pybind.cpp)
target_link_libraries(functional PRIVATE OptSuite)

pybind11_add_module(mat_wrapper mat_wrapper_pybind.cpp)
target_link_libraries(mat_wrapper PRIVATE OptSuite)

pybind11_add_module(solver solver_pybind.cpp)
target_link_libraries(solver PRIVATE OptSuite)
