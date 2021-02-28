function(configure_version symbols_space_name)
    message(STATUS "You may specify version and revision variables to overload default all-zeroes filled values")
    if (NOT DEFINED version)
        set(version 0.0.0 CACHE INTERNAL "Version String")
    endif ()
    if (NOT DEFINED revision)
        set(revision 0000000 CACHE INTERNAL "Revision String")
    endif ()
    if (NOT DEFINED symbols_space_name)
        message(FATAL_ERROR "You have to define variable named symbols_space_name to be able to use automatic version string generator")
    endif ()

    set(version_revision ${version}+g${revision} CACHE INTERNAL "Version Revision string")

    configure_file(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../res/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/generated/version.h @ONLY)
    configure_file(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../res/version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/generated/version.cpp @ONLY)

    message(STATUS "You are able to use fully qualified variable version_revision to refer to version and revision")
    message(STATUS "Don't forget to add CMAKE_CURRENT_BINARY_DIR/generated (which is ${CMAKE_CURRENT_BINARY_DIR}/generated) to your target include directories with PRIVATE BUILD_INTERFACE")
endfunction()

function(configure_birthday symbols_space_name)
    if (NOT DEFINED symbols_space_name)
        message(FATAL_ERROR "You have to define variable named symbols_space_name to be able to use automatic version string generator")
    endif ()

    string(TIMESTAMP birthday_string UTC)
    set(birthday ${birthday_string} CACHE INTERNAL "The date of birth")

    configure_file(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../res/birthday.h.in ${CMAKE_CURRENT_BINARY_DIR}/generated/birthday.h)
    configure_file(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../res/birthday.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/generated/birthday.cpp)

    message(STATUS "You are able to use variable birthday to refer to build date")
    message(STATUS "Don't forget to add CMAKE_CURRENT_BINARY_DIR/generated (which is ${CMAKE_CURRENT_BINARY_DIR}/generated) to your target include directories with PRIVATE BUILD_INTERFACE")
endfunction()
