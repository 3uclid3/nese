#pragma once

#include <array>

#include <nese/basic_types.hpp>
#include <nese/utility/assert.hpp>
#include <nese/utility/passkey.hpp>

namespace nese::v2 {

constexpr addr_t cpu_stack_offset = 0x100;

enum class cpu_status : byte_t
{
    none = 0,
    carry = 1 << 0,     // Carry Flag
    zero = 1 << 1,      // Zero Flag
    interrupt = 1 << 2, // Interrupt Disable
    decimal = 1 << 3,   // Decimal Mode (not used in NES)
    break_cmd = 1 << 4, // Break Command
    unused = 1 << 5,    // Unused bit, always set to 1
    overflow = 1 << 6,  // Overflow Flag
    negative = 1 << 7   // Negative Flag
};

struct cpu_registers
{
    [[nodiscard]] constexpr bool is_status_set(cpu_status flag) const;
    [[nodiscard]] constexpr bool is_status_clear(cpu_status flag) const;

    constexpr void set_status(cpu_status flag, bool value);
    constexpr void set_status(cpu_status flag);
    constexpr void clear_status(cpu_status flag);

    byte_t a;
    byte_t x;
    byte_t y;
    word_t pc;
    byte_t sp;
    byte_t status;
};

struct cpu_state
{
    cpu_registers registers;
    cpu_cycle_t cycle;
};

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

enum class cpu_opcode : byte_t
{
    nop_implied = 0xEA,
};

template<typename BusT>
class cpu
{
public:
    cpu(passkey<BusT>, BusT& bus);

public:
    void reset();
    void irq();
    void nmi();
    bool step();

public:
    [[nodiscard]] const cpu_state& get_state() const;
    [[nodiscard]] cpu_state& get_state(passkey<BusT>);

private:
    using instruction = void (cpu::*)();

    struct instruction_table
    {
        [[nodiscard]] constexpr const instruction& operator[](cpu_opcode op) const;
        [[nodiscard]] constexpr instruction& operator[](cpu_opcode op);

        std::array<instruction, 256> instructions{};
    };

    static consteval instruction_table create_instruction_table();

private:
    // instructions
    template<cpu_addr_mode AddrModeT>
    void instruction_nop();

    // memory
    [[nodiscard]] byte_t read(addr_t addr);
    [[nodiscard]] word_t readw(addr_t addr);

    void write(addr_t addr, byte_t byte);
    void writew(addr_t addr, word_t word);

    // stack
    [[nodiscard]] byte_t pop(addr_t addr);
    [[nodiscard]] word_t popw(addr_t addr);

    void push(addr_t addr, byte_t byte);
    void pushw(addr_t addr, word_t word);

    // decoding
    [[nodiscard]] byte_t decode();
    [[nodiscard]] word_t decodew();

    // operand
    template<cpu_addr_mode AddrModeT>
    [[nodiscard]] word_t decode_operand();

    template<cpu_addr_mode AddrModeT>
    [[nodiscard]] addr_t decode_operand_addr();

    template<cpu_addr_mode AddrModeT>
    [[nodiscard]] byte_t read_operand(word_t operand);

    template<cpu_addr_mode AddrModeT>
    void write_operand(word_t operand, byte_t value);

    // cycle cost
    template<cpu_addr_mode AddrModeT>
    [[nodiscard]] constexpr cpu_cycle_t get_addr_mode_cycle_cost(bool page_crossing = false);

private:
    static constexpr instruction_table _instruction_table{create_instruction_table()};

    ref_wrap<BusT> _bus;
    cpu_state _state{};
};

} // namespace nese::v2

#include <nese/cpu.inl>
