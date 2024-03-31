#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>
#include <nese/cpu/stack.hpp>
#include <nese/utility/format.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(execute_fixture, "php", "[cpu][instruction]")
{
    constexpr cycle_t cycle_cost = cpu_cycle_t(3);
    constexpr status_flags mandatory_flags = 0x30;

    // Clear all flags
    state().registers.p = 0;

    SECTION("addressing")
    {
        const byte_t s = GENERATE(as<byte_t>(), 0xFD, 0xFE, 0xFF, 0x01, 0x00);

        state().registers.s = s;

        expected_state().cycle = cycle_cost;
        expected_state().registers.s = s - 1;
        expected_memory().set_byte(stack_offset + s, mandatory_flags);

        execute_and_check(opcode::php_implied);
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

        const byte_t s = state().registers.s;

        DYNAMIC_SECTION(format("{}", flag_set))
        {
            state().registers.p = static_cast<byte_t>(flag_set);

            expected_state().cycle = cycle_cost;
            expected_state().registers.s = expected_state().registers.s - 1;
            expected_memory().set_byte(stack_offset + s, static_cast<byte_t>(flag_set) | mandatory_flags);

            execute_and_check(opcode::php_implied);
        }
    }

    SECTION("flag ignored")
    {
        auto flag_ignored = GENERATE(
            as<status_flag>(),
            status_flag::break_cmd);

        const byte_t s = state().registers.s;

        DYNAMIC_SECTION(format("{}", flag_ignored))
        {
            state().registers.p = static_cast<byte_t>(flag_ignored);

            expected_state().cycle = cycle_cost;
            expected_state().registers.s = expected_state().registers.s - 1;
            expected_memory().set_byte(stack_offset + s, mandatory_flags);

            execute_and_check(opcode::php_implied);
        }
    }
}

} // namespace nese::cpu::instruction
