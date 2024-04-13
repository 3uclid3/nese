#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

namespace nese::cpu::instruction {

struct bit_fixture : execute_fixture
{
    // clang-format off
    inline static const scenario addr_mode_scenario{
        .initial_changes = {set_register_a(0xFF), set_operand_value(0x01)},
        .expected_changes = {}
    };
    
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        // set zero
        {
            .initial_changes = {set_register_a(0xFF), set_operand_value(0x00)},
            .expected_changes = {set_status_flag_zero()},
        },
        {
            .initial_changes = {set_register_a(0xF0), set_operand_value(0x0F)},
            .expected_changes = {set_status_flag_zero()},
        },
        {
            .initial_changes = {set_register_a(0x40), set_operand_value(0x3F)},
            .expected_changes = {set_status_flag_zero()},
        },
        {
            .initial_changes = {set_register_a(0x20), set_operand_value(0x1F)},
            .expected_changes = {set_status_flag_zero()},
        },

        // not set zero
        {
            .initial_changes = {set_register_a(0xBF), set_operand_value(0x3F)},
            .expected_changes = {clear_status_flag_zero()},
        },
        {
            .initial_changes = {set_register_a(0x9F), set_operand_value(0x2F)},
            .expected_changes = {clear_status_flag_zero()},
        },
        
        // set overflow
        {
            .initial_changes = {set_register_a(0xFF), set_operand_value(0x40)},
            .expected_changes = {set_status_flag_overflow()},
        },
        {
            .initial_changes = {set_register_a(0xFF), set_operand_value(0x41)},
            .expected_changes = {set_status_flag_overflow()},
        },
        {
            .initial_changes = {set_register_a(0x7F), set_operand_value(0x40)},
            .expected_changes = {set_status_flag_overflow()},
        },
        {
            .initial_changes = {set_register_a(0xBF), set_operand_value(0x42)},
            .expected_changes = {set_status_flag_overflow()},
        },
        
        // not set overflow
        {
            .initial_changes = {set_register_a(0x01), set_operand_value(0x01)},
            .expected_changes = {clear_status_flag_overflow()},
        },
        {
            .initial_changes = {set_register_a(0x03), set_operand_value(0x02)},
            .expected_changes = {clear_status_flag_overflow()},
        },
        {
            .initial_changes = {set_register_a(0x0C), set_operand_value(0x08)},
            .expected_changes = {clear_status_flag_overflow()},
        },
        {
            .initial_changes = {set_register_a(0x10), set_operand_value(0x10)},
            .expected_changes = {clear_status_flag_overflow()},
        },
        
        // set negative
        {
            .initial_changes = {set_register_a(0xFF), set_operand_value(0x80)},
            .expected_changes = {set_status_flag_negative()},
        },
        {
            .initial_changes = {set_register_a(0xFF), set_operand_value(0x81)},
            .expected_changes = {set_status_flag_negative()},
        },
        {
            .initial_changes = {set_register_a(0x80), set_operand_value(0x80)},
            .expected_changes = {set_status_flag_negative()},
        },
        {
            .initial_changes = {set_register_a(0xBF), set_operand_value(0x82)},
            .expected_changes = {set_status_flag_negative()},
        },
        
        // not set negative
        {
            .initial_changes = {set_register_a(0xFF), set_operand_value(0x3F)},
            .expected_changes = {clear_status_flag_negative()},
        },
        {
            .initial_changes = {set_register_a(0xEF), set_operand_value(0x20)},
            .expected_changes = {clear_status_flag_negative()},
        },
        {
            .initial_changes = {set_register_a(0xF7), set_operand_value(0x1F)},
            .expected_changes = {clear_status_flag_negative()},
        }
    });
    // clang-format on
};

TEST_CASE_METHOD(bit_fixture, "bit", "[cpu][instruction]")
{
    test_zero_page(opcode::bit_zero_page, addr_mode_scenario, behavior_scenarios);
    test_absolute(opcode::bit_absolute, addr_mode_scenario, behavior_scenarios);
}

} // namespace nese::cpu::instruction
