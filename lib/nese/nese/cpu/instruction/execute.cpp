#include <nese/cpu/instruction/execute.hpp>

#include <nese/cpu/addr_mode.hpp>
#include <nese/cpu/instruction/execute_context.hpp>
#include <nese/cpu/instruction/opcode.hpp>
#include <nese/memory/mapper.hpp>
#include <nese/utility/log.hpp>

namespace nese::cpu::instruction {

using execute_callback = void (*)(execute_context);

struct execute_callback_table
{
    [[nodiscard]] constexpr const execute_callback& operator[](opcode op) const
    {
        return callbacks[static_cast<size_t>(op)];
    }

    [[nodiscard]] constexpr execute_callback& operator[](opcode op)
    {
        return callbacks[static_cast<size_t>(op)];
    }

    std::array<execute_callback, 256> callbacks{};
};

constexpr addr_t stack_offset = 0x100;

constexpr bool is_page_crossing(addr_t addr, addr_t new_addr)
{
    const addr_t page_start = addr & 0xFF00;
    const addr_t new_page_start = new_addr & 0xFF00;

    return page_start != new_page_start;
}

constexpr bool is_zero(byte_t value)
{
    return value == 0;
}

constexpr bool is_negative(byte_t value)
{
    return value & 0x80;
}

constexpr bool is_overflow(byte_t value)
{
    return value & 0x40;
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

void push_byte(execute_context ctx, byte_t value)
{
    // stack grow top->down
    // no underflow/overflow detection
    ctx.memory().set_byte(ctx.registers().s + stack_offset, value);

    --ctx.registers().s;
}

byte_t pop_byte(execute_context ctx)
{
    ++ctx.registers().s;

    return ctx.memory().get_byte(ctx.registers().s + stack_offset);
}

void push_word(execute_context ctx, word_t value)
{
    // high-order bytes push first since the stack grow top->down and the machine is little-endian
    push_byte(ctx, value >> 8);
    push_byte(ctx, value & 0xff);
}

word_t pop_word(execute_context ctx)
{
    // low-order bytes pop first since the stack grow top->down and the machine is little-endian
    const byte_t lo = pop_byte(ctx);
    const byte_t hi = pop_byte(ctx);
    return static_cast<word_t>(hi << 8) + lo;
}

byte_t decode_byte(execute_context ctx)
{
    const byte_t decoded = ctx.memory().get_byte(ctx.registers().pc);

    // cppcheck-suppress unreadVariable
    ctx.registers().pc += 1;

    return decoded;
}

word_t decode_word(execute_context ctx)
{
    const word_t decoded = ctx.memory().get_word(ctx.registers().pc);

    // cppcheck-suppress unreadVariable
    ctx.registers().pc += 2;

    return decoded;
}

template<addr_mode AddrModeT>
addr_t decode_operand_addr(execute_context ctx, bool& page_crossing);

template<>
addr_t decode_operand_addr<addr_mode::zero_page>(execute_context ctx, bool& page_crossing [[maybe_unused]])
{
    return decode_byte(ctx);
}

template<>
addr_t decode_operand_addr<addr_mode::zero_page_x>(execute_context ctx, bool& page_crossing [[maybe_unused]])
{
    return (decode_byte(ctx) + ctx.registers().x) & 0xff;
}

template<>
addr_t decode_operand_addr<addr_mode::zero_page_y>(execute_context ctx, bool& page_crossing [[maybe_unused]])
{
    return (decode_byte(ctx) + ctx.registers().y) & 0xff;
}

template<>
addr_t decode_operand_addr<addr_mode::absolute>(execute_context ctx, bool& page_crossing [[maybe_unused]])
{
    return decode_word(ctx);
}

template<>
addr_t decode_operand_addr<addr_mode::absolute_x>(execute_context ctx, bool& page_crossing)
{
    const addr_t addr = decode_word(ctx);
    const addr_t new_addr = addr + ctx.registers().x;

    page_crossing = is_page_crossing(addr, new_addr);

    return new_addr;
}

template<>
addr_t decode_operand_addr<addr_mode::absolute_y>(execute_context ctx, bool& page_crossing)
{
    const addr_t addr = decode_word(ctx);
    const addr_t new_addr = addr + ctx.registers().y;

    page_crossing = is_page_crossing(addr, new_addr);

    return new_addr;
}

template<>
addr_t decode_operand_addr<addr_mode::indexed_indirect>(execute_context ctx, bool& page_crossing [[maybe_unused]])
{
    const byte_t addr = decode_byte(ctx);
    return ctx.memory().get_byte((addr + ctx.registers().x) & 0xff) + static_cast<uint16_t>(ctx.memory().get_byte((addr + ctx.registers().x + 1) & 0xff) << 8);
}

template<>
addr_t decode_operand_addr<addr_mode::indirect_indexed>(execute_context ctx, bool& page_crossing)
{
    const byte_t addr_arg = decode_byte(ctx);
    const addr_t addr = static_cast<addr_t>(ctx.memory().get_byte(addr_arg)) + static_cast<addr_t>(static_cast<uint16_t>((ctx.memory().get_byte(addr_arg + 1) & 0xff)) << 8);
    const addr_t new_addr = addr + ctx.registers().y;

    page_crossing = is_page_crossing(addr, new_addr);

    return new_addr;
}

template<addr_mode AddrModeT>
addr_t decode_operand_addr(execute_context ctx)
{
    bool page_crossing{false};
    return decode_operand_addr<AddrModeT>(ctx, page_crossing);
}

template<addr_mode AddrModeT>
word_t decode_operand(execute_context ctx, bool& page_crossing)
{
    if constexpr (AddrModeT == addr_mode::accumulator)
    {
        return 0;
    }

    if constexpr (AddrModeT == addr_mode::immediate)
    {
        return decode_byte(ctx);
    }

    return decode_operand_addr<AddrModeT>(ctx, page_crossing);
}

template<addr_mode AddrModeT>
addr_t decode_operand(execute_context ctx)
{
    bool page_crossing{false};
    return decode_operand<AddrModeT>(ctx, page_crossing);
}

template<addr_mode AddrModeT>
byte_t read_operand(execute_context ctx, word_t operand)
{
    if constexpr (AddrModeT == addr_mode::accumulator)
    {
        return ctx.registers().a;
    }

    if constexpr (AddrModeT == addr_mode::immediate)
    {
        return static_cast<byte_t>(operand);
    }

    return ctx.memory().get_byte(operand);
}

template<addr_mode AddrModeT>
void write_operand(execute_context ctx, word_t operand, byte_t value)
{
    if constexpr (AddrModeT == addr_mode::accumulator)
    {
        // cppcheck-suppress unreadVariable
        ctx.registers().a = value;
    }
    else
    {
        ctx.memory().set_byte(operand, value);
    }
}

// AND (Logical AND):
// Performs a bitwise AND on the accumulator and a memory value, affecting the zero and negative flags.
template<addr_mode AddrModeT>
void execute_and(execute_context ctx)
{
    bool page_crossing{false};
    const addr_t addr = decode_operand<AddrModeT>(ctx, page_crossing);
    const byte_t byte = read_operand<AddrModeT>(ctx, addr);

    ctx.registers().a &= byte;

    ctx.registers().set_flag(status_flag::zero, is_zero(ctx.registers().a));
    ctx.registers().set_flag(status_flag::negative, is_negative(ctx.registers().a));

    ctx.step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing));
}

void execute_branch(execute_context ctx, bool condition)
{
    const addr_t initial_pc = ctx.registers().pc;
    const s8_t byte = static_cast<s8_t>(decode_byte(ctx));
    const addr_t target_pc = static_cast<addr_t>(ctx.registers().pc + byte);

    if (condition)
    {
        // if branch succeeds ++
        ctx.step_cycle(cpu_cycle_t(1));
        ctx.registers().pc += byte;

        // if crossing to a new page ++
        if (is_page_crossing(initial_pc, target_pc))
        {
            ctx.step_cycle(cpu_cycle_t(1));
        }

        ctx.registers().pc = target_pc;
    }

    ctx.step_cycle(cpu_cycle_t(2));
}

// BCC (Branch if Carry Clear):
// If the carry flag is clear, it adds the relative displacement to the program counter to branch to a new location.
template<addr_mode AddrModeT>
void execute_bcc(execute_context ctx)
{
    execute_branch(ctx, ctx.registers().is_flag_clear(status_flag::carry));
}

// BCS (Branch if Carry Set):
// If the carry flag is set, it adds the relative displacement to the program counter to branch to a new location.
template<addr_mode AddrModeT>
void execute_bcs(execute_context ctx)
{
    execute_branch(ctx, ctx.registers().is_flag_set(status_flag::carry));
}

// BEQ (Branch if Equal):
// If the zero flag is set, adds the relative displacement to the program counter to branch to a new location.
template<addr_mode AddrModeT>
void execute_beq(execute_context ctx)
{
    execute_branch(ctx, ctx.registers().is_flag_set(status_flag::zero));
}

// BIT (Bit Test):
// Tests bits in memory with the accumulator, affecting the zero, negative, and overflow flags.
template<addr_mode AddrModeT>
void execute_bit(execute_context ctx)
{
    bool page_crossing = false;
    const word_t operand = decode_operand<AddrModeT>(ctx, page_crossing);
    const byte_t byte = read_operand<AddrModeT>(ctx, operand);
    const byte_t new_byte = byte & ctx.registers().a;

    ctx.registers().set_flag(status_flag::zero, is_zero(new_byte));
    ctx.registers().set_flag(status_flag::negative, is_negative(byte));
    ctx.registers().set_flag(status_flag::overflow, is_overflow(byte));

    ctx.step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing));
}

// BMI (Branch if Minus):
// If the negative flag is set, it adds the relative displacement to the program counter to branch to a new location.
template<addr_mode AddrModeT>
void execute_bmi(execute_context ctx)
{
    execute_branch(ctx, ctx.registers().is_flag_set(status_flag::negative));
}

// BNE (Branch if Not Equal):
// If the zero flag is clear, adds the relative displacement to the program counter to branch to a new location.
template<addr_mode AddrModeT>
void execute_bne(execute_context ctx)
{
    execute_branch(ctx, ctx.registers().is_flag_clear(status_flag::zero));
}

// BPL (Branch if Positive):
// If the negative flag is clear, it adds the relative displacement to the program counter to branch to a new location.
template<addr_mode AddrModeT>
void execute_bpl(execute_context ctx)
{
    execute_branch(ctx, ctx.registers().is_flag_clear(status_flag::negative));
}

// BVC (Branch if Overflow Clear):
// If the overflow flag is clear, it adds the relative displacement to the program counter to branch to a new location.
template<addr_mode AddrModeT>
void execute_bvc(execute_context ctx)
{
    execute_branch(ctx, ctx.registers().is_flag_clear(status_flag::overflow));
}

// BVS (Branch if Overflow Set):
// If the overflow flag is set, it adds the relative displacement to the program counter to branch to a new location.
template<addr_mode AddrModeT>
void execute_bvs(execute_context ctx)
{
    execute_branch(ctx, ctx.registers().is_flag_set(status_flag::overflow));
}

// CLC (Clear Carry Flag):
// Clears the carry flag to 0.
template<addr_mode AddrModeT>
void execute_clc(execute_context ctx)
{
    ctx.registers().clear_flag(status_flag::carry);
    ctx.step_cycle(cpu_cycle_t(2));
}

// INX (Increment Register):
// Increases a register by one, affecting the zero and negative flags.
template<addr_mode AddrModeT>
void execute_inx(execute_context ctx)
{
    ++ctx.registers().x;

    ctx.registers().set_flag(status_flag::zero, is_zero(ctx.registers().x));
    ctx.registers().set_flag(status_flag::negative, is_negative(ctx.registers().x));
    ctx.step_cycle(cpu_cycle_t(2));
}

// INX (Increment X Register):
// Increases the X register by one, affecting the zero and negative flags.
template<addr_mode AddrModeT>
void execute_iny(execute_context ctx)
{
    ++ctx.registers().y;

    ctx.registers().set_flag(status_flag::zero, is_zero(ctx.registers().y));
    ctx.registers().set_flag(status_flag::negative, is_negative(ctx.registers().y));
    ctx.step_cycle(cpu_cycle_t(2));
}

// JMP (Jump):
// Sets the program counter to the address specified by the operand, effectively jumping to a new code location.
template<addr_mode AddrModeT>
void execute_jmp(execute_context ctx)
{
    const addr_t new_addr = decode_operand_addr<AddrModeT>(ctx);

    if (new_addr == ctx.registers().pc - 1)
    {
        // TODO Infinite loop
    }

    ctx.registers().pc = new_addr;
    ctx.step_cycle(cpu_cycle_t(3));
}

// JSR (Jump to Subroutine):
// Pushes the address (minus one) of the next operation on to the stack and sets the program counter to the target address, for subroutine calls.
template<addr_mode AddrModeT>
void execute_jsr(execute_context ctx)
{
    // we push the actual return address -1, which is the current place (before decoding the 16-bit addr) + 1
    push_word(ctx, ctx.registers().pc + 1);

    ctx.registers().pc = decode_operand_addr<AddrModeT>(ctx);
    ctx.step_cycle(cpu_cycle_t(6));
}

template<addr_mode AddrModeT>
void execute_load(execute_context ctx, byte_t& register_value)
{
    bool page_crossing{false};
    const word_t operand = decode_operand<AddrModeT>(ctx, page_crossing);

    register_value = read_operand<AddrModeT>(ctx, operand);

    ctx.registers().set_flag(status_flag::zero, is_zero(register_value));
    ctx.registers().set_flag(status_flag::negative, is_negative(register_value));
    ctx.step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing));
}

// LDA (Load Accumulator):
// Loads a value into the accumulator from memory or an immediate value, affecting the zero and negative flags.
template<addr_mode AddrModeT>
void execute_lda(execute_context ctx)
{
    execute_load<AddrModeT>(ctx, ctx.registers().a);
}

// LDX (Load X Register):
// Loads a value into the X register from memory or an immediate value, affecting the zero and negative flags.
template<addr_mode AddrModeT>
void execute_ldx(execute_context ctx)
{
    execute_load<AddrModeT>(ctx, ctx.registers().x);
}

// LDY (Load Y Register):
// Loads a value into the Y register from memory or an immediate value, affecting the zero and negative flags.
template<addr_mode AddrModeT>
void execute_ldy(execute_context ctx)
{
    execute_load<AddrModeT>(ctx, ctx.registers().y);
}

// NOP (No Operation):
// Performs no operation and is used for timing adjustments and code alignment.
template<addr_mode AddrModeT>
void execute_nop(execute_context ctx)
{
    ctx.step_cycle(cpu_cycle_t(2));
}

// PHP (Push Processor Status):
// Pushes a copy of the status flags onto the stack.
template<addr_mode AddrModeT>
void execute_php(execute_context ctx)
{
    // http://wiki.nesdev.com/w/index.php/status_flag_behavior
    // Set bit 5 and 4 to 1 when copy status into from PHP
    push_byte(ctx, ctx.registers().p | 0x30);

    ctx.step_cycle(cpu_cycle_t(3));
}

// PLA (Pull Accumulator):
// Pulls a byte from the stack into the accumulator, affecting the zero and negative flags.
template<addr_mode AddrMode>
void execute_pla(execute_context ctx)
{
    ctx.registers().a = pop_byte(ctx);

    ctx.registers().set_flag(status_flag::zero, is_zero(ctx.registers().a));
    ctx.registers().set_flag(status_flag::negative, is_negative(ctx.registers().a));

    ctx.step_cycle(cpu_cycle_t(4));
}

// PLP (Pull Processor Status):
// Pulls the processor status flags from the stack.
template<addr_mode AddrModeT>
void execute_plp(execute_context ctx)
{
    // http://wiki.nesdev.com/w/index.php/status_flag_behavior
    // Bit 5 and 4 are ignored when pulled from stack - which means they are preserved
    // @TODO - Nintendulator actually always sets bit 5, not sure which one is correct
    ctx.registers().p = (pop_byte(ctx) & 0xef) | (ctx.registers().p & 0x10) | 0x20;

    ctx.step_cycle(cpu_cycle_t(4));
}

// RTI (Return from Interrupt):
// Restores the CPU's state from the stack, including the program counter and processor flags, to conclude an interrupt service routine.
template<addr_mode AddrModeT>
void execute_rti(execute_context ctx)
{
    execute_plp<AddrModeT>(ctx);

    ctx.registers().pc = pop_word(ctx);

    ctx.step_cycle(cpu_cycle_t(2));
}

// RTS (Return from Subroutine):
// Pulls the program counter (plus one) from the stack, returning from a subroutine.
template<addr_mode AddrModeT>
void execute_rts(execute_context ctx)
{
    ctx.registers().pc = pop_word(ctx) + 1;

    ctx.step_cycle(cpu_cycle_t(6));
}

// SEC (Set Carry Flag):
// Sets the carry flag to 1.
template<addr_mode AddrMode>
void execute_sec(execute_context ctx)
{
    ctx.registers().set_flag(status_flag::carry);

    ctx.step_cycle(cpu_cycle_t(2));
}

// SED (Set Decimal Mode):
// Sets the decimal mode flag, affecting how ADC and SBC instructions work.
template<addr_mode AddrMode>
void execute_sed(execute_context ctx)
{
    ctx.registers().set_flag(status_flag::decimal);

    ctx.step_cycle(cpu_cycle_t(2));
}

// SEI (Set Interrupt Disable):
// Sets the interrupt disable flag, preventing interrupts.
template<addr_mode AddrModeT>
void execute_sei(execute_context ctx)
{
    ctx.registers().set_flag(status_flag::interrupt);

    ctx.step_cycle(cpu_cycle_t(2));
}

template<addr_mode AddrModeT>
void execute_store(execute_context ctx, byte_t value)
{
    const word_t operand = decode_operand<AddrModeT>(ctx);

    ctx.memory().set_byte(operand, value);
}

// STA (Store Accumulator):
// Stores the value in the accumulator into a specific location in memory.
template<addr_mode AddrModeT>
void execute_sta(execute_context ctx)
{
    execute_store<AddrModeT>(ctx, ctx.registers().a);

    ctx.step_cycle(get_addr_mode_cycle_cost<AddrModeT>(true));
}

// STX (Store X Register):
// Stores the value in the X register into a specified memory location.
template<addr_mode AddrModeT>
void execute_stx(execute_context ctx)
{
    execute_store<AddrModeT>(ctx, ctx.registers().x);

    ctx.step_cycle(get_addr_mode_cycle_cost<AddrModeT>());
}

// STY (Store Y Register):
// Stores the value in the Y register into a specified memory location.
template<addr_mode AddrModeT>
void execute_sty(execute_context ctx)
{
    execute_store<AddrModeT>(ctx, ctx.registers().y);

    ctx.step_cycle(get_addr_mode_cycle_cost<AddrModeT>());
}

consteval execute_callback_table create_execute_callback_table()
{
    execute_callback_table table{};

    table[opcode::and_immediate] = &execute_and<addr_mode::immediate>;
    table[opcode::and_zero_page] = &execute_and<addr_mode::zero_page>;
    table[opcode::and_zero_page_x] = &execute_and<addr_mode::zero_page_x>;
    table[opcode::and_absolute] = &execute_and<addr_mode::absolute>;
    table[opcode::and_absolute_x] = &execute_and<addr_mode::absolute_x>;
    table[opcode::and_absolute_y] = &execute_and<addr_mode::absolute_y>;
    // table[opcode::and_indexed_indirect] = &execute_and<addr_mode::indexed_indirect>;
    // table[opcode::and_indirect_indexed] = &execute_and<addr_mode::indirect_indexed>;

    table[opcode::bvs_relative] = &execute_bvs<addr_mode::relative>;
    table[opcode::bvc_relative] = &execute_bvc<addr_mode::relative>;
    table[opcode::bpl_relative] = &execute_bpl<addr_mode::relative>;
    table[opcode::bne_relative] = &execute_bne<addr_mode::relative>;
    table[opcode::bmi_relative] = &execute_bmi<addr_mode::relative>;
    table[opcode::beq_relative] = &execute_beq<addr_mode::relative>;
    table[opcode::bcs_relative] = &execute_bcs<addr_mode::relative>;
    table[opcode::bcc_relative] = &execute_bcc<addr_mode::relative>;

    table[opcode::bit_zero_page] = &execute_bit<addr_mode::zero_page>;
    table[opcode::bit_absolute] = &execute_bit<addr_mode::absolute>;

    table[opcode::clc_implied] = &execute_clc<addr_mode::implied>;

    table[opcode::inx_implied] = &execute_inx<addr_mode::implied>;
    table[opcode::iny_implied] = &execute_iny<addr_mode::implied>;

    table[opcode::jmp_absolute] = &execute_jmp<addr_mode::absolute>;
    table[opcode::jsr_absolute] = &execute_jsr<addr_mode::absolute>;

    table[opcode::lda_immediate] = &execute_lda<addr_mode::immediate>;
    table[opcode::lda_zero_page] = &execute_lda<addr_mode::zero_page>;
    table[opcode::lda_zero_page_x] = &execute_lda<addr_mode::zero_page_x>;
    table[opcode::lda_absolute] = &execute_lda<addr_mode::absolute>;
    table[opcode::lda_absolute_x] = &execute_lda<addr_mode::absolute_x>;
    table[opcode::lda_absolute_y] = &execute_lda<addr_mode::absolute_y>;
    // table[opcode::lda_indexed_indirect] = &execute_lda<addr_mode::indexed_indirect>;
    // table[opcode::lda_indirect_indexed] = &execute_lda<addr_mode::indirect_indexed>;

    table[opcode::ldx_immediate] = &execute_ldx<addr_mode::immediate>;
    table[opcode::ldx_zero_page] = &execute_ldx<addr_mode::zero_page>;
    table[opcode::ldx_zero_page_y] = &execute_ldx<addr_mode::zero_page_y>;
    table[opcode::ldx_absolute] = &execute_ldx<addr_mode::absolute>;
    table[opcode::ldx_absolute_y] = &execute_ldx<addr_mode::absolute_y>;

    table[opcode::ldy_immediate] = &execute_ldy<addr_mode::immediate>;
    table[opcode::ldy_zero_page] = &execute_ldy<addr_mode::zero_page>;
    table[opcode::ldy_zero_page_x] = &execute_ldy<addr_mode::zero_page_x>;
    table[opcode::ldy_absolute] = &execute_ldy<addr_mode::absolute>;
    table[opcode::ldy_absolute_x] = &execute_ldy<addr_mode::absolute_x>;

    table[opcode::nop_implied] = &execute_nop<addr_mode::implied>;

    table[opcode::php_implied] = &execute_php<addr_mode::implied>;

    table[opcode::pla_implied] = &execute_pla<addr_mode::implied>;
    table[opcode::plp_implied] = &execute_plp<addr_mode::implied>;

    table[opcode::rti_implied] = &execute_rti<addr_mode::implied>;
    table[opcode::rts_implied] = &execute_rts<addr_mode::implied>;

    table[opcode::sec_implied] = &execute_sec<addr_mode::implied>;
    table[opcode::sed_implied] = &execute_sed<addr_mode::implied>;
    table[opcode::sei_implied] = &execute_sei<addr_mode::implied>;

    table[opcode::sta_zero_page] = &execute_sta<addr_mode::zero_page>;
    table[opcode::sta_zero_page_x] = &execute_sta<addr_mode::zero_page_x>;
    table[opcode::sta_absolute] = &execute_sta<addr_mode::absolute>;
    table[opcode::sta_absolute_x] = &execute_sta<addr_mode::absolute_x>;
    table[opcode::sta_absolute_y] = &execute_sta<addr_mode::absolute_y>;
    // table[opcode::sta_indexed_indirect] = &execute_sta<addr_mode::indexed_indirect>;
    // table[opcode::sta_indirect_indexed] = &execute_sta<addr_mode::indirect_indexed>;

    table[opcode::stx_zero_page] = &execute_stx<addr_mode::zero_page>;
    table[opcode::stx_zero_page_y] = &execute_stx<addr_mode::zero_page_y>;
    table[opcode::stx_absolute] = &execute_stx<addr_mode::absolute>;

    table[opcode::sty_zero_page] = &execute_sty<addr_mode::zero_page>;
    table[opcode::sty_zero_page_x] = &execute_sty<addr_mode::zero_page_x>;
    table[opcode::sty_absolute] = &execute_sty<addr_mode::absolute>;

    return table;
}

constexpr execute_callback_table callback_table{create_execute_callback_table()};

bool execute(execute_context ctx)
{
    const auto code = static_cast<opcode>(ctx.memory().get_byte(ctx.registers().pc++));
    return execute(ctx, code);
}

bool execute(execute_context ctx, opcode code)
{
    const auto callback = callback_table[code];

    if (callback == nullptr) [[unlikely]]
    {
        NESE_ERROR("[cpu] Unimplemented opcode {:02X}", code);
        return false;
    }

    callback(ctx);
    return true;
}

} // namespace nese::cpu::instruction