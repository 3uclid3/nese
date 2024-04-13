#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct sbc_fixture : execute_fixture
{
    // clang-format off
    inline static const scenario addr_mode_scenario{
        .initial_changes = {set_register_a(0x00), set_operand_value(0x00), set_status_flag_carry()},
        .expected_changes = {set_status_flag_zero()}
    };
    
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        // carry flag initially clear
        {
            .initial_changes = {set_register_a(0x00), set_operand_value(0x00)},
            .expected_changes = {set_register_a(0xFF), set_status_flag_negative()}
        },
        {
            .initial_changes = {set_register_a(0x01), set_operand_value(0x00)},
            .expected_changes = {set_register_a(0x00), set_status_flag_carry(), set_status_flag_zero()}
        },
        {
            .initial_changes = {set_register_a(0x02), set_operand_value(0x01)},
            .expected_changes = {set_register_a(0x00), set_status_flag_carry(), set_status_flag_zero()}
        },
        {
            .initial_changes = {set_register_a(0x80), set_operand_value(0x7F)},
            .expected_changes = {set_register_a(0x00), set_status_flag_carry(), set_status_flag_zero(), set_status_flag_overflow()}
        },
        {
            .initial_changes = {set_register_a(0x80), set_operand_value(0x80)},
            .expected_changes = {set_register_a(0xFF), set_status_flag_negative()}
        },

        // carry flag initially set
        {
            .initial_changes = {set_register_a(0x00), set_operand_value(0x00), set_status_flag_carry()},
            .expected_changes = {set_register_a(0x00), set_status_flag_zero()}
        },
        {
            .initial_changes = {set_register_a(0x01), set_operand_value(0x01), set_status_flag_carry()},
            .expected_changes = {set_register_a(0x00), set_status_flag_zero()}
        },
        {
            .initial_changes = {set_register_a(0x02), set_operand_value(0x01), set_status_flag_carry()},
            .expected_changes = {set_register_a(0x01)}
        },
        {
            .initial_changes = {set_register_a(0x7F), set_operand_value(0x01), set_status_flag_carry()},
            .expected_changes = {set_register_a(0x7E)}
        },
        {
            .initial_changes = {set_register_a(0x80), set_operand_value(0x7F), set_status_flag_carry()},
            .expected_changes = {set_register_a(0x01), set_status_flag_overflow()}
        },
        {
            .initial_changes = {set_register_a(0x80), set_operand_value(0x80), set_status_flag_carry()},
            .expected_changes = {set_register_a(0x00), set_status_flag_zero()}
        },
        {
            .initial_changes = {set_register_a(0x7E), set_operand_value(0x7F), set_status_flag_carry()},
            .expected_changes = {set_register_a(0xFF), set_status_flag_negative(), clear_status_flag_carry()}
        },
        {
            .initial_changes = {set_register_a(0xFF), set_operand_value(0x7E), set_status_flag_carry()},
            .expected_changes = {set_register_a(0x81), set_status_flag_negative()}
        },
        {
            .initial_changes = {set_register_a(0x00), set_operand_value(0xFF), set_status_flag_carry()},
            .expected_changes = {set_register_a(0x01), clear_status_flag_carry()}
        },
    });
    // clang-format on
};

TEST_CASE_METHOD(sbc_fixture, "sbc", "[cpu][instruction]")
{
    test_immediate(opcode::sbc_immediate, addr_mode_scenario, behavior_scenarios);
    test_zero_page(opcode::sbc_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(opcode::sbc_zero_page_x, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(opcode::sbc_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::sbc_absolute_x, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::sbc_absolute_y, register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(opcode::sbc_indexed_indirect, addr_mode_scenario, behavior_scenarios);
}

} // namespace nese::cpu::instruction
