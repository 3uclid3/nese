#pragma once

#include <nese/basic_types.hpp>
#include <nese/cpu/status_flag.hpp>

namespace nese::cpu {

// A: Accumulator - supports using status register for carrying, overflow detection, etc
// X: Index register. Used for addressing modes, loop counters
// Y: Same as X
// PC: Program counter. Supports 65535 direct (unbanked) memory locations.
// S: Stack pointer. Can be accessed using interrupts, pulls, pushes, and transfers.
// P: Status register - used by ALU unit
struct registers
{
    static constexpr u8_t default_a{0};
    static constexpr u8_t default_x{0};
    static constexpr u8_t default_y{0};
    static constexpr u16_t default_pc{0};
    static constexpr u8_t default_s{0xfd};
    static constexpr u8_t default_p{0x24};

    [[nodiscard]] constexpr bool is_flag_set(status_flag flag) const;
    [[nodiscard]] constexpr bool is_flag_clear(status_flag flag) const;

    constexpr void set_flag(status_flag flag, bool value);
    constexpr void set_flag(status_flag flag);
    constexpr void clear_flag(status_flag flag);

    u8_t a{default_a};
    u8_t x{default_x};
    u8_t y{default_y};
    u16_t pc{default_pc};
    u8_t s{default_s};
    u8_t p{default_p};
};

constexpr bool registers::is_flag_set(status_flag flag) const
{
    return (p & flag) != 0;
}

constexpr bool registers::is_flag_clear(status_flag flag) const
{
    return !is_flag_set(flag);
}

constexpr void registers::set_flag(status_flag flag, bool value)
{
    p = value ? p | flag : p & ~flag;
}

constexpr void registers::set_flag(status_flag flag)
{
    p = p | flag;
}

constexpr void registers::clear_flag(status_flag flag)
{
    p = p & ~flag;
}

} // namespace nese::cpu
