#pragma once

#include <type_traits>

template<typename T, typename ...List>
constexpr void PortAudioTypeChecker() noexcept
{
    static_assert((std::is_same_v<T, List> || ...), "Invalid template parameter to build PortAudio");
}