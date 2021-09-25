#pragma once

#include "IEncoder.hpp"
#include "Exception.hpp"
#include "TypeChecker.hpp"
#include <opus/opus.h>

#include <iostream>

template<typename Raw>
class Opus : public IEncoder<Raw, unsigned char>
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
        babel::TypeChecker<Raw, short, float>();
    
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

    std::vector<unsigned char> encodeFrame(const typename std::vector<Raw>::const_iterator& frame)
    {
        std::vector<unsigned char> output(m_framesPerBuffer * m_channelCount * sizeof(Raw));
        int err = -1;

        if constexpr (std::is_same_v<Raw, short>)
            err = opus_encode(m_encoder, &*frame, m_framesPerBuffer, output.data(), output.size());
        else if constexpr (std::is_same_v<Raw, float>)
            err = opus_encode_float(m_encoder, &*frame, m_framesPerBuffer, output.data(), output.size());

        if (err < 0)
            throw babel::exception(opus_strerror(err));
        output.resize(err);
        return output;
    }

    std::vector<std::vector<unsigned char>> encode(const std::vector<Raw> &input) override
    {
        std::vector<std::vector<unsigned char>> encoded{};

        for (size_t i = 0; i < input.size(); i += m_framesPerBuffer)
            encoded.push_back(encodeFrame(input.begin() + i));
        return encoded;
    }

    std::vector<Raw> decodeFrame(const std::vector<unsigned char> &frame)
    {
        std::vector<Raw> output(m_framesPerBuffer * m_channelCount);
        int err = -1;

        if constexpr (std::is_same_v<Raw, short>)
            err = opus_decode(m_decoder, frame.data(), frame.size(), output.data(), m_framesPerBuffer, 0);
        else if constexpr (std::is_same_v<Raw, float>)
            err = opus_decode_float(m_decoder, frame.data(), frame.size(), output.data(), m_framesPerBuffer, 0);

        if (err < 0)
            throw babel::exception(opus_strerror(err));
        return output;
    }

    std::vector<Raw> decode(const std::vector<std::vector<unsigned char>> &input) override
    {
        std::vector<Raw> output{};

	    for (const auto &i : input) {
            auto dec = decodeFrame(i);
            for (const auto &d : dec)
                output.push_back(d);
        }
        return output;
    }
};