#include <nese/cpu/instruction/fixture/status_flag_fixture.hpp>

#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/status_flag.hpp>

namespace nese::cpu::instruction {

void status_flag_fixture::test_set(opcode opcode, status_flag flag)
{
    constexpr cycle_t cycle_cost = cpu_cycle_t(2);

    // clear flags
    state().registers.p = 0x00;

    SECTION("initially clear")
    {
        state().registers.clear_flag(flag);

        expected_state().cycle = cycle_cost;
        expected_state().registers.set_flag(flag);

        execute_and_check(opcode);
    }

    SECTION("initially set")
    {
        state().registers.set_flag(flag);

        expected_state().cycle = cycle_cost;
        expected_state().registers.set_flag(flag);

        execute_and_check(opcode);
    }
}

void status_flag_fixture::test_clear(opcode opcode, status_flag flag)
{
    constexpr cycle_t cycle_cost = cpu_cycle_t(2);

    // clear flags
    state().registers.p = 0x00;

    SECTION("initially clear")
    {
        state().registers.clear_flag(flag);

        expected_state().cycle = cycle_cost;
        expected_state().registers.clear_flag(flag);

        execute_and_check(opcode);
    }

    SECTION("initially set")
    {
        state().registers.set_flag(flag);

        expected_state().cycle = cycle_cost;
        expected_state().registers.clear_flag(flag);

        execute_and_check(opcode);
    }
}

}