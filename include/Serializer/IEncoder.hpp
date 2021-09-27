#pragma once

#include <vector>

template<typename Raw, typename Compressed = Raw>
class IEncoder
{
public:
    virtual ~IEncoder() = default;

    /**
     *  Encode an audio frame
     *
     *  @param buffer that will be encoded
     *  @return encoded audio
     */
    virtual std::vector<std::vector<Compressed>> encode(const std::vector<Raw> &buffer) = 0;
    /**
     *  Decode an audio frame
     *
     *  @param buffer that will be decoded
     *  @return decoded audio
     */
    virtual std::vector<Raw> decode(const std::vector<std::vector<Compressed>> &buffer) = 0;
};