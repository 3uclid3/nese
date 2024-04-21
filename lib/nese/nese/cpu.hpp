#pragma once

#include <nese/basic_types.hpp>
#include <nese/cpu/cpu_addr_mode.hpp>
#include <nese/cpu/cpu_opcode.hpp>
#include <nese/cpu/cpu_state.hpp>
#include <nese/utility/assert.hpp>

namespace nese {

template<typename BusT>
class cpu
{
public:
    explicit cpu(BusT& bus);

public:
    void reset();
    bool step();
    bool step(cpu_cycle_t to_cycle);
    bool step(cycle_t to_cycle);

    void irq();
    void nmi();

public:
    [[nodiscard]] const cpu_state& get_state() const;
    [[nodiscard]] cpu_state& get_state();

private:
    using instruction_callback = void (cpu::*)();

    struct instruction_callback_table : public cpu_opcode_table<instruction_callback>
    {
        static consteval instruction_callback_table create();
    };

    friend struct instruction_callback_table;

private:
#pragma region Instruction Helpers
    template<cpu_addr_mode AddrModeT>
    void add_with_carry(byte_t value);

    void branch(bool condition);

    template<cpu_addr_mode AddrModeT>
    void compare(byte_t to);

    template<cpu_addr_mode AddrModeT>
    void load(byte_t& value);

    template<cpu_addr_mode AddrModeT>
    void store(byte_t value);
#pragma endregion

#pragma region Instructions
    template<cpu_addr_mode AddrModeT>
    void instruction_adc();

    template<cpu_addr_mode AddrModeT>
    void instruction_and();

    template<cpu_addr_mode AddrModeT>
    void instruction_asl();

    template<cpu_addr_mode AddrModeT>
    void instruction_bcc();

    template<cpu_addr_mode AddrModeT>
    void instruction_bcs();

    template<cpu_addr_mode AddrModeT>
    void instruction_beq();

    template<cpu_addr_mode AddrModeT>
    void instruction_bit();

    template<cpu_addr_mode AddrModeT>
    void instruction_bmi();

    template<cpu_addr_mode AddrModeT>
    void instruction_bne();

    template<cpu_addr_mode AddrModeT>
    void instruction_bpl();

    template<cpu_addr_mode AddrModeT>
    void instruction_brk();

    template<cpu_addr_mode AddrModeT>
    void instruction_bvc();

    template<cpu_addr_mode AddrModeT>
    void instruction_bvs();

    template<cpu_addr_mode AddrModeT>
    void instruction_clc();

    template<cpu_addr_mode AddrModeT>
    void instruction_cld();

    template<cpu_addr_mode AddrModeT>
    void instruction_cli();

    template<cpu_addr_mode AddrModeT>
    void instruction_clv();

    template<cpu_addr_mode AddrModeT>
    void instruction_cmp();

    template<cpu_addr_mode AddrModeT>
    void instruction_cpx();

    template<cpu_addr_mode AddrModeT>
    void instruction_cpy();

    template<cpu_addr_mode AddrModeT>
    void instruction_dec();

    template<cpu_addr_mode AddrModeT>
    void instruction_dex();

    template<cpu_addr_mode AddrModeT>
    void instruction_dey();

    template<cpu_addr_mode AddrModeT>
    void instruction_eor();

    template<cpu_addr_mode AddrModeT>
    void instruction_inc();

    template<cpu_addr_mode AddrModeT>
    void instruction_inx();

    template<cpu_addr_mode AddrModeT>
    void instruction_iny();

    template<cpu_addr_mode AddrModeT>
    void instruction_jmp();

    template<cpu_addr_mode AddrModeT>
    void instruction_jsr();

    template<cpu_addr_mode AddrModeT>
    void instruction_lda();

    template<cpu_addr_mode AddrModeT>
    void instruction_ldx();

    template<cpu_addr_mode AddrModeT>
    void instruction_ldy();

    template<cpu_addr_mode AddrModeT>
    void instruction_lsr();

    template<cpu_addr_mode AddrModeT>
    void instruction_nop();

    template<cpu_addr_mode AddrModeT>
    void instruction_ora();

    template<cpu_addr_mode AddrModeT>
    void instruction_pha();

    template<cpu_addr_mode AddrModeT>
    void instruction_php();

    template<cpu_addr_mode AddrModeT>
    void instruction_pla();

    template<cpu_addr_mode AddrModeT>
    void instruction_plp();

    template<cpu_addr_mode AddrModeT>
    void instruction_rti();

    template<cpu_addr_mode AddrModeT>
    void instruction_rts();

    template<cpu_addr_mode AddrModeT>
    void instruction_rol();

    template<cpu_addr_mode AddrModeT>
    void instruction_ror();

    template<cpu_addr_mode AddrModeT>
    void instruction_sbc();

    template<cpu_addr_mode AddrModeT>
    void instruction_sec();

    template<cpu_addr_mode AddrModeT>
    void instruction_sed();

    template<cpu_addr_mode AddrModeT>
    void instruction_sei();

    template<cpu_addr_mode AddrModeT>
    void instruction_sta();

    template<cpu_addr_mode AddrModeT>
    void instruction_stx();

    template<cpu_addr_mode AddrModeT>
    void instruction_sty();

    template<cpu_addr_mode AddrModeT>
    void instruction_tax();

    template<cpu_addr_mode AddrModeT>
    void instruction_tay();

    template<cpu_addr_mode AddrModeT>
    void instruction_tsx();

    template<cpu_addr_mode AddrModeT>
    void instruction_txa();

    template<cpu_addr_mode AddrModeT>
    void instruction_txs();

    template<cpu_addr_mode AddrModeT>
    void instruction_tya();

#if NESE_UNOFFICIAL_INSTRUCTIONS_ENABLED
    template<cpu_addr_mode AddrModeT>
    void instruction_dcp();

    template<cpu_addr_mode AddrModeT>
    void instruction_isb();

    template<cpu_addr_mode AddrModeT>
    void instruction_lax();

    template<cpu_addr_mode AddrModeT>
    void instruction_rla();

    template<cpu_addr_mode AddrModeT>
    void instruction_rra();

    template<cpu_addr_mode AddrModeT>
    void instruction_sax();

    template<cpu_addr_mode AddrModeT>
    void instruction_slo();

    template<cpu_addr_mode AddrModeT>
    void instruction_sre();
#endif
#pragma endregion

    // registers
    byte_t& a();
    byte_t& x();
    byte_t& y();

    byte_t& sp();

    word_t& pc();

    byte_t& status();

    void step_cycle(u64_t cycle);
    void step_cycle(cpu_cycle_t cycle);

    // status
    [[nodiscard]] bool is_status_set(cpu_status status) const;
    [[nodiscard]] bool is_status_clear(cpu_status status) const;

    void set_status(cpu_status status, bool value);
    void set_status(cpu_status status);
    void clear_status(cpu_status status);

    // memory
    [[nodiscard]] byte_t read(addr_t addr);
    [[nodiscard]] word_t readw(addr_t addr);

    void write(addr_t addr, byte_t value);
    void writew(addr_t addr, word_t value);

    // stack
    [[nodiscard]] byte_t pop();
    [[nodiscard]] word_t popw();

    void push(byte_t value);
    void pushw(word_t value);

    // decoding
    [[nodiscard]] byte_t decode();
    [[nodiscard]] word_t decodew();

    // operand
    template<cpu_addr_mode AddrModeT>
    [[nodiscard]] word_t decode_operand();

    template<cpu_addr_mode AddrModeT>
    [[nodiscard]] word_t decode_operand(bool& page_crossing);

    template<cpu_addr_mode AddrModeT>
    [[nodiscard]] addr_t decode_operand_addr();

    template<cpu_addr_mode AddrModeT>
    [[nodiscard]] addr_t decode_operand_addr(bool& page_crossing);

    template<cpu_addr_mode AddrModeT>
    [[nodiscard]] byte_t read_operand(word_t operand);

    template<cpu_addr_mode AddrModeT>
    void write_operand(word_t operand, byte_t value);

    // cycle cost
    template<cpu_addr_mode AddrModeT>
    [[nodiscard]] constexpr cpu_cycle_t get_addr_mode_cycle_cost(bool page_crossing = false);

private:
    static constexpr instruction_callback_table _instructions{instruction_callback_table::create()};

    ref_wrap<BusT> _bus;
    cpu_state _state{};
};

} // namespace nese

#include <nese/cpu.inl>
