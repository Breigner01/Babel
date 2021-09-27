#pragma once

#include <vector>
#include <memory>

template<typename T>
class IAudioDevice {
public:
    virtual ~IAudioDevice() = default;

    /**
     *  Start the audio stream
     */
    virtual void start() = 0;
    /**
     *  Stop the audio stream
     */
    virtual void stop() = 0;
    /**
     *  Mute the audio stream
     */
    virtual void mute() noexcept = 0;
    /**
     *  Unmute the audio stream
     */
    virtual void unmute() noexcept = 0;
    /**
     *  @return the current buffer of the audio device
     */
    virtual std::vector<T> getBuffer() const noexcept = 0;
    /**
     *  @return the current buffer of the audio device and clear the buffer inside the class
     */
    virtual std::vector<T> popBuffer() noexcept = 0;
    /**
     *  Replace the audio device buffer by a new one
     *
     *  @param buffer that will replace the audio device buffer
     */
    virtual void setBuffer(std::vector<T> buffer) noexcept = 0;
    /**
     *  Push the buffer at the end of the audio device buffer
     *
     *  @param buffer that will be pushed at the end of the audio device buffer
     */
    virtual void pushBuffer(const std::vector<T> &buffer) = 0;
    /**
     *  Clear the audio device buffer
     */
    virtual void clearBuffer() noexcept = 0;
};

template<typename Input, typename Output = Input>
class IAudio {
public:
    virtual ~IAudio() = default;

    /**
     *  @return the input audio device
     */
    virtual std::unique_ptr<IAudioDevice<Input>> &getInputDevice() = 0;
    /**
     *  @return the output audio device
     */
    virtual std::unique_ptr<IAudioDevice<Output>> &getOutputDevice() = 0;
};