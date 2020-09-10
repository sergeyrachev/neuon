cmake_policy(PUSH)
cmake_policy(VERSION 3.10)

find_package(PkgConfig QUIET)
if(${PKG_CONFIG_FOUND})
    pkg_check_modules(Tensorflow tensorflow)
endif()

if(NOT Tensorflow_FOUND)
    find_path(Tensorflow_INCLUDE_DIRS NAMES tensorflow/c/c_api.h)
    find_library(Tensorflow_LIB NAMES tensorflow )

    if(NOT Tensorflow_INCLUDE_DIRS OR NOT Tensorflow_LIB)
        message(FATAL_ERROR "Tensorflow libraries were not found at ${Tensorflow_INCLUDE_DIRS}, try to specify Tensorflow_ROOT variable to point to root folder")
    endif()
    set(Tensorflow_LIBRARIES ${Tensorflow_LIB})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Tensorflow DEFAULT_MSG Tensorflow_LIBRARIES Tensorflow_INCLUDE_DIRS)
mark_as_advanced(
    Tensorflow_LIBRARIES
    Tensorflow_INCLUDE_DIRS)

message(STATUS "Tensorflow will be used from ${Tensorflow_INCLUDE_DIRS} with libs ${Tensorflow_LIBRARIES}" )

add_library(Tensorflow::Tensorflow INTERFACE IMPORTED)
set_target_properties(Tensorflow::Tensorflow PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${Tensorflow_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES "${Tensorflow_LIBRARIES}"
    INTERFACE_COMPILE_OPTIONS "${Tensorflow_CFLAGS};${Tensorflow_CFLAGS_OTHER}"
    )

cmake_policy(POP)
