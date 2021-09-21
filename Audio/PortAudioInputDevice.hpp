#pragma once

#include <portaudio.h>
#include "IAudio.hpp"
#include "Exception.hpp"

template<typename T>
class PortAudioInputDevice : public IAudioInputDevice<T>
{
private:
    std::vector<T> m_buffer{};
    PaStream *m_stream{};
    PaStreamParameters m_parameters;
public:
    PortAudioInputDevice()
    {
        m_parameters.device = Pa_GetDefaultInputDevice();
        if (auto err = m_parameters.device == paNoDevice)
            throw babel::exception(Pa_GetErrorText(err));
        m_parameters.channelCount = 1;

        if constexpr (std::is_same_v<T, uint8_t>)
            m_parameters.sampleFormat = paUInt8;
        else if constexpr (std::is_same_v<T, int8_t>)
            m_parameters.sampleFormat = paInt8;
        else if constexpr (std::is_same_v<T, short>)
            m_parameters.sampleFormat = paInt16;
        else if constexpr (std::is_same_v<T, int32_t>)
            m_parameters.sampleFormat = paInt32;
        else if constexpr (std::is_same_v<T, float>)
            m_parameters.sampleFormat = paFloat32;
        else
            throw babel::exception("Invalid template parameter to build PortAudio");

        m_parameters.suggestedLatency = Pa_GetDeviceInfo(m_parameters.device)->defaultLowInputLatency;
        m_parameters.hostApiSpecificStreamInfo = NULL;

        if (auto err = Pa_OpenStream(
            &m_stream,
            &m_parameters,
            NULL,
            SAMPLE_RATE,
            FRAMES_PER_BUFFER,
            paClipOff,
            recordCallback,
            this) != paNoError)
            throw babel::exception(Pa_GetErrorText(err));
    }

    ~PortAudioInputDevice() override = default;

    void startRecord() override
    {
        if (auto err = Pa_StartStream(m_stream) != paNoError)
            throw babel::exception(Pa_GetErrorText(err));
    }

    void stopRecord() override
    {
        if (auto err = Pa_StopStream(m_stream) != paNoError)
            throw babel::exception(Pa_GetErrorText(err));
    }

    std::vector<T> &getBuffer() override
    {
        return m_buffer;
    };

    static int recordCallback(const void* pInputBuffer, 
							void*, 
							unsigned long iFramesPerBuffer, 
							const PaStreamCallbackTimeInfo*, 
							PaStreamCallbackFlags, void *data)
    {
        const T* pData = (const T*) pInputBuffer;
        auto c = static_cast<PortAudioInputDevice *>(data);

	    if (pInputBuffer == NULL)
		    return paContinue;

	    for (unsigned long i = 0; i < iFramesPerBuffer; i++)
            c->m_buffer.push_back(pData[i]);

	    return paContinue;
    }
};