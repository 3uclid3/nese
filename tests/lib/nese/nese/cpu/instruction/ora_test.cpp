#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct ora_fixture : execute_fixture
{
    // clang-format off
    inline static const scenario addr_mode_scenario{
        .initial_changes = {set_register_a(0), set_operand_value(0)},
        .expected_changes = {set_register_a(0), set_status_flag_zero()}
    };
    
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        // zero, nothing to do
        
        // negative
        {
            .initial_changes = {set_register_a(0x80), set_operand_value(0x01)},
            .expected_changes = {set_register_a(0x81), set_status_flag_negative()},
        },
        {
            .initial_changes = {set_register_a(0x01), set_operand_value(0x80)},
            .expected_changes = {set_register_a(0x81), set_status_flag_negative()},
        },
        {
            .initial_changes = {set_register_a(0x80), set_operand_value(0x80)},
            .expected_changes = {set_register_a(0x80), set_status_flag_negative()},
        },
        {
            .initial_changes = {set_register_a(0xFF), set_operand_value(0x00)},
            .expected_changes = {set_register_a(0xFF), set_status_flag_negative()},
        },
        {
            .initial_changes = {set_register_a(0x00), set_operand_value(0xFF)},
            .expected_changes = {set_register_a(0xFF), set_status_flag_negative()},
        },
        {
            .initial_changes = {set_register_a(0xC0), set_operand_value(0x3F)},
            .expected_changes = {set_register_a(0xFF), set_status_flag_negative()},
        },
        {
            .initial_changes = {set_register_a(0x3F), set_operand_value(0xC0)},
            .expected_changes = {set_register_a(0xFF), set_status_flag_negative()},
        },
        
        // positive
        {
            .initial_changes = {set_register_a(0x7F), set_operand_value(0x00)},
            .expected_changes = {set_register_a(0x7F)},
        },
        {
            .initial_changes = {set_register_a(0x00), set_operand_value(0x7F)},
            .expected_changes = {set_register_a(0x7F)},
        },
        {
            .initial_changes = {set_register_a(0x3F), set_operand_value(0x40)},
            .expected_changes = {set_register_a(0x7F)},
        },
        {
            .initial_changes = {set_register_a(0x40), set_operand_value(0x3F)},
            .expected_changes = {set_register_a(0x7F)},
        },
        {
            .initial_changes = {set_register_a(0x1F), set_operand_value(0x20)},
            .expected_changes = {set_register_a(0x3F)},
        },
        {
            .initial_changes = {set_register_a(0x20), set_operand_value(0x1F)},
            .expected_changes = {set_register_a(0x3F)},
        },
        
    });
    // clang-format off
};

TEST_CASE_METHOD(ora_fixture, "ora", "[cpu][instruction]")
{
    test_immediate(opcode::ora_immediate, addr_mode_scenario, behavior_scenarios);
    test_zero_page(opcode::ora_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(opcode::ora_zero_page_x, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(opcode::ora_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::ora_absolute_x, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::ora_absolute_y, register_id::y, addr_mode_scenario, behavior_scenarios);
}

} // namespace nese::cpu::instruction
