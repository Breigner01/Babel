cmake_minimum_required(VERSION 3.17)

project(Babel)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

conan_basic_setup()

if (UNIX)
    add_compile_options(-Wall -Wextra -Weffc++)
else ()
    add_compile_options(/W4)
endif ()

add_executable(Babel
        Audio/Exception.hpp
        Audio/IAudio.hpp
        Audio/main.cpp
        Audio/PortAudio.hpp
        Audio/PortAudioInputDevice.hpp
        Audio/PortAudioOutputDevice.hpp
        Audio/PortAudioTypeChecker.hpp
)

target_include_directories(Babel PRIVATE
        ${CONAN_INCLUDE_DIRS}
        Audio
)

target_link_libraries(Babel ${CONAN_LIBS})