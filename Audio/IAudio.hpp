#pragma once

#include <vector>
#include <memory>

template<typename T>
class IAudioDevice {
public:
    virtual ~IAudioDevice() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void mute() = 0;
    virtual void unmute() = 0;

    virtual std::vector<T> &getBuffer() noexcept = 0;
    virtual void clear() noexcept = 0;
};

template<typename Input, typename Output = Input>
class IAudio {
public:
    virtual ~IAudio() = default;

    virtual std::unique_ptr<IAudioDevice<Input>> &getInputDevice() = 0;
    virtual std::unique_ptr<IAudioDevice<Output>> &getOutputDevice() = 0;
};