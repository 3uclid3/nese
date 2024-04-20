#pragma once

#include <variant>

namespace nese {
    

template<typename T, typename... Args>
struct concat;

template<typename... Args0, typename... Args1>
struct concat<std::variant<Args0...>, Args1...>
{
    using type = std::variant<Args0..., Args1...>;
};

template<typename T, typename... Args>
using concat_t = typename concat<T, Args...>::type;

}
