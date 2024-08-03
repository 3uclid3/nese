#pragma once

#include <fmt/format.h>

namespace nese {

namespace format_circular_buffer {

static constexpr size_t size = 8192;
static constexpr size_t count = 16;

} // namespace format_circular_buffer

[[nodiscard]] const char* v_format(fmt::string_view format, fmt::format_args args);

template<typename... T>
[[nodiscard]] constexpr const char* format(fmt::format_string<T...> format, T&&... args)
{
    return v_format(format, fmt::make_format_args(args...));
}

} // namespace nese
