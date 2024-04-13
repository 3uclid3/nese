#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct pla_fixture : execute_fixture
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(4);

    // clang-format off
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, 0x01)},
            .expected_changes = {set_register_s(0x01), set_register_a(0x01)},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0xFF), set_stack_value(0x00, 0x01)},
            .expected_changes = {set_register_s(0x00), set_register_a(0x01)},
            .base_cycle_cost = cycle_cost
        },

        // zero
        {
            .initial_changes = {set_register_s(0xFF), set_stack_value(0x00, 0x00)},
            .expected_changes = {set_register_s(0x00), set_register_a(0x00), set_status_flag_zero()},
            .base_cycle_cost = cycle_cost
        },

        // negative
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, 0x80)},
            .expected_changes = {set_register_s(0x01), set_register_a(0x80), set_status_flag_negative()},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, 0xC0)},
            .expected_changes = {set_register_s(0x01), set_register_a(0xC0), set_status_flag_negative()},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, 0xFF)},
            .expected_changes = {set_register_s(0x01), set_register_a(0xFF), set_status_flag_negative()},
            .base_cycle_cost = cycle_cost
        },

        // positive
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, 0x10)},
            .expected_changes = {set_register_s(0x01), set_register_a(0x10)},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, 0x40)},
            .expected_changes = {set_register_s(0x01), set_register_a(0x40)},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, 0x7F)},
            .expected_changes = {set_register_s(0x01), set_register_a(0x7F)},
            .base_cycle_cost = cycle_cost
        }
    });
    // clang-format on
};

TEST_CASE_METHOD(pla_fixture, "pla", "[cpu][instruction]")
{
    test_implied(opcode::pla_implied, behavior_scenarios);
}

} // namespace nese::cpu::instruction
