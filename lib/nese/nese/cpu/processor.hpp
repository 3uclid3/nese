#pragma once

#include <fmt/format.h>
#include <spdlog/logger.h>

#include <nese/basic_types.hpp>
#include <nese/cpu/addr_mode.hpp>
#include <nese/cpu/registers.hpp>
#include <nese/cycle.hpp>

#ifndef NESE_CPU_DEBUGBREAK_ENABLED
#define NESE_CPU_DEBUGBREAK_ENABLED 1
#endif

#ifndef NESE_CPU_LOG_NINTENDULATOR_ENABLED
#define NESE_CPU_LOG_NINTENDULATOR_ENABLED 1
#endif

namespace nese::memory {
class ram;
} // namespace nese::memory

namespace nese::cpu {

class processor
{
public:
    explicit processor(memory::ram& ram);

    void power_on();
    void reset();

    void step();

    [[deprecated("use step() instead")]]
    void step_to(cycle_t system_cycle);

    [[nodiscard]] bool has_stop_requested() const;
    void stop();

    [[nodiscard]] cycle_t get_cycle() const;
    [[nodiscard]] const registers& get_registers() const;

    void set_code_addr(addr_t addr);

    const memory::ram& get_ram() const { return _ram; } // TODO Remove

#if NESE_CPU_DEBUGBREAK_ENABLED
    void set_debugbreak(addr_t addr);
#endif

private:
    [[nodiscard]] byte_t get_byte_from_memory(addr_t addr) const;
    void set_byte_to_memory(addr_t addr, byte_t value);

    [[nodiscard]] word_t get_word_from_memory(addr_t addr) const;

private:
    [[nodiscard]] byte_t decode_byte();
    [[nodiscard]] word_t decode_word();

    template<addr_mode AddrMode>
    [[nodiscard]] word_t decode_operand();

    template<addr_mode AddrMode>
    [[nodiscard]] word_t decode_operand_addr();

    template<addr_mode AddrMode>
    [[nodiscard]] byte_t read_operand();

    template<addr_mode AddrMode>
    [[nodiscard]] byte_t read_operand(word_t op);

    template<addr_mode AddrMode>
    void write_operand(byte_t addr, byte_t value);

private:
    void push_byte(byte_t value);
    [[nodiscard]] byte_t pop_byte();

    void push_word(word_t value);
    [[nodiscard]] word_t pop_word();

private:
    [[nodiscard]] bool has_status_flags(status_flags mask) const;
    [[nodiscard]] bool has_status_flag(status_flag mask) const;
    void set_status_flag(status_flag mask, bool value);

    [[nodiscard]] bool has_status_carry() const;
    void set_status_carry(bool value);

    [[nodiscard]] bool has_status_zero() const;
    void set_status_zero(bool value);

    [[nodiscard]] bool has_status_interrupt() const;
    void set_status_interrupt(bool value);

    [[nodiscard]] bool has_status_decimal() const;
    void set_status_decimal(bool value);

    [[nodiscard]] bool has_status_break_cmd() const;
    void set_status_break_cmd(bool value);

    [[nodiscard]] bool has_status_unused() const;
    void set_status_unused(bool value);

    [[nodiscard]] bool has_status_overflow() const;
    void set_status_overflow(bool value);

    [[nodiscard]] bool has_status_negative() const;
    void set_status_negative(bool value);

    void set_status_alu(byte_t value);

    static bool is_sign_overflow(byte_t old_byte, byte_t new_byte, byte_t byte);

private:
    using instruction_callback = void (*)(processor&);

    struct instruction_table;
    struct instruction_table_singleton;

    [[nodiscard]] static addr_mode get_instruction_addr_mode(byte_t op_code);
    [[nodiscard]] static instruction_callback get_instruction_callback(byte_t op_code);
    [[nodiscard]] static std::string_view get_instruction_string(byte_t op_code);

private:
    void execute_next_instruction();

    template<addr_mode AddrMode>
    static void execute_instruction_adc(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_and(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_asl(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_b__(processor& self, bool condition);

    template<addr_mode AddrMode>
    static void execute_instruction_bcc(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_bcs(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_beq(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_bmi(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_bne(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_bpl(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_bvc(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_bvs(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_bit(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_clc(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_cld(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_cli(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_clv(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_c__(processor& self, byte_t to_byte);

    template<addr_mode AddrMode>
    static void execute_instruction_cmp(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_cpx(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_cpy(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_eor(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_de_(processor& self, byte_t& byte);

    template<addr_mode AddrMode>
    static void execute_instruction_dex(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_dey(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_in_(processor& self, byte_t& byte);

    template<addr_mode AddrMode>
    static void execute_instruction_inx(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_iny(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_jmp(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_jsr(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_ld_(processor& self, byte_t& byte);

    template<addr_mode AddrMode>
    static void execute_instruction_lda(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_ldx(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_ldy(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_lsr(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_nop(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_ora(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_pha(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_php(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_pla(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_plp(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_rti(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_rts(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_rol(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_ror(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_sbc(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_sec(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_sed(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_sei(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_st_(processor& self, byte_t byte);

    template<addr_mode AddrMode>
    static void execute_instruction_sta(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_stx(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_sty(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_t__(processor& self, byte_t from_byte, byte_t& to_byte);

    template<addr_mode AddrMode>
    static void execute_instruction_tax(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_tay(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_tsx(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_txa(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_txs(processor& self);

    template<addr_mode AddrMode>
    static void execute_instruction_tya(processor& self);

private:
    registers _registers{};
    cycle_t _cycle{0};

    memory::ram& _ram;

    bool _stop_requested{false};

#if NESE_CPU_DEBUGBREAK_ENABLED
    bool _debugbreak{false};
    uint16_t _debugbreak_addr{0x0};
#endif

#if NESE_CPU_LOG_NINTENDULATOR_ENABLED
    std::shared_ptr<spdlog::logger> _nintendulator_logger;
#endif

    friend struct fmt::formatter<processor>;
};

inline bool processor::has_stop_requested() const
{
    return _stop_requested;
}

inline void processor::stop()
{
    _stop_requested = true;
}

inline cycle_t processor::get_cycle() const
{
    return _cycle;
}

inline const registers& processor::get_registers() const
{
    return _registers;
}

inline void processor::set_code_addr(addr_t addr)
{
    _registers.pc = addr;
}

#if NESE_CPU_DEBUGBREAK_ENABLED
inline void processor::set_debugbreak(addr_t addr)
{
    _debugbreak = true;
    _debugbreak_addr = addr;
}
#endif

} // namespace nese::cpu

#include <nese/cpu/processor_fmt.inl>
