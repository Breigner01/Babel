#pragma once

#include <portaudio.h>
#include "IAudio.hpp"
#include "Exception.hpp"
#include "PortAudioInputDevice.hpp"
#include "PortAudioOutputDevice.hpp"

template<typename Input, typename Output = Input>
class PortAudio : public IAudio<Input, Output>
{
private:
    PaError m_error{};
    std::unique_ptr<IAudioDevice<Input>> m_inputDevice{};
    std::unique_ptr<IAudioDevice<Output>> m_outputDevice{};
public:
    PortAudio()
    {
        m_error = Pa_Initialize();
        if (m_error != paNoError)
            throw babel::exception("PortAudio : ", Pa_GetErrorText(m_error));

        m_inputDevice = std::make_unique<PortAudioInputDevice<Input>>();
        m_outputDevice = std::make_unique<PortAudioOutputDevice<Output>>();
    }

    ~PortAudio() override
    {
        if (m_error == paNoError)
            Pa_Terminate();
    }

    std::unique_ptr<IAudioDevice<Input>> &getInputDevice() override
    {
        return m_inputDevice;
    }

    std::unique_ptr<IAudioDevice<Output>> &getOutputDevice() override
    {
        return m_outputDevice;
    }
};