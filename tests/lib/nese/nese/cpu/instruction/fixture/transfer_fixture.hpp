#pragma once

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

namespace nese::cpu::instruction {

enum class opcode;

struct transfer_fixture : execute_fixture
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(2);

    // clang-format off
    template<register_id FromRegisterT, register_id ToRegisterT, bool AffectFlags = true>
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial_changes = {set_register<FromRegisterT>(0x00)},
            .expected_changes = {set_register<ToRegisterT>(0x00), conditional(set_status_flag_zero(), AffectFlags)},
            .description = "zero value",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<FromRegisterT>(0x01)},
            .expected_changes = {set_register<ToRegisterT>(0x01)},
            .description = "smallest positive value",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<FromRegisterT>(0x40)},
            .expected_changes = {set_register<ToRegisterT>(0x40)},
            .description = "mid positive value",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<FromRegisterT>(0x7F)},
            .expected_changes = {set_register<ToRegisterT>(0x7F)},
            .description = "largest positive value",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<FromRegisterT>(0xFF)},
            .expected_changes = {set_register<ToRegisterT>(0xFF), conditional(set_status_flag_negative(), AffectFlags)},
            .description = "smallest negative value",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<FromRegisterT>(0xC0)},
            .expected_changes = {set_register<ToRegisterT>(0xC0), conditional(set_status_flag_negative(), AffectFlags)},
            .description = "mid negative value",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register<FromRegisterT>(0x80)},
            .expected_changes = {set_register<ToRegisterT>(0x80), conditional(set_status_flag_negative(), AffectFlags)},
            .description = "largest negative value",
            .base_cycle_cost = cycle_cost
        }
    });
    // clang-format on
};

} // namespace nese::cpu::instruction
