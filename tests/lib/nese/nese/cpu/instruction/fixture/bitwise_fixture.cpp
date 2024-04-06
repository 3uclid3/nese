#include <nese/cpu/instruction/fixture/bitwise_fixture.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction/opcode.hpp>
#include <nese/cpu/status_flag.hpp>
#include <nese/utility/format.hpp>

namespace nese::cpu::instruction {

void bitwise_fixture::test_immediate(opcode opcode, behavior_scenarios behavior_scenarios)
{
    SECTION("immediate")
    {
        constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(2);

        SECTION("addressing")
        {
            const addr_t addr = GENERATE(as<addr_t>(), 0x0000, 0x7FFE, 0xFFFE);

            state().registers.pc = addr;
            state().registers.a = 0;
            memory().set_byte(addr, 0);

            expected_state().cycle = cycle_cost;
            expected_state().registers.pc = addr + 1;
            expected_state().registers.set_flag(status_flag::zero, true);

            execute_and_check(opcode);
        }

        SECTION("behavior")
        {
            state().registers.pc = default_pc_addr;

            SECTION("result in zero")
            {
                auto [a, value] = GENERATE_COPY(from_range(behavior_scenarios.zero_scenarios));

                state().registers.a = a;
                memory().set_byte(default_pc_addr, value);

                expected_state().cycle = cycle_cost;
                expected_state().registers.a = 0x00;
                expected_state().registers.pc = default_pc_addr + 1;
                expected_state().registers.set_flag(status_flag::zero, true);
                expected_state().registers.set_flag(status_flag::negative, false);

                execute_and_check(opcode);
            }

            SECTION("result in negative")
            {
                auto [a, value, expected_a] = GENERATE_COPY(from_range(behavior_scenarios.negative_scenarios));

                state().registers.a = a;
                memory().set_byte(default_pc_addr, value);

                expected_state().cycle = cycle_cost;
                expected_state().registers.a = expected_a;
                expected_state().registers.pc = default_pc_addr + 1;
                expected_state().registers.set_flag(status_flag::zero, false);
                expected_state().registers.set_flag(status_flag::negative, true);

                execute_and_check(opcode);
            }

            SECTION("result in positive")
            {
                auto [a, value, expected_a] = GENERATE_COPY(from_range(behavior_scenarios.positive_scenarios));

                state().registers.a = a;
                memory().set_byte(default_pc_addr, value);

                expected_state().cycle = cycle_cost;
                expected_state().registers.a = expected_a;
                expected_state().registers.pc = default_pc_addr + 1;
                expected_state().registers.set_flag(status_flag::zero, false);
                expected_state().registers.set_flag(status_flag::negative, false);

                execute_and_check(opcode);
            }
        }
    }
}

void bitwise_fixture::test_zero_page(opcode opcode, behavior_scenarios behavior_scenarios)
{
    SECTION("zero_page")
    {
        constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(3);

        SECTION("addressing")
        {
            const auto [pc_addr, base_addr] = GENERATE_COPY(from_range(zero_page_scenarios));

            state().registers.a = 0;
            state().registers.pc = pc_addr;
            memory().set_byte(pc_addr, base_addr);
            memory().set_byte(base_addr, 0);

            expected_state().cycle = cycle_cost;
            expected_state().registers.pc = pc_addr + 1;
            expected_state().registers.set_flag(status_flag::zero, true);

            execute_and_check(opcode);
        }

        SECTION("behavior")
        {
            state().registers.pc = default_pc_addr;
            memory().set_byte(default_pc_addr, zero_page_base_addr);

            SECTION("result in zero")
            {
                auto [a, value] = GENERATE_COPY(from_range(behavior_scenarios.zero_scenarios));

                state().registers.a = a;
                memory().set_byte(zero_page_base_addr, value);

                expected_state().cycle = cycle_cost;
                expected_state().registers.a = 0x00;
                expected_state().registers.pc = default_pc_addr + 1;
                expected_state().registers.set_flag(status_flag::zero, true);
                expected_state().registers.set_flag(status_flag::negative, false);

                execute_and_check(opcode);
            }

            SECTION("result in negative")
            {
                auto [a, value, expected_a] = GENERATE_COPY(from_range(behavior_scenarios.negative_scenarios));

                state().registers.a = a;
                memory().set_byte(zero_page_base_addr, value);

                expected_state().cycle = cycle_cost;
                expected_state().registers.a = expected_a;
                expected_state().registers.pc = default_pc_addr + 1;
                expected_state().registers.set_flag(status_flag::zero, false);
                expected_state().registers.set_flag(status_flag::negative, true);

                execute_and_check(opcode);
            }

            SECTION("result in positive")
            {
                auto [a, value, expected_a] = GENERATE_COPY(from_range(behavior_scenarios.positive_scenarios));

                state().registers.a = a;
                memory().set_byte(zero_page_base_addr, value);

                expected_state().cycle = cycle_cost;
                expected_state().registers.a = expected_a;
                expected_state().registers.pc = default_pc_addr + 1;
                expected_state().registers.set_flag(status_flag::zero, false);
                expected_state().registers.set_flag(status_flag::negative, false);

                execute_and_check(opcode);
            }
        }
    }
}

void bitwise_fixture::test_zero_page_indexed(opcode opcode, register_id index_register, behavior_scenarios behavior_scenarios)
{
    DYNAMIC_SECTION(format("zero_page_{}", index_register))
    {
        constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(4);

        SECTION("addressing")
        {
            const auto [pc_addr, base_addr, idx] = GENERATE(from_range(zero_page_indexed_scenarios));
            const byte_t indexed_addr = base_addr + idx & 0xff;

            state().registers.a = 0;
            state().registers.pc = pc_addr;
            memory().set_byte(pc_addr, base_addr);
            memory().set_byte(indexed_addr, 0);
            set_register(state().registers, index_register, idx);

            expected_state().cycle = cycle_cost;
            expected_state().registers.pc = pc_addr + 1;
            expected_state().registers.set_flag(status_flag::zero, true);

            execute_and_check(opcode);
        }

        SECTION("behavior")
        {
            constexpr byte_t indexed_addr = zero_page_base_addr + indexed_offset;

            state().registers.pc = default_pc_addr;
            memory().set_byte(default_pc_addr, zero_page_base_addr);
            set_register(state().registers, index_register, indexed_offset);

            SECTION("result in zero")
            {
                auto [a, value] = GENERATE_COPY(from_range(behavior_scenarios.zero_scenarios));

                state().registers.a = a;
                memory().set_byte(indexed_addr, value);

                expected_state().cycle = cycle_cost;
                expected_state().registers.a = 0x00;
                expected_state().registers.pc = default_pc_addr + 1;
                expected_state().registers.set_flag(status_flag::zero, true);
                expected_state().registers.set_flag(status_flag::negative, false);

                execute_and_check(opcode);
            }

            SECTION("result in negative")
            {
                auto [a, value, expected_a] = GENERATE_COPY(from_range(behavior_scenarios.negative_scenarios));

                state().registers.a = a;
                memory().set_byte(indexed_addr, value);

                expected_state().cycle = cycle_cost;
                expected_state().registers.a = expected_a;
                expected_state().registers.pc = default_pc_addr + 1;
                expected_state().registers.set_flag(status_flag::zero, false);
                expected_state().registers.set_flag(status_flag::negative, true);

                execute_and_check(opcode);
            }

            SECTION("result in positive")
            {
                auto [a, value, expected_a] = GENERATE_COPY(from_range(behavior_scenarios.positive_scenarios));

                state().registers.a = a;
                memory().set_byte(indexed_addr, value);

                expected_state().cycle = cycle_cost;
                expected_state().registers.a = expected_a;
                expected_state().registers.pc = default_pc_addr + 1;
                expected_state().registers.set_flag(status_flag::zero, false);
                expected_state().registers.set_flag(status_flag::negative, false);

                execute_and_check(opcode);
            }
        }
    }
}

void bitwise_fixture::test_absolute(opcode opcode, behavior_scenarios behavior_scenarios)
{
    SECTION("absolute")
    {
        constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(4);

        SECTION("addressing")
        {
            const auto [pc_addr, base_addr] = GENERATE(from_range(absolute_scenarios));

            state().registers.a = 0;
            state().registers.pc = pc_addr;
            memory().set_word(pc_addr, base_addr);
            memory().set_byte(base_addr, 0);

            expected_state().cycle = cycle_cost;
            expected_state().registers.pc = pc_addr + 2;
            expected_state().registers.set_flag(status_flag::zero, true);

            execute_and_check(opcode);
        }

        SECTION("behavior")
        {
            state().registers.pc = default_pc_addr;
            memory().set_word(default_pc_addr, absolute_base_addr);

            SECTION("result in zero")
            {
                auto [a, value] = GENERATE_COPY(from_range(behavior_scenarios.zero_scenarios));

                state().registers.a = a;
                memory().set_byte(absolute_base_addr, value);

                expected_state().cycle = cycle_cost;
                expected_state().registers.a = 0x00;
                expected_state().registers.pc = default_pc_addr + 2;
                expected_state().registers.set_flag(status_flag::zero, true);
                expected_state().registers.set_flag(status_flag::negative, false);

                execute_and_check(opcode);
            }

            SECTION("result in negative")
            {
                auto [a, value, expected_a] = GENERATE_COPY(from_range(behavior_scenarios.negative_scenarios));

                state().registers.a = a;
                memory().set_byte(absolute_base_addr, value);

                expected_state().cycle = cycle_cost;
                expected_state().registers.a = expected_a;
                expected_state().registers.pc = default_pc_addr + 2;
                expected_state().registers.set_flag(status_flag::zero, false);
                expected_state().registers.set_flag(status_flag::negative, true);

                execute_and_check(opcode);
            }

            SECTION("result in positive")
            {
                auto [a, value, expected_a] = GENERATE_COPY(from_range(behavior_scenarios.positive_scenarios));

                state().registers.a = a;
                memory().set_byte(absolute_base_addr, value);

                expected_state().cycle = cycle_cost;
                expected_state().registers.a = expected_a;
                expected_state().registers.pc = default_pc_addr + 2;
                expected_state().registers.set_flag(status_flag::zero, false);
                expected_state().registers.set_flag(status_flag::negative, false);

                execute_and_check(opcode);
            }
        }
    }
}

void bitwise_fixture::test_absolute_indexed(opcode opcode, register_id index_register, behavior_scenarios behavior_scenarios)
{
    DYNAMIC_SECTION(format("absolute_{}", index_register))
    {
        SECTION("addressing")
        {
            const auto [pc_addr, base_addr, idx] = GENERATE(from_range(absolute_indexed_scenarios));
            const addr_t indexed_addr = base_addr + idx;

            state().registers.a = 0;
            state().registers.pc = pc_addr;
            memory().set_word(pc_addr, base_addr);
            memory().set_byte(indexed_addr, 0);
            set_register(state().registers, index_register, idx);

            // expected_state().cycle = cycle_cost;
            expected_state().registers.pc = pc_addr + 2;
            expected_state().registers.set_flag(status_flag::zero, true);

            execute_and_check(opcode, false);
        }

        SECTION("behavior")
        {
            constexpr addr_t indexed_addr = absolute_base_addr + indexed_offset;

            state().registers.pc = default_pc_addr;
            memory().set_word(default_pc_addr, absolute_base_addr);
            set_register(state().registers, index_register, indexed_offset);

            SECTION("result in zero")
            {
                auto [a, value] = GENERATE_COPY(from_range(behavior_scenarios.zero_scenarios));

                state().registers.a = a;
                memory().set_byte(indexed_addr, value);

                // expected_state().cycle = cycle_cost;
                expected_state().registers.a = 0x00;
                expected_state().registers.pc = default_pc_addr + 2;
                expected_state().registers.set_flag(status_flag::zero, true);
                expected_state().registers.set_flag(status_flag::negative, false);

                execute_and_check(opcode, false);
            }

            SECTION("result in negative")
            {
                auto [a, value, expected_a] = GENERATE_COPY(from_range(behavior_scenarios.negative_scenarios));

                state().registers.a = a;
                memory().set_byte(indexed_addr, value);

                // expected_state().cycle = cycle_cost;
                expected_state().registers.a = expected_a;
                expected_state().registers.pc = default_pc_addr + 2;
                expected_state().registers.set_flag(status_flag::zero, false);
                expected_state().registers.set_flag(status_flag::negative, true);

                execute_and_check(opcode, false);
            }

            SECTION("result in positive")
            {
                auto [a, value, expected_a] = GENERATE_COPY(from_range(behavior_scenarios.positive_scenarios));

                state().registers.a = a;
                memory().set_byte(indexed_addr, value);

                // expected_state().cycle = cycle_cost;
                expected_state().registers.a = expected_a;
                expected_state().registers.pc = default_pc_addr + 2;
                expected_state().registers.set_flag(status_flag::zero, false);
                expected_state().registers.set_flag(status_flag::negative, false);

                execute_and_check(opcode, false);
            }
        }
    }
}

} // namespace nese::cpu::instruction
