#pragma once

#include <portaudio.h>
#include "IAudio.hpp"
#include "Exception.hpp"

template<typename T>
class PortAudioOutputDevice : public IAudioOutputDevice<T>
{
private:
    std::vector<T> m_buffer{};
    PaStream *m_stream{};
    PaStreamParameters m_parameters;
public:
    PortAudioOutputDevice()
    {
        m_parameters.device = Pa_GetDefaultOutputDevice();
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

        m_parameters.suggestedLatency = Pa_GetDeviceInfo(m_parameters.device)->defaultLowOutputLatency;
        m_parameters.hostApiSpecificStreamInfo = NULL;

        if (auto err = Pa_OpenStream(
          &m_stream,
          nullptr,
          &m_parameters,
          SAMPLE_RATE,
          FRAMES_PER_BUFFER,
          paClipOff,
          playCallback,
          this) != paNoError)
            throw babel::exception(Pa_GetErrorText(err));
    }

    ~PortAudioOutputDevice() override = default;

    void play() override
    {
        if (auto err = Pa_StartStream(m_stream) != paNoError)
            throw babel::exception(Pa_GetErrorText(err));
    }

    void stop() override
    {
        if (auto err = Pa_StopStream(m_stream) != paNoError)
            throw babel::exception(Pa_GetErrorText(err));
    }

    std::vector<T> &getBuffer() override
    {
        return m_buffer;
    };

    static int playCallback(const void*, 
						void* pOutputBuffer, 
						unsigned long iFramesPerBuffer, 
						const PaStreamCallbackTimeInfo*, 
						PaStreamCallbackFlags, void *data)
    {
        T* pData = (T*) pOutputBuffer;
        auto c = static_cast<PortAudioOutputDevice *>(data);
	    size_t iOutput = 0;

	    if (pOutputBuffer == NULL)
		    return paComplete;

	    while (iOutput < iFramesPerBuffer)
	    {
            while (iOutput < iFramesPerBuffer) {
                if (c->m_buffer.empty())
                    break;
                pData[iOutput] = c->m_buffer.front();
                c->m_buffer.erase(c->m_buffer.begin());
		        iOutput++;
            }

            while (iOutput < iFramesPerBuffer) {
				pData[iOutput] = 0.0;
				iOutput++;
		    }
		    return paContinue;
	    }
	    return paContinue;
    }
};