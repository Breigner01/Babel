add_executable(babel_server
        ../../include/Network/ASIO.hpp
        ../../include/Network/INetwork.hpp
        ../../include/Exception.hpp
        ../../include/Tools.hpp
        ../../include/TypeChecker.hpp
        main.cpp
)

target_include_directories(babel_server PRIVATE
        ../../include
)

conan_target_link_libraries(babel_server CONAN_PKG::portaudio)

#add_custom_command(TARGET babel_server POST_BUILD
#        COMMAND cp bin/babel_server ${CMAKE_CURRENT_LIST_DIR})