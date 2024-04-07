#include <nese/cpu/instruction/fixture/transfer_fixture.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <nese/cpu/instruction/opcode.hpp>
#include <nese/cpu/status_flag.hpp>

namespace nese::cpu::instruction {

void transfer_fixture::test_implied(opcode opcode, register_id from_register, register_id to_register, bool affect_flags)
{
    SECTION("implied")
    {
        constexpr cycle_t cycle_cost = cpu_cycle_t(2);

        SECTION("transfer zero")
        {
            set_register(state().registers, from_register, 0x00);

            expected_state().cycle = cycle_cost;
            set_register(expected_state().registers, to_register, 0x00);

            if (affect_flags)
            {
                expected_state().registers.set_flag(status_flag::zero, true);
                expected_state().registers.set_flag(status_flag::negative, false);
            }

            execute_and_check(opcode);
        }

        SECTION("transfer negative")
        {
            const byte_t value = GENERATE(as<byte_x>(), 0x80, 0xC0, 0xFE);

            CAPTURE(value);

            set_register(state().registers, from_register, value);

            expected_state().cycle = cycle_cost;
            set_register(expected_state().registers, to_register, value);
            if (affect_flags)
            {
                expected_state().registers.set_flag(status_flag::zero, false);
                expected_state().registers.set_flag(status_flag::negative, true);
            }

            execute_and_check(opcode);
        }

        SECTION("transfer positive")
        {
            const byte_t value = GENERATE(as<byte_t>(), 0x01, 0x10, 0x40, 0x7F);

            CAPTURE(value);

            set_register(state().registers, from_register, value);

            expected_state().cycle = cycle_cost;
            set_register(expected_state().registers, to_register, value);

            if (affect_flags)
            {
                expected_state().registers.set_flag(status_flag::zero, false);
                expected_state().registers.set_flag(status_flag::negative, false);
            }

            execute_and_check(opcode);
        }
    }
}

} // namespace nese::cpu::instruction
