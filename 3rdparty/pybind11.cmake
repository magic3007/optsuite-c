# The target Python version can be selected by setting
# PYBIND11_PYTHON_VERSION
# or an exact Python installation can be specified with
# PYTHON_EXECUTABLE

configure_file(pybind11.cmake.in pybind11-download/CMakeLists.txt)

message(STATUS "Downloading pybind11")
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/pybind11-download)
if (result)
    message(FATAL_ERROR "CMake step for pybind11 failed: ${result}")
endif ()
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/pybind11-download)
if (result)
    message(FATAL_ERROR "Build step for pybind11 failed: ${result}")
endif ()


add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/pybind11-src
        ${CMAKE_CURRENT_BINARY_DIR}/pybind11-build
        EXCLUDE_FROM_ALL)

