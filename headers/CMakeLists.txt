cmake_minimum_required(VERSION 3.16)
cmake_policy(PUSH)
cmake_policy(VERSION 3.16)

include(../depends/sdk/cmake/version.cmake)
include(GNUInstallDirs)

project(neuon-headers VERSION ${version} LANGUAGES CXX)

add_library(headers INTERFACE)
add_library(neuon::headers ALIAS headers)
target_include_directories(headers INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

set(API_NAME NEUON)
include(../depends/sdk/cmake/export.cmake)
configure_file(${CMAKE_CURRENT_BINARY_DIR}/res/export_configuration.h.in ${CMAKE_CURRENT_BINARY_DIR}/neuon/export_configuration.h @ONLY)

install(TARGETS headers
    EXPORT neuon-headers
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
    OBJECTS DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

install(EXPORT neuon-headers
    NAMESPACE neuon::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/neuon/cmake
    FILE neuon-headers-targets.cmake
    COMPONENT devel
    )

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT devel)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/neuon
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT devel)

cmake_policy(POP)
