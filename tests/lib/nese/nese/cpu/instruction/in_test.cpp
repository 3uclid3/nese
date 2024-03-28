#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/fixture.hpp>

namespace nese::cpu::instruction {

struct in_fixture : fixture
{
    template<typename InstructionExecuteT, typename SetRegisterFunctorT>
    void test_implied(const InstructionExecuteT& execute, const SetRegisterFunctorT& set_register)
    {
        SECTION("implied")
        {
            SECTION("increment 0xFF results in overflow to 0x00")
            {
                set_register(state.registers, 0xFF);

                state_mock expected_state = state;
                set_register(expected_state.registers, 0x00);
                expected_state.registers.set_flag(status_flag::zero, true);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state);
            }

            SECTION("increment 0x7F results in overflow to 0x80")
            {
                set_register(state.registers, 0x7F);

                state_mock expected_state = state;
                set_register(expected_state.registers, 0x80);
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, true);

                execute(state);

                check_state(expected_state);
            }

            SECTION("increment negative")
            {
                const byte_t value = GENERATE(0x80,
                                              0xC0,
                                              0xFE);

                set_register(state.registers, value);

                state_mock expected_state = state;
                set_register(expected_state.registers, value + 1);
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, true);

                execute(state);

                check_state(expected_state);
            }

            SECTION("increment positive")
            {
                const byte_t value = GENERATE(0x00,
                                              0x10,
                                              0x40,
                                              0x7E);

                set_register(state.registers, value);

                state_mock expected_state = state;
                set_register(expected_state.registers, value + 1);
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state);
            }
        }
    }
};

TEST_CASE_METHOD(in_fixture, "inx", "[cpu][instruction]")
{
    test_implied(execute_inx<addr_mode::implied>, set_register_x);
}

TEST_CASE_METHOD(in_fixture, "iny", "[cpu][instruction]")
{
    test_implied(execute_iny<addr_mode::implied>, set_register_y);
}

} // namespace nese::cpu::instruction