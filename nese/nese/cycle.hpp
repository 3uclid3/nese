#pragma once

#include <chrono>

#include <nese/basic_types.hpp>

namespace nese {

// Given that 1 CPU cycle = 3 PPU cycle, we'll count in terms of PPU cycle
using cycle_t = std::chrono::duration<s64_t, std::ratio<1, 1>>;

 // 3 cycle_t = 3 PPU cycle = 1 CPU cycle
using cpu_cycle_t = std::chrono::duration<s64_t, std::ratio<3, 1>>;

// 1 cycle_t = 1 PPU cycle = 1/3 CPU cycle
using ppu_cycle_t = std::chrono::duration<s64_t, std::ratio<1, 1>>; 

// NES 6502 CPU 21.477272 / 12 MHz
// NES PPU 21.477272 / 4 MHz
static inline constexpr cycle_t clock_hz{21477272ll / 4};

static constexpr cycle_t ms_to_cycle(s64_t ms)
{
    return {clock_hz / 1000 * ms};
}

} // namespace nese
