#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct rts_fixture : execute_fixture
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(6);

    // clang-format off
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial_changes = {set_register_s(0xFD), set_stack_value(0xFE, 0x00), set_stack_value(0xFF, 0x40)},
            .expected_changes = {set_register_pc(0x4001), set_register_s(0xFF)},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0xFE), set_stack_value(0xFF, 0x34), set_stack_value(0x00, 0x12)},
            .expected_changes = {set_register_pc(0x1235), set_register_s(0x00)},
            .description = "SP at FE to simulate edge case of wrapping around from 0x01FF to 0x0100",
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0xFF), set_stack_value(0x00, 0x00), set_stack_value(0x01, 0x02)},
            .expected_changes = {set_register_pc(0x0201), set_register_s(0x01)},
            .base_cycle_cost = cycle_cost
        }
    });
    // clang-format on
};

TEST_CASE_METHOD(rts_fixture, "rts", "[cpu][instruction]")
{
    test_implied(opcode::rts_implied, behavior_scenarios);
}

} // namespace nese::cpu::instruction
