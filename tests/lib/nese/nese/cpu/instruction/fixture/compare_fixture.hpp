#pragma once

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

namespace nese::cpu::instruction {

enum class opcode;

struct compare_fixture : execute_fixture
{
    // clang-format off
    template<register_id RegisterT>
    inline static const scenario addr_mode_scenario{
        .initial_changes = {set_register<RegisterT>(0), set_operand_value(0)},
        .expected_changes = {set_status_flag_carry(), set_status_flag_zero()}
    };

    template<register_id RegisterT>
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial_changes = {set_register<RegisterT>(0x01), set_operand_value(0x00)},
            .expected_changes = {set_status_flag_carry()},
        },
        {
            .initial_changes = {set_register<RegisterT>(0x00), set_operand_value(0x01)},
            .expected_changes = {set_status_flag_negative()},
        },
        {
            .initial_changes = {set_register<RegisterT>(0x7F), set_operand_value(0x80)},
            .expected_changes = {set_status_flag_negative()},
        },
        {
            .initial_changes = {set_register<RegisterT>(0x80), set_operand_value(0x7F)},
            .expected_changes = {set_status_flag_carry()},
        },
        {
            .initial_changes = {set_register<RegisterT>(0x80), set_operand_value(0x00)},
            .expected_changes = {set_status_flag_carry(), set_status_flag_negative()},
        },
        {
            .initial_changes = {set_register<RegisterT>(0x00), set_operand_value(0x80)},
            .expected_changes = {set_status_flag_negative()},
        },
        {
            .initial_changes = {set_register<RegisterT>(0xFF), set_operand_value(0xFE)},
            .expected_changes = {set_status_flag_carry()},
        },
        {
            .initial_changes = {set_register<RegisterT>(0xFE), set_operand_value(0xFF)},
            .expected_changes = {set_status_flag_negative()},
        },
        {
            .initial_changes = {set_register<RegisterT>(0x7F), set_operand_value(0x7F)},
            .expected_changes = {set_status_flag_carry(), set_status_flag_zero()},
        },
        {
            .initial_changes = {set_register<RegisterT>(0x80), set_operand_value(0x80)},
            .expected_changes = {set_status_flag_carry(), set_status_flag_zero()},
        },

    });
    // clang-format on
};

} // namespace nese::cpu::instruction
