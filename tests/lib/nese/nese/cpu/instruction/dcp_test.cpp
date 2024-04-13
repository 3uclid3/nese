#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct dcp_fixture : execute_fixture
{
    static constexpr cpu_cycle_t base_cycle_cost = cpu_cycle_t(2);

    // clang-format off
    inline static const scenario addr_mode_scenario{
        .initial_changes = {set_operand_value(0x02), set_register_a(0x00)},
        .expected_changes = {set_operand_value(0x01), set_status_flag_negative()},
        .base_cycle_cost = base_cycle_cost
    };

    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial_changes = {set_operand_value(0x00), set_register_a(0x00)},
            .expected_changes = {set_operand_value(0xFF)},
            .description = "0x00 results in overflow to 0xFF",
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0x80), set_register_a(0x00)},
            .expected_changes = {set_operand_value(0x7F), set_status_flag_negative()},
            .description = "0x80 results in overflow to 0x7F",
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0x01), set_register_a(0x00)},
            .expected_changes = {set_operand_value(0x00), set_status_flag_carry(), set_status_flag_zero()},
            .description = "zero",
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0xC1), set_register_a(0x00)},
            .expected_changes = {set_operand_value(0xC0)},
            .description = "negative value",
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0x41), set_register_a(0x00)},
            .expected_changes = {set_operand_value(0x40), set_status_flag_negative()},
            .description = "positive value",
            .base_cycle_cost = base_cycle_cost
        },
        
        {
            .initial_changes = {set_operand_value(0x01), set_register_a(0x01)},
            .expected_changes = {set_operand_value(0x00), set_status_flag_carry()},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0x80), set_register_a(0x80)},
            .expected_changes = {set_operand_value(0x7F), set_status_flag_carry()},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0xFF), set_register_a(0xFF)},
            .expected_changes = {set_operand_value(0xFE), set_status_flag_carry()},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0x00), set_register_a(0xFE)},
            .expected_changes = {set_operand_value(0xFF), set_status_flag_negative()},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0xFF), set_register_a(0xFE)},
            .expected_changes = {set_operand_value(0xFE), set_status_flag_carry(), set_status_flag_zero()},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0x81), set_register_a(0x80)},
            .expected_changes = {set_operand_value(0x80), set_status_flag_carry(), set_status_flag_zero()},
            .base_cycle_cost = base_cycle_cost
        },
    });
    // clang-format on
};

TEST_CASE_METHOD(dcp_fixture, "dcp", "[cpu][instruction]")
{
    test_zero_page(opcode::dcp_zero_page_unofficial, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(opcode::dcp_zero_page_x_unofficial, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(opcode::dcp_absolute_unofficial, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::dcp_absolute_x_unofficial, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::dcp_absolute_y_unofficial, register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(opcode::dcp_indexed_indirect_unofficial, addr_mode_scenario, behavior_scenarios);
    test_indirect_indexed(opcode::dcp_indirect_indexed_unofficial, addr_mode_scenario, behavior_scenarios);
}

} // namespace nese::cpu::instruction