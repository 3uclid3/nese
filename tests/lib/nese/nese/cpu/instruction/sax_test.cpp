#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/store_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct sax_fixture : execute_fixture
{
    // clang-format off
    inline static const scenario addr_mode_scenario{
        .initial_changes = {set_register_a(0x00), set_register_x(0x00)},
        .expected_changes = {set_operand_value(0x00)}
    };

    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        // zero
        {
            .initial_changes = {set_register_a(0x00), set_register_x(0xFF)},
            .expected_changes = {set_operand_value(0x00)},
        },
        {
            .initial_changes = {set_register_a(0xFF), set_register_x(0x00)},
            .expected_changes = {set_operand_value(0x00)},
        },
        {
            .initial_changes = {set_register_a(0x0F), set_register_x(0xF0)},
            .expected_changes = {set_operand_value(0x00)},
        },
        
        // negative
        {
            .initial_changes = {set_register_a(0x80), set_register_x(0xFF)},
            .expected_changes = {set_operand_value(0x80)},
        },
        {
            .initial_changes = {set_register_a(0xFF), set_register_x(0x80)},
            .expected_changes = {set_operand_value(0x80)},
        },
        {
            .initial_changes = {set_register_a(0xC0), set_register_x(0xF0)},
            .expected_changes = {set_operand_value(0xC0)},
        },
        {
            .initial_changes = {set_register_a(0xF0), set_register_x(0xC0)},
            .expected_changes = {set_operand_value(0xC0)},
        },
        {
            .initial_changes = {set_register_a(0x80), set_register_x(0x80)},
            .expected_changes = {set_operand_value(0x80)},
        },
        {
            .initial_changes = {set_register_a(0xFF), set_register_x(0xC0)},
            .expected_changes = {set_operand_value(0xC0)},
        },
        
        // positive
        {
            .initial_changes = {set_register_a(0x7F), set_register_x(0xFF)},
            .expected_changes = {set_operand_value(0x7F)},
        },
        {
            .initial_changes = {set_register_a(0x3F), set_register_x(0x7F)},
            .expected_changes = {set_operand_value(0x3F)},
        },
        {
            .initial_changes = {set_register_a(0x1F), set_register_x(0x2F)},
            .expected_changes = {set_operand_value(0x0F)},
        },
        {
            .initial_changes = {set_register_a(0x0F), set_register_x(0x8F)},
            .expected_changes = {set_operand_value(0x0F)},
        },
        {
            .initial_changes = {set_register_a(0x7F), set_register_x(0x3F)},
            .expected_changes = {set_operand_value(0x3F)},
        },
        {
            .initial_changes = {set_register_a(0x2F), set_register_x(0x1F)},
            .expected_changes = {set_operand_value(0x0F)},
        },
        {
            .initial_changes = {set_register_a(0x01), set_register_x(0x01)},
            .expected_changes = {set_operand_value(0x01)},
        }
    });
    // clang-format on
};

TEST_CASE_METHOD(sax_fixture, "sax", "[cpu][instruction]")
{
    test_zero_page(opcode::sax_zero_page_unofficial, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(opcode::sax_zero_page_y_unofficial, register_id::y, addr_mode_scenario, behavior_scenarios);
    test_absolute(opcode::sax_absolute_unofficial, addr_mode_scenario, behavior_scenarios);

    // TODO test indexed_indirect
    //test_indexed_indirect(opcode::sax_indexed_indirect_unofficial, addr_mode_scenario, behavior_scenarios);
}

} // namespace nese::cpu::instruction
