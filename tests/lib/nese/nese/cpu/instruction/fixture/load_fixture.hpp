#pragma once

#include <array>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

namespace nese::cpu::instruction {

struct load_fixture : execute_fixture
{
    // clang-format off
    template<register_id RegisterT>
    inline static const scenario addr_mode_scenario{
        .initial_changes = {set_operand_value(1)},
        .expected_changes = {set_register<RegisterT>(1)}
    };

    template<register_id RegisterT>
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial_changes = {set_operand_value(0)},
            .expected_changes = {set_register<RegisterT>(0), set_status_flag_zero()},
            .description = "zero value"
        },
        {
            .initial_changes = {set_operand_value(0x01)},
            .expected_changes = {set_register<RegisterT>(0x01)},
            .description = "smallest positive value"
        },
        {
            .initial_changes = {set_operand_value(0x40)},
            .expected_changes = {set_register<RegisterT>(0x40)},
            .description = "mid positive value"
        },
        {
            .initial_changes = {set_operand_value(0x7F)},
            .expected_changes = {set_register<RegisterT>(0x7F)},
            .description = "largest positive value"
        },
        {
            .initial_changes = {set_operand_value(0xFF)},
            .expected_changes = {set_register<RegisterT>(0xFF), set_status_flag_negative()},
            .description = "smallest negative value"
        },
        {
            .initial_changes = {set_operand_value(0xC0)},
            .expected_changes = {set_register<RegisterT>(0xC0), set_status_flag_negative()},
            .description = "mid negative value"
        },
        {
            .initial_changes = {set_operand_value(0x80)},
            .expected_changes = {set_register<RegisterT>(0x80), set_status_flag_negative()},
            .description = "largest negative value"
        }
    });
    // clang-format on
};

} // namespace nese::cpu::instruction
