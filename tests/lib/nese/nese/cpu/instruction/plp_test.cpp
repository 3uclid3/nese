#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct plp_fixture : execute_fixture
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(4);

    // clang-format off
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, 0x00)},
            .expected_changes = {set_register_s(0x01), set_status_flags(status_flag::unused)},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0xFF), set_stack_value(0x00, 0x00)},
            .expected_changes = {set_register_s(0x00), set_status_flags(status_flag::unused)},
            .base_cycle_cost = cycle_cost
        },
        
        // flag set
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, static_cast<byte_t>(status_flag::carry))},
            .expected_changes = {set_register_s(0x01), set_status_flags(status_flag::carry | status_flag::unused)},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, static_cast<byte_t>(status_flag::decimal))},
            .expected_changes = {set_register_s(0x01), set_status_flags(status_flag::decimal | status_flag::unused)},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, static_cast<byte_t>(status_flag::unused))},
            .expected_changes = {set_register_s(0x01), set_status_flags(status_flag::unused)},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, static_cast<byte_t>(status_flag::overflow))},
            .expected_changes = {set_register_s(0x01), set_status_flags(status_flag::overflow | status_flag::unused)},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, static_cast<byte_t>(status_flag::negative))},
            .expected_changes = {set_register_s(0x01), set_status_flags(status_flag::negative | status_flag::unused)},
            .base_cycle_cost = cycle_cost
        },
        
        // flag ignored
        {
            .initial_changes = {set_register_s(0x00), set_stack_value(0x01, static_cast<byte_t>(status_flag::break_cmd))},
            .expected_changes = {set_register_s(0x01), set_status_flags(status_flag::unused)},
            .base_cycle_cost = cycle_cost
        }
    });
    // clang-format on
};

TEST_CASE_METHOD(plp_fixture, "plp", "[cpu][instruction]")
{
    test_implied(opcode::plp_implied, behavior_scenarios);
}

} // namespace nese::cpu::instruction
