#pragma once

#include <array>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

namespace nese::cpu::instruction {

struct decrement_fixture : execute_fixture
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(2);

    // clang-format off
    template<register_id RegisterT>
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial_changes = {set_register<RegisterT>(0x00), set_status_flag_zero()},
            .expected_changes = {set_register<RegisterT>(0xFF), clear_status_flag_zero(), set_status_flag_negative()},
            .description = "0x00 results in overflow to 0xFF",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<RegisterT>(0x80), set_status_flag_negative()},
            .expected_changes = {set_register<RegisterT>(0x7F), clear_status_flag_negative()},
            .description = "0x80 results in overflow to 0x7F",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<RegisterT>(0x01)},
            .expected_changes = {set_register<RegisterT>(0x00), set_status_flag_zero()},
            .description = "zero",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<RegisterT>(0x81), set_status_flag_negative()},
            .expected_changes = {set_register<RegisterT>(0x80)},
            .description = "smallest negative value minus one",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<RegisterT>(0xC1), set_status_flag_negative()},
            .expected_changes = {set_register<RegisterT>(0xC0)},
            .description = "mid negative value",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<RegisterT>(0xFF), set_status_flag_negative()},
            .expected_changes = {set_register<RegisterT>(0xFE)},
            .description = "largest negative value",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<RegisterT>(0x02)},
            .expected_changes = {set_register<RegisterT>(0x01)},
            .description = "smallest positive value minus one",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<RegisterT>(0x41)},
            .expected_changes = {set_register<RegisterT>(0x40)},
            .description = "mid positive value",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<RegisterT>(0x7F)},
            .expected_changes = {set_register<RegisterT>(0x7E)},
            .description = "largest positive value",
            .base_cycle_cost = cycle_cost
        }
    });
    // clang-format on
};

} // namespace nese::cpu::instruction
