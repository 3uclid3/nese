#include <nese/cpu/instruction/fixture/decrement_fixture.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <nese/cpu/status_flag.hpp>
#include <nese/utility/format.hpp>
#include <nese/utility/hex.hpp>

namespace nese::cpu::instruction {

void decrement_fixture::test_implied(opcode opcode, register_id decrement_register)
{
    constexpr cycle_t cycle_cost = cpu_cycle_t(2);

    SECTION("decrement 0x00 results in overflow to 0xFF")
    {
        set_register(state().registers, decrement_register, 0x00);

        expected_state().cycle = cycle_cost;
        expected_state().registers.set_flag(status_flag::zero, false);
        expected_state().registers.set_flag(status_flag::negative, true);
        set_register(expected_state().registers, decrement_register, 0xFF);

        execute_and_check(opcode);
    }

    SECTION("decrement 0x80 results in overflow to 0x7F")
    {
        set_register(state().registers, decrement_register, 0x80);

        expected_state().cycle = cycle_cost;
        expected_state().registers.set_flag(status_flag::zero, false);
        expected_state().registers.set_flag(status_flag::negative, false);
        set_register(expected_state().registers, decrement_register, 0x7F);

        execute_and_check(opcode);
    }

    SECTION("decrement zero")
    {
        set_register(state().registers, decrement_register, 0x01);

        expected_state().cycle = cycle_cost;
        expected_state().registers.set_flag(status_flag::zero, true);
        expected_state().registers.set_flag(status_flag::negative, false);
        set_register(expected_state().registers, decrement_register, 0x00);

        execute_and_check(opcode);
    }

    SECTION("decrement negative")
    {
        const byte_t value = GENERATE(as<byte_x>(), 0x81, 0xC0, 0xFF);

        CAPTURE(value);

        set_register(state().registers, decrement_register, value);

        expected_state().cycle = cycle_cost;
        expected_state().registers.set_flag(status_flag::zero, false);
        expected_state().registers.set_flag(status_flag::negative, true);
        set_register(expected_state().registers, decrement_register, value - 1);

        execute_and_check(opcode);
    }

    SECTION("decrement positive")
    {
        const byte_t value = GENERATE(as<byte_x>(), 0x02, 0x10, 0x40, 0x7F);

        CAPTURE(value);

        set_register(state().registers, decrement_register, value);

        expected_state().cycle = cycle_cost;
        expected_state().registers.set_flag(status_flag::zero, false);
        expected_state().registers.set_flag(status_flag::negative, false);
        set_register(expected_state().registers, decrement_register, value - 1);

        execute_and_check(opcode);
    }
}

} // namespace nese::cpu::instruction