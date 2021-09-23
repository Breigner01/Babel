#pragma once

#include <mutex>
#include <portaudio.h>
#include "IAudio.hpp"
#include "Exception.hpp"
#include "PortAudioTypeChecker.hpp"

template<typename T>
class PortAudioDevice : public IAudioDevice<T>
{
protected:
    std::vector<T> m_buffer{};
    mutable std::mutex m_mutex{};
    PaStream *m_stream{};
    PaStreamParameters m_parameters{};

    unsigned long m_framesPerBuffer;
    double m_sampleRate;
    bool m_muted = false;
public:

    PortAudioDevice(const PortAudioDevice &) = delete;
    PortAudioDevice &operator=(const PortAudioDevice &) = delete;

    explicit PortAudioDevice(unsigned long framesPerBuffer = 120, double sampleRate = 48000.0, int channelCount = 1)
        : m_framesPerBuffer(framesPerBuffer), m_sampleRate(sampleRate)
    {
        PortAudioTypeChecker<T, unsigned char, char, short, int, float>();

        m_parameters.channelCount = channelCount;
        m_parameters.hostApiSpecificStreamInfo = nullptr;
        if constexpr (std::is_same_v<T, unsigned char>)
            m_parameters.sampleFormat = paUInt8;
        else if constexpr (std::is_same_v<T, char>)
            m_parameters.sampleFormat = paInt8;
        else if constexpr (std::is_same_v<T, short>)
            m_parameters.sampleFormat = paInt16;
        else if constexpr (std::is_same_v<T, int>)
            m_parameters.sampleFormat = paInt32;
        else if constexpr (std::is_same_v<T, float>)
            m_parameters.sampleFormat = paFloat32;
    }
    ~PortAudioDevice() override = default;

    void start() override
    {
        if (auto err = Pa_StartStream(m_stream) != paNoError)
            throw babel::exception(Pa_GetErrorText(err));
    }

    void stop() override
    {
        if (auto err = Pa_StopStream(m_stream) != paNoError)
            throw babel::exception(Pa_GetErrorText(err));
    }

    void mute() noexcept override
    {
        m_muted = true;
    }
    
    void unmute() noexcept override
    {
        m_muted = false;
    }

    std::vector<T> getBuffer() const noexcept override
    {
        std::scoped_lock lock(m_mutex);
        return m_buffer;
    }

    std::vector<T> popBuffer() noexcept override
    {
        std::scoped_lock lock(m_mutex);
        return std::move(m_buffer);
    }

    void setBuffer(std::vector<T> other) noexcept override
    {
        std::scoped_lock lock(m_mutex);
        m_buffer = std::move(other);
    }

    void pushBuffer(const std::vector<T> &other) override
    {
        std::scoped_lock lock(m_mutex);
        for (const auto &o : other)
            m_buffer.push_back(o);
    }

    void clearBuffer() noexcept override
    {
        std::scoped_lock lock(m_mutex);
        m_buffer.clear();
    }
};