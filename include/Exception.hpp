#pragma once

#include <exception>
#include <string>

namespace babel {

class exception : public std::exception
{
private:
    const std::string m_err;
public:
    explicit exception(std::string err) : m_err(std::move(err)) {}
    ~exception() override = default;
    [[nodiscard]] const char *what() const noexcept override {return m_err.c_str();}
};
}