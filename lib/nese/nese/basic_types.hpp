#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

namespace nese {

template<typename T>
using ref_wrap = std::reference_wrapper<T>;

using string = std::string;
using string_view = std::string_view;

using s8_t = std::int8_t;
using s16_t = std::int16_t;
using s32_t = std::int32_t;
using s64_t = std::int64_t;

using u8_t = std::uint8_t;
using u16_t = std::uint16_t;
using u32_t = std::uint32_t;
using u64_t = std::uint64_t;

using f32_t = float;
using f64_t = double;

using size_t = std::size_t;

using byte_t = u8_t;
using word_t = u16_t;
using addr_t = word_t;

constexpr addr_t null_addr = addr_t{0};

} // namespace nese

#include <nese/cycle.hpp>
