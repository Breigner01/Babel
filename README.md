# Babel

The project consists in a client/server architecture voice over IP application, similar to Skype or TeamSpeak.

## Build and Run the Project

First you need to install [CMake](https://cmake.org/download/) and [Conan](https://conan.io/downloads.html).

For this project you need to create a profile with conan that uses C++11 as the minimum C++ standard.

You need to add a remote for Conan as a dependency used in this project requires it, so just run:
```shell
conan remote add bincrafters https://bincrafters.jfrog.io/artifactory/api/conan/public-conan
```

Then you need to install the dependencies for the project using Conan.
We recommend creating a build folder at the root of the repository in which everything will.

In the build folder you execute:
```shell
conan install .. --build=missing
cmake ..
cmake --build .
```

You will then find the `babel` executable at the root of the repository.


## Mandatory part

### Contents
- [ ] Documentation of binary protocol communications
- [ ] UML compliant class diagram (client and server)
- [ ] Network abstraction (Asio, Qt Network)
- [ ] Qt GUI client
- [ ] PortAudio (19.7.0) -> abstraction
- [ ] Opus (1.3.1) -> abstraction
- [ ] Contact list
- [ ] Make a call
- [ ] Hang up