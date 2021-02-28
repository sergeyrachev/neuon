function(configure_export SYMBOLS_SPACE_NAME INCLUDE_DIRECTORY)
    if (NOT SYMBOLS_SPACE_NAME)
        message(FATAL_ERROR "Set SYMBOLS_SPACE_NAME variable before to use export.cmake")
    endif ()

    set(${SYMBOLS_SPACE_NAME}_SHARED_LIBRARY ${BUILD_SHARED_LIBS})
    set(CMAKE_DEFINE "#cmakedefine")

    configure_file(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../res/export_configuration.h.in.in ${CMAKE_CURRENT_BINARY_DIR}/export_configuration.h.in @ONLY)
    configure_file(${CMAKE_CURRENT_BINARY_DIR}/export_configuration.h.in ${CMAKE_CURRENT_BINARY_DIR}/${INCLUDE_DIRECTORY}/export_configuration.h @ONLY)
    message(STATUS "You need to add ${SYMBOLS_SPACE_NAME}_ORIGINAL_BUILD definition to target compile definition to explicitly enable symbols export if you build library")
    message(STATUS "Don't forget to add CMAKE_CURRENT_BINARY_DIR and/or CMAKE_CURRENT_BINARY_DIR/INCLUDE_DIRECTORY(which is ${CMAKE_CURRENT_BINARY_DIR}/${INCLUDE_DIRECTORY}) to your target include directories with BUILD_INTERFACE")
endfunction()
