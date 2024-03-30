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
    const addr_t page_start = addr & 0xFF00;
    const addr_t new_page_start = new_addr & 0xFF00;

    return page_start != new_page_start;
}

template<addr_mode AddrModeT>
constexpr cpu_cycle_t get_addr_mode_cycle_cost(bool page_crossing = false)
{
    switch (AddrModeT)
    {
    case addr_mode::accumulator:
    case addr_mode::immediate:
        return cpu_cycle_t(2);

    case addr_mode::zero_page:
        return cpu_cycle_t(3);

    case addr_mode::zero_page_x:
    case addr_mode::zero_page_y:
    case addr_mode::absolute:
        return cpu_cycle_t(4);

    case addr_mode::absolute_x:
    case addr_mode::absolute_y:
        return cpu_cycle_t(page_crossing ? 5 : 4);

    case addr_mode::indexed_indirect:
        return cpu_cycle_t(6);

    case addr_mode::indirect_indexed:
        return cpu_cycle_t(page_crossing ? 6 : 5);
    }

    NESE_ASSUME(false);
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

        ADD(bcc, 0x90, addr_mode::relative);
        ADD(bcs, 0xb0, addr_mode::relative);
        ADD(beq, 0xf0, addr_mode::relative);
        ADD(bne, 0xd0, addr_mode::relative);
        ADD(bvs, 0x70, addr_mode::relative);

        ADD(bit, 0x24, addr_mode::zero_page);
        ADD(bit, 0x2c, addr_mode::absolute);

        ADD(clc, 0x18, addr_mode::implied);

        ADD(inx, 0xe8, addr_mode::implied);
        ADD(iny, 0xc8, addr_mode::implied);

        ADD(jmp, 0x4c, addr_mode::absolute);
        ADD(jsr, 0x20, addr_mode::absolute);

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

        ADD(nop, 0xea, addr_mode::implied);

        ADD(sec, 0x38, addr_mode::implied);

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

void push_byte(state& state, byte_t value)
{
    // stack grow top->down
    // no underflow/overflow detection
    state.memory.get().set_byte(state.registers.s + stack_offset, value);

    --state.registers.s;
}

byte_t pop_byte(state& state)
{
    ++state.registers.s;

    return state.memory.get().get_byte(state.registers.s + stack_offset);
}

void push_word(state& state, word_t value)
{
    // high-order bytes push first since the stack grow top->down and the machine is little-endian
    push_byte(state, value >> 8);
    push_byte(state, value & 0xff);
}

word_t pop_word(state& state)
{
    // low-order bytes pop first since the stack grow top->down and the machine is little-endian
    const byte_t lo = pop_byte(state);
    const byte_t hi = pop_byte(state);
    return static_cast<word_t>(hi << 8) + lo;
}

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
    const addr_t addr = static_cast<addr_t>(memory.get_byte(addr_arg)) + static_cast<addr_t>(static_cast<uint16_t>((memory.get_byte(addr_arg + 1) & 0xff)) << 8);
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
        return static_cast<byte_t>(operand);
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

void execute_branch(state& state, bool condition)
{
    const addr_t initial_pc = state.registers.pc;
    const s8_t byte = decode_byte(state);
    const addr_t target_pc = state.registers.pc + byte;

    if (condition)
    {
        // if branch succeeds ++
        state.cycle += cpu_cycle_t(1);
        state.registers.pc += byte;

        // if crossing to a new page ++
        if (is_page_crossing(initial_pc, target_pc))
        {
            state.cycle += cpu_cycle_t(1);
        }

        state.registers.pc = target_pc;
    }

    state.cycle += cpu_cycle_t(2);
}

template<addr_mode AddrModeT>
void execute_bcc(state& state)
{
    execute_branch(state, state.registers.is_flag_clear(status_flag::carry));
}

template<addr_mode AddrModeT>
void execute_bcs(state& state)
{
    execute_branch(state, state.registers.is_flag_set(status_flag::carry));
}

template<addr_mode AddrModeT>
void execute_beq(state& state)
{
    execute_branch(state, state.registers.is_flag_set(status_flag::zero));
}

template<addr_mode AddrModeT>
void execute_bit(state& state)
{
    bool page_crossing = false;
    const operand_t operand = decode_operand<AddrModeT>(state, page_crossing);
    const byte_t byte = read_operand<AddrModeT>(state, operand);
    const byte_t new_byte = byte & state.registers.a;

    state.registers.set_flag(status_flag::zero, new_byte == 0);
    state.registers.set_flag(status_flag::overflow, byte & 0x40);
    state.registers.set_flag(status_flag::negative, byte & 0x80);

    state.cycle += get_addr_mode_cycle_cost<AddrModeT>(page_crossing);
}

template<addr_mode AddrModeT>
void execute_bne(state& state)
{
    execute_branch(state, state.registers.is_flag_clear(status_flag::zero));
}

template<addr_mode AddrModeT>
void execute_bvs(state& state)
{
    execute_branch(state, state.registers.is_flag_set(status_flag::overflow));
}

template<addr_mode AddrModeT>
void execute_clc(state& state)
{
    state.registers.clear_flag(status_flag::carry);

    state.cycle += cpu_cycle_t(2);
}

template<addr_mode AddrModeT>
void execute_inx(state& state)
{
    state.registers.set_alu_flag(++state.registers.x);
    state.cycle += cpu_cycle_t(2);
}

template<addr_mode AddrModeT>
void execute_iny(state& state)
{
    state.registers.set_alu_flag(++state.registers.y);
    state.cycle += cpu_cycle_t(2);
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
    state.cycle += cpu_cycle_t(3);
}

template<addr_mode AddrModeT>
void execute_jsr(state& state)
{
    // we push the actual return address -1, which is the current place (before decoding the 16-bit addr) + 1
    push_word(state, state.registers.pc + 1);

    state.registers.pc = decode_operand_addr<AddrModeT>(state);
    state.cycle += cpu_cycle_t(6);
}

template<addr_mode AddrModeT>
void execute_ld_impl(state& state, byte_t& register_value)
{
    bool page_crossing{false};
    const operand_t operand = decode_operand<AddrModeT>(state, page_crossing);

    register_value = read_operand<AddrModeT>(state, operand);

    state.registers.set_alu_flag(register_value);
    state.cycle += get_addr_mode_cycle_cost<AddrModeT>(page_crossing);
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
void execute_nop(state& state [[maybe_unused]])
{
    state.cycle += cpu_cycle_t(2);
}

template<addr_mode AddrMode>
void execute_sec(state& state)
{
    state.registers.set_flag(status_flag::carry);

    state.cycle += cpu_cycle_t(2);
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

    state.cycle += get_addr_mode_cycle_cost<AddrModeT>(true);
}

template<addr_mode AddrModeT>
void execute_stx(state& state)
{
    execute_st_impl<AddrModeT>(state, state.registers.x);

    state.cycle += get_addr_mode_cycle_cost<AddrModeT>();
}

template<addr_mode AddrModeT>
void execute_sty(state& state)
{
    execute_st_impl<AddrModeT>(state, state.registers.y);

    state.cycle += get_addr_mode_cycle_cost<AddrModeT>();
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

EXPLICIT_INSTANTIATE(bcc, addr_mode::relative);
EXPLICIT_INSTANTIATE(bcs, addr_mode::relative);
EXPLICIT_INSTANTIATE(beq, addr_mode::relative);
EXPLICIT_INSTANTIATE(bne, addr_mode::relative);
EXPLICIT_INSTANTIATE(bvs, addr_mode::relative);

EXPLICIT_INSTANTIATE(bit, addr_mode::zero_page);
EXPLICIT_INSTANTIATE(bit, addr_mode::absolute);

EXPLICIT_INSTANTIATE(clc, addr_mode::implied);

EXPLICIT_INSTANTIATE(inx, addr_mode::implied);
EXPLICIT_INSTANTIATE(iny, addr_mode::implied);

EXPLICIT_INSTANTIATE(jmp, addr_mode::absolute);
EXPLICIT_INSTANTIATE(jsr, addr_mode::absolute);

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

EXPLICIT_INSTANTIATE(nop, addr_mode::implied);

EXPLICIT_INSTANTIATE(sec, addr_mode::implied);

EXPLICIT_INSTANTIATE(stx, addr_mode::zero_page);
EXPLICIT_INSTANTIATE(stx, addr_mode::zero_page_y);
EXPLICIT_INSTANTIATE(stx, addr_mode::absolute);

EXPLICIT_INSTANTIATE(sty, addr_mode::zero_page);
EXPLICIT_INSTANTIATE(sty, addr_mode::zero_page_x);
EXPLICIT_INSTANTIATE(sty, addr_mode::absolute);

#undef EXPLICIT_INSTANTIATE_ALU
#undef EXPLICIT_INSTANTIATE

} // namespace nese::cpu::instruction
