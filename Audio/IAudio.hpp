#pragma once

#include <vector>
#include <memory>

template<typename T>
class IAudioDevice {
public:
    virtual ~IAudioDevice() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void mute() noexcept = 0;
    virtual void unmute() noexcept = 0;

    virtual std::vector<T> getBuffer() const noexcept = 0;
    virtual std::vector<T> popBuffer() noexcept = 0;
    virtual void setBuffer(std::vector<T>) noexcept = 0;
    virtual void pushBuffer(const std::vector<T> &) = 0;
    virtual void clearBuffer() noexcept = 0;
};

template<typename Input, typename Output = Input>
class IAudio {
public:
    virtual ~IAudio() = default;

    virtual std::unique_ptr<IAudioDevice<Input>> &getInputDevice() = 0;
    virtual std::unique_ptr<IAudioDevice<Output>> &getOutputDevice() = 0;
};