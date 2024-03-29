#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/fixture.hpp>

namespace nese::cpu::instruction {

struct clc_fixture : fixture
{
    template<typename ExecuteFunctorT>
    void test_implied(const ExecuteFunctorT& execute)
    {
        SECTION("implied")
        {
            constexpr cycle_t cycle_cost = cpu_cycle_t(2);

            expected_state = state;
            expected_state.registers.clear_flag(status_flag::carry);
            expected_state.cycle = cycle_cost;

            SECTION("initially clear")
            {
                state.registers.clear_flag(status_flag::carry);

                execute(state);

                check_state();
            }

            SECTION("initially set")
            {
                state.registers.set_flag(status_flag::carry);

                execute(state);

                check_state();
            }
        }
    }
};

TEST_CASE_METHOD(clc_fixture, "clc", "[cpu][instruction]")
{
    test_implied(execute_clc<addr_mode::implied>);
}

} // namespace nese::cpu::instruction
