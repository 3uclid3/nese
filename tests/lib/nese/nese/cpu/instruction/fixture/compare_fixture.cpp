#include <nese/cpu/instruction/fixture/compare_fixture.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/utility/format.hpp>

namespace nese::cpu::instruction {

void compare_fixture::test_immediate(opcode opcode, register_id cmp_register)
{
    SECTION("immediate")
    {
        constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(2);

        SECTION("addressing")
        {
            const addr_t addr = GENERATE(as<addr_t>(), 0x0000, 0x7FFE, 0xFFFE);

            state().registers.pc = addr;
            memory().set_byte(addr, 0);
            set_register(state().registers, cmp_register, 0);

            expected_state().cycle = cycle_cost;
            expected_state().registers.pc = addr + 1;
            expected_state().registers.set_flag(status_flag::carry);
            expected_state().registers.set_flag(status_flag::zero);

            execute_and_check(opcode);
        }

        SECTION("behavior")
        {
            state().registers.pc = default_pc_addr;
            state().registers.p = 0;

            auto [registers_value, value, flags] = GENERATE(from_range(scenarios));

            memory().set_byte(default_pc_addr, value);
            set_register(state().registers, cmp_register, registers_value);

            expected_state().cycle = cycle_cost;
            expected_state().registers.pc = default_pc_addr + 1;
            expected_state().registers.p = flags;

            execute_and_check(opcode);
        }
    }
}

void compare_fixture::test_zero_page(opcode opcode, register_id cmp_register)
{
    SECTION("zero_page")
    {
        constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(3);

        SECTION("addressing")
        {
            const auto [pc_addr, base_addr] = GENERATE(from_range(zero_page_scenarios));

            state().registers.pc = pc_addr;
            memory().set_byte(pc_addr, base_addr);
            memory().set_byte(base_addr, 0);
            set_register(state().registers, cmp_register, 0);

            expected_state().cycle = cycle_cost;
            expected_state().registers.pc = pc_addr + 1;
            expected_state().registers.set_flag(status_flag::carry);
            expected_state().registers.set_flag(status_flag::zero);

            execute_and_check(opcode);
        }

        SECTION("behavior")
        {
            state().registers.p = 0;
            state().registers.pc = default_pc_addr;
            memory().set_byte(default_pc_addr, zero_page_base_addr);

            auto [registers_value, value, flags] = GENERATE(from_range(scenarios));

            memory().set_byte(zero_page_base_addr, value);
            set_register(state().registers, cmp_register, registers_value);

            expected_state().cycle = cycle_cost;
            expected_state().registers.pc = default_pc_addr + 1;
            expected_state().registers.p = flags;

            execute_and_check(opcode);
        }
    }
}

void compare_fixture::test_zero_page_indexed(opcode opcode, register_id cmp_register, register_id index_register)
{
    DYNAMIC_SECTION(format("zero_page_{}", index_register))
    {
        constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(4);

        SECTION("addressing")
        {
            const auto [pc_addr, base_addr, idx] = GENERATE(from_range(zero_page_indexed_scenarios));
            const byte_t indexed_addr = base_addr + idx & 0xff;

            state().registers.pc = pc_addr;
            memory().set_byte(pc_addr, base_addr);
            memory().set_byte(indexed_addr, 0);
            set_register(state().registers, index_register, idx);
            set_register(state().registers, cmp_register, 0);

            expected_state().cycle = cycle_cost;
            expected_state().registers.pc = pc_addr + 1;
            expected_state().registers.set_flag(status_flag::carry);
            expected_state().registers.set_flag(status_flag::zero);

            execute_and_check(opcode);
        }

        SECTION("behavior")
        {
            constexpr byte_t indexed_addr = zero_page_base_addr + indexed_offset;

            state().registers.p = 0;
            state().registers.pc = default_pc_addr;
            memory().set_byte(default_pc_addr, zero_page_base_addr);

            auto [registers_value, value, flags] = GENERATE(from_range(scenarios));

            memory().set_byte(indexed_addr, value);
            set_register(state().registers, index_register, indexed_offset);
            set_register(state().registers, cmp_register, registers_value);

            expected_state().cycle = cycle_cost;
            expected_state().registers.pc = default_pc_addr + 1;
            expected_state().registers.p = flags;

            execute_and_check(opcode);
        }
    }
}

void compare_fixture::test_absolute(opcode opcode, register_id cmp_register)
{
    SECTION("absolute")
    {
        constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(4);

        SECTION("addressing")
        {
            const auto [pc_addr, base_addr] = GENERATE(from_range(absolute_scenarios));

            state().registers.pc = pc_addr;
            memory().set_word(pc_addr, base_addr);
            memory().set_byte(base_addr, 0);
            set_register(state().registers, cmp_register, 0);

            expected_state().cycle = cycle_cost;
            expected_state().registers.pc = pc_addr + 2;
            expected_state().registers.set_flag(status_flag::carry);
            expected_state().registers.set_flag(status_flag::zero);

            execute_and_check(opcode);
        }

        SECTION("behavior")
        {
            state().registers.p = 0;
            state().registers.pc = default_pc_addr;
            memory().set_word(default_pc_addr, absolute_base_addr);

            auto [registers_value, value, flags] = GENERATE(from_range(scenarios));

            memory().set_byte(absolute_base_addr, value);
            set_register(state().registers, cmp_register, registers_value);

            expected_state().cycle = cycle_cost;
            expected_state().registers.pc = default_pc_addr + 2;
            expected_state().registers.p = flags;

            execute_and_check(opcode);
        }
    }
}

void compare_fixture::test_absolute_indexed(opcode opcode, register_id cmp_register, register_id index_register)
{
    DYNAMIC_SECTION(format("absolute_{}", index_register))
    {
        // constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(4);

        SECTION("addressing")
        {
            const auto [pc_addr, base_addr, idx] = GENERATE(from_range(absolute_indexed_scenarios));
            const addr_t indexed_addr = base_addr + idx;

            state().registers.pc = pc_addr;
            memory().set_word(pc_addr, base_addr);
            memory().set_byte(indexed_addr, 0);
            set_register(state().registers, index_register, idx);
            set_register(state().registers, cmp_register, 0);

            // expected_state().cycle = cycle_cost;
            expected_state().registers.pc = pc_addr + 2;
            expected_state().registers.set_flag(status_flag::carry);
            expected_state().registers.set_flag(status_flag::zero);

            execute_and_check(opcode, false);
        }

        SECTION("behavior")
        {
            constexpr addr_t indexed_addr = absolute_base_addr + indexed_offset;

            state().registers.p = 0;
            state().registers.pc = default_pc_addr;
            memory().set_word(default_pc_addr, absolute_base_addr);

            auto [registers_value, value, flags] = GENERATE(from_range(scenarios));

            memory().set_byte(indexed_addr, value);
            set_register(state().registers, index_register, indexed_offset);
            set_register(state().registers, cmp_register, registers_value);

            // expected_state().cycle = cycle_cost;
            expected_state().registers.pc = default_pc_addr + 2;
            expected_state().registers.p = flags;

            execute_and_check(opcode, false);
        }
    }
}

}