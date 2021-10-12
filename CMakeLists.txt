cmake_minimum_required(VERSION 3.17)

project(Babel)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

set(CONAN_DISABLE_CHECK_COMPILER 1)

conan_basic_setup(TARGETS)

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

if (UNIX)
    add_compile_options(-Wall -Wextra -Weffc++)
else ()
    add_compile_options(/W4)
endif ()

add_executable(babel_client
        src/Client/main.cpp
        src/Client/Communication.cpp
        src/Client/MainWindow.cpp
        include/Network/QtNetwork.cpp
)

target_include_directories(babel_client PRIVATE
        include
)

target_link_libraries(babel_client Qt5::Widgets Qt5::Network)

conan_target_link_libraries(babel_client CONAN_PKG::portaudio)

add_executable(babel_server
        src/Server/main.cpp
)

target_include_directories(babel_server PRIVATE
        include
)

conan_target_link_libraries(babel_server CONAN_PKG::portaudio)