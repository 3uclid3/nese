#include "fixture/execute_fixture.hpp"

#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct inc_fixture : execute_fixture
{
    // clang-format off
    inline static const scenario addr_mode_scenario{
        .initial_changes = {set_operand_value(0x01)},
        .expected_changes = {set_operand_value(0x02)}
    };

    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial_changes = {set_operand_value(0xFF), set_status_flag_negative()},
            .expected_changes = {set_operand_value(0x00), set_status_flag_zero(), clear_status_flag_negative()},
            .description = "0xFF results in overflow to 0x00"
        },
        {
            .initial_changes = {set_operand_value(0x7F)},
            .expected_changes = {set_operand_value(0x80), set_status_flag_negative()},
            .description = "0x7F results in overflow to 0x80"
        },
        {
            .initial_changes = {set_operand_value(0x00), set_status_flag_zero()},
            .expected_changes = {set_operand_value(0x01), clear_status_flag_zero()},
            .description = "zero"
        },
        {
            .initial_changes = {set_operand_value(0x80), set_status_flag_negative()},
            .expected_changes = {set_operand_value(0x81)},
            .description = "smallest negative value"
        },
        {
            .initial_changes = {set_operand_value(0xC0), set_status_flag_negative()},
            .expected_changes = {set_operand_value(0xC1)},
            .description = "mid negative value"
        },
        {
            .initial_changes = {set_operand_value(0xFE), set_status_flag_negative()},
            .expected_changes = {set_operand_value(0xFF)},
            .description = "largest negative value minus one"
        },
        {
            .initial_changes = {set_operand_value(0x01)},
            .expected_changes = {set_operand_value(0x02)},
            .description = "smallest positive value"
        },
        {
            .initial_changes = {set_operand_value(0x40)},
            .expected_changes = {set_operand_value(0x41)},
            .description = "mid positive value"
        },
        {
            .initial_changes = {set_operand_value(0x7E)},
            .expected_changes = {set_operand_value(0x7F)},
            .description = "largest positive value minus one"
        }
    });
    // clang-format on
};

TEST_CASE_METHOD(inc_fixture, "inc", "[cpu][instruction]")
{
    test_zero_page(opcode::inc_zero_page, addr_mode_scenario, behavior_scenarios, cpu_cycle_t(5));
    test_zero_page_indexed(opcode::inc_zero_page_x, register_id::x, addr_mode_scenario, behavior_scenarios, cpu_cycle_t(6));
    test_absolute(opcode::inc_absolute, addr_mode_scenario, behavior_scenarios, cpu_cycle_t(6));
    test_absolute_indexed(opcode::inc_absolute_x, register_id::x, addr_mode_scenario, behavior_scenarios, cpu_cycle_t(7));
}

} // namespace nese::cpu::instruction