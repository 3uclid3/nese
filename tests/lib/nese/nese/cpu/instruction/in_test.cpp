#include <catch2/catch_test_macros.hpp>
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
            constexpr cycle_t cycle_cost = cpu_cycle_t(2);

            SECTION("increment 0xFF results in overflow to 0x00")
            {
                set_register(state.registers, 0xFF);

                expected_state = state;
                expected_state.cycle = cycle_cost;
                set_register(expected_state.registers, 0x00);
                expected_state.registers.set_flag(status_flag::zero, true);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state();
            }

            SECTION("increment 0x7F results in overflow to 0x80")
            {
                set_register(state.registers, 0x7F);

                expected_state = state;
                expected_state.cycle = cycle_cost;
                set_register(expected_state.registers, 0x80);
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, true);

                execute(state);

                check_state();
            }

            SECTION("increment negative")
            {
                const byte_t value = GENERATE(byte_t{0x80},
                                              byte_t{0xC0},
                                              byte_t{0xFE});

                set_register(state.registers, value);

                expected_state = state;
                expected_state.cycle = cycle_cost;
                set_register(expected_state.registers, value + 1);
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, true);

                execute(state);

                check_state();
            }

            SECTION("increment positive")
            {
                const byte_t value = GENERATE(byte_t{0x00},
                                              byte_t{0x10},
                                              byte_t{0x40},
                                              byte_t{0x7E});

                set_register(state.registers, value);

                expected_state = state;
                expected_state.cycle = cycle_cost;
                set_register(expected_state.registers, value + 1);
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state();
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