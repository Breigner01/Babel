cmake_minimum_required(VERSION 3.17)

project(Babel)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

set(CONAN_DISABLE_CHECK_COMPILER 1)

if (${CMAKE_BUILD_TYPE} MATCHES "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT IPO_Supported OUTPUT ErrorMessage)

    if (IPO_Supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
        message(STATUS "Interprocedural optimizations enabled")
    endif ()
endif ()

conan_basic_setup()

find_package(Qt5Widgets CONFIG REQUIRED)

if (UNIX)
    add_compile_options(-Wall -Wextra -Weffc++)
else ()
    add_compile_options(/W4)
endif ()

add_executable(babel
        src/main.cpp
)

target_include_directories(babel PRIVATE
        include
)

conan_target_link_libraries(babel ${CONAN_LIBS})