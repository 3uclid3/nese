#include <nese/cpu/instruction/fixture/increment_fixture.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <nese/cpu/status_flag.hpp>
#include <nese/utility/format.hpp>

namespace nese::cpu::instruction {

void increment_fixture::test_implied(opcode opcode, register_id increment_register)
{
    SECTION("implied")
    {
        constexpr cycle_t cycle_cost = cpu_cycle_t(2);

        SECTION("increment 0xFF results in overflow to 0x00")
        {
            set_register(state().registers, increment_register, 0xFF);

            expected_state().cycle = cycle_cost;
            expected_state().registers.set_flag(status_flag::zero, true);
            expected_state().registers.set_flag(status_flag::negative, false);
            set_register(expected_state().registers, increment_register, 0x00);

            execute_and_check(opcode);
        }

        SECTION("increment 0x7F results in overflow to 0x80")
        {
            set_register(state().registers, increment_register, 0x7F);

            expected_state().cycle = cycle_cost;
            expected_state().registers.set_flag(status_flag::zero, false);
            expected_state().registers.set_flag(status_flag::negative, true);
            set_register(expected_state().registers, increment_register, 0x80);

            execute_and_check(opcode);
        }

        SECTION("increment negative")
        {
            const byte_t value = GENERATE(as<byte_t>(), 0x80, 0xC0, 0xFE);
            set_register(state().registers, increment_register, value);

            expected_state().cycle = cycle_cost;
            expected_state().registers.set_flag(status_flag::zero, false);
            expected_state().registers.set_flag(status_flag::negative, true);
            set_register(expected_state().registers, increment_register, value + 1);

            execute_and_check(opcode);
        }

        SECTION("increment positive")
        {
            const byte_t value = GENERATE(as<byte_t>(), 0x00, 0x10, 0x40, 0x7E);
            set_register(state().registers, increment_register, value);

            expected_state().cycle = cycle_cost;
            expected_state().registers.set_flag(status_flag::zero, false);
            expected_state().registers.set_flag(status_flag::negative, false);
            set_register(expected_state().registers, increment_register, value + 1);

            execute_and_check(opcode);
        }
    }
}

} // namespace nese::cpu::instruction