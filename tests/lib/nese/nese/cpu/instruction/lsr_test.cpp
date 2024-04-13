#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

namespace nese::cpu::instruction {

struct lsr_fixture : execute_fixture
{
    // clang-format off
    inline static const scenario addr_mode_scenario{
        .initial_changes = {set_operand_value(0x00)},
        .expected_changes = {set_operand_value(0x00), set_status_flag_zero()}
    };

    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        // flag carry = (original value & 0x1)
        {
            .initial_changes = {set_operand_value(0x11)},
            .expected_changes = {set_operand_value(0x08), set_status_flag_carry()}
        },
        {
            .initial_changes = {set_operand_value(0x1)},
            .expected_changes = {set_operand_value(0x00), set_status_flag_carry(), set_status_flag_zero()}
        },        
        // no flag
        {
            .initial_changes = {set_operand_value(0b00000100)},
            .expected_changes = {set_operand_value(0b00000010)}
        },
        {
            .initial_changes = {set_operand_value(0b01100110)},
            .expected_changes = {set_operand_value(0b00110011)}
        },
    });
    // clang-format on
};

TEST_CASE_METHOD(lsr_fixture, "lsr", "[cpu][instruction]")
{
    test_acculumator(opcode::lsr_accumulator, behavior_scenarios);
    test_zero_page(opcode::lsr_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(opcode::lsr_zero_page_x, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(opcode::lsr_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::lsr_absolute_x, register_id::x, addr_mode_scenario, behavior_scenarios);
}

} // namespace nese::cpu::instruction
