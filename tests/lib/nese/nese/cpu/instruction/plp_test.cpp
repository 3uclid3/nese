#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>
#include <nese/cpu/stack.hpp>
#include <nese/utility/format.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(execute_fixture, "plp", "[cpu][instruction]")
{
    constexpr cycle_t cycle_cost = cpu_cycle_t(4);

    // Clear all flags
    state().registers.p = 0;

    SECTION("addressing")
    {
        const byte_t s = GENERATE(as<byte_t>(), 0xFD, 0xFE, 0xFF, 0x01, 0x00);
        const byte_t next_s = s + 1;

        state().registers.s = s;
        memory().set_byte(stack_offset + next_s, 0x00);

        expected_state().cycle = cycle_cost;
        expected_state().registers.s = next_s;
        expected_state().registers.p = static_cast<u8_t>(status_flag::unused);

        execute_and_check(opcode::plp_implied);
    }

    SECTION("flag set")
    {
        auto flag_set = GENERATE(
            as<status_flag>(),
            status_flag::carry,
            status_flag::zero,
            status_flag::interrupt,
            status_flag::decimal,
            status_flag::unused,
            status_flag::overflow,
            status_flag::negative);

        DYNAMIC_SECTION(format("{}", flag_set))
        {
            memory().set_byte(stack_offset + state().registers.s + 1, static_cast<byte_t>(flag_set));

            expected_state().cycle = cycle_cost;
            expected_state().registers.s = expected_state().registers.s + 1;
            expected_state().registers.p = flag_set | status_flag::unused; // unused always set

            execute_and_check(opcode::plp_implied);
        }
    }

    SECTION("flag ignored")
    {
        auto flag_ignored = GENERATE(
            as<status_flag>(),
            status_flag::break_cmd);

        DYNAMIC_SECTION(format("{}", flag_ignored))
        {
            state().registers.p = static_cast<byte_t>(flag_ignored);
            memory().set_byte(stack_offset + state().registers.s + 1, 0x00);

            expected_state().cycle = cycle_cost;
            expected_state().registers.s = expected_state().registers.s + 1;
            expected_state().registers.p = flag_ignored | status_flag::unused; // unused always set

            execute_and_check(opcode::plp_implied);
        }
    }
}

} // namespace nese::cpu::instruction
