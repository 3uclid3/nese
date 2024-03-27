#include <nese/cpu/instruction.hpp>

#include <array>

#include <nese/cpu/state.hpp>
#include <nese/memory/mapper.hpp>
#include <nese/utility/assert.hpp>
#include <nese/utility/log.hpp>

namespace nese::cpu::instruction {

enum alu_opcode_base : u8_t
{
    alu_opcode_base_ora = 0x00,
    alu_opcode_base_and = 0x20,
    alu_opcode_base_eor = 0x40,
    alu_opcode_base_adc = 0x60,
    alu_opcode_base_sta = 0x80,
    alu_opcode_base_lda = 0xA0,
    alu_opcode_base_cmp = 0xC0,
    alu_opcode_base_sbc = 0xE0
};

enum alu_opcode_offset : u8_t
{
    alu_opcode_offset_immediate = 0x9,
    alu_opcode_offset_zero_page = 0x5,
    alu_opcode_offset_zero_page_x = 0x15,
    alu_opcode_offset_absolute = 0xd,
    alu_opcode_offset_absolute_x = 0x1d,
    alu_opcode_offset_absolute_y = 0x19,
    alu_opcode_offset_indexed_indirect = 0x1,
    alu_opcode_offset_indirect_indexed = 0x11
};

enum rmw_opcode_base : u8_t
{
    rmw_opcode_base_asl = 0x00,
    rmw_opcode_base_rol = 0x20,
    rmw_opcode_base_lsr = 0x40,
    rmw_opcode_base_ror = 0x60
};

constexpr bool is_page_crossing(addr_t addr, addr_t new_addr)
{
    return ((addr & 0xff00) != (new_addr & 0xff00));
}

using callback = void (*)(state&);

struct table
{
    static constexpr size_t capacity = 256;

    static consteval table create()
    {
        table table{};

#define ADD(mnemonic, opcode, addr_mode) \
    table.addr_modes[opcode] = addr_mode; \
    table.callbacks[opcode] = &execute_##mnemonic<addr_mode>; \
    table.mnemonics[opcode] = #mnemonic

#define ALU_OPCODE(mnemonic, addr_mode) \
    (static_cast<std::size_t>(alu_opcode_base_##mnemonic) + static_cast<std::size_t>(alu_opcode_offset_##addr_mode))

#define ADD_ALU_NO_IMMEDIATE(mnemonic) \
    ADD(mnemonic, ALU_OPCODE(mnemonic, zero_page), addr_mode::zero_page); \
    ADD(mnemonic, ALU_OPCODE(mnemonic, zero_page_x), addr_mode::zero_page_x); \
    ADD(mnemonic, ALU_OPCODE(mnemonic, absolute), addr_mode::absolute); \
    ADD(mnemonic, ALU_OPCODE(mnemonic, absolute_x), addr_mode::absolute_x); \
    ADD(mnemonic, ALU_OPCODE(mnemonic, absolute_y), addr_mode::absolute_y); \
    ADD(mnemonic, ALU_OPCODE(mnemonic, indexed_indirect), addr_mode::indexed_indirect); \
    ADD(mnemonic, ALU_OPCODE(mnemonic, indirect_indexed), addr_mode::indirect_indexed)

#define ADD_ALU(mnemonic) \
    ADD(mnemonic, ALU_OPCODE(mnemonic, immediate), addr_mode::immediate); \
    ADD_ALU_NO_IMMEDIATE(mnemonic)

        ADD_ALU(lda);

        ADD_ALU_NO_IMMEDIATE(sta);

        ADD(inx, 0xe8, addr_mode::implied);
        ADD(iny, 0xc8, addr_mode::implied);

        ADD(jmp, 0x4c, addr_mode::absolute);

        ADD(ldx, 0xa2, addr_mode::immediate);
        ADD(ldx, 0xa6, addr_mode::zero_page);
        ADD(ldx, 0xae, addr_mode::absolute);
        ADD(ldx, 0xb6, addr_mode::zero_page_y);
        ADD(ldx, 0xbe, addr_mode::absolute_y);

        ADD(ldy, 0xa0, addr_mode::immediate);
        ADD(ldy, 0xa4, addr_mode::zero_page);
        ADD(ldy, 0xac, addr_mode::absolute);
        ADD(ldy, 0xb4, addr_mode::zero_page_x);
        ADD(ldy, 0xbc, addr_mode::absolute_x);

        ADD(stx, 0x86, addr_mode::zero_page);
        ADD(stx, 0x96, addr_mode::zero_page_y);
        ADD(stx, 0x8e, addr_mode::absolute);

        ADD(sty, 0x84, addr_mode::zero_page);
        ADD(sty, 0x94, addr_mode::zero_page_x);
        ADD(sty, 0x8c, addr_mode::absolute);

#undef ADD_ALU
#undef ALU_OPCODE
#undef ADD

        return table;
    }

    std::array<addr_mode, capacity> addr_modes;
    std::array<callback, capacity> callbacks;
    std::array<string_view, capacity> mnemonics;
};

constexpr table table{table::create()};

byte_t decode_byte(state& state)
{
    const memory::mapper& memory = state.memory;
    const byte_t decoded = memory.get_byte(state.registers.pc);

    state.registers.pc += 1;

    return decoded;
}

word_t decode_word(state& state)
{
    const memory::mapper& memory = state.memory;
    const word_t decoded = memory.get_word(state.registers.pc);

    state.registers.pc += 2;

    return decoded;
}

template<addr_mode AddrModeT>
addr_t decode_operand_addr(state& state, bool& page_crossing);

template<>
addr_t decode_operand_addr<addr_mode::zero_page>(state& state, bool& page_crossing [[maybe_unused]])
{
    return decode_byte(state);
}

template<>
addr_t decode_operand_addr<addr_mode::zero_page_x>(state& state, bool& page_crossing [[maybe_unused]])
{
    return (decode_byte(state) + state.registers.x) & 0xff;
}

template<>
addr_t decode_operand_addr<addr_mode::zero_page_y>(state& state, bool& page_crossing [[maybe_unused]])
{
    return (decode_byte(state) + state.registers.y) & 0xff;
}

template<>
addr_t decode_operand_addr<addr_mode::absolute>(state& state, bool& page_crossing [[maybe_unused]])
{
    return decode_word(state);
}

template<>
addr_t decode_operand_addr<addr_mode::absolute_x>(state& state, bool& page_crossing)
{
    const addr_t addr = decode_word(state);
    const addr_t new_addr = addr + state.registers.x;

    page_crossing = is_page_crossing(addr, new_addr);

    return new_addr;
}

template<>
addr_t decode_operand_addr<addr_mode::absolute_y>(state& state, bool& page_crossing)
{
    const addr_t addr = decode_word(state);
    const addr_t new_addr = addr + state.registers.y;

    page_crossing = is_page_crossing(addr, new_addr);

    return new_addr;
}

template<>
addr_t decode_operand_addr<addr_mode::indexed_indirect>(state& state, bool& page_crossing [[maybe_unused]])
{
    const memory::mapper& memory = state.memory;
    const byte_t addr = decode_byte(state);
    return memory.get_byte((addr + state.registers.x) & 0xff) + static_cast<uint16_t>((memory.get_byte((addr + state.registers.x + 1) & 0xff)) << 8);
}

template<>
addr_t decode_operand_addr<addr_mode::indirect_indexed>(state& state, bool& page_crossing)
{
    const memory::mapper& memory = state.memory;
    const byte_t addr_arg = decode_byte(state);
    const addr_t addr = memory.get_byte(addr_arg) + (static_cast<uint16_t>((memory.get_byte(addr_arg + 1) & 0xff)) << 8);
    const addr_t new_addr = addr + state.registers.y;

    page_crossing = is_page_crossing(addr, new_addr);

    return new_addr;
}

template<addr_mode AddrModeT>
addr_t decode_operand_addr(state& state)
{
    bool page_crossing{false};
    return decode_operand_addr<AddrModeT>(state, page_crossing);
}

template<addr_mode AddrModeT>
operand_t decode_operand(state& state, bool& page_crossing)
{
    if constexpr (AddrModeT == addr_mode::accumulator)
    {
        return 0;
    }

    if constexpr (AddrModeT == addr_mode::immediate)
    {
        return decode_byte(state);
    }

    return decode_operand_addr<AddrModeT>(state, page_crossing);
}

template<addr_mode AddrModeT>
addr_t decode_operand(state& state)
{
    bool page_crossing{false};
    return decode_operand<AddrModeT>(state, page_crossing);
}

template<addr_mode AddrModeT>
byte_t read_operand(state& state, operand_t operand)
{
    if constexpr (AddrModeT == addr_mode::accumulator)
    {
        return state.registers.a;
    }

    if constexpr (AddrModeT == addr_mode::immediate)
    {
        return operand;
    }

    return state.memory.get().get_byte(operand);
}

template<addr_mode AddrModeT>
void write_operand(state& state, operand_t operand, byte_t value)
{
    if constexpr (AddrModeT == addr_mode::accumulator)
    {
        state.registers.a = value;
    }
    else
    {
        state.memory.get().set_byte(operand, value);
    }
}

byte_t get_addr_mode_operand_byte_count(addr_mode mode)
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

string_view get_opcode_mnemonic(opcode_t opcode)
{
    return table.mnemonics[opcode];
}

addr_mode get_opcode_addr_mode(opcode_t opcode)
{
    return table.addr_modes[opcode];
}

byte_t get_opcode_operand_byte_count(opcode_t opcode)
{
    return get_addr_mode_operand_byte_count(table.addr_modes[opcode]);
}

bool execute(opcode_t opcode, state& state)
{
    const auto callback = table.callbacks[opcode];

    if (callback == nullptr) [[unlikely]]
    {
        NESE_ERROR("[instruction_set] Unimplemented opcode {:02X}", opcode);
        return false;
    }

    callback(state);
    return true;
}

template<addr_mode AddrModeT>
void execute_inx(state& state)
{
    state.registers.set_alu_flag(++state.registers.x);
}

template<addr_mode AddrModeT>
void execute_iny(state& state)
{
    state.registers.set_alu_flag(++state.registers.y);
}

template<addr_mode AddrModeT>
void execute_jmp(state& state)
{
    const addr_t new_addr = decode_operand_addr<AddrModeT>(state);

    if (new_addr == state.registers.pc - 1)
    {
        // TODO Infinite loop
    }

    state.registers.pc = new_addr;
}

template<addr_mode AddrModeT>
void execute_ld_impl(state& state, byte_t& register_value)
{
    const operand_t operand = decode_operand<AddrModeT>(state);

    register_value = read_operand<AddrModeT>(state, operand);

    state.registers.set_alu_flag(register_value);
}

template<addr_mode AddrModeT>
void execute_lda(state& state)
{
    execute_ld_impl<AddrModeT>(state, state.registers.a);
}

template<addr_mode AddrModeT>
void execute_ldx(state& state)
{
    execute_ld_impl<AddrModeT>(state, state.registers.x);
}

template<addr_mode AddrModeT>
void execute_ldy(state& state)
{
    execute_ld_impl<AddrModeT>(state, state.registers.y);
}

template<addr_mode AddrModeT>
void execute_st_impl(state& state, byte_t value)
{
    const operand_t operand = decode_operand<AddrModeT>(state);

    state.memory.get().set_byte(operand, value);
}

template<addr_mode AddrModeT>
void execute_sta(state& state)
{
    execute_st_impl<AddrModeT>(state, state.registers.a);
}

template<addr_mode AddrModeT>
void execute_stx(state& state)
{
    execute_st_impl<AddrModeT>(state, state.registers.x);
}

template<addr_mode AddrModeT>
void execute_sty(state& state)
{
    execute_st_impl<AddrModeT>(state, state.registers.y);
}

#define EXPLICIT_INSTANTIATE(mnemonic, addr_mode) \
    template void execute_##mnemonic<addr_mode>(state & state)

#define EXPLICIT_INSTANTIATE_ALU_NO_IMMEDIATE(mnemonic) \
    EXPLICIT_INSTANTIATE(mnemonic, addr_mode::zero_page); \
    EXPLICIT_INSTANTIATE(mnemonic, addr_mode::zero_page_x); \
    EXPLICIT_INSTANTIATE(mnemonic, addr_mode::absolute); \
    EXPLICIT_INSTANTIATE(mnemonic, addr_mode::absolute_x); \
    EXPLICIT_INSTANTIATE(mnemonic, addr_mode::absolute_y); \
    EXPLICIT_INSTANTIATE(mnemonic, addr_mode::indexed_indirect); \
    EXPLICIT_INSTANTIATE(mnemonic, addr_mode::indirect_indexed)

#define EXPLICIT_INSTANTIATE_ALU(mnemonic) \
    EXPLICIT_INSTANTIATE(mnemonic, addr_mode::immediate); \
    EXPLICIT_INSTANTIATE_ALU_NO_IMMEDIATE(mnemonic)

EXPLICIT_INSTANTIATE_ALU(lda);

EXPLICIT_INSTANTIATE_ALU_NO_IMMEDIATE(sta);

EXPLICIT_INSTANTIATE(inx, addr_mode::implied);
EXPLICIT_INSTANTIATE(iny, addr_mode::implied);

EXPLICIT_INSTANTIATE(jmp, addr_mode::absolute);

EXPLICIT_INSTANTIATE(ldx, addr_mode::absolute);
EXPLICIT_INSTANTIATE(ldx, addr_mode::absolute_y);
EXPLICIT_INSTANTIATE(ldx, addr_mode::immediate);
EXPLICIT_INSTANTIATE(ldx, addr_mode::zero_page);
EXPLICIT_INSTANTIATE(ldx, addr_mode::zero_page_y);

EXPLICIT_INSTANTIATE(ldy, addr_mode::absolute);
EXPLICIT_INSTANTIATE(ldy, addr_mode::absolute_x);
EXPLICIT_INSTANTIATE(ldy, addr_mode::immediate);
EXPLICIT_INSTANTIATE(ldy, addr_mode::zero_page);
EXPLICIT_INSTANTIATE(ldy, addr_mode::zero_page_x);

EXPLICIT_INSTANTIATE(stx, addr_mode::zero_page);
EXPLICIT_INSTANTIATE(stx, addr_mode::zero_page_y);
EXPLICIT_INSTANTIATE(stx, addr_mode::absolute);

EXPLICIT_INSTANTIATE(sty, addr_mode::zero_page);
EXPLICIT_INSTANTIATE(sty, addr_mode::zero_page_x);
EXPLICIT_INSTANTIATE(sty, addr_mode::absolute);

#undef EXPLICIT_INSTANTIATE_ALU
#undef EXPLICIT_INSTANTIATE

} // namespace nese::cpu::instruction
