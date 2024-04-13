#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct php_fixture : execute_fixture
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(3);
    static constexpr status_flags mandatory_flags = static_cast<status_flags>(0x30);

    // clang-format off
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        // set flag
        {
            .initial_changes = {set_register_s(0x00), set_status_flags(status_flag::carry)},
            .expected_changes = {set_register_s(0xFF), set_stack_value(0x00, static_cast<byte_t>(status_flag::carry | mandatory_flags))},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_status_flags(status_flag::zero)},
            .expected_changes = {set_register_s(0xFF), set_stack_value(0x00, static_cast<byte_t>(status_flag::zero | mandatory_flags))},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_status_flags(status_flag::interrupt)},
            .expected_changes = {set_register_s(0xFF), set_stack_value(0x00, static_cast<byte_t>(status_flag::interrupt | mandatory_flags))},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_status_flags(status_flag::decimal)},
            .expected_changes = {set_register_s(0xFF), set_stack_value(0x00, static_cast<byte_t>(status_flag::decimal | mandatory_flags))},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_status_flags(status_flag::unused)},
            .expected_changes = {set_register_s(0xFF), set_stack_value(0x00, static_cast<byte_t>(status_flag::unused | mandatory_flags))},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_status_flags(status_flag::overflow)},
            .expected_changes = {set_register_s(0xFF), set_stack_value(0x00, static_cast<byte_t>(status_flag::overflow | mandatory_flags))},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_s(0x00), set_status_flags(status_flag::negative)},
            .expected_changes = {set_register_s(0xFF), set_stack_value(0x00, static_cast<byte_t>(status_flag::negative | mandatory_flags))},
            .base_cycle_cost = cycle_cost
        },

        // ignore flag
        {
            .initial_changes = {set_register_s(0x00), set_status_flags(status_flag::break_cmd)},
            .expected_changes = {set_register_s(0xFF), set_stack_value(0x00, static_cast<byte_t>(mandatory_flags))},
            .base_cycle_cost = cycle_cost
        }
    });
    // clang-format on
};

TEST_CASE_METHOD(php_fixture, "php", "[cpu][instruction]")
{
    test_implied(opcode::php_implied, behavior_scenarios);
}

} // namespace nese::cpu::instruction
