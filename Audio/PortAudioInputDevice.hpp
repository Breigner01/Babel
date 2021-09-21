#pragma once

#include <portaudio.h>
#include "IAudio.hpp"
#include "Exception.hpp"

template<typename T>
class PortAudioInputDevice : public IAudioDevice<T>
{
private:
    std::vector<T> m_buffer{};
    PaStream *m_stream{};
    PaStreamParameters m_parameters;

    unsigned long m_framesPerBuffer;
    double m_sampleRate;
public:
    PortAudioInputDevice(unsigned long framesPerBuffer = 64, double sampleRate = 16000.0)
        : m_framesPerBuffer(framesPerBuffer), m_sampleRate(sampleRate)
    {
        m_parameters.device = Pa_GetDefaultInputDevice();
        if (auto err = m_parameters.device == paNoDevice)
            throw babel::exception(Pa_GetErrorText(err));
        m_parameters.channelCount = 1;

        /*if constexpr (std::is_same_v<T, unsigned char>)
            m_parameters.sampleFormat = paUInt8;
        else if constexpr (std::is_same_v<T, char>)
            m_parameters.sampleFormat = paInt8;
        else if constexpr (std::is_same_v<T, short>)*/
            m_parameters.sampleFormat = paInt16;
        /*else if constexpr (std::is_same_v<T, int>)
            m_parameters.sampleFormat = paInt32;
        else if constexpr (std::is_same_v<T, float>)
            m_parameters.sampleFormat = paFloat32;
        else
            static_assert(false, "Invalid template parameter to build PortAudio");*/

        m_parameters.suggestedLatency = Pa_GetDeviceInfo(m_parameters.device)->defaultLowInputLatency;
        m_parameters.hostApiSpecificStreamInfo = NULL;

        if (auto err = Pa_OpenStream(
            &m_stream,
            &m_parameters,
            NULL,
            m_sampleRate,
            m_framesPerBuffer,
            paClipOff,
            recordCallback,
            this) != paNoError)
            throw babel::exception(Pa_GetErrorText(err));
    }

    ~PortAudioInputDevice() override = default;

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

    void mute() override
    {

    }
    
    void unmute() override
    {

    }

    std::vector<T> &getBuffer() noexcept override
    {
        return m_buffer;
    }

    void clear() noexcept override
    {
        m_buffer.clear();
    }

    static int recordCallback(const void* pInputBuffer, 
							void*, 
							unsigned long iFramesPerBuffer, 
							const PaStreamCallbackTimeInfo*, 
							PaStreamCallbackFlags, void *data)
    {
        const T *pData = static_cast<const T *>(pInputBuffer);
        PortAudioInputDevice *c = static_cast<PortAudioInputDevice *>(data);

	    if (pInputBuffer == NULL)
		    return paContinue;

	    for (unsigned long i = 0; i < iFramesPerBuffer; i++)
            c->m_buffer.push_back(pData[i]);

	    return paContinue;
    }
};