#pragma once

#include <vector>
#include <memory>

template<typename T>
class IAudioInputDevice {
public:
    virtual ~IAudioInputDevice() = default;

    virtual void startRecord() = 0;
    virtual void stopRecord() = 0;
    virtual std::vector<T> &getBuffer() = 0;
};

template<typename T>
class IAudioOutputDevice {
public:
    virtual ~IAudioOutputDevice() = default;

    virtual void play() = 0;
    virtual void stop() = 0;
    virtual std::vector<T> &getBuffer() = 0;
};

template<typename T>
class IAudio {
public:
    virtual ~IAudio() = default;

    virtual std::unique_ptr<IAudioInputDevice<T>> &getInputDevice() = 0;
    virtual std::unique_ptr<IAudioOutputDevice<T>> &getOutputDevice() = 0;
};

constexpr auto FRAMES_PER_BUFFER = 64;
constexpr auto SAMPLE_RATE = 16000.0;
constexpr auto TABLE_SIZE = 200;