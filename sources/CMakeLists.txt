cmake_minimum_required(VERSION 3.28)

include(FetchContent)
FetchContent_Declare(
    mementor
    GIT_REPOSITORY  https://gitlab.com/sergeyrachev/mementor.git
)
FetchContent_MakeAvailable(mementor)
list(APPEND CMAKE_MODULE_PATH "${MEMENTOR_MODULE_PATH}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GenerateSemanticVersionHeader)
set_semantic_version_variables(NAMESPACE neuon)
generate_semantic_version_header(NAMESPACE neuon)

project(neuon-library VERSION ${NEUON_VERSION} LANGUAGES CXX)
include(GNUInstallDirs)

if (NOT TARGET neuon::headers)
    find_package(neuon REQUIRED COMPONENTS headers)
endif()

if (NOT TARGET mementor::mementor)
    find_package(mementor REQUIRED)
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
    message(ERROR "Manually set python path to ${Python3_EXECUTABLE} as it was not found automatically")
endif ()

add_executable(neuon-data)
target_sources(neuon-data
    PRIVATE
        src/demo.cpp
        src/neuon_data.cpp
        src/license.cpp
        src/neuon_cxx.cpp
        src/configuration.cpp
)
target_link_libraries(neuon-data
    PRIVATE
        Aquila::Aquila
        dlib::dlib
        Threads::Threads
        neuon-version-header
        mementor::splitter
        mementor::options
        mementor::interruption
)
target_compile_features(neuon-data PRIVATE cxx_std_17)
set_target_properties(neuon-data PROPERTIES INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
target_include_directories(neuon-data
    PRIVATE src/
)
install(TARGETS neuon-data DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT internal)

include(GenerateSymbolExportHeader)
generate_symbol_export_header(NAMESPACE neuon AS_FOR_SHARED_LIBRARY ${BUILD_SHARED_LIBS})

add_library(neuon)
add_library(neuon::neuon ALIAS neuon)
target_sources(neuon
    PRIVATE
        src/license.cpp
        src/model.cpp
        src/tensorflow_api.cpp
        src/tensorflow_dynamic.cpp
        src/tensorflow_static.cpp
        src/neuon_c.cpp
        src/neuon_cxx.cpp
        src/demo.cpp
        src/configuration.cpp
        src/speech_detection.cpp
        src/face_detection.cpp
        src/log.cpp
)
target_link_libraries(neuon
        PRIVATE
        Threads::Threads
        JsonCpp::JsonCpp
        Aquila::Aquila
        dlib::dlib
        Tensorflow::Tensorflow
        Boost::filesystem
        neuon-export-macro
        neuon-version-header
        PUBLIC
        neuon::headers
        neuon-export-header
)
target_compile_features(neuon PRIVATE cxx_std_17)
set_target_properties(neuon PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(neuon PROPERTIES INSTALL_RPATH "$ORIGIN")
set_target_properties(neuon PROPERTIES C_VISIBILITY_PRESET hidden)
set_target_properties(neuon PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(neuon PROPERTIES VISIBILITY_INLINES_HIDDEN On)
target_compile_definitions(neuon PRIVATE BOOST_UBLAS_INLINE= )
target_include_directories(neuon
    PRIVATE src/
)

install(
    FILES cmake/FindTensorflow.cmake
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/neuon/cmake/
    COMPONENT devel
)

install(
    TARGETS neuon neuon-version-header neuon-export-header neuon-export-macro
    EXPORT neuon-library
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel NAMELINK_COMPONENT devel
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILE_SET headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT devel
)
install(
    EXPORT neuon-library
    NAMESPACE neuon::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/neuon/cmake
    FILE neuon-targets.cmake
    COMPONENT devel
)
install(
    FILES
        res/model.pb
        res/normale.json
        res/shape_predictor_68_face_landmarks.dat
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/neuon/res
    COMPONENT runtime
)

set(NEUON_PACKAGE_LOCATION "${CMAKE_CURRENT_LIST_DIR}/src/gym")
configure_file(src/gym/setup.py.in gym/setup.py @ONLY)
add_custom_target(neuon-gym-package ALL ${Python3_EXECUTABLE} setup.py bdist_wheel --dist-dir ${CMAKE_CURRENT_BINARY_DIR} WORKING_DIRECTORY gym)

set(NEUON_PACKAGE_LOCATION "${CMAKE_CURRENT_LIST_DIR}/src/details")
configure_file(src/details/setup.py.in details/setup.py @ONLY)
add_custom_target(neuon-details-package ALL ${Python3_EXECUTABLE} setup.py bdist_wheel --dist-dir ${CMAKE_CURRENT_BINARY_DIR} WORKING_DIRECTORY details)

set(NEUON_PACKAGE_LOCATION "${CMAKE_CURRENT_LIST_DIR}/src/show")
configure_file(src/show/setup.py.in show/setup.py @ONLY)
add_custom_target(neuon-show-package ALL ${Python3_EXECUTABLE} setup.py bdist_wheel --dist-dir ${CMAKE_CURRENT_BINARY_DIR} WORKING_DIRECTORY show)
