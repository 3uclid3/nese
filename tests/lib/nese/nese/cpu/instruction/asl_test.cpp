#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

namespace nese::cpu::instruction {

struct asl_fixture : execute_fixture
{
    // clang-format off
    inline static const scenario addr_mode_scenario{
        .initial_changes = {set_operand_value(0x00)},
        .expected_changes = {set_operand_value(0x00), set_status_flag_zero()}
    };

    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        // flag carry = (original value & 0x80)
        {
            .initial_changes = {set_operand_value(0b10000000)},
            .expected_changes = {set_operand_value(0b00000000), set_status_flag_carry(), set_status_flag_zero()}
        },
        {
            .initial_changes = {set_operand_value(0b10000001)},
            .expected_changes = {set_operand_value(0b00000010), set_status_flag_carry()}
        },
        
        // flag negative
        {
            .initial_changes = {set_operand_value(0b01000000)},
            .expected_changes = {set_operand_value(0b10000000), set_status_flag_negative()}
        },
        {
            .initial_changes = {set_operand_value(0b01001000)},
            .expected_changes = {set_operand_value(0b10010000), set_status_flag_negative()}
        },
        
        // no flag
        {
            .initial_changes = {set_operand_value(0b00000100)},
            .expected_changes = {set_operand_value(0b00001000)}
        },
    });
    // clang-format on
};

TEST_CASE_METHOD(asl_fixture, "asl", "[cpu][instruction]")
{
    test_acculumator(opcode::asl_accumulator, behavior_scenarios);
    test_zero_page(opcode::asl_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(opcode::asl_zero_page_x, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(opcode::asl_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::asl_absolute_x, register_id::x, addr_mode_scenario, behavior_scenarios);
}

} // namespace nese::cpu::instruction
