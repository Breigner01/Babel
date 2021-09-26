#pragma once

#include <type_traits>

template<typename T, typename ...List>
concept TypeChecker = (std::is_same_v<T, List> || ...);