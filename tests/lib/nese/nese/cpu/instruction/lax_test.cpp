#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

#if NESE_UNOFFICIAL_INSTRUCTIONS_ENABLED
namespace nese::cpu::instruction {

struct lax_fixture : execute_fixture
{
    // clang-format off
    inline static const scenario addr_mode_scenario{
        .initial_changes = {set_operand_value(1)},
        .expected_changes = {set_register_a(1), set_register_x(1)}
    };

    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial_changes = {set_operand_value(0x00)},
            .expected_changes = {set_register_a(0x00), set_register_x(0x00), set_status_flag_zero()},
            .description = "zero value"
        },
        {
            .initial_changes = {set_operand_value(0x01)},
            .expected_changes = {set_register_a(0x01), set_register_x(0x01)},
            .description = "smallest positive value"
        },
        {
            .initial_changes = {set_operand_value(0x40)},
            .expected_changes = {set_register_a(0x40), set_register_x(0x40)},
            .description = "mid positive value"
        },
        {
            .initial_changes = {set_operand_value(0x7F)},
            .expected_changes = {set_register_a(0x7F), set_register_x(0x7F)},
            .description = "largest positive value"
        },
        {
            .initial_changes = {set_operand_value(0xFF)},
            .expected_changes = {set_register_a(0xFF), set_register_x(0xFF), set_status_flag_negative()},
            .description = "smallest negative value"
        },
        {
            .initial_changes = {set_operand_value(0xC0)},
            .expected_changes = {set_register_a(0xC0), set_register_x(0xC0), set_status_flag_negative()},
            .description = "mid negative value"
        },
        {
            .initial_changes = {set_operand_value(0x80)},
            .expected_changes = {set_register_a(0x80), set_register_x(0x80), set_status_flag_negative()},
            .description = "largest negative value"
        }
    });
    // clang-format on
};

TEST_CASE_METHOD(lax_fixture, "lax", "[cpu][instruction]")
{
    test_zero_page(opcode::lax_zero_page_unofficial, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(opcode::lax_zero_page_y_unofficial, register_id::y, addr_mode_scenario, behavior_scenarios);
    test_absolute(opcode::lax_absolute_unofficial, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::lax_absolute_y_unofficial, register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(opcode::lax_indexed_indirect_unofficial, addr_mode_scenario, behavior_scenarios);
    test_indirect_indexed(opcode::lax_indirect_indexed_unofficial, addr_mode_scenario, behavior_scenarios);
}

} // namespace nese::cpu::instruction

#endif // NESE_UNOFFICIAL_INSTRUCTIONS_ENABLED
