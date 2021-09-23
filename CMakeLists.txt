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
        src/main.cpp
        include/Exception.hpp
        include/Audio/IAudio.hpp
        include/Audio/PortAudio.hpp
        include/Audio/PortAudioDevice.hpp
        include/Audio/PortAudioInputDevice.hpp
        include/Audio/PortAudioOutputDevice.hpp
        include/Audio/PortAudioTypeChecker.hpp
        include/Serializer/IEncoder.hpp
        include/Serializer/Opus.hpp
)

target_include_directories(Babel PRIVATE
        ${CONAN_INCLUDE_DIRS}
        include
)

target_link_libraries(Babel ${CONAN_LIBS})