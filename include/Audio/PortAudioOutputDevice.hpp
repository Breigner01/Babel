#pragma once

#include "PortAudioDevice.hpp"

template<typename T>
class PortAudioOutputDevice : public PortAudioDevice<T>
{
public:
    explicit PortAudioOutputDevice(unsigned long framesPerBuffer = 120, double sampleRate = 48000.0, int channelCount = 1)
        : PortAudioDevice<T>(framesPerBuffer, sampleRate, channelCount)
    {
        this->m_parameters.device = Pa_GetDefaultOutputDevice();
        if (auto err = this->m_parameters.device == paNoDevice)
            throw babel::exception(Pa_GetErrorText(err));
        this->m_parameters.suggestedLatency = Pa_GetDeviceInfo(this->m_parameters.device)->defaultLowOutputLatency;

        if (auto err = Pa_OpenStream(
            &this->m_stream,
            nullptr,
            &this->m_parameters,
            this->m_sampleRate,
            this->m_framesPerBuffer,
            paClipOff,
            playCallback,
            this) != paNoError)
            throw babel::exception(Pa_GetErrorText(err));
    }

    ~PortAudioOutputDevice() override = default;

    static int playCallback(const void*, 
						    void* pOutputBuffer, 
						    unsigned long iFramesPerBuffer, 
						    const PaStreamCallbackTimeInfo*, 
						    PaStreamCallbackFlags, void *data)
    {
        T *pData = static_cast<T *>(pOutputBuffer);
        PortAudioOutputDevice *c = static_cast<PortAudioOutputDevice *>(data);
        std::scoped_lock lock(c->m_mutex);
	    size_t iOutput = 0;

        if (!pOutputBuffer) {
            return paContinue;
        }
        else if (c->m_muted) {
            while (iOutput < iFramesPerBuffer) {
				pData[iOutput] = 0;
				iOutput++;
		    }
        }
        else {
            while (iOutput < iFramesPerBuffer) {
                if (c->m_buffer.empty())
                    break;
                pData[iOutput] = c->m_buffer.front();
                c->m_buffer.erase(c->m_buffer.begin());
                iOutput++;
            }
            while (iOutput < iFramesPerBuffer) {
                pData[iOutput] = 0;
                iOutput++;
            }
        }
        return paContinue;
    }
};