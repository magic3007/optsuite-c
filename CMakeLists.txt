cmake_minimum_required(VERSION 3.1)

project(optsuite VERSION 0.1.0)

# disable in-source builds
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# set C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set module path
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_subdirectory(3rdparty)

file(GLOB_RECURSE ALL_SOURCE ${PROJECT_SOURCE_DIR}/src/*.cpp)
set(SOURCE ${ALL_SOURCE})
list(FILTER SOURCE EXCLUDE REGEX "${PROJECT_SOURCE_DIR}/src/Interface/.*\.cpp")
set(MATLAB_SOURCE ${PROJECT_SOURCE_DIR}/src/Interface/matlab.cpp)

# define options
option(BUILD_SHARED_LIBS "Enable shared library. Default: ON" ON)
option(USE_MKL "Use Intel MKL for Eigen3 backend. Default: OFF" OFF)
option(BUILD_SINGLE_PRECISION
    "Enable the build of a single precision library. Default: OFF" OFF)
option(BUILD_MATLAB_INTERFACE
    "Enable the build of matlab interface. Default: OFF" OFF)
option(EIGEN_USE_BUILTIN
    "Force Eigen to use built-in BLAS/LAPACK implementations. Default: OFF" OFF)

# define targets
add_executable(block example/block.cpp)
add_executable(opt example/opt.cpp)
add_executable(fft example/fft.cpp)
add_library(OptSuite ${SOURCE})
add_library(pyoptsuite ${PROJECT_SOURCE_DIR}/src/Interface/pyoptsuite.cpp)


include_directories(3rdparty/pybind11/include)
include_directories(/usr/include/python3.8)

target_link_libraries(pyoptsuite python3.8)
target_link_libraries(pyoptsuite OptSuite)

target_link_libraries(block Eigen3::Eigen OptSuite)
target_link_libraries(opt Eigen3::Eigen OptSuite)
target_link_libraries(fft Eigen3::Eigen OptSuite FFTW::Double)
target_link_libraries(OptSuite Eigen3::Eigen)

# define single precision targets
if (BUILD_SINGLE_PRECISION)
    add_executable(fft_f example/fft.cpp)
    add_library(OptSuite_f ${SOURCE})

    target_compile_definitions(fft_f PUBLIC OPTSUITE_SCALAR_TOKEN=1)
    target_compile_definitions(OptSuite_f PUBLIC OPTSUITE_SCALAR_TOKEN=1)

    target_link_libraries(fft_f Eigen3::Eigen OptSuite_f FFTW::Float)
    target_link_libraries(OptSuite_f Eigen3::Eigen)
endif()

configure_file(include/OptSuite/config.h.in config.h)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    message(STATUS "CMAKE_BUILD_TYPE is unset, defaulting to Debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -m64 -march=native")
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        add_compile_options(-Wpedantic)
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    # note: not working for current version because C++17 support is poor
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -inline-forceinline")
else()
    message(WARNING "Unsupport CXX compiler ${CMAKE_CXX_COMPILER_ID}")
endif()

include_directories("${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/include")

find_package(Eigen3 3.3 REQUIRED NO_MODULE)

# blas/lapack support
if (EIGEN_USE_BUILTIN)
    add_definitions(-DOPTSUITE_EIGEN_USE_BUILTIN)
endif()

find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

# check whether it is MKL
if (BLAS_LIBRARIES MATCHES "libmkl_core")
    message(STATUS "Enable MKL support in OptSuite.")
    add_definitions(-DOPTSUITE_USE_MKL)
    target_include_directories(OptSuite PUBLIC ${MKL_INCLUDE_DIRS})
    if (BUILD_SINGLE_PRECISION)
        target_include_directories(OptSuite_f PUBLIC ${MKL_INCLUDE_DIRS})
    endif()
endif()

target_link_libraries(OptSuite ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES})
if (BUILD_SINGLE_PRECISION)
    target_link_libraries(OptSuite_f ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES})
endif()

# fftw support
if (BUILD_SINGLE_PRECISION)
    find_package(FFTW REQUIRED COMPONENTS FLOAT_LIB DOUBLE_LIB)
    target_link_libraries(OptSuite_f FFTW::Float)
else()
    find_package(FFTW REQUIRED COMPONENTS DOUBLE_LIB)
endif()
target_link_libraries(OptSuite FFTW::Double)

# matlab support
if (BUILD_MATLAB_INTERFACE)
    find_package(Matlab COMPONENTS MAT_LIBRARY)

    if (Matlab_FOUND)
        message(STATUS "Matlab found at ${Matlab_ROOT_DIR}")

        # add matlab interface library
        add_library(OptSuite_matlab ${MATLAB_SOURCE})
        add_executable(readmat example/readmat.cpp)
        add_executable(svd example/svd.cpp)

        # compile defs
        set(Matlab_COMPILE_DEFS MATLAB_DEFAULT_RELEASE=R2017b MATLAB_MEX_FILE)

        target_compile_definitions(OptSuite_matlab PUBLIC ${Matlab_COMPILE_DEFS})
        target_compile_definitions(readmat PUBLIC ${Matlab_COMPILE_DEFS})
        target_compile_definitions(svd PUBLIC ${Matlab_COMPILE_DEFS})
        target_include_directories(OptSuite_matlab PUBLIC ${Matlab_INCLUDE_DIRS})
        target_link_libraries(OptSuite_matlab Eigen3::Eigen OptSuite
            ${Matlab_MEX_LIBRARY} ${Matlab_MX_LIBRARY} ${Matlab_MAT_LIBRARY})
        target_link_libraries(readmat Eigen3::Eigen OptSuite_matlab OptSuite)
        target_link_libraries(svd Eigen3::Eigen OptSuite_matlab OptSuite)

        # single precision lib
        if (BUILD_SINGLE_PRECISION)
            add_library(OptSuite_matlab_f ${MATLAB_SOURCE})

            target_compile_definitions(OptSuite_matlab_f PUBLIC OPTSUITE_SCALAR_TOKEN=1)
            target_include_directories(OptSuite_matlab_f PUBLIC ${Matlab_INCLUDE_DIRS})

            target_link_libraries(OptSuite_matlab_f Eigen3::Eigen OptSuite_f
                ${Matlab_MEX_LIBRARY} ${Matlab_MX_LIBRARY} ${Matlab_MAT_LIBRARY})
            target_link_libraries(wf_f 
                Eigen3::Eigen OptSuite_matlab_f OptSuite_f FFTW::Float)
        endif()
    else()
        message(WARNING "-DBUILD_MATLAB_INTERFACE=ON is set but Matlab not found. "
            "Try passing -DMatlab_ROOT_DIR to specify Matlab installation location")
    endif()
endif()


# propack support
if (BUILD_SINGLE_PRECISION)
    find_package(PROPACK REQUIRED COMPONENTS FLOAT_LIB DOUBLE_LIB)
    target_link_libraries(OptSuite_f PROPACK::Float -lgfortran)
else()
    find_package(PROPACK REQUIRED COMPONENTS DOUBLE_LIB)
endif()
target_link_libraries(OptSuite PROPACK::Double -lgfortran)


enable_testing()
add_subdirectory(unittest)
