#pragma once

#include <span>
#include <variant>

#include <nese/basic_types.hpp>
#include <nese/cpu.hpp>
#include <nese/cpu_bus_mock.hpp>
#include <nese/utility/concat.hpp>
#include <nese/utility/hex.hpp>

namespace nese {

template<typename ValueT>
concept cpu_memval = std::same_as<ValueT, byte_t> || std::same_as<ValueT, word_t>;

struct cpu_fixture
{
    // Standard PC address
    static constexpr addr_x default_pc_addr = 0x0201;

    // Small offset
    static constexpr byte_x default_base_addr = 0x10;

    // Small offset
    static constexpr byte_x indexed_offset = 0x20;

    // Well within the zero page
    static constexpr byte_x zero_page_base_addr = 0x20;

    // Well within the absolute
    static constexpr addr_x absolute_base_addr = 0x0301;

    struct operation_context
    {
        addr_t operand_addr;
        cpu_addr_mode addr_mode;
        cpu_bus_mock& bus;
    };

    struct op
    {
        template<cpu_memval ValueT>
        struct set_operand
        {
            set_operand(ValueT v);

            void execute(operation_context& ctx) const;
            [[nodiscard]] string_view to_string() const;

            hex<ValueT> value;
        };

        template<cpu_memval ValueT>
        struct set_memory
        {
            set_memory(addr_t a, ValueT v);

            void execute(operation_context& ctx) const;
            [[nodiscard]] string_view to_string() const;

            addr_x addr;
            hex<ValueT> value;
        };

        template<cpu_memval ValueT>
        struct set_stack
        {
            set_stack(byte_t sp_, ValueT value_);

            void execute(operation_context& ctx) const;
            [[nodiscard]] string_view to_string() const;

            byte_x sp;
            hex<ValueT> value;
        };

        template<cpu_memval ValueT>
        struct push_stack
        {
            push_stack(ValueT byte);

            void execute(operation_context& ctx) const;
            [[nodiscard]] string_view to_string() const;

            hex<ValueT> value;
        };

        template<cpu_register_id RegisterIdT, cpu_memval ValueT = byte_t>
        struct set_register
        {
            set_register(ValueT v);

            void execute(operation_context& ctx) const;
            [[nodiscard]] string_view to_string() const;

            hex<ValueT> value;
        };

        template<cpu_status StatusFlagT>
        struct set_register_status
        {
            void execute(operation_context& ctx) const;
            [[nodiscard]] string_view to_string() const;
        };

        template<cpu_status StatusFlagT>
        struct clear_register_status
        {
            void execute(operation_context& ctx) const;
            [[nodiscard]] string_view to_string() const;
        };

        template<typename BasicOperationT>
        struct conditional
        {
            conditional(BasicOperationT op, bool condition);

            void execute(operation_context& ctx) const;
            [[nodiscard]] string_view to_string() const;

            BasicOperationT operation;
            bool condition;
        };
    };

    using basic_operation = std::variant<
        op::set_operand<byte_t>,
        op::set_operand<word_t>,

        op::set_memory<byte_t>,
        op::set_memory<word_t>,

        op::set_stack<byte_t>,
        op::set_stack<word_t>,

        op::push_stack<byte_t>,
        op::push_stack<word_t>,

        op::set_register<cpu_register_id::a, byte_t>,
        op::set_register<cpu_register_id::x, byte_t>,
        op::set_register<cpu_register_id::y, byte_t>,
        op::set_register<cpu_register_id::sp, byte_t>,
        op::set_register<cpu_register_id::pc, word_t>,
        op::set_register<cpu_register_id::status, byte_t>,

        op::set_register_status<cpu_status::carry>,
        op::set_register_status<cpu_status::break_cmd>,
        op::set_register_status<cpu_status::decimal>,
        op::set_register_status<cpu_status::interrupt>,
        op::set_register_status<cpu_status::negative>,
        op::set_register_status<cpu_status::overflow>,
        op::set_register_status<cpu_status::unused>,
        op::set_register_status<cpu_status::zero>,

        op::clear_register_status<cpu_status::carry>,
        op::clear_register_status<cpu_status::break_cmd>,
        op::clear_register_status<cpu_status::decimal>,
        op::clear_register_status<cpu_status::interrupt>,
        op::clear_register_status<cpu_status::negative>,
        op::clear_register_status<cpu_status::overflow>,
        op::clear_register_status<cpu_status::unused>,
        op::clear_register_status<cpu_status::zero>

        >;

    using operation = concat_t<basic_operation, op::conditional<basic_operation>>;

    struct scenario
    {
        [[nodiscard]] string to_string() const;
        [[nodiscard]] bool should_debug_break(cpu_opcode opcode) const;

        std::vector<operation> initial;
        std::vector<operation> expected;
        string description{};
        cpu_cycle_t cycle_cost{0};
        std::optional<cpu_opcode> debug_break_opcode{std::nullopt};
        bool debug_break{false};
    };

    static op::set_operand<byte_t> set_operand(byte_t value);
    static op::set_operand<word_t> set_operandw(word_t value);

    static op::set_memory<byte_t> set_memory(addr_t addr, byte_t value);
    static op::set_memory<word_t> set_memoryw(addr_t addr, word_t value);

    static op::set_stack<byte_t> set_stack(byte_t sp, byte_t value);
    static op::set_stack<word_t> set_stackw(byte_t sp, word_t value);

    static op::push_stack<byte_t> push_stack(byte_t value);
    static op::push_stack<word_t> push_stackw(word_t value);

    static op::set_register<cpu_register_id::a, byte_t> set_a(byte_t value);
    static op::set_register<cpu_register_id::x, byte_t> set_x(byte_t value);
    static op::set_register<cpu_register_id::y, byte_t> set_y(byte_t value);
    static op::set_register<cpu_register_id::sp, byte_t> set_sp(byte_t value);
    static op::set_register<cpu_register_id::pc, word_t> set_pc(word_t value);
    static op::set_register<cpu_register_id::status, byte_t> set_status(byte_t value);
    static op::set_register<cpu_register_id::status, byte_t> set_status(cpu_status value);

    static op::set_register_status<cpu_status::carry> set_status_carry();
    static op::set_register_status<cpu_status::break_cmd> set_status_break_cmd();
    static op::set_register_status<cpu_status::decimal> set_status_decimal();
    static op::set_register_status<cpu_status::interrupt> set_status_interrupt();
    static op::set_register_status<cpu_status::negative> set_status_negative();
    static op::set_register_status<cpu_status::overflow> set_status_overflow();
    static op::set_register_status<cpu_status::unused> set_status_unused();
    static op::set_register_status<cpu_status::zero> set_status_zero();

    static op::clear_register_status<cpu_status::carry> clear_status_carry();
    static op::clear_register_status<cpu_status::break_cmd> clear_status_break_cmd();
    static op::clear_register_status<cpu_status::decimal> clear_status_decimal();
    static op::clear_register_status<cpu_status::interrupt> clear_status_interrupt();
    static op::clear_register_status<cpu_status::negative> clear_status_negative();
    static op::clear_register_status<cpu_status::overflow> clear_status_overflow();
    static op::clear_register_status<cpu_status::unused> clear_status_unused();
    static op::clear_register_status<cpu_status::zero> clear_status_zero();

    static op::conditional<basic_operation> conditional(basic_operation operation, bool condition);

    void execute_operations(auto& operations, operation_context& context);

    void test_implied(cpu_opcode opcode, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost = cpu_cycle_t(0));
    void test_immediate(cpu_opcode opcode, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost = cpu_cycle_t(2));
    void test_acculumator(cpu_opcode opcode, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost = cpu_cycle_t(2));
    void test_zero_page(cpu_opcode opcode, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost = cpu_cycle_t(3));
    void test_zero_page_indexed(cpu_opcode opcode, cpu_register_id index_register, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost = cpu_cycle_t(4));
    void test_absolute(cpu_opcode opcode, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost = cpu_cycle_t(4));
    void test_absolute_indexed(cpu_opcode opcode, cpu_register_id index_register, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost = cpu_cycle_t(4));
    void test_indexed_indirect(cpu_opcode opcode, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost = cpu_cycle_t(6));
    void test_indirect_indexed(cpu_opcode opcode, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost = cpu_cycle_t(5));
    void test_relative(cpu_opcode opcode, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost = cpu_cycle_t(0));
    void test_unspecified(cpu_opcode opcode, std::span<const scenario> behavior_scenarios, cpu_cycle_t cycle_cost = cpu_cycle_t(0));

    void execute_and_check(cpu_opcode opcode, bool should_check_cycle = true);
    void check_registers() const;
    void check_memory() const;
    void check_cycle() const;

    cpu_bus_mock bus;
    cpu_bus_mock expected_bus;
};

inline std::ostream& operator<<(std::ostream& os, const cpu_fixture::scenario& scenario)
{
    os << scenario.to_string();
    return os;
}

} // namespace nese
