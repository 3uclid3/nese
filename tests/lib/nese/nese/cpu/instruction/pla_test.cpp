#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>
#include <nese/cpu/stack.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(execute_fixture, "pla", "[cpu][instruction]")
{
    constexpr cycle_t cycle_cost = cpu_cycle_t(4);

    SECTION("addressing")
    {
        const byte_t s = GENERATE(from_range(stack_offset_scenarios));
        const byte_t next_s = s + 1;

        state().registers.s = s;
        memory().set_byte(stack_offset + next_s, 0x01);

        expected_state().cycle = cycle_cost;
        expected_state().registers.s = next_s;
        expected_state().registers.a = 0x01;

        execute_and_check(opcode::pla_implied);
    }

    SECTION("flags")
    {
        const byte_t next_s = state().registers.s + 1;

        SECTION("push a zero")
        {
            memory().set_byte(stack_offset + next_s, 0x00);

            expected_state().cycle = cycle_cost;
            expected_state().registers.s = next_s;
            expected_state().registers.a = 0x00;
            expected_state().registers.set_flag(status_flag::zero, true);
            expected_state().registers.set_flag(status_flag::negative, false);

            execute_and_check(opcode::pla_implied);
        }

        SECTION("push a negative")
        {
            const byte_t value = GENERATE(from_range(negative_byte_values));
            memory().set_byte(stack_offset + next_s, value);

            expected_state().cycle = cycle_cost;
            expected_state().registers.s = next_s;
            expected_state().registers.a = value;
            expected_state().registers.set_flag(status_flag::zero, false);
            expected_state().registers.set_flag(status_flag::negative, true);

            execute_and_check(opcode::pla_implied);
        }

        SECTION("push a positive")
        {
            const byte_t value = GENERATE(from_range(positive_byte_values));
            memory().set_byte(stack_offset + next_s, value);

            expected_state().cycle = cycle_cost;
            expected_state().registers.s = next_s;
            expected_state().registers.a = value;
            expected_state().registers.set_flag(status_flag::zero, false);
            expected_state().registers.set_flag(status_flag::negative, false);

            execute_and_check(opcode::pla_implied);
        }
    }
}

} // namespace nese::cpu::instruction
