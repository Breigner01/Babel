#include "PortAudio.hpp"
#include <iostream>

PortAudio::PortAudio()
{
    if (auto err = Pa_Initialize() != paNoError)
        throw IAudio::exception(Pa_GetErrorText(err));

    m_inputParameters.device = Pa_GetDefaultInputDevice();
    if (auto err = m_inputParameters.device == paNoDevice)
        throw IAudio::exception(Pa_GetErrorText(err));
    m_inputParameters.channelCount = 1;
    m_inputParameters.sampleFormat = paInt16;
    m_inputParameters.suggestedLatency = Pa_GetDeviceInfo(m_inputParameters.device)->defaultLowInputLatency;
    m_inputParameters.hostApiSpecificStreamInfo = NULL;

    m_outputParameters.device = Pa_GetDefaultOutputDevice();
    if (auto err = m_outputParameters.device == paNoDevice)
        throw IAudio::exception(Pa_GetErrorText(err));
    m_outputParameters.channelCount = 1;
    m_outputParameters.sampleFormat = paInt16;
    m_outputParameters.suggestedLatency = Pa_GetDeviceInfo(m_outputParameters.device)->defaultLowOutputLatency;
    m_outputParameters.hostApiSpecificStreamInfo = NULL;

    if (auto err = Pa_OpenStream(
          &m_inputStream,
          &m_inputParameters,
          NULL,
          SAMPLE_RATE,
          FRAMES_PER_BUFFER,
          paClipOff,
          RecordCallback,
          this) != paNoError)
        throw IAudio::exception(Pa_GetErrorText(err));
    if (auto err = Pa_SetStreamFinishedCallback(m_inputStream, paStreamFinished) != paNoError)
        throw IAudio::exception(Pa_GetErrorText(err));

    if (auto err = Pa_OpenStream(
          &m_outputStream,
          nullptr,
          &m_outputParameters,
          SAMPLE_RATE,
          FRAMES_PER_BUFFER,
          paClipOff,
          PlaybackCallback,
          this) != paNoError)
        throw IAudio::exception(Pa_GetErrorText(err));
    if (auto err = Pa_SetStreamFinishedCallback(m_outputStream, paStreamFinished) != paNoError)
        throw IAudio::exception(Pa_GetErrorText(err));
}

PortAudio::~PortAudio()
{
    Pa_Terminate();
}

int PortAudio::RecordCallback(const void* pInputBuffer, 
								void* pOutputBuffer, 
								unsigned long iFramesPerBuffer, 
								const PaStreamCallbackTimeInfo* timeInfo, 
								PaStreamCallbackFlags statusFlags, void *data)
{
	const short* pData = (const short*) pInputBuffer;
    auto c = static_cast<PortAudio *>(data);

	if (pInputBuffer == NULL)
	{
		std::cout << "AudioBuffer::RecordCallback, input buffer was NULL!" << std::endl;
		return paContinue;
	}

	// Copy all the frames over to our internal vector of samples
	for (unsigned long i = 0; i < iFramesPerBuffer; i++) {
        c->m_inputBuffer.push_back(pData[i]);
    }

	return paContinue;
}

int PortAudio::PlaybackCallback(const void* pInputBuffer, 
									void* pOutputBuffer, 
									unsigned long iFramesPerBuffer, 
									const PaStreamCallbackTimeInfo* timeInfo, 
									PaStreamCallbackFlags statusFlags, void *data)
{
	short* pData = (short*) pOutputBuffer;
    auto c = static_cast<PortAudio *>(data);
	size_t iOutput = 0;

	if (pOutputBuffer == NULL)
	{
		std::cout << "AudioBuffer::PlaybackCallback was NULL!" << std::endl;
		return paComplete;
	}

	while (iOutput < iFramesPerBuffer)
	{
        while (iOutput < iFramesPerBuffer) {
            if (c->m_outputBuffer.empty())
                break;
            pData[iOutput] = c->m_outputBuffer.front();
            c->m_outputBuffer.erase(c->m_outputBuffer.begin());
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

void PortAudio::paStreamFinishedMethod()
{
    //std::cout << "Stream Completed" << std::endl;
}

void PortAudio::paStreamFinished(void* userData)
{
    return ((PortAudio*)userData)->paStreamFinishedMethod();
}

void PortAudio::play()
{
    if (auto err = Pa_StartStream(m_outputStream) != paNoError)
        throw IAudio::exception(Pa_GetErrorText(err));
}

void PortAudio::stop()
{
    if (auto err = Pa_StopStream(m_outputStream) != paNoError)
        throw IAudio::exception(Pa_GetErrorText(err));
}

void PortAudio::startRecord()
{
    if (auto err = Pa_StartStream(m_inputStream) != paNoError)
        throw IAudio::exception(Pa_GetErrorText(err));
}

void PortAudio::stopRecord()
{
    if (auto err = Pa_StopStream(m_inputStream) != paNoError)
        throw IAudio::exception(Pa_GetErrorText(err));
}