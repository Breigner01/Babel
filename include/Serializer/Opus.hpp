#pragma once

#include "IEncoder.hpp"
#include "Exception.hpp"
#include <opus/opus.h>

#include <iostream>

class Opus : public IEncoder<short, unsigned char>
{
private:
    OpusEncoder	*m_encoder{};
	OpusDecoder	*m_decoder{};
    int m_error{};
    int m_framesPerBuffer;
    int m_channelCount;
public:

    Opus(const Opus &) = delete;
    Opus &operator=(const Opus &) = delete;

    explicit Opus(int framesPerBuffer = 120, int sampleRate = 48000, int channelCount = 1)
        : m_framesPerBuffer(framesPerBuffer), m_channelCount(channelCount)
    {
        m_encoder = opus_encoder_create(sampleRate, m_channelCount, OPUS_APPLICATION_VOIP, &m_error);
    	if (m_error != OPUS_OK)
	    	throw babel::exception(opus_strerror(m_error));

        m_decoder = opus_decoder_create(sampleRate, m_channelCount, &m_error);
        if (m_error != OPUS_OK)
		    throw babel::exception(opus_strerror(m_error));
    }

    ~Opus() override
    {
        if (m_encoder)
            opus_encoder_destroy(m_encoder);
        if (m_decoder)
            opus_decoder_destroy(m_decoder);
    }

    std::vector<unsigned char> encodeFrame(const std::vector<short>::const_iterator& frame)
    {
        std::vector<unsigned char> output(m_framesPerBuffer * m_channelCount * sizeof(short));

        int err = opus_encode(m_encoder,
			&*frame,
			m_framesPerBuffer,
			output.data(),
			output.size());

        if (err < 0)
            throw babel::exception(opus_strerror(err));
        output.resize(err);
        return output;
    }

    std::vector<std::vector<unsigned char>> encode(const std::vector<short> &input) override
    {
        std::vector<std::vector<unsigned char>> encoded{};

        for (size_t i = 0; i < input.size(); i += m_framesPerBuffer)
            encoded.push_back(encodeFrame(input.begin() + i));
        return encoded;
    }

    std::vector<short> decodeFrame(const std::vector<unsigned char> &frame)
    {
        std::vector<short> output(m_framesPerBuffer * m_channelCount);
        auto err = opus_decode(m_decoder, frame.data(), frame.size(),
                            output.data(), m_framesPerBuffer, 0);
        if (err < 0)
            throw babel::exception(opus_strerror(err));

        return output;
    }

    std::vector<short> decode(const std::vector<std::vector<unsigned char>> &input) override
    {
        std::vector<short> output{};

	    for (const auto &i : input) {
            auto dec = decodeFrame(i);
            for (const auto &d : dec)
                output.push_back(d);
        }
        return output;
    }
};