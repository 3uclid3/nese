#pragma once

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

}
