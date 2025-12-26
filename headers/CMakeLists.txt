cmake_minimum_required(VERSION 3.28)

include(FetchContent)
FetchContent_Declare(
    mementor
    GIT_REPOSITORY  https://gitlab.com/sergeyrachev/mementor.git
)
FetchContent_MakeAvailable(mementor)
list(APPEND CMAKE_MODULE_PATH "${MEMENTOR_MODULE_PATH}")

include(GenerateSemanticVersionHeader)
set_semantic_version_variables(NAMESPACE neuon)

project(headers VERSION ${NEUON_VERSION} LANGUAGES CXX)

include(GNUInstallDirs)

add_library(headers INTERFACE)
add_library(neuon::headers ALIAS headers)
target_sources(headers
    INTERFACE FILE_SET headers TYPE HEADERS BASE_DIRS include FILES include/neuon/neuon_c.h
)
target_include_directories(headers
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(
    TARGETS headers
    EXPORT headers
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
    FILE_SET headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT devel
)

install(
    EXPORT headers
    NAMESPACE neuon::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/neuon/cmake
    FILE neuon-headers-targets.cmake
    COMPONENT devel
)
