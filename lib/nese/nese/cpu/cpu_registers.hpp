#pragma once

#include <nese/basic_types.hpp>
#include <nese/cpu/cpu_status.hpp>

namespace nese {

struct cpu_registers
{
    [[nodiscard]] constexpr bool is_status_set(cpu_status flag) const;
    [[nodiscard]] constexpr bool is_status_clear(cpu_status flag) const;

    constexpr void set_status(cpu_status flag, bool value);
    constexpr void set_status(cpu_status flag);
    constexpr void clear_status(cpu_status flag);

    byte_t a;
    byte_t x;
    byte_t y;
    word_t pc;
    byte_t sp;
    byte_t status;
};

constexpr bool cpu_registers::is_status_set(cpu_status flag) const
{
    return (status & flag) != cpu_status::none;
}

constexpr bool cpu_registers::is_status_clear(cpu_status flag) const
{
    return !is_status_set(flag);
}

constexpr void cpu_registers::set_status(cpu_status flag, bool value)
{
    status = static_cast<byte_t>(value ? status | flag : status & ~flag);
}

constexpr void cpu_registers::set_status(cpu_status flag)
{
    status = static_cast<byte_t>(status | flag);
}

constexpr void cpu_registers::clear_status(cpu_status flag)
{
    status = static_cast<byte_t>(status & ~flag);
}

} // namespace nese
