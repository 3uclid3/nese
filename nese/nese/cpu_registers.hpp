#pragma once

#include <nese/basic_types.hpp>
#include <nese/cpu_status.hpp>

namespace nese {

struct cpu_registers
{
    constexpr void set_flag(cpu_status_flag flag, bool value);

    u8_t a{0};   // Accumulator - supports using status register for carrying, overflow detection, etc
    u8_t x{0};   // Index register. Used for addressing modes, loop counters
    u8_t y{0};   // Same as X
    u16_t pc{0}; // Program counter. Supports 65535 direct (unbanked) memory locations.
    u8_t s{0};   // Stack pointer. Can be accessed using interrupts, pulls, pushes, and transfers.
    u8_t p{0};   // Status register - used by ALU unit
};

constexpr void cpu_registers::set_flag(cpu_status_flag flag, bool value)
{
    s = value ? s | flag : s & ~flag;
}

}
