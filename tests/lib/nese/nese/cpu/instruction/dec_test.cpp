#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct dec_fixture : execute_fixture
{
    // clang-format off
    inline static const scenario addr_mode_scenario{
        .initial_changes = {set_operand_value(0x02)},
        .expected_changes = {set_operand_value(0x01)}
    };

    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial_changes = {set_operand_value(0x00), set_status_flag_zero()},
            .expected_changes = {set_operand_value(0xFF), clear_status_flag_zero(), set_status_flag_negative()},
            .description = "0x00 results in overflow to 0xFF",
        },
        {
            .initial_changes = {set_operand_value(0x80), set_status_flag_negative()},
            .expected_changes = {set_operand_value(0x7F), clear_status_flag_negative()},
            .description = "0x80 results in overflow to 0x7F",
        },
        {
            .initial_changes = {set_operand_value(0x01)},
            .expected_changes = {set_operand_value(0x00), set_status_flag_zero()},
            .description = "zero",
        },
        {
            .initial_changes = {set_operand_value(0x81), set_status_flag_negative()},
            .expected_changes = {set_operand_value(0x80)},
            .description = "smallest negative value minus one",
        },
        {
            .initial_changes = {set_operand_value(0xC1), set_status_flag_negative()},
            .expected_changes = {set_operand_value(0xC0)},
            .description = "mid negative value",
        },
        {
            .initial_changes = {set_operand_value(0xFF), set_status_flag_negative()},
            .expected_changes = {set_operand_value(0xFE)},
            .description = "largest negative value",
        },
        {
            .initial_changes = {set_operand_value(0x02)},
            .expected_changes = {set_operand_value(0x01)},
            .description = "smallest positive value minus one",
        },
        {
            .initial_changes = {set_operand_value(0x41)},
            .expected_changes = {set_operand_value(0x40)},
            .description = "mid positive value",
        },
        {
            .initial_changes = {set_operand_value(0x7F)},
            .expected_changes = {set_operand_value(0x7E)},
            .description = "largest positive value",
        }
    });
    // clang-format on
};

TEST_CASE_METHOD(dec_fixture, "dec", "[cpu][instruction]")
{
    test_zero_page(opcode::dec_zero_page, addr_mode_scenario, behavior_scenarios, cpu_cycle_t(5));
    test_zero_page_indexed(opcode::dec_zero_page_x, register_id::x, addr_mode_scenario, behavior_scenarios, cpu_cycle_t(6));
    test_absolute(opcode::dec_absolute, addr_mode_scenario, behavior_scenarios, cpu_cycle_t(6));
    test_absolute_indexed(opcode::dec_absolute_x, register_id::x, addr_mode_scenario, behavior_scenarios, cpu_cycle_t(7));
}

} // namespace nese::cpu::instruction