#include <nese/cpu/processor.hpp>

#include <spdlog/sinks/basic_file_sink.h>

#include <nese/log.hpp>
#include <nese/memory/ram.hpp>

namespace nese::cpu {

enum opcode_base : u8_t
{
    opcode_base_ora = 0x00,
    opcode_base_and = 0x20,
    opcode_base_eor = 0x40,
    opcode_base_adc = 0x60,
    opcode_base_sta = 0x80,
    opcode_base_lda = 0xA0,
    opcode_base_cmp = 0xC0,
    opcode_base_sbc = 0xE0
};

constexpr addr_t stack_offset = 0x100;

template<addr_mode AddrMode>
constexpr u8_t get_addr_mode_offset()
{
    switch (AddrMode)
    {
    case addr_mode::immediate:
        return 0x9;

    case addr_mode::zero_page:
        return 0x5;

    case addr_mode::zero_page_x:
        return 0x15;

    case addr_mode::absolute:
        return 0xd;

    case addr_mode::absolute_x:
        return 0x1d;

    case addr_mode::absolute_y:
        return 0x19;

    case addr_mode::indexed_indirect:
        return 0x1d;

    case addr_mode::indirect_indexed:
        return 0x19;
    }

    __assume(false);
}

template<addr_mode AddrMode>
constexpr cpu_cycle_t get_addr_mode_cycle()
{
    switch (AddrMode)
    {
    case addr_mode::immediate:
        return cpu_cycle_t{2};

    case addr_mode::zero_page:
        return cpu_cycle_t{3};

    case addr_mode::zero_page_x:
        return cpu_cycle_t{3};

    case addr_mode::absolute:
    case addr_mode::absolute_x:
    case addr_mode::absolute_y:
        return cpu_cycle_t{4};

    case addr_mode::indexed_indirect:
    case addr_mode::indirect_indexed:
        return cpu_cycle_t{6};
    }

    __assume(false);
}

#define GET_ALU_OPCODE(op, mode) \
    static_cast<u8_t>(opcode_base_##op) + get_addr_mode_offset<mode>()

#define SET_ALU_CALLBACK(table, op, mode) \
    table._addr_modes[GET_ALU_OPCODE(op, mode)] = mode; \
    table._callbacks[GET_ALU_OPCODE(op, mode)] = &processor::execute_instruction_##op<mode>; \
    table._strings[GET_ALU_OPCODE(op, mode)] = #op

#define SET_ALU_CALLBACKS(table, op) \
    SET_ALU_CALLBACK(table, op, addr_mode::immediate); \
    SET_ALU_CALLBACK(table, op, addr_mode::zero_page); \
    SET_ALU_CALLBACK(table, op, addr_mode::zero_page_x); \
    SET_ALU_CALLBACK(table, op, addr_mode::absolute); \
    SET_ALU_CALLBACK(table, op, addr_mode::absolute_x); \
    SET_ALU_CALLBACK(table, op, addr_mode::absolute_y); \
    SET_ALU_CALLBACK(table, op, addr_mode::indexed_indirect); \
    SET_ALU_CALLBACK(table, op, addr_mode::indirect_indexed)

#define SET_ADDR_CALLBACK(table, op, opcode, mode) \
    table._addr_modes[static_cast<size_t>(opcode)] = mode; \
    table._callbacks[static_cast<size_t>(opcode)] = &processor::execute_instruction_##op<mode>; \
    table._strings[static_cast<size_t>(opcode)] = #op

#define SET_CALLBACK(table, op, opcode) \
    SET_ADDR_CALLBACK(table, op, opcode, addr_mode::implied)

struct processor::instruction_table
{
    static consteval instruction_table create()
    {
        instruction_table table{};

        SET_ALU_CALLBACKS(table, adc);
        SET_ALU_CALLBACKS(table, and);
        SET_ALU_CALLBACKS(table, cmp);
        SET_ALU_CALLBACKS(table, eor);
        SET_ALU_CALLBACKS(table, lda);
        SET_ALU_CALLBACKS(table, sta);
        SET_ALU_CALLBACKS(table, ora);
        SET_ALU_CALLBACKS(table, sbc);

        SET_CALLBACK(table, clc, 0x18);
        SET_CALLBACK(table, cld, 0xd8);
        SET_CALLBACK(table, cli, 0x58);
        SET_CALLBACK(table, clv, 0xb8);

        SET_CALLBACK(table, dex, 0xca);
        SET_CALLBACK(table, dey, 0x88);

        SET_CALLBACK(table, inx, 0xe8);
        SET_CALLBACK(table, iny, 0xc8);

        SET_CALLBACK(table, nop, 0xea);

        SET_CALLBACK(table, pha, 0x48);
        SET_CALLBACK(table, php, 0x08);

        SET_CALLBACK(table, pla, 0x68);
        SET_CALLBACK(table, plp, 0x28);

        SET_CALLBACK(table, rts, 0x60);

        SET_CALLBACK(table, sec, 0x38);
        SET_CALLBACK(table, sed, 0xf8);
        SET_CALLBACK(table, sei, 0x78);

        SET_CALLBACK(table, tax, 0xaa);
        SET_CALLBACK(table, tay, 0xa8);
        SET_CALLBACK(table, tsx, 0xba);
        SET_CALLBACK(table, txa, 0x8a);
        SET_CALLBACK(table, txs, 0x9a);
        SET_CALLBACK(table, tya, 0x98);

        SET_ADDR_CALLBACK(table, bcc, 0x90, addr_mode::relative);
        SET_ADDR_CALLBACK(table, bcs, 0xb0, addr_mode::relative);
        SET_ADDR_CALLBACK(table, beq, 0xf0, addr_mode::relative);
        SET_ADDR_CALLBACK(table, bmi, 0x30, addr_mode::relative);
        SET_ADDR_CALLBACK(table, bne, 0xd0, addr_mode::relative);
        SET_ADDR_CALLBACK(table, bpl, 0x10, addr_mode::relative);
        SET_ADDR_CALLBACK(table, bvc, 0x50, addr_mode::relative);
        SET_ADDR_CALLBACK(table, bvs, 0x70, addr_mode::relative);

        SET_ADDR_CALLBACK(table, bit, 0x24, addr_mode::zero_page);
        SET_ADDR_CALLBACK(table, bit, 0x2c, addr_mode::absolute);

        SET_ADDR_CALLBACK(table, cpx, 0xe0, addr_mode::immediate);
        SET_ADDR_CALLBACK(table, cpx, 0xe4, addr_mode::zero_page);
        SET_ADDR_CALLBACK(table, cpx, 0xec, addr_mode::absolute);

        SET_ADDR_CALLBACK(table, cpy, 0xc0, addr_mode::immediate);
        SET_ADDR_CALLBACK(table, cpy, 0xc4, addr_mode::zero_page);
        SET_ADDR_CALLBACK(table, cpy, 0xcc, addr_mode::absolute);

        SET_ADDR_CALLBACK(table, jsr, 0x20, addr_mode::absolute);
        SET_ADDR_CALLBACK(table, jmp, 0x4c, addr_mode::absolute);

        SET_ADDR_CALLBACK(table, ldx, 0xa2, addr_mode::immediate);
        SET_ADDR_CALLBACK(table, ldx, 0xa6, addr_mode::zero_page);
        SET_ADDR_CALLBACK(table, ldx, 0xb6, addr_mode::zero_page_y);
        SET_ADDR_CALLBACK(table, ldx, 0xae, addr_mode::absolute);
        SET_ADDR_CALLBACK(table, ldx, 0xbe, addr_mode::absolute_y);

        SET_ADDR_CALLBACK(table, ldy, 0xa0, addr_mode::immediate);
        SET_ADDR_CALLBACK(table, ldy, 0xa4, addr_mode::zero_page);
        SET_ADDR_CALLBACK(table, ldy, 0xb4, addr_mode::zero_page_x);
        SET_ADDR_CALLBACK(table, ldy, 0xac, addr_mode::absolute);
        SET_ADDR_CALLBACK(table, ldy, 0xbc, addr_mode::absolute_x);

        SET_ADDR_CALLBACK(table, stx, 0x86, addr_mode::zero_page);
        SET_ADDR_CALLBACK(table, stx, 0x96, addr_mode::zero_page_y);
        SET_ADDR_CALLBACK(table, stx, 0x96, addr_mode::zero_page_y);
        SET_ADDR_CALLBACK(table, stx, 0x96, addr_mode::zero_page_y);
        SET_ADDR_CALLBACK(table, stx, 0x96, addr_mode::zero_page_y);
        SET_ADDR_CALLBACK(table, stx, 0x96, addr_mode::zero_page_y);
        SET_ADDR_CALLBACK(table, stx, 0x96, addr_mode::zero_page_y);
        SET_ADDR_CALLBACK(table, stx, 0x96, addr_mode::zero_page_y);
        SET_ADDR_CALLBACK(table, stx, 0x96, addr_mode::zero_page_y);
        SET_ADDR_CALLBACK(table, stx, 0x96, addr_mode::zero_page_y);
        SET_ADDR_CALLBACK(table, stx, 0x96, addr_mode::zero_page_y);
        SET_ADDR_CALLBACK(table, stx, 0x96, addr_mode::zero_page_y);
        SET_ADDR_CALLBACK(table, stx, 0x96, addr_mode::zero_page_y);
        SET_ADDR_CALLBACK(table, stx, 0x96, addr_mode::zero_page_y);
        SET_ADDR_CALLBACK(table, stx, 0x96, addr_mode::zero_page_y);
        SET_ADDR_CALLBACK(table, stx, 0x8e, addr_mode::absolute);

        SET_ADDR_CALLBACK(table, sty, 0x84, addr_mode::zero_page);
        SET_ADDR_CALLBACK(table, sty, 0x94, addr_mode::zero_page_x);
        SET_ADDR_CALLBACK(table, sty, 0x8c, addr_mode::absolute);

        return table;
    }

    [[nodiscard]] constexpr instruction_callback get_callback(byte_t code) const
    {
        return _callbacks[static_cast<size_t>(code)];
    }

    [[nodiscard]] constexpr std::string_view get_string(byte_t code) const
    {
        return _strings[static_cast<size_t>(code)];
    }

    [[nodiscard]] constexpr addr_mode get_addr_mode(byte_t code) const
    {
        return _addr_modes[static_cast<size_t>(code)];
    }

    std::array<addr_mode, 256> _addr_modes;
    std::array<instruction_callback, 256> _callbacks;
    std::array<std::string_view, 256> _strings;
};

struct processor::instruction_table_singleton
{
    static constexpr instruction_table instance{instruction_table::create()};
};

#undef SET_ALU_CALLBACK
#undef SET_ALU_CALLBACKS

processor::processor(memory::ram& ram)
    : _ram(ram)
#if NESE_CPU_LOG_NINTENDULATOR_ENABLED
    , _nintendulator_logger(spdlog::basic_logger_st("nintendulator", "cpu-nintendulator.log", true))
#endif
{
#if NESE_CPU_LOG_NINTENDULATOR_ENABLED
    _nintendulator_logger->set_level(spdlog::level::trace);
    _nintendulator_logger->set_pattern("%v");
#endif
}

void processor::power_on()
{
    // TODO Full power-on state
    // http://wiki.nesdev.com/w/index.php/CPU_power_up_state
    _registers.a = 0;
    _registers.x = 0;
    _registers.y = 0;
    _registers.pc = 0;
    _registers.s = 0xfd;
    _registers.p = 0x24;
}

void processor::reset()
{
}

void processor::step_to(cycle_t cycle)
{
    while (_cycle < cycle && !has_stop_requested())
    {
#if NESE_CPU_DEBUGBREAK_ENABLED
        if (_debugbreak && _debugbreak_addr == _registers.pc)
        {
            __debugbreak();
        }
#endif

        execute_next_instruction();
    }
}

byte_t processor::get_byte_from_memory(addr_t addr) const
{
    return _ram.get_byte(addr);
}

void processor::set_byte_to_memory(addr_t addr, byte_t value)
{
    _ram.set_byte(addr, value);
}

word_t processor::get_word_from_memory(addr_t addr) const
{
    return _ram.get_word(addr);
}

byte_t processor::decode_byte()
{
    const byte_t decoded = get_byte_from_memory(_registers.pc);

    _registers.pc += 1;

    return decoded;
}

word_t processor::decode_word()
{
    const word_t decoded = get_word_from_memory(_registers.pc);

    _registers.pc += 2;

    return decoded;
}

template<>
word_t processor::decode_operand<addr_mode::accumulator>()
{
    return 0;
}

template<>
word_t processor::decode_operand<addr_mode::immediate>()
{
    return decode_byte();
}

template<addr_mode AddrMode>
word_t processor::decode_operand()
{
    return decode_operand_addr<AddrMode>();
}

template<>
word_t processor::decode_operand_addr<addr_mode::zero_page>()
{
    // zero page - next byte is 8-bit address
    return decode_byte();
}

template<>
word_t processor::decode_operand_addr<addr_mode::zero_page_x>()
{
    return decode_byte() + _registers.x;
}

template<>
word_t processor::decode_operand_addr<addr_mode::zero_page_y>()
{
    return decode_byte() + _registers.y;
}

template<>
word_t processor::decode_operand_addr<addr_mode::absolute>()
{
    return decode_word();
}

template<>
word_t processor::decode_operand_addr<addr_mode::absolute_x>()
{
    return decode_word() + _registers.x;
}

template<>
word_t processor::decode_operand_addr<addr_mode::absolute_y>()
{
    return decode_word() + _registers.y;
}

template<>
word_t processor::decode_operand_addr<addr_mode::indexed_indirect>()
{
    // zero page - next byte is 8-bit address
    return decode_byte();
}

template<>
word_t processor::decode_operand_addr<addr_mode::indirect_indexed>()
{
    // Indirect Indexed
    // implies a table of table address in zero page
    const uint8_t addr = decode_byte();
    return _ram.get_byte(addr) + (static_cast<uint16_t>(_ram.get_byte(addr + 1)) << 8) + _registers.y;
}

template<addr_mode AddrMode>
byte_t processor::read_operand()
{
    return read_operand<AddrMode>(decode_operand<AddrMode>());
}

template<>
byte_t processor::read_operand<addr_mode::accumulator>(word_t op)
{
    (void)op;
    return _registers.a;
}

template<>
byte_t processor::read_operand<addr_mode::immediate>(word_t op)
{
    return static_cast<byte_t>(op);
}

template<addr_mode AddrMode>
byte_t processor::read_operand(word_t op)
{
    return get_byte_from_memory(op);
}

void processor::push_byte(byte_t value)
{
    // stack grow top->down
    // no underflow/overflow detection
    set_byte_to_memory(_registers.s + stack_offset, value);

    --_registers.s;
}

byte_t processor::pop_byte()
{
    ++_registers.s;

    return get_byte_from_memory(_registers.s + stack_offset);
}

void processor::push_word(word_t value)
{
    // high-order bytes push first since the stack grow top->down and the machine is little-endian
    push_byte(value >> 8);
    push_byte(value & 0xff);
}

word_t processor::pop_word()
{
    // low-order bytes pop first since the stack grow top->down and the machine is little-endian
    const byte_t lo = pop_byte();
    const byte_t hi = pop_byte();
    return static_cast<word_t>(hi << 8) + lo;
}

bool processor::has_status_flags(status_flags mask) const
{
    return (_registers.p & mask) == mask;
}

bool processor::has_status_flag(status_flag mask) const
{
    return (_registers.p & mask) != 0;
}

void processor::set_status_flag(status_flag mask, bool value)
{
    _registers.p = value ? _registers.p | mask : _registers.p & ~mask;
}

bool processor::has_status_carry() const
{
    return has_status_flag(status_flag::carry);
}

void processor::set_status_carry(bool value)
{
    set_status_flag(status_flag::carry, value);
}

bool processor::has_status_zero() const
{
    return has_status_flag(status_flag::zero);
}

void processor::set_status_zero(bool value)
{
    set_status_flag(status_flag::zero, value);
}

bool processor::has_status_interrupt() const
{
    return has_status_flag(status_flag::interrupt);
}

void processor::set_status_interrupt(bool value)
{
    set_status_flag(status_flag::interrupt, value);
}

bool processor::has_status_decimal() const
{
    return has_status_flag(status_flag::decimal);
}

void processor::set_status_decimal(bool value)
{
    set_status_flag(status_flag::decimal, value);
}

bool processor::has_status_break_cmd() const
{
    return has_status_flag(status_flag::break_cmd);
}

void processor::set_status_break_cmd(bool value)
{
    set_status_flag(status_flag::break_cmd, value);
}

bool processor::has_status_unused() const
{
    return has_status_flag(status_flag::unused);
}

void processor::set_status_unused(bool value)
{
    set_status_flag(status_flag::unused, value);
}

bool processor::has_status_overflow() const
{
    return has_status_flag(status_flag::overflow);
}

void processor::set_status_overflow(bool value)
{
    set_status_flag(status_flag::overflow, value);
}

bool processor::has_status_negative() const
{
    return has_status_flag(status_flag::negative);
}

void processor::set_status_negative(bool value)
{
    set_status_flag(status_flag::negative, value);
}

void processor::set_status_alu(byte_t value)
{
    set_status_zero(value == 0);
    set_status_negative(value & 0x80);
}

bool processor::is_sign_overflow(byte_t old_byte, byte_t new_byte, byte_t byte)
{
    return (old_byte & 0x80) == (byte & 0x80) && (old_byte & 0x80) != (new_byte & 0x80);
}

addr_mode processor::get_instruction_addr_mode(byte_t code)
{
    return instruction_table_singleton::instance.get_addr_mode(code);
}

processor::instruction_callback processor::get_instruction_callback(byte_t code)
{
    return instruction_table_singleton::instance.get_callback(code);
}

std::string_view processor::get_instruction_string(byte_t code)
{
    return instruction_table_singleton::instance.get_string(code);
}

void processor::execute_next_instruction()
{
#if NESE_CPU_LOG_NINTENDULATOR_ENABLED
    _nintendulator_logger->trace("{}", *this);
#endif

    const byte_t opcode = decode_byte();
    const instruction_callback callback = get_instruction_callback(opcode);

    NESE_CRITICAL_ASSERT(callback, "[cpu][{}] unsupported instruction with op code = 0x{:02x}", _cycle.count(), opcode);

    callback(*this);
}

// ADC (Add with Carry):
// Adds a memory value and the carry flag to the accumulator, affecting flags for carry, zero, overflow, and negative.
template<addr_mode AddrMode>
void processor::execute_instruction_adc(processor& self)
{
    const byte_t old_byte = self._registers.a;
    const byte_t byte = self.read_operand<AddrMode>();

    self._registers.a += byte + (self.has_status_carry() ? 1 : 0);

    // flags
    self.set_status_overflow(is_sign_overflow(old_byte, self._registers.a, byte));
    self.set_status_carry(old_byte > self._registers.a);
    self.set_status_alu(self._registers.a);
}

// AND (Logical AND):
// Performs a bitwise AND on the accumulator and a memory value, affecting the zero and negative flags.
template<addr_mode AddrMode>
void processor::execute_instruction_and(processor& self)
{
    const byte_t byte = self.read_operand<AddrMode>();

    self._registers.a &= byte;

    self.set_status_alu(self._registers.a);

    self._cycle += get_addr_mode_cycle<AddrMode>();
}

// BCC (Branch if Carry Clear):
// If the condition, it adds the relative displacement to the program counter to branch to a new location.
template<addr_mode AddrMode>
void processor::execute_instruction_b__(processor& self, bool condition)
{
    const byte_t byte = self.decode_byte();

    if (condition)
    {
        self._registers.pc += byte;
    }
}

// BCC (Branch if Carry Clear):
// If the carry flag is clear, it adds the relative displacement to the program counter to branch to a new location.
template<addr_mode AddrMode>
void processor::execute_instruction_bcc(processor& self)
{
    self.execute_instruction_b__<AddrMode>(self, !self.has_status_carry());
}

// BCS (Branch if Carry Set):
// If the carry flag is set, it adds the relative displacement to the program counter to branch to a new location.
template<addr_mode AddrMode>
void processor::execute_instruction_bcs(processor& self)
{
    self.execute_instruction_b__<AddrMode>(self, self.has_status_carry());
}

// BEQ (Branch if Equal):
// If the zero flag is set, adds the relative displacement to the program counter to branch to a new location.
template<addr_mode AddrMode>
void processor::execute_instruction_beq(processor& self)
{
    self.execute_instruction_b__<AddrMode>(self, self.has_status_zero());
}

// BMI (Branch if Minus):
// If the negative flag is set, it adds the relative displacement to the program counter to branch to a new location.
template<addr_mode AddrMode>
void processor::execute_instruction_bmi(processor& self)
{
    self.execute_instruction_b__<AddrMode>(self, self.has_status_negative());
}

// BNE (Branch if Not Equal):
// If the zero flag is clear, adds the relative displacement to the program counter to branch to a new location.
template<addr_mode AddrMode>
void processor::execute_instruction_bne(processor& self)
{
    self.execute_instruction_b__<AddrMode>(self, !self.has_status_zero());
}

// BPL (Branch if Positive):
// If the negative flag is clear, it adds the relative displacement to the program counter to branch to a new location.
template<addr_mode AddrMode>
void processor::execute_instruction_bpl(processor& self)
{
    self.execute_instruction_b__<AddrMode>(self, !self.has_status_negative());
}

// BVC (Branch if Overflow Clear):
// If the overflow flag is clear, it adds the relative displacement to the program counter to branch to a new location.
template<addr_mode AddrMode>
void processor::execute_instruction_bvc(processor& self)
{
    self.execute_instruction_b__<AddrMode>(self, !self.has_status_overflow());
}

// BVS (Branch if Overflow Set):
// If the overflow flag is set, it adds the relative displacement to the program counter to branch to a new location.
template<addr_mode AddrMode>
void processor::execute_instruction_bvs(processor& self)
{
    self.execute_instruction_b__<AddrMode>(self, self.has_status_overflow());
}

// BIT (Bit Test):
// Tests bits in memory with the accumulator, affecting the zero, negative, and overflow flags.
template<addr_mode AddrMode>
void processor::execute_instruction_bit(processor& self)
{
    const byte_t byte = self.read_operand<AddrMode>();
    const byte_t new_byte = byte & self._registers.a;

    self.set_status_zero(new_byte == 0);
    self.set_status_overflow(byte & 0x40);
    self.set_status_negative(byte & 0x80);
}

// CLC (Clear Carry Flag):
// Clears the carry flag to 0.
template<addr_mode AddrMode>
void processor::execute_instruction_clc(processor& self)
{
    self.set_status_carry(false);
}

// CLD (Clear Decimal Mode):
// Clears the decimal mode flag, affecting how ADC and SBC instructions work.
template<addr_mode AddrMode>
void processor::execute_instruction_cld(processor& self)
{
    self.set_status_decimal(false);
}

// CLI (Clear Interrupt Disable):
// Clears the interrupt disable flag, allowing interrupts.
template<addr_mode AddrMode>
void processor::execute_instruction_cli(processor& self)
{
    self.set_status_interrupt(false);
}

// CLV (Clear Overflow Flag):
// Clears the overflow flag to 0, affecting subsequent arithmetic and branch instructions.
template<addr_mode AddrMode>
void processor::execute_instruction_clv(processor& self)
{
    self.set_status_overflow(false);
}

// CMP (Compare Accumulator):
// Compares to_byte with a memory value, setting flags based on the subtraction result (carry, zero, and negative flags).
template<addr_mode AddrMode>
void processor::execute_instruction_c__(processor& self, byte_t to_byte)
{
    const byte_t byte = self.read_operand<AddrMode>();
    const byte_t diff = to_byte - byte;

    self.set_status_carry(to_byte >= byte);
    self.set_status_zero(diff == 0);
    self.set_status_negative(diff & 0x80);
}

// CMP (Compare Accumulator):
// Compares the accumulator with a memory value, setting flags based on the subtraction result (carry, zero, and negative flags).
template<addr_mode AddrMode>
void processor::execute_instruction_cmp(processor& self)
{
    execute_instruction_c__<AddrMode>(self, self._registers.a);
}

// CPX (Compare X Register):
// Compares the X register with a memory value, setting flags based on the subtraction result (carry, zero, and negative flags).
template<addr_mode AddrMode>
void processor::execute_instruction_cpx(processor& self)
{
    execute_instruction_c__<AddrMode>(self, self._registers.x);
}

// CPY (Compare Y Register):
// Compares the Y register with a memory value, setting flags based on the subtraction result (carry, zero, and negative flags).
template<addr_mode AddrMode>
void processor::execute_instruction_cpy(processor& self)
{
    execute_instruction_c__<AddrMode>(self, self._registers.y);
}

// EOR (Exclusive OR):
// Performs a bitwise exclusive OR between the accumulator and a memory value, affecting the zero and negative flags.
template<addr_mode AddrMode>
void processor::execute_instruction_eor(processor& self)
{
    self._registers.a ^= self.read_operand<AddrMode>();

    self.set_status_alu(self._registers.a);
}

template<addr_mode AddrMode>
void processor::execute_instruction_de_(processor& self, byte_t& byte)
{
    --byte;

    self.set_status_alu(byte);
}

// DEY (Decrement X Register):
// Decreases the value in the Y register by one, affecting the zero and negative flags.
template<addr_mode AddrMode>
void processor::execute_instruction_dex(processor& self)
{
    execute_instruction_de_<AddrMode>(self, self._registers.x);
}

// DEY (Decrement Y Register):
// Decreases the value in the Y register by one, affecting the zero and negative flags.
template<addr_mode AddrMode>
void processor::execute_instruction_dey(processor& self)
{
    execute_instruction_de_<AddrMode>(self, self._registers.y);
}

// INX (Increment Register):
// Increases a register by one, affecting the zero and negative flags.
template<addr_mode AddrMode>
void processor::execute_instruction_in_(processor& self, byte_t& byte)
{
    ++byte;

    self.set_status_alu(byte);
}

// INX (Increment X Register):
// Increases the X register by one, affecting the zero and negative flags.
template<addr_mode AddrMode>
void processor::execute_instruction_inx(processor& self)
{
    execute_instruction_in_<AddrMode>(self, self._registers.x);
}

// INY (Increment Y Register):
// Increases the Y register by one, affecting the zero and negative flags.
template<addr_mode AddrMode>
void processor::execute_instruction_iny(processor& self)
{
    execute_instruction_in_<AddrMode>(self, self._registers.y);
}

// JMP (Jump):
// Sets the program counter to the address specified by the operand, effectively jumping to a new code location.
template<addr_mode AddrMode>
void processor::execute_instruction_jmp(processor& self)
{
    self._registers.pc = self.decode_operand_addr<AddrMode>();

    self._cycle += cpu_cycle_t{AddrMode == addr_mode::absolute ? 3 : 4};
}

// JSR (Jump to Subroutine):
// Pushes the address (minus one) of the next operation on to the stack and sets the program counter to the target address, for subroutine calls.
template<addr_mode AddrMode>
void processor::execute_instruction_jsr(processor& self)
{
    // we push the actual return address -1, which is the current place (before decoding the 16-bit addr) + 1
    self.push_word(self._registers.pc + 1);

    self._registers.pc = self.decode_operand_addr<AddrMode>();
}

// LDA (Load):
// Loads a value into byte from memory or an immediate value, affecting the zero and negative flags.
template<addr_mode AddrMode>
void processor::execute_instruction_ld_(processor& self, byte_t& byte)
{
    byte = self.read_operand<AddrMode>();

    self.set_status_alu(byte);
}

// LDA (Load Accumulator):
// Loads a value into the accumulator from memory or an immediate value, affecting the zero and negative flags.
template<addr_mode AddrMode>
void processor::execute_instruction_lda(processor& self)
{
    execute_instruction_ld_<AddrMode>(self, self._registers.a);
}

// LDX (Load X Register):
// Loads a value into the X register from memory or an immediate value, affecting the zero and negative flags.
template<addr_mode AddrMode>
void processor::execute_instruction_ldx(processor& self)
{
    execute_instruction_ld_<AddrMode>(self, self._registers.x);
}

// LDY (Load Y Register):
// Loads a value into the Y register from memory or an immediate value, affecting the zero and negative flags.
template<addr_mode AddrMode>
void processor::execute_instruction_ldy(processor& self)
{
    execute_instruction_ld_<AddrMode>(self, self._registers.y);
}

// ORA (Logical Inclusive OR):
// Performs a bitwise OR between the accumulator and a memory value, affecting the zero and negative flags.
template<addr_mode AddrMode>
void processor::execute_instruction_ora(processor& self)
{
    self._registers.a |= self.read_operand<AddrMode>();

    self.set_status_alu(self._registers.a);
}

// NOP (No Operation):
// Performs no operation and is used for timing adjustments and code alignment.
template<addr_mode AddrMode>
void processor::execute_instruction_nop(processor& self)
{
    (void)self;
}

// PHA (Push Accumulator):
// Pushes a copy of the accumulator onto the stack.
template<addr_mode AddrMode>
void processor::execute_instruction_pha(processor& self)
{
    self.push_byte(self._registers.a);
}

// PHP (Push Processor Status):
// Pushes a copy of the status flags onto the stack.
template<addr_mode AddrMode>
void processor::execute_instruction_php(processor& self)
{
    // http://wiki.nesdev.com/w/index.php/status_flag_behavior
    // Set bit 5 and 4 to 1 when copy status into from PHP
    self.push_byte(self._registers.p | 0x30);
}

// PLA (Pull Accumulator):
// Pulls a byte from the stack into the accumulator, affecting the zero and negative flags.
template<addr_mode AddrMode>
void processor::execute_instruction_pla(processor& self)
{
    self._registers.a = self.pop_byte();

    self.set_status_alu(self._registers.a);
}

// PLP (Pull Processor Status):
// Pulls the processor status flags from the stack.
template<addr_mode AddrMode>
void processor::execute_instruction_plp(processor& self)
{
    // http://wiki.nesdev.com/w/index.php/status_flag_behavior
    // Bit 5 and 4 are ignored when pulled from stack - which means they are preserved
    // @TODO - Nintendulator actually always sets bit 5, not sure which one is correct
    self._registers.p = (self.pop_byte() & 0xef) | (self._registers.p & 0x10) | 0x20;
}

// RTS (Return from Subroutine):
// Pulls the program counter (plus one) from the stack, returning from a subroutine.
template<addr_mode AddrMode>
void processor::execute_instruction_rts(processor& self)
{
    // See JSR - we pushed actual return address - 1
    self._registers.pc = self.pop_word() + 1;
}

// SBC (Subtract with Carry):
// Subtracts a memory value and the carry flag from the accumulator, affecting flags for carry, zero, overflow, and negative.
template<addr_mode AddrMode>
void processor::execute_instruction_sbc(processor& self)
{
    byte_t byte = self.read_operand<AddrMode>();

    byte = ~byte + 1;                                      // turn it into a add operand
    byte = byte - ((1 - self.has_status_carry()) ? 1 : 0); // account for the carry

    const uint8_t old_byte = self._registers.a;

    self._registers.a = self._registers.a + byte;

    // flags
    self.set_status_overflow(is_sign_overflow(old_byte, self._registers.a, byte));
    self.set_status_carry(self._registers.a <= old_byte);
    self.set_status_alu(self._registers.a);
}

// SEC (Set Carry Flag):
// Sets the carry flag to 1.
template<addr_mode AddrMode>
void processor::execute_instruction_sec(processor& self)
{
    self.set_status_carry(true);
}

// SED (Set Decimal Mode):
// Sets the decimal mode flag, affecting how ADC and SBC instructions work.
template<addr_mode AddrMode>
void processor::execute_instruction_sed(processor& self)
{
    self.set_status_decimal(true);
}

// SEI (Set Interrupt Disable):
// Sets the interrupt disable flag, preventing interrupts.
template<addr_mode AddrMode>
void processor::execute_instruction_sei(processor& self)
{
    self.set_status_interrupt(true);
}

// ST_ (Store):
// Stores a value into a specific location in memory.
template<addr_mode AddrMode>
void processor::execute_instruction_st_(processor& self, byte_t byte)
{
    self.set_byte_to_memory(self.decode_operand_addr<AddrMode>(), byte);
}

// STA (Store Accumulator):
// Stores the value in the accumulator into a specific location in memory.
template<addr_mode AddrMode>
void processor::execute_instruction_sta(processor& self)
{
    // STA imm (0x89) = NOP
    if constexpr (AddrMode != addr_mode::immediate)
    {
        self.execute_instruction_st_<AddrMode>(self, self._registers.a);
    }
}

// STX (Store X Register):
// Stores the value in the X register into a specified memory location.
template<addr_mode AddrMode>
void processor::execute_instruction_stx(processor& self)
{
    self.execute_instruction_st_<AddrMode>(self, self._registers.x);
}

// STY (Store Y Register):
// Stores the value in the Y register into a specified memory location.
template<addr_mode AddrMode>
void processor::execute_instruction_sty(processor& self)
{
    self.execute_instruction_st_<AddrMode>(self, self._registers.y);
}

template<addr_mode AddrMode>
void processor::execute_instruction_t__(processor& self, byte_t from_byte, byte_t& to_byte)
{
    to_byte = from_byte;

    self.set_status_alu(to_byte);
}

// TAX (Transfer Accumulator to X):
// Transfers the value in the accumulator to the X register, affecting the zero and negative flags.
template<addr_mode AddrMode>
void processor::execute_instruction_tax(processor& self)
{
    execute_instruction_t__<AddrMode>(self, self._registers.a, self._registers.x);
}

// TAY (Transfer Accumulator to Y):
// Transfers the value in the accumulator to the Y register, affecting the zero and negative flags.
template<addr_mode AddrMode>
void processor::execute_instruction_tay(processor& self)
{
    execute_instruction_t__<AddrMode>(self, self._registers.a, self._registers.y);
}

// TSX (Transfer Stack Pointer to X):
// Transfers the current stack pointer value to the X register, affecting the zero and negative flags.
template<addr_mode AddrMode>
void processor::execute_instruction_tsx(processor& self)
{
    execute_instruction_t__<AddrMode>(self, self._registers.s, self._registers.x);
}

// TXA (Transfer X to Accumulator):
// Transfers the value in the X register to the accumulator, affecting the zero and negative flags.
template<addr_mode AddrMode>
void processor::execute_instruction_txa(processor& self)
{
    execute_instruction_t__<AddrMode>(self, self._registers.x, self._registers.a);
}

// TXS (Transfer X to Stack Pointer):
// Transfers the value in the X register to the stack pointer. Note that this instruction does not affect any flags.
template<addr_mode AddrMode>
void processor::execute_instruction_txs(processor& self)
{
    self._registers.s = self._registers.x;
}

// TYA (Transfer Y to Accumulator):
// Transfers the value in the Y register to the accumulator, affecting the zero and negative flags.
template<addr_mode AddrMode>
void processor::execute_instruction_tya(processor& self)
{
    execute_instruction_t__<AddrMode>(self, self._registers.y, self._registers.a);
}

} // namespace nese
