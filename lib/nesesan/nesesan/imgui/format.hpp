#pragma once

#include <fmt/format.h>

namespace nese::san::imgui {

[[nodiscard]] const char* vformat(fmt::string_view format, fmt::format_args args);

template<typename... T>
[[nodiscard]] constexpr const char* format(fmt::format_string<T...> format, T&&... args)
{
    return san::imgui::vformat(format, fmt::make_format_args(args...));
}

} // namespace nese::san::imgui
