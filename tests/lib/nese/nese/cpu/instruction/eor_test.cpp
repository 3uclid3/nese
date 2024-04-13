#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct eor_fixture : execute_fixture
{
    // clang-format off
    inline static const scenario addr_mode_scenario{
        .initial_changes = {set_register_a(0), set_operand_value(0)},
        .expected_changes = {set_register_a(0), set_status_flag_zero()}
    };
    
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        // zero
        {
            .initial_changes = {set_register_a(0x00), set_operand_value(0x00)},
            .expected_changes = {set_register_a(0x00), set_status_flag_zero()},
        },
        {
            .initial_changes = {set_register_a(0x80), set_operand_value(0x80)},
            .expected_changes = {set_register_a(0x00), set_status_flag_zero()},
        },
        
        // negative
        {
            .initial_changes = {set_register_a(0x80), set_operand_value(0x00)},
            .expected_changes = {set_register_a(0x80), set_status_flag_negative()},
        },
        {
            .initial_changes = {set_register_a(0x00), set_operand_value(0x80)},
            .expected_changes = {set_register_a(0x80), set_status_flag_negative()},
        },
        {
            .initial_changes = {set_register_a(0xFF), set_operand_value(0x7F)},
            .expected_changes = {set_register_a(0x80), set_status_flag_negative()},
        },
        {
            .initial_changes = {set_register_a(0x7F), set_operand_value(0xFF)},
            .expected_changes = {set_register_a(0x80), set_status_flag_negative()},
        },
        {
            .initial_changes = {set_register_a(0xC0), set_operand_value(0x40)},
            .expected_changes = {set_register_a(0x80), set_status_flag_negative()},
        },
        {
            .initial_changes = {set_register_a(0x40), set_operand_value(0xC0)},
            .expected_changes = {set_register_a(0x80), set_status_flag_negative()},
        },
        
        // positive
        {
            .initial_changes = {set_register_a(0x01), set_operand_value(0x02)},
            .expected_changes = {set_register_a(0x03)},
        },
        {
            .initial_changes = {set_register_a(0x0F), set_operand_value(0x10)},
            .expected_changes = {set_register_a(0x1F)},
        },
        {
            .initial_changes = {set_register_a(0x20), set_operand_value(0x03)},
            .expected_changes = {set_register_a(0x23)},
        },
        {
            .initial_changes = {set_register_a(0x4C), set_operand_value(0x33)},
            .expected_changes = {set_register_a(0x7F)},
        },
        {
            .initial_changes = {set_register_a(0x22), set_operand_value(0x11)},
            .expected_changes = {set_register_a(0x33)},
        },

    });
    // clang-format on
};

TEST_CASE_METHOD(eor_fixture, "eor", "[cpu][instruction]")
{
    test_immediate(opcode::eor_immediate, addr_mode_scenario, behavior_scenarios);
    test_zero_page(opcode::eor_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(opcode::eor_zero_page_x, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(opcode::eor_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::eor_absolute_x, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::eor_absolute_y, register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(opcode::eor_indexed_indirect, addr_mode_scenario, behavior_scenarios);
    test_indirect_indexed(opcode::eor_indirect_indexed, addr_mode_scenario, behavior_scenarios);
}

} // namespace nese::cpu::instruction
