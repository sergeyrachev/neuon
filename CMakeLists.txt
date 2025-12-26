cmake_minimum_required(VERSION 3.28)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(
    mementor
    GIT_REPOSITORY  https://gitlab.com/sergeyrachev/mementor.git
)
FetchContent_MakeAvailable(mementor)
list(APPEND CMAKE_MODULE_PATH "${MEMENTOR_MODULE_PATH}")

include(GenerateSemanticVersionHeader)
set_semantic_version_variables(NAMESPACE neuon)

project(neuon VERSION ${NEUON_VERSION} LANGUAGES CXX)
message(STATUS "Build ${PROJECT_NAME}-${NEUON_VERSION_LONG} at ${NEUON_TIMESTAMP} on ${CMAKE_SYSTEM_NAME}")

add_subdirectory(headers)
add_subdirectory(sources)
add_subdirectory(examples)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/neuon-config.cmake.in
[=[
cmake_policy(VERSION 3.10)

@PACKAGE_INIT@

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

include(CMakeFindDependencyMacro)
find_dependency(Tensorflow)
find_dependency(JsonCpp)

include ( "${CMAKE_CURRENT_LIST_DIR}/neuon-headers-targets.cmake" )
include ( "${CMAKE_CURRENT_LIST_DIR}/neuon-targets.cmake" )
]=]
)

configure_package_config_file(
    ${CMAKE_CURRENT_BINARY_DIR}/neuon-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/neuon-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/neuon/cmake
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/neuon-config-version.cmake
    VERSION ${NEUON_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/neuon-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/neuon-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/neuon/cmake
    COMPONENT devel
)

if (NEUON_PREFIX_PATH)
    set(NEUON_PREFIX_COMPONENT devel)
    if(BUILD_SHARED_LIBS)
        set(NEUON_PREFIX_COMPONENT runtime)
    endif()
    install(DIRECTORY ${NEUON_PREFIX_PATH}/ DESTINATION . COMPONENT ${NEUON_PREFIX_COMPONENT})
endif()

include(CPackComponent)

cpack_add_component(runtime)
cpack_add_component(devel DEPENDS runtime)
cpack_add_component(sample DEPENDS runtime devel)

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)
set(CPACK_PACKAGE_CONTACT "info@example.com")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_ARCHIVE_COMPONENT_INSTALL On)

set(CPACK_DEB_COMPONENT_INSTALL On)
set(CPACK_DEBIAN_DEVEL_PACKAGE_DEPENDS "libopenblas-dev, libblas-dev, libjsoncpp-dev, libboost-all-dev, libdlib-dev, libjpeg-dev")
set(CPACK_DEBIAN_SAMPLE_PACKAGE_DEPENDS "libavformat-dev, libavcodec-dev, libswscale-dev, libswresample-dev, libavutil-dev, libopenblas-dev, libcurl4-openssl-dev, libgtest-dev, libgmock-dev, libspdlog-dev")

include(CPack)
