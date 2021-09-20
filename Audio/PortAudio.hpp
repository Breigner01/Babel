#pragma once

#include "IAudio.hpp"
#include <portaudio.h>

class PortAudio : public IAudio
{
private:
    PaStreamParameters m_inputParameters;
    PaStreamParameters m_outputParameters;
    PaStream *m_inputStream{};
    PaStream *m_outputStream{};
    std::vector<short> m_inputBuffer{};
    std::vector<short> m_outputBuffer{};
public:
    PortAudio();
    ~PortAudio() override;
    void play() override;
    void stop() override;
    void startRecord() override;
    void stopRecord() override;
    std::vector<short> &getInput() override {return m_inputBuffer;}
    std::vector<short> &getOutput() override {return m_outputBuffer;}

    // Class implementation

    static int RecordCallback(const void* pInputBuffer, 
								void* pOutputBuffer, 
								unsigned long iFramesPerBuffer, 
								const PaStreamCallbackTimeInfo* timeInfo, 
								PaStreamCallbackFlags statusFlags, void *data);

    static int PlaybackCallback(const void* pInputBuffer, 
									void* pOutputBuffer, 
									unsigned long iFramesPerBuffer, 
									const PaStreamCallbackTimeInfo* timeInfo, 
									PaStreamCallbackFlags statusFlags, void *data);

    void paStreamFinishedMethod();
    static void paStreamFinished(void* userData);
};