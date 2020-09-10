cmake_minimum_required(VERSION 3.16)
cmake_policy(PUSH)
cmake_policy(VERSION 3.16)

include(../depends/sdk/cmake/version.cmake)
include(GNUInstallDirs)

project(neuon-library VERSION ${version} LANGUAGES CXX)

if (NOT TARGET neuon::headers)
    find_package(neuon REQUIRED COMPONENTS headers)
endif()

find_package(Threads REQUIRED)
find_package(Tensorflow REQUIRED)
find_package(JsonCpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(dlib REQUIRED)
find_package(Aquila REQUIRED)
find_package(Boost 1.70 REQUIRED COMPONENTS program_options filesystem)
find_package(FFMPEG REQUIRED COMPONENTS avcodec avformat swscale swresample)
find_package(Python3 COMPONENTS Interpreter)
if (NOT Python3_FOUND)
    option(NEUON_PYTHON_EXECUTABLE "Set to Python 3 executable path" /usr/bin/python3)
    set(Python3_EXECUTABLE ${NEUON_PYTHON_EXECUTABLE})
    message(WARNING "Manually set python path to ${Python3_EXECUTABLE} as it was not found automatically")
endif ()

configure_file(../depends/sdk/src/version.cpp.in version.cpp)
configure_file(../depends/sdk/src/birthday.cpp.in birthday.cpp)

add_executable(neuon-data
    ${CMAKE_CURRENT_BINARY_DIR}/version.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/birthday.cpp
    src/demo.cpp
    src/neuon_data.cpp
    src/license.cpp
    ../depends/sdk/src/extraction.cpp
    ../depends/sdk/src/source.cpp
    ../depends/sdk/src/demuxer.cpp
    ../depends/sdk/src/decoder.cpp
    ../depends/sdk/src/interruption.cpp
    ../depends/sdk/src/track_adapter.cpp
    ../depends/sdk/src/access_unit_adapter.cpp
    ../depends/sdk/src/resample.cpp
    ../depends/sdk/src/options.cpp
    src/neuon_cxx.cpp
    src/configuration.cpp
    )
target_link_libraries(neuon-data PRIVATE Aquila::Aquila dlib::dlib Threads::Threads Boost::program_options Boost::headers FFMPEG::avcodec FFMPEG::avformat FFMPEG::swscale FFMPEG::swresample spdlog::spdlog)
target_compile_features(neuon-data PRIVATE cxx_std_17)
set_target_properties(neuon-data PROPERTIES INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
target_include_directories(neuon-data
    PRIVATE src/ ../depends/sdk/src
    )
install(TARGETS neuon-data DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT internal)

add_library(neuon
    ${CMAKE_CURRENT_BINARY_DIR}/version.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/birthday.cpp
    src/license.cpp
    src/model.cpp
    src/tensorflow_api.cpp
    src/tensorflow_dynamic.cpp
    src/tensorflow_static.cpp
    src/neuon_c.cpp
    src/neuon_cxx.cpp
    src/demo.cpp
    src/configuration.cpp
    )
add_library(neuon::neuon ALIAS neuon)
target_link_libraries(neuon PRIVATE Threads::Threads JsonCpp::JsonCpp Boost::headers Aquila::Aquila dlib::dlib Tensorflow::Tensorflow Boost::filesystem spdlog::spdlog PUBLIC neuon::headers  )
target_compile_features(neuon PRIVATE cxx_std_17)
target_link_options(neuon PRIVATE "-Wl,--as-needed")
target_compile_options(neuon PRIVATE -fvisibility=hidden -fvisibility-inlines-hidden)
target_compile_definitions(neuon PRIVATE BOOST_UBLAS_INLINE= NEUON_ORIGINAL_BUILD )
set_target_properties(neuon PROPERTIES INSTALL_RPATH "$ORIGIN")
set_target_properties(neuon PROPERTIES C_VISIBILITY_PRESET hidden)
set_target_properties(neuon PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(neuon PROPERTIES VISIBILITY_INLINES_HIDDEN On)
target_include_directories(neuon
    PRIVATE src/
    ../depends/sdk/src
    )

install(TARGETS neuon EXPORT neuon-library
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel NAMELINK_COMPONENT devel
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
    OBJECTS DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
install(EXPORT neuon-library NAMESPACE neuon:: DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/neuon/cmake FILE neuon-targets.cmake COMPONENT devel)
install(FILES res/model.pb res/normale.json res/shape_predictor_68_face_landmarks.dat DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/neuon/res COMPONENT runtime)

set(NEUON_PACKAGE_LOCATION "${CMAKE_CURRENT_LIST_DIR}/src/gym")
configure_file(src/gym/setup.py.in gym/setup.py @ONLY)
add_custom_target(neuon-gym-package ALL ${Python3_EXECUTABLE} setup.py bdist_wheel --dist-dir ${CMAKE_CURRENT_BINARY_DIR} WORKING_DIRECTORY gym)

set(NEUON_PACKAGE_LOCATION "${CMAKE_CURRENT_LIST_DIR}/src/details")
configure_file(src/details/setup.py.in details/setup.py @ONLY)
add_custom_target(neuon-details-package ALL ${Python3_EXECUTABLE} setup.py bdist_wheel --dist-dir ${CMAKE_CURRENT_BINARY_DIR} WORKING_DIRECTORY details)

set(NEUON_PACKAGE_LOCATION "${CMAKE_CURRENT_LIST_DIR}/src/show")
configure_file(src/show/setup.py.in show/setup.py @ONLY)
add_custom_target(neuon-show-package ALL ${Python3_EXECUTABLE} setup.py bdist_wheel --dist-dir ${CMAKE_CURRENT_BINARY_DIR} WORKING_DIRECTORY show)

cmake_policy(POP)
