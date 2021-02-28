CMAKE_MINIMUM_REQUIRED(VERSION 3.17)
ENABLE_TESTING()

cmake_policy(VERSION 3.17)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(cmake/version.cmake)
configure_version(mementor)
configure_birthday(mementor)

include(cmake/export.cmake)
configure_export(MEMENTOR mementor)

project(mementor VERSION ${version})
message(STATUS "Build ${PROJECT_NAME}-${version_revision} at ${birthday} on ${CMAKE_SYSTEM_NAME}")

find_package(FFMPEG COMPONENTS avcodec avformat swscale swresample)
find_package(Boost 1.70 MODULE REQUIRED COMPONENTS program_options log log_setup)
find_package(CURL)
find_package(spdlog)
find_package(GTest 1.8 CONFIG REQUIRED COMPONENTS gtest gmock gmock_main )
include(GoogleTest)

if(CURL_FOUND)
    add_library(curl OBJECT src/curl)
    target_link_libraries(curl PUBLIC CURL::libcurl)
    add_executable(ut_curl
        test/ut_curl.cpp
        src/curl.cpp
        )
    target_link_libraries(ut_curl PRIVATE GTest::gmock GTest::gmock_main CURL::libcurl)
    target_include_directories(ut_curl PRIVATE src/)
    gtest_discover_tests(ut_curl)
endif()

add_library(hash_fnv1a OBJECT src/hash_fnv1a.cpp)
add_executable(ut_hash_fnv1a
    test/ut_hash_fnv1a.cpp
    src/hash_fnv1a.cpp
)
target_link_libraries(ut_hash_fnv1a PRIVATE GTest::gtest GTest::gmock_main)
target_include_directories(ut_hash_fnv1a PRIVATE src/)
gtest_discover_tests(ut_hash_fnv1a)

add_library(interruption OBJECT src/interruption.cpp)
add_executable(ut_interruption
    test/ut_interruption.cpp
    src/interruption.cpp
)
target_link_libraries(ut_interruption PRIVATE GTest::gtest GTest::gmock_main)
target_include_directories(ut_interruption PRIVATE src/)
gtest_discover_tests(ut_interruption)

if(Boost_FOUND)
    add_library(ntp OBJECT src/ntp.cpp)
    target_link_libraries(ntp PUBLIC Boost::date_time)
    add_executable(ut_ntp
        test/ut_ntp.cpp
        src/ntp.cpp
    )
    target_link_libraries(ut_ntp PRIVATE GTest::gtest GTest::gmock_main Boost::date_time)
    target_include_directories(ut_ntp PRIVATE src/)
    gtest_discover_tests(ut_ntp)

    add_library(options OBJECT src/options.cpp)
    target_link_libraries(options PUBLIC Boost::program_options )
    add_executable(ut_options
        test/ut_options.cpp
        src/options.cpp
    )
    target_link_libraries(ut_options PRIVATE GTest::gtest GTest::gmock_main Boost::program_options)
    target_include_directories(ut_options PRIVATE src/)
    gtest_discover_tests(ut_options)
endif()

if(NOT CMAKE_HOST_WIN32 AND Boost_FOUND AND spdlog_FOUND)
    add_library(external_tool OBJECT src/external_tool.cpp)
    target_link_libraries(external_tool PUBLIC Boost::filesystem spdlog::spdlog)
    add_executable(ut_external_tool
        test/ut_external_tool.cpp
        src/external_tool.cpp
    )
    target_link_libraries(ut_external_tool PRIVATE GTest::gtest GTest::gmock_main Boost::filesystem spdlog::spdlog)
    target_include_directories(ut_external_tool PRIVATE src/)
    gtest_discover_tests(ut_external_tool)
endif()

if(Boost_FOUND AND FFMPEG_FOUND)
    add_executable(sb_splitter
        test/sb_splitter.cpp
        src/demuxer.cpp
        src/source.cpp
        src/extraction.cpp
        src/decoder.cpp
        src/interruption.cpp
        src/options.cpp
        src/access_unit_adapter.cpp
        src/track_adapter.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/generated/version.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/generated/birthday.cpp
        )
    target_include_directories(sb_splitter PRIVATE src/ ${CMAKE_CURRENT_BINARY_DIR}/generated)
    target_link_libraries(sb_splitter PRIVATE spdlog::spdlog Boost::program_options Boost::dynamic_linking  Boost::headers FFMPEG::avutil FFMPEG::avformat FFMPEG::avcodec FFMPEG::swscale FFMPEG::swresample)
endif()
