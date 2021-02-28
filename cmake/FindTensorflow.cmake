cmake_policy(PUSH)
cmake_policy(VERSION 3.10)

find_package(PkgConfig QUIET)
if(${PKG_CONFIG_FOUND})
    pkg_check_modules(Tensorflow_PKGCONF QUIET tensorflow)
    if (Tensorflow_PKGCONF_INCLUDE_DIRS)
        set(Tensorflow_INCLUDE_HINTS ${Tensorflow_PKGCONF_INCLUDE_DIRS})
    endif ()
    if (Tensorflow_PKGCONF_LIBRARY_DIRS)
        set(Tensorflow_LIB_HINTS ${Tensorflow_PKGCONF_LIBRARY_DIRS})
    endif ()
    if (Tensorflow_PKGCONF_LIBRARIES)
        set(Tensorflow_LIB_NAMES ${Tensorflow_PKGCONF_LIBRARIES})
    endif ()
endif()

find_path(Tensorflow_INCLUDE_DIR NAMES tensorflow/c/c_api.h PATH_SUFFIXES tensorflow tensorflow/include HINTS ${Tensorflow_INCLUDE_HINTS})
find_library(Tensorflow_LIBRARY NAMES tensorflow ${Tensorflow_LIB_NAMES} PATH_SUFFIXES tensorflow tensorflow/lib HINTS ${Tensorflow_LIB_HINTS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Tensorflow DEFAULT_MSG Tensorflow_INCLUDE_DIR Tensorflow_LIBRARY)

if (Tensorflow_FOUND)
    mark_as_advanced(
        Tensorflow_LIBRARY
        Tensorflow_INCLUDE_DIR)

    message(STATUS "Tensorflow will be used from ${Tensorflow_INCLUDE_DIR} with libs ${Tensorflow_LIBRARY}")

    add_library(Tensorflow::Tensorflow INTERFACE IMPORTED)
    set_target_properties(Tensorflow::Tensorflow PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${Tensorflow_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "${Tensorflow_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${Tensorflow_CFLAGS};${Tensorflow_CFLAGS_OTHER}"
        )
else()
    message(FATAL_ERROR "Tensorflow libraries were not found at ${Tensorflow_INCLUDE_DIR}, try to specify Tensorflow_ROOT variable to point to Tensorflow root folder")
endif()

cmake_policy(POP)
