#pragma once

#include <nese/basic_types.hpp>

namespace nese::cpu {

enum class status_flag
{
    carry = 1 << 0,     // Carry Flag
    zero = 1 << 1,      // Zero Flag
    interrupt = 1 << 2, // Interrupt Disable
    decimal = 1 << 3,   // Decimal Mode (not used in NES)
    break_cmd = 1 << 4, // Break Command
    unused = 1 << 5,    // Unused bit, always set to 1
    overflow = 1 << 6,  // Overflow Flag
    negative = 1 << 7   // Negative Flag
};

using status_flags = u8_t;

[[nodiscard]] constexpr status_flags operator|(status_flag lhs, status_flag rhs) noexcept
{
    // every static_cast is intentional
    return static_cast<status_flags>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

[[nodiscard]] constexpr status_flags operator|(status_flags lhs, status_flag rhs) noexcept
{
    // every static_cast is intentional
    return static_cast<status_flags>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

[[nodiscard]] constexpr status_flags operator|(status_flag lhs, status_flags rhs) noexcept
{
    // every static_cast is intentional
    return static_cast<status_flags>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

[[nodiscard]] constexpr status_flags operator&(status_flag lhs, status_flag rhs) noexcept
{
    // every static_cast is intentional
    return static_cast<status_flags>(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

[[nodiscard]] constexpr status_flags operator&(status_flags lhs, status_flag rhs) noexcept
{
    // every static_cast is intentional
    return static_cast<status_flags>(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

[[nodiscard]] constexpr status_flags operator&(status_flag lhs, status_flags rhs) noexcept
{
    // every static_cast is intentional
    return static_cast<status_flags>(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

[[nodiscard]] constexpr status_flag operator~(status_flag w) noexcept
{
    // every static_cast is intentional
    return static_cast<status_flag>(static_cast<u8_t>(~static_cast<unsigned int>(w)));
}

} // namespace nese
