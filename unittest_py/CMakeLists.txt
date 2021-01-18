find_package(PythonInterp REQUIRED)

configure_file(core_n_test.py.in core_n_test.py)
add_test(NAME py_core_n_test
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/core_n_test.py
        )

configure_file(functional_test.py.in functional_test.py)
add_test(NAME py_functional_test
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/functional_test.py)
