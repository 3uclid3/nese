#pragma once

#include <nese/basic_types.hpp>
#include <nese/cpu/addr_mode.hpp>

namespace nese::cpu {

struct state;

constexpr addr_t stack_offset = 0x100;

namespace instruction {

using opcode_t = byte_t;
using operand_t = word_t;

using instruction::opcode_t;
using instruction::operand_t;

byte_t get_addr_mode_operand_byte_count(addr_mode mode);

string_view get_opcode_mnemonic(opcode_t opcode);
addr_mode get_opcode_addr_mode(opcode_t opcode);
byte_t get_opcode_operand_byte_count(opcode_t opcode);

bool execute(opcode_t opcode, state& state);

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

// JMP (Jump):
// Sets the program counter to the address specified by the operand, effectively jumping to a new code location.
template<addr_mode AddrModeT>
void execute_jmp(state& state);

// JSR (Jump to Subroutine):
// Pushes the address (minus one) of the next operation on to the stack and sets the program counter to the target address, for subroutine calls.
template<addr_mode AddrModeT>
void execute_jsr(state& state);

// LDA (Load Accumulator):
// Loads a value into the accumulator from memory or an immediate value, affecting the zero and negative flags.
template<addr_mode AddrModeT>
void execute_lda(state& state);

// LDX (Load X Register):
// Loads a value into the X register from memory or an immediate value, affecting the zero and negative flags.
template<addr_mode AddrModeT>
void execute_ldx(state& state);

// LDY (Load Y Register):
// Loads a value into the Y register from memory or an immediate value, affecting the zero and negative flags.
template<addr_mode AddrModeT>
void execute_ldy(state& state);

// STA (Store Accumulator):
// Stores the value in the accumulator into a specific location in memory.
template<addr_mode AddrModeT>
void execute_sta(state& state);

// STX (Store X Register):
// Stores the value in the X register into a specified memory location.
template<addr_mode AddrModeT>
void execute_stx(state& state);

// STY (Store Y Register):
// Stores the value in the Y register into a specified memory location.
template<addr_mode AddrModeT>
void execute_sty(state& state);

#define EXPLICIT_INSTANTIATION(mnemonic, addr_mode) \
    extern template void execute_##mnemonic<addr_mode>(state&)

#define EXPLICIT_INSTANTIATION_ALU_NO_IMMEDIATE(mnemonic) \
    EXPLICIT_INSTANTIATION(mnemonic, addr_mode::immediate); \
    EXPLICIT_INSTANTIATION(mnemonic, addr_mode::zero_page); \
    EXPLICIT_INSTANTIATION(mnemonic, addr_mode::zero_page_x); \
    EXPLICIT_INSTANTIATION(mnemonic, addr_mode::absolute); \
    EXPLICIT_INSTANTIATION(mnemonic, addr_mode::absolute_x); \
    EXPLICIT_INSTANTIATION(mnemonic, addr_mode::absolute_y); \
    EXPLICIT_INSTANTIATION(mnemonic, addr_mode::indexed_indirect); \
    EXPLICIT_INSTANTIATION(mnemonic, addr_mode::indirect_indexed)

#define EXPLICIT_INSTANTIATION_ALU(mnemonic) \
    EXPLICIT_INSTANTIATION(mnemonic, addr_mode::immediate); \
    EXPLICIT_INSTANTIATION_ALU_NO_IMMEDIATE(mnemonic)

EXPLICIT_INSTANTIATION_ALU(lda);

EXPLICIT_INSTANTIATION_ALU_NO_IMMEDIATE(sta);

EXPLICIT_INSTANTIATION(inx, addr_mode::implied);
EXPLICIT_INSTANTIATION(iny, addr_mode::implied);

EXPLICIT_INSTANTIATION(jmp, addr_mode::absolute);
EXPLICIT_INSTANTIATION(jsr, addr_mode::absolute);

EXPLICIT_INSTANTIATION(ldx, addr_mode::absolute);
EXPLICIT_INSTANTIATION(ldx, addr_mode::absolute_y);
EXPLICIT_INSTANTIATION(ldx, addr_mode::immediate);
EXPLICIT_INSTANTIATION(ldx, addr_mode::zero_page);
EXPLICIT_INSTANTIATION(ldx, addr_mode::zero_page_y);

EXPLICIT_INSTANTIATION(ldy, addr_mode::absolute);
EXPLICIT_INSTANTIATION(ldy, addr_mode::absolute_x);
EXPLICIT_INSTANTIATION(ldy, addr_mode::immediate);
EXPLICIT_INSTANTIATION(ldy, addr_mode::zero_page);
EXPLICIT_INSTANTIATION(ldy, addr_mode::zero_page_x);

EXPLICIT_INSTANTIATION(stx, addr_mode::zero_page);
EXPLICIT_INSTANTIATION(stx, addr_mode::zero_page_y);
EXPLICIT_INSTANTIATION(stx, addr_mode::absolute);

EXPLICIT_INSTANTIATION(sty, addr_mode::zero_page);
EXPLICIT_INSTANTIATION(sty, addr_mode::zero_page_x);
EXPLICIT_INSTANTIATION(sty, addr_mode::absolute);

#undef EXPLICIT_INSTANTIATION_ALU
#undef EXPLICIT_INSTANTIATION

} // namespace instruction
} // namespace nese::cpu
