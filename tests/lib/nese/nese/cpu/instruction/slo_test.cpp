#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct slo_fixture : execute_fixture
{
    static constexpr cpu_cycle_t base_cycle_cost = cpu_cycle_t(2);

    // clang-format off
    inline static const scenario addr_mode_scenario{
        .initial_changes = {set_operand_value(0x00), set_register_a(0x00)},
        .expected_changes = {set_operand_value(0x00), set_status_flag_zero()},
        .base_cycle_cost = base_cycle_cost
    };

    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        // flag carry = (original value & 0x80)
        {
            .initial_changes = {set_operand_value(0b10000000), set_register_a(0x00)},
            .expected_changes = {set_operand_value(0b00000000), set_status_flag_carry(), set_status_flag_zero()},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0b10000001), set_register_a(0x00)},
            .expected_changes = {set_operand_value(0b00000010), set_register_a(0b00000010), set_status_flag_carry()},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0b10000001), set_register_a(0b00000100)},
            .expected_changes = {set_operand_value(0b00000010), set_register_a(0b00000110), set_status_flag_carry()},
            .base_cycle_cost = base_cycle_cost
        },

        // flag negative
        {
            .initial_changes = {set_operand_value(0b01000000), set_register_a(0x00)},
            .expected_changes = {set_operand_value(0b10000000), set_register_a(0b10000000), set_status_flag_negative()},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0b01001000), set_register_a(0x00)},
            .expected_changes = {set_operand_value(0b10010000), set_register_a(0b10010000), set_status_flag_negative()},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0b01001000), set_register_a(0b00000100)},
            .expected_changes = {set_operand_value(0b10010000), set_register_a(0b10010100), set_status_flag_negative()},
            .base_cycle_cost = base_cycle_cost
        },

        // no flag
        {
            .initial_changes = {set_operand_value(0b00000100), set_register_a(0x00)},
            .expected_changes = {set_operand_value(0b00001000), set_register_a(0b00001000)},
            .base_cycle_cost = base_cycle_cost
        },
        {
            .initial_changes = {set_operand_value(0b00000100), set_register_a(0b00000100)},
            .expected_changes = {set_operand_value(0b00001000), set_register_a(0b00001100)},
            .base_cycle_cost = base_cycle_cost
        },
    });
    // clang-format on
};

TEST_CASE_METHOD(slo_fixture, "slo", "[cpu][instruction]")
{
    test_zero_page(opcode::slo_zero_page_unofficial, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(opcode::slo_zero_page_x_unofficial, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(opcode::slo_absolute_unofficial, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::slo_absolute_x_unofficial, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::slo_absolute_y_unofficial, register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(opcode::slo_indexed_indirect_unofficial, addr_mode_scenario, behavior_scenarios);
    test_indirect_indexed(opcode::slo_indirect_indexed_unofficial, addr_mode_scenario, behavior_scenarios);
}

} // namespace nese::cpu::instruction