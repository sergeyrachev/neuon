cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_CURRENT_SOURCE_DIR}/depends/sdk/cmake")
include(${CMAKE_CURRENT_SOURCE_DIR}/depends/sdk/cmake/version.cmake)

project(neuon-sample VERSION ${version})
message(STATUS "Build ${PROJECT_NAME}-${version_revision} at ${birthday} on ${CMAKE_SYSTEM_NAME}")

find_package(Threads REQUIRED)
find_package(spdlog REQUIRED)
find_package(Boost 1.70 REQUIRED COMPONENTS program_options)
find_package(FFMPEG REQUIRED COMPONENTS avcodec avformat swscale swresample)

if(NOT TARGET neuon::neuon)
    find_package(Neuon REQUIRED COMPONENTS headers neuon)
endif()

configure_file(depends/sdk/src/version.cpp.in version.cpp)
configure_file(depends/sdk/src/birthday.cpp.in birthday.cpp)

add_executable(neuon-sample
    ${CMAKE_CURRENT_BINARY_DIR}/version.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/birthday.cpp
    src/neuon_sample.cpp
    depends/sdk/src/extraction.cpp
    depends/sdk/src/options.cpp
    depends/sdk/src/source.cpp
    depends/sdk/src/demuxer.cpp
    depends/sdk/src/decoder.cpp
    depends/sdk/src/interruption.cpp
    depends/sdk/src/track_adapter.cpp
    depends/sdk/src/access_unit_adapter.cpp
    depends/sdk/src/resample.cpp
    )
target_link_libraries(neuon-sample PRIVATE spdlog::spdlog Threads::Threads Boost::program_options FFMPEG::avcodec FFMPEG::avformat FFMPEG::swscale FFMPEG::swresample neuon::neuon)
target_compile_features(neuon-sample PRIVATE cxx_std_11)
target_include_directories(neuon-sample
    PRIVATE src/
    depends/sdk/src
    )

include(GNUInstallDirs)
install(TARGETS neuon-sample
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT sample
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT sample
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT sample
    OBJECTS DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT sample
    )

configure_file(res/arch.Dockerfile.in arch.Dockerfile @ONLY)
configure_file(res/centos-8.Dockerfile.in centos-8.Dockerfile @ONLY)
configure_file(res/ubuntu-18.Dockerfile.in ubuntu-18.Dockerfile @ONLY)

install(DIRECTORY .
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/neuon/examples
    COMPONENT sample)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/arch.Dockerfile ${CMAKE_CURRENT_BINARY_DIR}/centos-8.Dockerfile ${CMAKE_CURRENT_BINARY_DIR}/ubuntu-18.Dockerfile
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/neuon/examples/ci
    COMPONENT sample)
