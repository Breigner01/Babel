#pragma once

#include <type_traits>

namespace babel {

template<typename T, typename ...List>
constexpr void TypeChecker() noexcept
{
    static_assert((std::is_same_v<T, List> || ...), "Bad template parameter");
}
}