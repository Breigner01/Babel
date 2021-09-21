#pragma once

#include "IAudio.hpp"
#include "Exception.hpp"
#include <portaudio.h>
#include "PortAudioInputDevice.hpp"
#include "PortAudioOutputDevice.hpp"

template<typename T>
class PortAudio : public IAudio<T>
{
private:
    PaError m_error;
    std::unique_ptr<IAudioInputDevice<T>> m_inputDevice;
    std::unique_ptr<IAudioOutputDevice<T>> m_outputDevice;
public:
    PortAudio()
    {
        m_error = Pa_Initialize();
        if (m_error != paNoError)
            throw babel::exception(Pa_GetErrorText(m_error));

        m_inputDevice = std::make_unique<PortAudioInputDevice<T>>();
        m_outputDevice = std::make_unique<PortAudioOutputDevice<T>>();
    }

    ~PortAudio() override
    {
        if (m_error == paNoError)
            Pa_Terminate();
    }

    std::unique_ptr<IAudioInputDevice<T>> &getInputDevice() override
    {
        return m_inputDevice;
    }

    std::unique_ptr<IAudioOutputDevice<T>> &getOutputDevice() override
    {
        return m_outputDevice;
    }
};