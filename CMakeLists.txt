cmake_minimum_required(VERSION 3.17)

project(Babel)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

set(CONAN_DISABLE_CHECK_COMPILER 1)

conan_basic_setup(TARGETS)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

if (UNIX)
    add_compile_options(-Wall -Wextra -Weffc++)
else ()
    add_compile_options(/W4)
endif ()

add_subdirectory(src/Client)
add_subdirectory(src/Server)

add_dependencies(babel_client babel_server)

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
        ${CMAKE_CURRENT_LIST_DIR}/babel_client
        ${CMAKE_CURRENT_LIST_DIR}/babel_server
)