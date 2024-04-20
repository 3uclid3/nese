#pragma once

#include <nese/basic_types.hpp>

namespace nese {

struct bus;

namespace nintendulator {

enum class format_flag : u8_t
{
    pc = 1 << 0,
    instruction_bytes = 1 << 1,
    opcode = 1 << 2,
    operand = 1 << 3,
    registers = 1 << 4,
    ppu = 1 << 5,
    cycle = 1 << 6,

    none = 0,
    all = pc | instruction_bytes | opcode | operand | registers | ppu | cycle
};

using format_flags = format_flag;

[[nodiscard]] constexpr format_flags operator|(format_flag lhs, format_flag rhs) noexcept
{
    // every static_cast is intentional
    return static_cast<format_flags>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

[[nodiscard]] constexpr format_flags operator|(u8_t lhs, format_flag rhs) noexcept
{
    return static_cast<format_flags>(lhs) | rhs;
}

[[nodiscard]] constexpr format_flags operator&(format_flag lhs, format_flag rhs) noexcept
{
    // every static_cast is intentional
    return static_cast<format_flags>(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

[[nodiscard]] constexpr format_flags operator&(u8_t lhs, format_flag rhs) noexcept
{
    return static_cast<format_flags>(lhs) & rhs;
}

[[nodiscard]] constexpr format_flag operator~(format_flag w) noexcept
{
    // every static_cast is intentional
    return static_cast<format_flag>(static_cast<u8_t>(~static_cast<unsigned int>(w)));
}

const char* format(const bus& bus, format_flags flags = format_flag::all);

} // namespace nintendulator

} // namespace nese
