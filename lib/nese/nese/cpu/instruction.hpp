#pragma once

#include <nese/basic_types.hpp>
#include <nese/cpu/addr_mode.hpp>

namespace nese::cpu {

struct state;

namespace instruction {

using opcode_t = byte_t;
using operand_t = byte_t;

using instruction::opcode_t;
using instruction::operand_t;

string_view get_opcode_mnemonic(opcode_t opcode);
addr_mode get_opcode_addr_mode(opcode_t opcode);
byte_t get_opcode_operand_count(opcode_t opcode);

bool execute(opcode_t opcode, state& state);

template<addr_mode AddrModeT>
void execute_adc(state& state);

// DEY (Decrement X Register):
// Decreases the value in the X register by one, affecting the zero and negative flags.
template<addr_mode AddrModeT>
void execute_dex(state& state);

// DEY (Decrement Y Register):
// Decreases the value in the Y register by one, affecting the zero and negative flags.
template<addr_mode AddrModeT>
void execute_dey(state& state);

// INX (Increment Register):
// Increases the X register by one, affecting the zero and negative flags.
template<addr_mode AddrModeT>
void execute_inx(state& state);

// INX (Increment X Register):
// Increases the Y register by one, affecting the zero and negative flags.
template<addr_mode AddrModeT>
void execute_iny(state& state);

// LDA (Load Accumulator):
// Loads a value into the accumulator from memory or an immediate value, affecting the zero and negative flags.
template<addr_mode AddrModeT>
void execute_lda(state& state);

template<addr_mode AddrModeT>
void execute_jmp(state& state);

#define EXPLICIT_INSTANTIATION(mnemonic, addr_mode) \
    extern template void execute_##mnemonic<addr_mode>(state&)

#define EXPLICIT_INSTANTIATION_ALU(mnemonic) \
    EXPLICIT_INSTANTIATION(mnemonic, addr_mode::immediate); \
    EXPLICIT_INSTANTIATION(mnemonic, addr_mode::zero_page); \
    EXPLICIT_INSTANTIATION(mnemonic, addr_mode::zero_page_x); \
    EXPLICIT_INSTANTIATION(mnemonic, addr_mode::absolute); \
    EXPLICIT_INSTANTIATION(mnemonic, addr_mode::absolute_x); \
    EXPLICIT_INSTANTIATION(mnemonic, addr_mode::absolute_y); \
    EXPLICIT_INSTANTIATION(mnemonic, addr_mode::indexed_indirect); \
    EXPLICIT_INSTANTIATION(mnemonic, addr_mode::indirect_indexed)

EXPLICIT_INSTANTIATION_ALU(lda);

EXPLICIT_INSTANTIATION(adc, addr_mode::immediate);
EXPLICIT_INSTANTIATION(inx, addr_mode::implied);
EXPLICIT_INSTANTIATION(iny, addr_mode::implied);

#undef EXPLICIT_INSTANTIATION_ALU
#undef EXPLICIT_INSTANTIATION

} // namespace instruction
} // namespace nese::cpu
