#pragma once

#include <nese/utility/assert.hpp>

namespace nese::cpu {

enum class addr_mode
{
    // Implied addressing mode (no operand)
    implied,

    // Accumulator addressing mode (operand is the accumulator register)
    accumulator,

    // Immediate addressing mode (operand is a constant value)
    immediate,

    // Zero Page addressing mode (operand is in the first 256 bytes of memory)
    zero_page,

    // Zero Page,X addressing mode (operand is in the zero page, indexed by the X register)
    zero_page_x,

    // Zero Page,Y addressing mode (operand is in the zero page, indexed by the Y register)
    zero_page_y,

    // Absolute addressing mode (operand is a 16-bit memory address)
    absolute,

    // Absolute,X addressing mode (operand is a 16-bit memory address, indexed by the X register)
    absolute_x,

    // Absolute,Y addressing mode (operand is a 16-bit memory address, indexed by the Y register)
    absolute_y,

    // Relative addressing mode (operand is a signed 8-bit offset from the program counter)
    relative,

    // Indirect addressing mode (operand is a 16-bit memory address that points to the actual address)
    indirect,

    // Indexed Indirect addressing mode (operand is a zero page address, indexed by the X register)
    indexed_indirect,

    // Indirect Indexed addressing mode (operand is a zero page address, indexed indirectly by the Y register)
    indirect_indexed
};

constexpr std::string_view to_string_view(addr_mode mode)
{
    switch (mode)
    {
    case addr_mode::implied:
        return "implied";

    case addr_mode::accumulator:
        return "accumulator";

    case addr_mode::immediate:
        return "immediate";

    case addr_mode::zero_page:
        return "zero_page";

    case addr_mode::zero_page_x:
        return "zero_page_x";

    case addr_mode::zero_page_y:
        return "zero_page_y";

    case addr_mode::absolute:
        return "absolute";

    case addr_mode::absolute_x:
        return "absolute_x";

    case addr_mode::absolute_y:
        return "absolute_y";

    case addr_mode::relative:
        return "relative";

    case addr_mode::indirect:
        return "indirect";

    case addr_mode::indexed_indirect:
        return "indexed_indirect";

    case addr_mode::indirect_indexed:
        return "indirect_indexed";
    }

    NESE_ASSUME(false);
}

} // namespace nese::cpu
