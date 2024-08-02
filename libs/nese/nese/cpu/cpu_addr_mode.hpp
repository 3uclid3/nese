#pragma once

#include <magic_enum.hpp>

#include <nese/basic_types.hpp>

namespace nese {

enum class cpu_addr_mode : byte_t
{
    implied,          // Implied addressing mode (no operand)
    accumulator,      // Accumulator addressing mode (operand is the accumulator register)
    immediate,        // Immediate addressing mode (operand is a constant value)
    zero_page,        // Zero Page addressing mode (operand is in the first 256 bytes of memory)
    zero_page_x,      // Zero Page,X addressing mode (operand is in the zero page, indexed by the X register)
    zero_page_y,      // Zero Page,Y addressing mode (operand is in the zero page, indexed by the Y register)
    absolute,         // Absolute addressing mode (operand is a 16-bit memory address)
    absolute_x,       // Absolute,X addressing mode (operand is a 16-bit memory address, indexed by the X register)
    absolute_y,       // Absolute,Y addressing mode (operand is a 16-bit memory address, indexed by the Y register)
    relative,         // Relative addressing mode (operand is a signed 8-bit offset from the program counter)
    indirect,         // Indirect addressing mode (operand is a 16-bit memory address that points to the actual address)
    indexed_indirect, // Indexed Indirect addressing mode (operand is a zero page address, indexed by the X register)
    indirect_indexed, // Indirect Indexed addressing mode (operand is a zero page address, indexed indirectly by the Y register)

    count [[maybe_unused]]
};

} // namespace nese

template<>
struct magic_enum::customize::enum_range<nese::cpu_addr_mode>
{
    static constexpr bool is_flags = false;

    static constexpr int min = 0;
    static constexpr int max = static_cast<int>(nese::cpu_addr_mode::count);
};

namespace nese {

constexpr auto format_as(cpu_addr_mode status)
{
    return magic_enum::enum_name(status);
}

} // namespace nese
