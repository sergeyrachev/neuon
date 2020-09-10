cmake_minimum_required(VERSION 3.12)
cmake_policy(PUSH)
cmake_policy(VERSION 3.12)

enable_testing()

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_CURRENT_SOURCE_DIR}/depends/sdk/cmake")
include(depends/sdk/cmake/version.cmake)

project(neuon VERSION ${version} LANGUAGES CXX)
message(STATUS "Build ${PROJECT_NAME}-${version_revision} at ${birthday} on ${CMAKE_SYSTEM_NAME}")

add_subdirectory(headers)
add_subdirectory(sources)
add_subdirectory(examples)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
configure_package_config_file(res/neuon-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/neuon-config.cmake INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR})
write_basic_package_version_file( ${CMAKE_CURRENT_BINARY_DIR}/neuon-config-version.cmake VERSION ${version} COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/neuon-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/neuon-config-version.cmake DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/neuon/cmake COMPONENT devel)

install(FILES cmake/FindTensorflow.cmake DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/neuon/cmake/modules COMPONENT devel)
install(FILES cmake/FindJsonCpp.cmake DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/neuon/cmake/modules COMPONENT devel)

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

set(CPACK_ARCHIVE_COMPONENT_INSTALL On)

set(CPACK_RPM_COMPONENT_INSTALL On)
set(CPACK_RPM_PACKAGE_AUTOREQ On)
set(CPACK_RPM_PACKAGE_AUTOREQPROV Off)
set(CPACK_RPM_DEVEL_PACKAGE_REQUIRES "blas, lapack, atlas, jsoncpp-devel")
set(CPACK_RPM_SAMPLE_PACKAGE_REQUIRES "ffmpeg-devel, jsoncpp-devel")

set(CPACK_DEB_COMPONENT_INSTALL On)
set(CPACK_DEBIAN_DEVEL_PACKAGE_DEPENDS "libopenblas-base, libblas3, libjsoncpp-dev, libjsoncpp1, libopenblas-dev")
set(CPACK_DEBIAN_SAMPLE_PACKAGE_DEPENDS "libavformat-dev, libavcodec-dev, libswscale-dev, libswresample-dev, libavutil-dev, libopenblas-dev")

include(CPack)


cmake_policy(POP)
