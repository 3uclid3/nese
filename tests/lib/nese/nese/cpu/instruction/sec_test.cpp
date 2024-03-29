#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/fixture.hpp>

namespace nese::cpu::instruction {

struct sec_fixture : fixture
{
    template<typename ExecuteFunctorT>
    void test_implied(const ExecuteFunctorT& execute)
    {
        SECTION("implied")
        {
            state_mock expected_state = state;
            expected_state.registers.set_flag(status_flag::carry);
            expected_state.cycle = cpu_cycle_t(2);

            SECTION("initially clear")
            {
                state.registers.clear_flag(status_flag::carry);

                execute(state);

                check_state(expected_state);
            }

            SECTION("initially set")
            {
                state.registers.set_flag(status_flag::carry);

                execute(state);

                check_state(expected_state);
            }
        }
    }
};

TEST_CASE_METHOD(sec_fixture, "sec", "[cpu][instruction]")
{
    test_implied(execute_sec<addr_mode::implied>);
}

} // namespace nese::cpu::instruction
