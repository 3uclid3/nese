#pragma once

#include <array>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

namespace nese::cpu::instruction {

struct increment_fixture : execute_fixture
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(2);

    // clang-format off
    template<register_id RegisterT>
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial_changes = {set_register<RegisterT>(0xFF), set_status_flag_negative()},
            .expected_changes = {set_register<RegisterT>(0x00), set_status_flag_zero(), clear_status_flag_negative()},
            .description = "0xFF results in overflow to 0x00",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<RegisterT>(0x7F)},
            .expected_changes = {set_register<RegisterT>(0x80), set_status_flag_negative()},
            .description = "0x7F results in overflow to 0x80",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<RegisterT>(0x00), set_status_flag_zero()},
            .expected_changes = {set_register<RegisterT>(0x01), clear_status_flag_zero()},
            .description = "zero",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<RegisterT>(0x80), set_status_flag_negative()},
            .expected_changes = {set_register<RegisterT>(0x81)},
            .description = "smallest negative value",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<RegisterT>(0xC0), set_status_flag_negative()},
            .expected_changes = {set_register<RegisterT>(0xC1)},
            .description = "mid negative value",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<RegisterT>(0xFE), set_status_flag_negative()},
            .expected_changes = {set_register<RegisterT>(0xFF)},
            .description = "largest negative value minus one",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<RegisterT>(0x01)},
            .expected_changes = {set_register<RegisterT>(0x02)},
            .description = "smallest positive value",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<RegisterT>(0x40)},
            .expected_changes = {set_register<RegisterT>(0x41)},
            .description = "mid positive value",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<RegisterT>(0x7E)},
            .expected_changes = {set_register<RegisterT>(0x7F)},
            .description = "largest positive value minus one",
            .base_cycle_cost = cycle_cost
        }
    });
    // clang-format on
};

} // namespace nese::cpu::instruction
