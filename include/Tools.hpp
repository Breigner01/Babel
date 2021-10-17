#pragma once

#include <string>
#include <vector>

namespace tools {
    inline std::string bufferToString(const std::vector<uint8_t> &data)
    {
        std::string str{};
        for (auto &d : data)
            str.push_back(d);
        return str;
    }

    inline std::vector<uint8_t> stringToBuffer(const std::string &data)
    {
        std::vector<uint8_t> vec{};
        for (auto &d : data)
            vec.push_back(d);
        return vec;
    }

    inline std::vector<std::string> stringToVector(const std::string &str, char separator)
    {
        std::vector<std::string> array{};
        std::string temp{};
        size_t len = str.size();

        for (size_t i = 0; i < len; i++) {
            if (str[i] == separator) {
                array.push_back(temp);
                temp.clear();
            }
            else
                temp.push_back(str[i]);
        }
        array.push_back(temp);
        return array;
    }
}