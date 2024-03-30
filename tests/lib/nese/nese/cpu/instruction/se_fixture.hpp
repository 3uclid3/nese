#pragma once

#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/fixture.hpp>

namespace nese::cpu::instruction {

struct se_fixture : fixture
{
    template<typename ExecuteFunctorT>
    void test_implied(const ExecuteFunctorT& execute, status_flag flag)
    {
        SECTION("implied")
        {
            constexpr cycle_t cycle_cost = cpu_cycle_t(2);

            expected_state = state;
            expected_state.registers.set_flag(flag);
            expected_state.cycle = cycle_cost;

            SECTION("initially clear")
            {
                state.registers.clear_flag(flag);

                execute(state);

                check_state();
            }

            SECTION("initially set")
            {
                state.registers.set_flag(flag);

                execute(state);

                check_state();
            }
        }
    }
};

} // namespace nese::cpu::instruction
