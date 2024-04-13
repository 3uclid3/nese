#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct isb_fixture : execute_fixture
{
    static constexpr cpu_cycle_t base_cycle_cost = cpu_cycle_t(2);

    // clang-format off
    inline static const scenario addr_mode_scenario{
        .initial_changes = {set_operand_value(0xFF), set_register_a(0x00), set_status_flag_carry()},
        .expected_changes = {set_operand_value(0x00), set_status_flag_zero()},
        .base_cycle_cost = base_cycle_cost
    };

    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        // carry flag initially clear
        {
            .initial_changes = {set_operand_value(0xFF), set_register_a(0x00)},
            .expected_changes = {set_operand_value(0x00), set_register_a(0xFF), set_status_flag_negative()},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0xFF), set_register_a(0x01)},
            .expected_changes = {set_operand_value(0x00), set_register_a(0x00), set_status_flag_carry(), set_status_flag_zero()},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0x00), set_register_a(0x02)},
            .expected_changes = {set_operand_value(0x01), set_register_a(0x00), set_status_flag_carry(), set_status_flag_zero()},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0x7E), set_register_a(0x80)},
            .expected_changes = {set_operand_value(0x7F), set_register_a(0x00), set_status_flag_carry(), set_status_flag_zero(), set_status_flag_overflow()},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0x7F), set_register_a(0x80)},
            .expected_changes = {set_operand_value(0x80), set_register_a(0xFF), set_status_flag_negative()},
            .base_cycle_cost = base_cycle_cost
        },
        
        // carry flag initially set
        {
            .initial_changes = {set_operand_value(0xFF), set_register_a(0x00), set_status_flag_carry()},
            .expected_changes = {set_operand_value(0x00), set_register_a(0x00), set_status_flag_zero()},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0x00), set_register_a(0x01), set_status_flag_carry()},
            .expected_changes = {set_operand_value(0x01), set_register_a(0x00), set_status_flag_zero()},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0x00), set_register_a(0x02), set_status_flag_carry()},
            .expected_changes = {set_operand_value(0x01), set_register_a(0x01)},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0x00), set_register_a(0x7F), set_status_flag_carry()},
            .expected_changes = {set_operand_value(0x01), set_register_a(0x7E)},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0x7E), set_register_a(0x80), set_status_flag_carry()},
            .expected_changes = {set_operand_value(0x7F), set_register_a(0x01), set_status_flag_overflow()},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0x7F), set_register_a(0x80), set_status_flag_carry()},
            .expected_changes = {set_operand_value(0x80), set_register_a(0x00), set_status_flag_zero()},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0x7E), set_register_a(0x7E), set_status_flag_carry()},
            .expected_changes = {set_operand_value(0x7F), set_register_a(0xFF), set_status_flag_negative(), clear_status_flag_carry()},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0x7D), set_register_a(0xFF), set_status_flag_carry()},
            .expected_changes = {set_operand_value(0x7E), set_register_a(0x81), set_status_flag_negative()},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0xFE), set_register_a(0x00), set_status_flag_carry()},
            .expected_changes = {set_operand_value(0xFF), set_register_a(0x01), clear_status_flag_carry()},
            .base_cycle_cost = base_cycle_cost
        },
    });
    // clang-format on
};

TEST_CASE_METHOD(isb_fixture, "isb", "[cpu][instruction]")
{
    test_zero_page(opcode::isb_zero_page_unofficial, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(opcode::isb_zero_page_x_unofficial, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(opcode::isb_absolute_unofficial, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::isb_absolute_x_unofficial, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::isb_absolute_y_unofficial, register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(opcode::isb_indexed_indirect_unofficial, addr_mode_scenario, behavior_scenarios);
    test_indirect_indexed(opcode::isb_indirect_indexed_unofficial, addr_mode_scenario, behavior_scenarios);
}

} // namespace nese::cpu::instruction