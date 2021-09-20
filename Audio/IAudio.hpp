#pragma once

#include <vector>
#include <string>

class IAudio {
public:
    class exception : public std::exception
    {
    private:
        const std::string m_err;
    public:
        explicit exception(std::string err) : m_err(std::move(err)) {}
        ~exception() = default;
        const char *what() const noexcept override {return m_err.c_str();}
    };

    virtual ~IAudio() = default;

    virtual void play() = 0;
    virtual void stop() = 0;
    virtual void startRecord() = 0;
    virtual void stopRecord() = 0;
    virtual std::vector<short> &getInput() = 0;
    virtual std::vector<short> &getOutput() = 0;
};

constexpr auto FRAMES_PER_BUFFER = 64;
constexpr auto SAMPLE_RATE = 16000.0;
constexpr auto TABLE_SIZE = 200;