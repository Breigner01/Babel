#pragma once

#include "PortAudioDevice.hpp"

template<typename T>
class PortAudioInputDevice : public PortAudioDevice<T>
{
public:
    explicit PortAudioInputDevice(unsigned long framesPerBuffer = 120, double sampleRate = 48000.0, int channelCount = 1)
        : PortAudioDevice<T>(framesPerBuffer, sampleRate, channelCount)
    {
        this->m_parameters.device = Pa_GetDefaultInputDevice();
        if (auto err = this->m_parameters.device == paNoDevice)
            throw babel::exception(Pa_GetErrorText(err));
        this->m_parameters.suggestedLatency = Pa_GetDeviceInfo(this->m_parameters.device)->defaultLowInputLatency;

        if (auto err = Pa_OpenStream(
            &this->m_stream,
            &this->m_parameters,
            nullptr,
            this->m_sampleRate,
            this->m_framesPerBuffer,
            paClipOff,
            recordCallback,
            this) != paNoError)
            throw babel::exception(Pa_GetErrorText(err));
    }

    ~PortAudioInputDevice() override = default;

    static int recordCallback(const void* pInputBuffer, 
							  void*, 
							  unsigned long iFramesPerBuffer, 
							  const PaStreamCallbackTimeInfo*, 
						      PaStreamCallbackFlags, void *data)
    {
        const T *pData = static_cast<const T *>(pInputBuffer);
        PortAudioInputDevice *c = static_cast<PortAudioInputDevice *>(data);
        std::scoped_lock lock(c->m_mutex);

        if (!pInputBuffer) {
            return paContinue;
        }
        else if (c->m_muted) {
            for (unsigned long i = 0; i < iFramesPerBuffer; i++)
                c->m_buffer.push_back(0);
        }
        else {
            for (unsigned long i = 0; i < iFramesPerBuffer; i++)
                c->m_buffer.push_back(pData[i]);
        }

	    return paContinue;
    }
};