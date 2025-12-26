cmake_minimum_required(VERSION 3.28)

include(FetchContent)
FetchContent_Declare(
    mementor
    GIT_REPOSITORY https://gitlab.com/sergeyrachev/mementor.git
)
FetchContent_MakeAvailable(mementor)
list(APPEND CMAKE_MODULE_PATH "${MEMENTOR_MODULE_PATH}")

include(GenerateSemanticVersionHeader)
set_semantic_version_variables(NAMESPACE neuon_example)
generate_semantic_version_header(NAMESPACE neuon_example)

project(neuon-sample VERSION ${NEUON_EXAMPLE_VERSION} LANGUAGES CXX)
message(STATUS "Build ${PROJECT_NAME}-${NEUON_EXAMPLE_VERSION_LONG} at ${NEUON_EXAMPLE_TIMESTAMP} on ${CMAKE_SYSTEM_NAME}")

if (NOT TARGET mementor::mementor)
    find_package(mementor REQUIRED)
endif()

find_package(Threads REQUIRED)
find_package(spdlog REQUIRED)
find_package(Boost 1.70 REQUIRED COMPONENTS date_time program_options filesystem log log_setup)
find_package(FFMPEG REQUIRED COMPONENTS avcodec avformat swscale swresample)

if(NOT TARGET neuon::headers)
    find_package(Neuon REQUIRED)
endif()

add_executable(neuon-sample)

target_sources(neuon-sample
    PRIVATE
        src/neuon_sample.cpp
)

target_link_libraries(neuon-sample
    PRIVATE
        Threads::Threads
        neuon::neuon
        neuon_example-version-header
        mementor::splitter
        mementor::options
        mementor::interruption
)
target_compile_features(neuon-sample PRIVATE cxx_std_11)
set_target_properties(neuon-sample PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(neuon-sample
    PRIVATE src/
)

include(GNUInstallDirs)
install(
    TARGETS neuon-sample
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT sample
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT sample
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT sample
    )

configure_file(res/demo.Dockerfile.in demo.Dockerfile @ONLY)

install(
    DIRECTORY .
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/neuon/examples
    COMPONENT sample
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/demo.Dockerfile
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/neuon/examples/ci
    COMPONENT sample
)
