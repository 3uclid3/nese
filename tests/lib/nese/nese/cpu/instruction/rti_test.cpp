#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>
#include <nese/cpu/stack.hpp>

namespace nese::cpu::instruction {

struct rti_fixture : execute_fixture
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(6);

    // clang-format off
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        // addr
        {
            .initial_changes = {set_register_s(0xFC), set_stack_value(0xFD, 0x00), set_stack_word_value(0xFE, 0x4000)},
            .expected_changes = {set_register_pc(0x4000), set_register_s(0xFF), set_status_flag_unused()},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0xFD), set_stack_value(0xFE, 0x00), set_stack_word_value(0xFF, 0x1234)},
            .expected_changes = {set_register_pc(0x1234), set_register_s(0x00), set_status_flag_unused()},
            .description = "SP at FE to simulate edge case of wrapping around from 0x01FF to 0x0100",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0xFE), set_stack_value(0xFF, 0x00), set_stack_word_value(0x00, 0x0200)},
            .expected_changes = {set_register_pc(0x0200), set_register_s(0x01), set_status_flag_unused()},
            .base_cycle_cost = cycle_cost
        },

        // flag set
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, static_cast<byte_t>(status_flag::carry)), set_stack_word_value(0x02, 0x0200)},
            .expected_changes = {set_register_pc(0x0200), set_register_s(0x03), set_status_flag_carry(), set_status_flag_unused()},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, static_cast<byte_t>(status_flag::decimal)), set_stack_word_value(0x02, 0x0200)},
            .expected_changes = {set_register_pc(0x0200), set_register_s(0x03), set_status_flag_decimal(), set_status_flag_unused()},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, static_cast<byte_t>(status_flag::interrupt)), set_stack_word_value(0x02, 0x0200)},
            .expected_changes = {set_register_pc(0x0200), set_register_s(0x03), set_status_flag_interrupt(), set_status_flag_unused()},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, static_cast<byte_t>(status_flag::negative)), set_stack_word_value(0x02, 0x0200)},
            .expected_changes = {set_register_pc(0x0200), set_register_s(0x03), set_status_flag_negative(), set_status_flag_unused()},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, static_cast<byte_t>(status_flag::overflow)), set_stack_word_value(0x02, 0x0200)},
            .expected_changes = {set_register_pc(0x0200), set_register_s(0x03), set_status_flag_overflow(), set_status_flag_unused()},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, static_cast<byte_t>(status_flag::unused)), set_stack_word_value(0x02, 0x0200)},
            .expected_changes = {set_register_pc(0x0200), set_register_s(0x03), set_status_flag_unused()},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, static_cast<byte_t>(status_flag::zero)), set_stack_word_value(0x02, 0x0200)},
            .expected_changes = {set_register_pc(0x0200), set_register_s(0x03), set_status_flag_zero(), set_status_flag_unused()},
            .base_cycle_cost = cycle_cost
        },

        // flag ignore
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, static_cast<byte_t>(status_flag::break_cmd)), set_stack_word_value(0x02, 0x0200)},
            .expected_changes = {set_register_pc(0x0200), set_register_s(0x03), set_status_flag_unused()},
            .base_cycle_cost = cycle_cost
        },
    });
    // clang-format on
};

TEST_CASE_METHOD(rti_fixture, "rti", "[cpu][instruction]")
{
    test_implied(opcode::rti_implied, behavior_scenarios);
}

} // namespace nese::cpu::instruction
