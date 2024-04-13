#pragma once

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

namespace nese::cpu::instruction {

struct store_fixture : execute_fixture
{
    // clang-format off
    template<register_id RegisterT>
    inline static const scenario addr_mode_scenario{
        .initial_changes = {set_register<RegisterT>(1)},
        .expected_changes = {set_operand_value(1)}
    };

    template<register_id RegisterT>
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial_changes = {set_register<RegisterT>(0x00)},
            .expected_changes = {set_operand_value(0x00)},
            .description = "zero value"
        },
        {
            .initial_changes = {set_register<RegisterT>(0x80)},
            .expected_changes = {set_operand_value(0x80)},
            .description = "mid value"
        },
        {
            .initial_changes = {set_register<RegisterT>(0xFF)},
            .expected_changes = {set_operand_value(0xFF)},
            .description = "max value"
        },
    });
    // clang-format on
};

} // namespace nese::cpu::instruction
