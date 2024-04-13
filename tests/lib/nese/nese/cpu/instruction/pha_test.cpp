#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct pha_fixture : execute_fixture
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(3);

    // clang-format off
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial_changes = {set_register_s(0x00), set_register_a(0x00)},
            .expected_changes = {set_register_s(0xFF), set_stack_value(0x00, 0x00)},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_register_a(0xFF)},
            .expected_changes = {set_register_s(0xFF), set_stack_value(0x00, 0xFF)},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_register_a(0x80)},
            .expected_changes = {set_register_s(0xFF), set_stack_value(0x00, 0x80)},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_register_a(0x55)},
            .expected_changes = {set_register_s(0xFF), set_stack_value(0x00, 0x55)},
            .base_cycle_cost = cycle_cost
        },
    });
    // clang-format on
};

TEST_CASE_METHOD(pha_fixture, "pha", "[cpu][instruction]")
{
    test_implied(opcode::pha_implied, behavior_scenarios);
}

} // namespace nese::cpu::instruction
