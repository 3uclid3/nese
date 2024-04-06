#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>
#include <nese/cpu/stack.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(execute_fixture, "pha", "[cpu][instruction]")
{
    constexpr cycle_t cycle_cost = cpu_cycle_t(3);

    SECTION("addressing")
    {
        const byte_t s = GENERATE(from_range(stack_offset_scenarios));

        state().registers.s = s;
        state().registers.a = 0;

        expected_state().cycle = cycle_cost;
        expected_state().registers.s = s - 1;
        expected_memory().set_byte(stack_offset + s, 0);

        execute_and_check(opcode::pha_implied);
    }

    SECTION("behavior")
    {
        const byte_t s = state().registers.s;
        const byte_t a = GENERATE(as<byte_t>(), 0x00, 0xFF, 0x80, 0x55);

        state().registers.a = a;

        expected_state().cycle = cycle_cost;
        expected_state().registers.s = s - 1;
        expected_memory().set_byte(stack_offset + s, a);

        execute_and_check(opcode::pha_implied);
    }
}

} // namespace nese::cpu::instruction
