#pragma once

#include <array>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/status_flag.hpp>

namespace nese::cpu::instruction {

struct set_status_flag_fixture : execute_fixture
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(2);

    // clang-format off
    template<status_flag StatusFlagT>
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial_changes = {set_status_flag<StatusFlagT>()},
            .expected_changes = {set_status_flag<StatusFlagT>()},
            .description = "initially set",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {clear_status_flag<StatusFlagT>()},
            .expected_changes = {set_status_flag<StatusFlagT>()},
            .description = "initially clear",
            .base_cycle_cost = cycle_cost
        }
    });
    // clang-format on
};

} // namespace nese::cpu::instruction
