#pragma once

#include <variant>

#include <nese/cpu/instruction/execute.hpp>
#include <nese/cpu/instruction/opcode.hpp>
#include <nese/cpu/stack.hpp>
#include <nese/cpu/state.hpp>
#include <nese/cpu/state_mock.hpp>
#include <nese/cpu/status_flag.hpp>
#include <nese/memory/mapper.hpp>
#include <nese/utility/assert.hpp>
#include <nese/utility/format.hpp>
#include <nese/utility/hex.hpp>

namespace nese::cpu::instruction {

struct execute_fixture
{
    template<typename T, typename... Args>
    struct concat;

    template<typename... Args0, typename... Args1>
    struct concat<std::variant<Args0...>, Args1...>
    {
        using type = std::variant<Args0..., Args1...>;
    };

    template<typename T, typename... Args>
    using concat_t = typename concat<T, Args...>::type;

    static inline state_mock default_state{[] {
        state_mock state;

        state.registers.a = 0xA;
        state.registers.x = 0xB;
        state.registers.y = 0xC;

        state.registers.p = 0x0; // clear flags

        return state;
    }()};

    static inline memory::mapper default_memory{[] {
        constexpr byte_t first_tag = 0xDE;
        constexpr byte_t second_tag = 0xAD;

        memory::mapper memory;

        byte_t value = first_tag;
        for (size_t i = 0; i < memory::mapper::capacity; ++i)
        {
            memory.set_byte(static_cast<addr_t>(i), value);

            value = value == first_tag ? second_tag : first_tag;
        }

        return memory;
    }()};

    struct change_context
    {
        addr_t operand_addr;
        addr_mode addr_mode;
        cpu::state_mock& state;
        memory::mapper& memory;
    };

    struct set_operand_value
    {
        constexpr set_operand_value(byte_t byte)
            : operand_value(byte) {}

        void apply(change_context& ctx) const
        {
            switch (ctx.addr_mode)
            {
            case addr_mode::accumulator:
                ctx.state.registers.a = operand_value;
                break;

            default:
                ctx.memory.set_byte(ctx.operand_addr, operand_value);
                break;
            }
        }

        string_view to_string() const
        {
            return format("operand_value := {}", operand_value);
        }

        byte_x operand_value;
    };

    struct set_operand_word_value
    {
        constexpr set_operand_word_value(word_t word)
            : operand_value(word) {}

        void apply(change_context& ctx) const
        {
            ctx.memory.set_word(ctx.operand_addr, operand_value);
        }

        string_view to_string() const
        {
            return format("operand_value := {}", operand_value);
        }

        word_x operand_value;
    };

    struct set_memory_value
    {
        constexpr set_memory_value(addr_t a, byte_t v)
            : addr(a)
            , value(v) {}

        void apply(change_context& ctx) const
        {
            ctx.memory.set_byte(addr, value);
        }

        string_view to_string() const
        {
            return format("addr := {}, value := {}", addr, value);
        }

        addr_x addr;
        byte_x value;
    };

    struct set_memory_word_value
    {
        constexpr set_memory_word_value(addr_t a, word_t v)
            : addr(a)
            , value(v) {}

        void apply(change_context& ctx) const
        {
            ctx.memory.set_word(addr, value);
        }

        string_view to_string() const
        {
            return format("addr := {}, value := {}", addr, value);
        }

        addr_x addr;
        word_x value;
    };

    struct set_stack_value
    {
        constexpr set_stack_value(byte_t s_, byte_t v)
            : s(s_)
            , value(v) {}

        void apply(change_context& ctx) const
        {
            ctx.memory.set_byte(stack_offset + s, value);
        }

        string_view to_string() const
        {
            return format("s := {}, value := {}", s, value);
        }

        byte_x s;
        byte_x value;
    };

    struct set_stack_word_value
    {
        constexpr set_stack_word_value(byte_t s_, word_t v)
            : s(s_)
            , value(v) {}

        void apply(change_context& ctx) const
        {
            const byte_t s0 = s;
            const byte_t s1 = s + 1;

            ctx.memory.set_byte(stack_offset + s0, value & 0xFF);
            ctx.memory.set_byte(stack_offset + s1, value >> 8);
        }

        string_view to_string() const
        {
            return format("s := {}, value := {}", s, value);
        }

        byte_x s;
        word_x value;
    };

    template<register_id RegisterT>
    struct set_register
    {
        constexpr set_register(byte_t v)
            : value(v) {}

        void apply(change_context& ctx) const
        {
            ctx.state.set_register(RegisterT, value);
        }

        string_view to_string() const
        {
            return format("{} := {}", RegisterT, value);
        }

        byte_x value;
    };

    template<>
    struct set_register<register_id::pc>
    {
        constexpr set_register(word_t v)
            : value(v) {}

        void apply(change_context& ctx) const
        {
            ctx.state.registers.pc = value;
        }

        string_view to_string() const
        {
            return format("{} := {}", register_id::pc, value);
        }

        word_x value;
    };

    using set_register_a = set_register<register_id::a>;
    using set_register_x = set_register<register_id::x>;
    using set_register_y = set_register<register_id::y>;
    using set_register_s = set_register<register_id::s>;
    using set_register_pc = set_register<register_id::pc>;

    template<status_flag StatusFlagT>
    struct set_status_flag
    {
        void apply(change_context& ctx) const
        {
            ctx.state.registers.set_flag(StatusFlagT);
        }

        string_view to_string() const
        {
            return format("{} := true", StatusFlagT);
        }
    };

    template<status_flag StatusFlagT>
    struct clear_status_flag
    {
        void apply(change_context& ctx) const
        {
            ctx.state.registers.clear_flag(StatusFlagT);
        }

        string_view to_string() const
        {
            return format("{} := false", StatusFlagT);
        }
    };

    using set_status_flag_carry = set_status_flag<status_flag::carry>;
    using set_status_flag_break_cmd = set_status_flag<status_flag::break_cmd>;
    using set_status_flag_decimal = set_status_flag<status_flag::decimal>;
    using set_status_flag_interrupt = set_status_flag<status_flag::interrupt>;
    using set_status_flag_negative = set_status_flag<status_flag::negative>;
    using set_status_flag_overflow = set_status_flag<status_flag::overflow>;
    using set_status_flag_unused = set_status_flag<status_flag::unused>;
    using set_status_flag_zero = set_status_flag<status_flag::zero>;

    using clear_status_flag_carry = clear_status_flag<status_flag::carry>;
    using clear_status_flag_break_cmd = clear_status_flag<status_flag::break_cmd>;
    using clear_status_flag_decimal = clear_status_flag<status_flag::decimal>;
    using clear_status_flag_interrupt = clear_status_flag<status_flag::interrupt>;
    using clear_status_flag_negative = clear_status_flag<status_flag::negative>;
    using clear_status_flag_overflow = clear_status_flag<status_flag::overflow>;
    using clear_status_flag_unused = clear_status_flag<status_flag::unused>;
    using clear_status_flag_zero = clear_status_flag<status_flag::zero>;

    struct set_status_flags
    {
        constexpr set_status_flags(status_flag v)
            : flags(v) {}

        void apply(change_context& ctx) const
        {
            ctx.state.registers.p = static_cast<u8_t>(flags);
        }

        string_view to_string() const
        {
            return nese::format("p := {:02X}", static_cast<u8_t>(flags));
        }

        status_flags flags;
    };

    struct push_stack
    {
        constexpr push_stack(byte_t byte)
            : value(byte) {}

        void apply(change_context& ctx) const
        {
            ctx.memory.set_byte(ctx.state.registers.s + stack_offset, value);
            --ctx.state.registers.s;
        }

        string_view to_string() const
        {
            return format("push value := {}", value);
        }

        byte_x value;
    };

    struct push_word_stack
    {
        constexpr push_word_stack(word_t word)
            : value(word) {}

        void apply(change_context& ctx) const
        {
            ctx.memory.set_byte(ctx.state.registers.s + stack_offset, value >> 8);
            --ctx.state.registers.s;

            ctx.memory.set_byte(ctx.state.registers.s + stack_offset, value & 0xFF);
            --ctx.state.registers.s;
        }

        string_view to_string() const
        {
            return format("push value := {}", value);
        }

        word_x value;
    };

    using base_scenario_change = std::variant<
        set_operand_value,
        set_operand_word_value,

        set_memory_value,
        set_memory_word_value,

        set_stack_value,
        set_stack_word_value,

        set_register_a,
        set_register_x,
        set_register_y,
        set_register_s,
        set_register_pc,

        set_status_flag_break_cmd,
        set_status_flag_carry,
        set_status_flag_decimal,
        set_status_flag_interrupt,
        set_status_flag_negative,
        set_status_flag_overflow,
        set_status_flag_unused,
        set_status_flag_zero,
        set_status_flags,

        clear_status_flag_break_cmd,
        clear_status_flag_carry,
        clear_status_flag_decimal,
        clear_status_flag_interrupt,
        clear_status_flag_negative,
        clear_status_flag_overflow,
        clear_status_flag_unused,
        clear_status_flag_zero,

        push_stack,
        push_word_stack>;

    struct conditional
    {
        using scenario_change = base_scenario_change;

        constexpr conditional(scenario_change change, bool condition)
            : change(change)
            , condition(condition)
        {}

        void apply(change_context& ctx) const
        {
            if (condition)
            {
                std::visit([&ctx](const auto& c) { c.apply(ctx); }, change);
            }
        }

        string_view to_string() const
        {
            string_view result{};

            if (condition)
            {
                std::visit([&result](const auto& c) { result = c.to_string(); }, change);
            }

            return result;
        }

        scenario_change change;
        bool condition;
    };

    using scenario_change = concat_t<
        base_scenario_change,
        conditional>;

    struct scenario
    {
        string to_string() const;

        std::vector<scenario_change> initial_changes;
        std::vector<scenario_change> expected_changes;
        std::string description{};
        cpu_cycle_t base_cycle_cost{0};
    };

    // Standard PC address
    static constexpr addr_x default_pc_addr = 0x0200;

    // Small offset
    static constexpr byte_x default_base_addr = 0x10;

    // Small offset
    static constexpr byte_x indexed_offset = 0x20;

    // Well within the zero page
    static constexpr byte_x zero_page_base_addr = 0x20;

    // Well within the absolute
    static constexpr addr_x absolute_base_addr = 0x0300;

    void test_acculumator(opcode opcode, std::span<const scenario> behavior_scenarios, cpu_cycle_t base_cycle_cost = cpu_cycle_t(2));
    void test_implied(opcode opcode, std::span<const scenario> behavior_scenarios, cpu_cycle_t base_cycle_cost = cpu_cycle_t(0));
    void test_immediate(opcode opcode, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t base_cycle_cost = cpu_cycle_t(2));
    void test_zero_page(opcode opcode, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t base_cycle_cost = cpu_cycle_t(3));
    void test_zero_page_indexed(opcode opcode, register_id index_register, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t base_cycle_cost = cpu_cycle_t(4));
    void test_absolute(opcode opcode, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t base_cycle_cost = cpu_cycle_t(4));
    void test_absolute_indexed(opcode opcode, register_id index_register, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t base_cycle_cost = cpu_cycle_t(4));
    void test_indexed_indirect(opcode opcode, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t base_cycle_cost = cpu_cycle_t(6));
    void test_indirect_indexed(opcode opcode, const scenario& addressing_scenario, std::span<const scenario> behavior_scenarios, cpu_cycle_t base_cycle_cost = cpu_cycle_t(5));

    void test_unspecified(opcode opcode, std::span<const scenario> behavior_scenarios, cpu_cycle_t base_cycle_cost = cpu_cycle_t(0));

    void apply_changes(auto& changes, change_context& context);

    void execute_and_check(opcode code, bool should_check_cycle = true);

    void check_registers() const;
    void check_memory() const;
    void check_cycle() const;

    cpu::state_mock state{default_state};
    memory::mapper memory{default_memory};

    cpu::state_mock expected_state{};
    memory::mapper expected_memory{};
};

inline std::ostream& operator<<(std::ostream& os, const execute_fixture::scenario& value)
{
    os << value.to_string();
    return os;
}

} // namespace nese::cpu::instruction
