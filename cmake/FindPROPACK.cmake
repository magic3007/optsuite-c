# - Find the PROPACK library
#
#   Copyright (c) 2020, Haoyang Liu
#
# Usage:
#   find_package(PROPACK [REQUIRED] [QUIET] [COMPONENTS component1 ... componentX] )
#
# It sets the following variables:
#   PROPACK_FOUND                  ... true if PROPACK is found on the system
#   PROPACK_[component]_LIB_FOUND  ... true if the component is found on the system (see components below)
#   PROPACK_LIBRARIES              ... full paths to all found fftw libraries
#   PROPACK_[component]_LIB        ... full path to one of the components (see below)
#
# The following variables will be checked by the function
#   PROPACK_ROOT_DIR                   ... if set, the libraries are exclusively searched
#                                      under this path
#   PROPACK_LIB_SUFFIX             --- if set, append this suffix to all libraries
#
# This package supports the following components:
#   FLOAT_LIB
#   DOUBLE_LIB
#   COMPLEX_FLOAT_LIB
#   COMPLEX_DOUBLE_LIB
#

# TODO (maybe): extend with ExternalProject download + build option


if( NOT PROPACK_ROOT_DIR AND DEFINED ENV{PROPACK_DIR} )
    set( PROPACK_ROOT_DIR $ENV{PROPACK_DIR} )
endif()

if ( PROPACK_LIB_SUFFIX )
    set(LOCAL_SUFFIX "_${PROPACK_LIB_SUFFIX}")
else()
    set(LOCAL_SUFFIX "")
endif()

if( PROPACK_ROOT_DIR )
    # find libs
    find_library(
        PROPACK_FLOAT_LIB
        NAMES "spropack${LOCAL_SUFFIX}"
        PATHS ${PROPACK_ROOT_DIR}
        PATH_SUFFIXES "lib" "lib64" "single" "lib/single" "lib64/single"
        NO_DEFAULT_PATH)

    find_library(
        PROPACK_DOUBLE_LIB
        NAMES "dpropack${LOCAL_SUFFIX}"
        PATHS ${PROPACK_ROOT_DIR}
        PATH_SUFFIXES "lib" "lib64" "double" "lib/double" "lib64/double"
        NO_DEFAULT_PATH)

    find_library(
        PROPACK_COMPLEX_FLOAT_LIB
        NAMES "cpropack${LOCAL_SUFFIX}"
        PATHS ${PROPACK_ROOT_DIR}
        PATH_SUFFIXES "lib" "lib64" "complex8" "lib/complex8" "lib64/complex8"
        NO_DEFAULT_PATH)

    find_library(
        PROPACK_COMPLEX_DOUBLE_LIB
        NAMES "zpropack${LOCAL_SUFFIX}"
        PATHS ${PROPACK_ROOT_DIR}
        PATH_SUFFIXES "lib" "lib64" "complex16" "lib/complex16" "lib64/complex16"
        NO_DEFAULT_PATH)

else()
    # find libs
    find_library(
        PROPACK_FLOAT_LIB
        NAMES "spropack${LOCAL_SUFFIX}"
        PATH_SUFFIXES "lib" "lib64" "single" "lib/single" "lib64/single")

    find_library(
        PROPACK_DOUBLE_LIB
        NAMES "dpropack${LOCAL_SUFFIX}"
        PATH_SUFFIXES "lib" "lib64" "double" "lib/double" "lib64/double")

    find_library(
        PROPACK_COMPLEX_FLOAT_LIB
        NAMES "cpropack${LOCAL_SUFFIX}"
        PATH_SUFFIXES "lib" "lib64" "complex8" "lib/complex8" "lib64/complex8")

    find_library(
        PROPACK_COMPLEX_DOUBLE_LIB
        NAMES "zpropack${LOCAL_SUFFIX}"
        PATH_SUFFIXES "lib" "lib64" "complex16" "lib/complex16" "lib64/complex16")

endif( PROPACK_ROOT_DIR )

#--------------------------------------- components

if (PROPACK_DOUBLE_LIB)
    set(PROPACK_DOUBLE_LIB_FOUND TRUE)
    set(PROPACK_LIBRARIES ${PROPACK_LIBRARIES} ${PROPACK_DOUBLE_LIB})
    add_library(PROPACK::Double INTERFACE IMPORTED)
    set_target_properties(PROPACK::Double
        PROPERTIES INTERFACE_LINK_LIBRARIES "${PROPACK_DOUBLE_LIB}"
        )
else()
    set(PROPACK_DOUBLE_LIB_FOUND FALSE)
endif()

if (PROPACK_FLOAT_LIB)
    set(PROPACK_FLOAT_LIB_FOUND TRUE)
    set(PROPACK_LIBRARIES ${PROPACK_LIBRARIES} ${PROPACK_FLOAT_LIB})
    add_library(PROPACK::Float INTERFACE IMPORTED)
    set_target_properties(PROPACK::Float
        PROPERTIES INTERFACE_LINK_LIBRARIES "${PROPACK_FLOAT_LIB}"
        )
else()
    set(PROPACK_FLOAT_LIB_FOUND FALSE)
endif()

if (PROPACK_COMPLEX_DOUBLE_LIB)
    set(PROPACK_COMPLEX_DOUBLE_LIB_FOUND TRUE)
    set(PROPACK_LIBRARIES ${PROPACK_LIBRARIES} ${PROPACK_COMPLEX_DOUBLE_LIB})
    add_library(PROPACK::ComplexDouble INTERFACE IMPORTED)
    set_target_properties(PROPACK::ComplexDouble
        PROPERTIES INTERFACE_LINK_LIBRARIES "${PROPACK_COMPLEX_DOUBLE_LIB}"
        )
else()
    set(PROPACK_COMPLEX_DOUBLE_LIB_FOUND FALSE)
endif()

if (PROPACK_COMPLEX_FLOAT_LIB)
    set(PROPACK_COMPLEX_FLOAT_LIB_FOUND TRUE)
    set(PROPACK_LIBRARIES ${PROPACK_LIBRARIES} ${PROPACK_COMPLEX_FLOAT_LIB})
    add_library(PROPACK::ComplexFloat INTERFACE IMPORTED)
    set_target_properties(PROPACK::ComplexFloat
        PROPERTIES INTERFACE_LINK_LIBRARIES "${PROPACK_COMPLEX_FLOAT_LIB}"
        )
else()
    set(PROPACK_COMPLEX_FLOAT_LIB_FOUND FALSE)
endif()


#--------------------------------------- end components

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(PROPACK
    REQUIRED_VARS PROPACK_LIBRARIES
    HANDLE_COMPONENTS
    )

mark_as_advanced(
    PROPACK_LIBRARIES
    PROPACK_FLOAT_LIB
    PROPACK_DOUBLE_LIB
    PROPACK_COMPLEX_FLOAT_LIB
    PROPACK_COMPLEX_DOUBLE_LIB
    )
