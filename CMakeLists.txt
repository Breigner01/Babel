cmake_minimum_required(VERSION 3.17)

project(Babel)

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
        include
)

target_include_directories(babel SYSTEM PRIVATE
        ${CONAN_INCLUDE_DIRS}
)

find_package(Qt5Network REQUIRED)

target_link_libraries(babel ${CONAN_LIBS} Qt5::Network)