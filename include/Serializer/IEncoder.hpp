#pragma once

#include <vector>

template<typename Raw, typename Compressed = Raw>
class IEncoder
{
public:
    virtual ~IEncoder() = default;

    virtual std::vector<std::vector<Compressed>> encode(const std::vector<Raw> &buffer) = 0;
    virtual std::vector<Raw> decode(const std::vector<std::vector<Compressed>> &buffer) = 0;
};