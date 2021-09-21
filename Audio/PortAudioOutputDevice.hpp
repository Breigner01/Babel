#pragma once

#include <portaudio.h>
#include "IAudio.hpp"
#include "Exception.hpp"

template<typename T>
class PortAudioOutputDevice : public IAudioDevice<T>
{
private:
    std::vector<T> m_buffer{};
    PaStream *m_stream{};
    PaStreamParameters m_parameters;

    unsigned long m_framesPerBuffer;
    double m_sampleRate;
public:
    PortAudioOutputDevice(unsigned long framesPerBuffer = 64, double sampleRate = 16000.0)
        : m_framesPerBuffer(framesPerBuffer), m_sampleRate(sampleRate)
    {
        m_parameters.device = Pa_GetDefaultOutputDevice();
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

        m_parameters.suggestedLatency = Pa_GetDeviceInfo(m_parameters.device)->defaultLowOutputLatency;
        m_parameters.hostApiSpecificStreamInfo = NULL;

        if (auto err = Pa_OpenStream(
          &m_stream,
          nullptr,
          &m_parameters,
          m_sampleRate,
          m_framesPerBuffer,
          paClipOff,
          playCallback,
          this) != paNoError)
            throw babel::exception(Pa_GetErrorText(err));
    }

    ~PortAudioOutputDevice() override = default;

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

    static int playCallback(const void*, 
						void* pOutputBuffer, 
						unsigned long iFramesPerBuffer, 
						const PaStreamCallbackTimeInfo*, 
						PaStreamCallbackFlags, void *data)
    {
        T *pData = static_cast<T *>(pOutputBuffer);
        PortAudioOutputDevice *c = static_cast<PortAudioOutputDevice *>(data);
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