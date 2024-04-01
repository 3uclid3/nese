#pragma once

#include <nese/basic_types.hpp>
#include <nese/cpu/instruction/opcode.hpp>
#include <nese/cpu/addr_mode.hpp>
#include <nese/utility/assert.hpp>

namespace nese::cpu::instruction {

constexpr byte_t get_operand_size(addr_mode mode)
{
    switch (mode)
    {
    case addr_mode::implied:
    case addr_mode::accumulator:
        return 0;

    case addr_mode::relative:
    case addr_mode::immediate:
    case addr_mode::zero_page:
    case addr_mode::zero_page_x:
    case addr_mode::zero_page_y:
    case addr_mode::indexed_indirect:
    case addr_mode::indirect_indexed:
        return 1;

    case addr_mode::indirect:
    case addr_mode::absolute:
    case addr_mode::absolute_x:
    case addr_mode::absolute_y:
        return 2;
    }

    NESE_ASSUME(false);
}

constexpr byte_t get_operand_size(opcode opcode)
{
    return get_operand_size(addr_modes[opcode]);
}
    
}
