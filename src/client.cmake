add_executable(babel_client
        include/Audio/IAudio.hpp
        include/Audio/PortAudio.hpp
        include/Audio/PortAudioDevice.hpp
        include/Audio/PortAudioInputDevice.hpp
        include/Audio/PortAudioOutputDevice.hpp
        include/Network/ASIO.hpp
        include/Network/INetwork.hpp
        include/Network/QtNetwork.cpp
        include/Network/QtNetwork.hpp
        include/Serializer/IEncoder.hpp
        include/Serializer/Opus.hpp
        include/Exception.hpp
        include/Tools.hpp
        include/TypeChecker.hpp
        src/Client/Communication.cpp
        src/Client/MainWindow.cpp
        src/Client/main.cpp
)

target_include_directories(babel_client PRIVATE
        include
)

target_link_libraries(babel_client Qt5::Widgets Qt5::Network)

conan_target_link_libraries(babel_client CONAN_PKG::portaudio)