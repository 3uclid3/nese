#pragma once

#include <fmt/format.h>
#include <spdlog/logger.h>

#include <nese/addr_mode.hpp>
#include <nese/basic_types.hpp>
#include <nese/cpu_registers.hpp>
#include <nese/cycle.hpp>

#ifndef NESE_CPU_DEBUGBREAK_ENABLED
#define NESE_CPU_DEBUGBREAK_ENABLED 1
#endif

#ifndef NESE_CPU_LOG_NINTENDULATOR_ENABLED
#define NESE_CPU_LOG_NINTENDULATOR_ENABLED 1
#endif

namespace nese {

class ram;

class cpu
{
public:
    explicit cpu(ram& ram);

    void power_on();
    void reset();

    void step_to(cycle_t system_cycle);

    [[nodiscard]] bool has_stop_requested() const;
    void stop();

    [[nodiscard]] cycle_t get_cycle() const;
    [[nodiscard]] const cpu_registers& get_registers() const;

    void set_code_addr(addr_t addr);

    const ram& get_ram() const { return _ram; } // TODO Remove

#if NESE_CPU_DEBUGBREAK_ENABLED
    void set_debugbreak(addr_t addr);
#endif

private:
    using opcode_callback = void (*)(cpu&);

    struct opcode_table;
    struct opcode_table_singleton;

    [[nodiscard]] static addr_mode get_opcode_addr_mode(byte_t code);
    [[nodiscard]] static opcode_callback get_opcode_callback(byte_t code);
    [[nodiscard]] static std::string_view get_opcode_string(byte_t code);

private:
    void execute_next_instruction();

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

private:
    void push_byte(byte_t value);
    [[nodiscard]] byte_t pop_byte();

    void push_word(word_t value);
    [[nodiscard]] word_t pop_word();

private:
    [[nodiscard]] bool has_status_flags(cpu_status_flags mask) const;
    [[nodiscard]] bool has_status_flag(cpu_status_flag mask) const;
    void set_status_flag(cpu_status_flag mask, bool value);

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
    template<addr_mode AddrMode>
    static void execute_opcode_adc(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_and(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_b__(cpu& self, bool condition);

    template<addr_mode AddrMode>
    static void execute_opcode_bcc(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_bcs(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_beq(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_bmi(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_bne(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_bpl(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_bvc(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_bvs(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_bit(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_clc(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_cld(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_cli(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_clv(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_c__(cpu& self, byte_t to_byte);

    template<addr_mode AddrMode>
    static void execute_opcode_cmp(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_cpx(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_cpy(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_eor(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_de_(cpu& self, byte_t& byte);

    template<addr_mode AddrMode>
    static void execute_opcode_dex(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_dey(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_in_(cpu& self, byte_t& byte);

    template<addr_mode AddrMode>
    static void execute_opcode_inx(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_iny(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_jmp(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_jsr(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_ld_(cpu& self, byte_t& byte);

    template<addr_mode AddrMode>
    static void execute_opcode_lda(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_ldx(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_ldy(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_nop(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_ora(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_pha(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_php(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_pla(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_plp(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_rts(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_sbc(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_sec(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_sed(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_sei(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_st_(cpu& self, byte_t byte);

    template<addr_mode AddrMode>
    static void execute_opcode_sta(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_stx(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_sty(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_t__(cpu& self, byte_t from_byte, byte_t& to_byte);

    template<addr_mode AddrMode>
    static void execute_opcode_tax(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_tay(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_tsx(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_txa(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_txs(cpu& self);

    template<addr_mode AddrMode>
    static void execute_opcode_tya(cpu& self);

private:
    cpu_registers _registers{};
    cycle_t _cycle{0};

    ram& _ram;

    bool _stop_requested{false};

#if NESE_CPU_DEBUGBREAK_ENABLED
    bool _debugbreak{false};
    uint16_t _debugbreak_addr{0x0};
#endif

#if NESE_CPU_LOG_NINTENDULATOR_ENABLED
    std::shared_ptr<spdlog::logger> _nintendulator_logger;
#endif

    friend struct fmt::formatter<cpu>;
};

inline bool cpu::has_stop_requested() const
{
    return _stop_requested;
}

inline void cpu::stop()
{
    _stop_requested = true;
}

inline cycle_t cpu::get_cycle() const
{
    return _cycle;
}

inline const cpu_registers& cpu::get_registers() const
{
    return _registers;
}

inline void cpu::set_code_addr(addr_t addr)
{
    _registers.pc = addr;
}

#if NESE_CPU_DEBUGBREAK_ENABLED
inline void cpu::set_debugbreak(addr_t addr)
{
    _debugbreak = true;
    _debugbreak_addr = addr;
}
#endif

} // namespace nese

#include <nese/cpu_fmt.inl>
