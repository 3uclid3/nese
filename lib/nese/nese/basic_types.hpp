#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

namespace nese {

template<typename T, std::size_t SizeT>
using array = std::array<T, SizeT>;

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

template<typename T>
constexpr T clamp(const T& v, const T& min, const T& max)
{
    return std::max<T>(min, std::min<T>(max, v));
}

//
// Given that 1 CPU cycle = 3 PPU cycle, we'll count in terms of PPU cycle
// 1 nes_cycle_t = 1 PPU cycle = 1/3 CPU cycle
// 3 nes_cycle_t = 3 PPU cycle = 1 CPU cycle
//
using cycle_t = std::chrono::duration<s64_t, std::ratio<1, 1>>;
using cpu_cycle_t = std::chrono::duration<s64_t, std::ratio<3, 1>>;
using ppu_cycle_t = std::chrono::duration<s64_t, std::ratio<1, 1>>;

//
// NES 6502 CPU 21.477272 / 12 MHz
// NES PPU 21.477272 / 4 MHz
//
static inline constexpr cycle_t clock_hz{21477272ll / 4};
static inline constexpr ppu_cycle_t ppu_scanline_cycle(341);

static constexpr cycle_t ms_to_cycle(s64_t ms)
{
    return {clock_hz / 1000 * ms};
}

} // namespace nese
