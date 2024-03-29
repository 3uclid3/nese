#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/fixture.hpp>

namespace nese::cpu::instruction {

struct in_fixture : fixture
{
    template<typename InstructionExecuteT>
    void test_implied(const InstructionExecuteT& execute, register_id increment_register)
    {
        SECTION("implied")
        {
            constexpr cycle_t cycle_cost = cpu_cycle_t(2);

            SECTION("increment 0xFF results in overflow to 0x00")
            {
                set_register(state, increment_register , 0xFF);

                expected_state = state;
                expected_state.cycle = cycle_cost;
                expected_state.registers.set_flag(status_flag::zero, true);
                expected_state.registers.set_flag(status_flag::negative, false);
                set_register(expected_state, increment_register, 0x00);

                execute(state);

                check_state();
            }

            SECTION("increment 0x7F results in overflow to 0x80")
            {
                set_register(state, increment_register, 0x7F);

                expected_state = state;
                expected_state.cycle = cycle_cost;
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, true);
                set_register(expected_state, increment_register, 0x80);

                execute(state);

                check_state();
            }

            SECTION("increment negative")
            {
                const byte_t value = GENERATE(as<byte_t>(), 0x80, 0xC0, 0xFE);
                set_register(state, increment_register, value);

                expected_state = state;
                expected_state.cycle = cycle_cost;
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, true);
                set_register(expected_state, increment_register, value + 1);

                execute(state);

                check_state();
            }

            SECTION("increment positive")
            {
                const byte_t value = GENERATE(as<byte_t>(), 0x00, 0x10, 0x40, 0x7E);
                set_register(state, increment_register, value);

                expected_state = state;
                expected_state.cycle = cycle_cost;
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, false);
                set_register(expected_state, increment_register, value + 1);

                execute(state);

                check_state();
            }
        }
    }
};

TEST_CASE_METHOD(in_fixture, "inx", "[cpu][instruction]")
{
    test_implied(execute_inx<addr_mode::implied>, register_id::x);
}

TEST_CASE_METHOD(in_fixture, "iny", "[cpu][instruction]")
{
    test_implied(execute_iny<addr_mode::implied>, register_id::y);
}

} // namespace nese::cpu::instruction