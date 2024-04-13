#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct jsr_fixture : execute_fixture
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(6);

    // clang-format off
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial_changes = {set_register_pc(0x0200), set_memory_word_value(0x0200, 0x0100)},
            .expected_changes = {set_register_pc(0x0100), push_word_stack(0x0201)},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_pc(0x0200), set_memory_word_value(0x0200, 0x8000)},
            .expected_changes = {set_register_pc(0x8000), push_word_stack(0x0201)},
            .base_cycle_cost = cycle_cost
        },
        
        {
            .initial_changes = {set_register_pc(0x01FF), set_memory_word_value(0x01FF, 0x0000)},
            .expected_changes = {set_register_pc(0x0000), push_word_stack(0x0200)},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_pc(0x0200), set_memory_word_value(0x0200, 0xFFFF)},
            .expected_changes = {set_register_pc(0xFFFF), push_word_stack(0x0201)},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_pc(0xF000), set_memory_word_value(0xF000, 0x4000)},
            .expected_changes = {set_register_pc(0x4000), push_word_stack(0xF001)},
            .base_cycle_cost = cycle_cost
        },

        {
            .initial_changes = {set_register_pc(0x0000), set_memory_word_value(0x0000, 0x0200)},
            .expected_changes = {set_register_pc(0x0200), push_word_stack(0x0001)},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_pc(0x8000), set_memory_word_value(0x8000, 0x0300)},
            .expected_changes = {set_register_pc(0x0300), push_word_stack(0x8001)},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = {set_register_pc(0xFFFC), set_memory_word_value(0xFFFC, 0x0400)},
            .expected_changes = {set_register_pc(0x0400), push_word_stack(0xFFFD)},
            .base_cycle_cost = cycle_cost
        }
    });
    // clang-format on
};

TEST_CASE_METHOD(jsr_fixture, "jsr", "[cpu][instruction]")
{
    test_unspecified(opcode::jsr_absolute, behavior_scenarios);
}

} // namespace nese::cpu::instruction
