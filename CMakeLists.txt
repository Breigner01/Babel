cmake_minimum_required(VERSION 3.17)

project(Babel)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

set(CONAN_DISABLE_CHECK_COMPILER 1)

conan_basic_setup()

if (UNIX)
    add_compile_options(-Wall -Wextra -Weffc++)
else ()
    add_compile_options(/W4)
endif ()

add_executable(babel
        src/main.cpp
        include/Exception.hpp
        include/TypeChecker.hpp
        include/Audio/IAudio.hpp
        include/Audio/PortAudio.hpp
        include/Audio/PortAudioDevice.hpp
        include/Audio/PortAudioInputDevice.hpp
        include/Audio/PortAudioOutputDevice.hpp
        include/Network/ASIO.hpp
        include/Network/INetwork.hpp
        include/Network/QtNetwork.hpp
        include/Serializer/IEncoder.hpp
        include/Serializer/Opus.hpp
)

target_include_directories(babel PRIVATE
        ${CONAN_INCLUDE_DIRS}
        include
)

target_link_libraries(babel ${CONAN_LIBS})