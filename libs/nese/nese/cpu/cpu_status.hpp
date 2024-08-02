#pragma once

#include <magic_enum.hpp>

#include <nese/basic_types.hpp>

namespace nese {

enum class cpu_status : byte_t
{
    none = 0,
    carry = 1 << 0,     // Carry Flag
    zero = 1 << 1,      // Zero Flag
    interrupt = 1 << 2, // Interrupt Disable
    decimal = 1 << 3,   // Decimal Mode (not used in NES)
    break_cmd = 1 << 4, // Break Command
    unused = 1 << 5,    // Unused bit, always set to 1
    overflow = 1 << 6,  // Overflow Flag
    negative = 1 << 7   // Negative Flag
};

[[nodiscard]] constexpr cpu_status operator|(cpu_status lhs, cpu_status rhs) noexcept
{
    // every static_cast is intentional
    return static_cast<cpu_status>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

[[nodiscard]] constexpr cpu_status operator|(u8_t lhs, cpu_status rhs) noexcept
{
    return static_cast<cpu_status>(lhs) | rhs;
}

[[nodiscard]] constexpr cpu_status operator&(cpu_status lhs, cpu_status rhs) noexcept
{
    // every static_cast is intentional
    return static_cast<cpu_status>(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

[[nodiscard]] constexpr cpu_status operator&(u8_t lhs, cpu_status rhs) noexcept
{
    return static_cast<cpu_status>(lhs) & rhs;
}

[[nodiscard]] constexpr cpu_status operator~(cpu_status w) noexcept
{
    // every static_cast is intentional
    return static_cast<cpu_status>(static_cast<u8_t>(~static_cast<unsigned int>(w)));
}

} // namespace nese

template<>
struct magic_enum::customize::enum_range<nese::cpu_status>
{
    static constexpr bool is_flags = true;
};

namespace nese {

constexpr auto format_as(cpu_status status)
{
    return magic_enum::enum_name(status);
}

} // namespace nese
