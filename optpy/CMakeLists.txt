configure_file(__init__.py.in __init__.py)
pybind11_add_module(core_n core_n_pybind.cpp)
target_link_libraries(core_n PRIVATE OptSuite)

add_subdirectory(base)
add_subdirectory(linalg)