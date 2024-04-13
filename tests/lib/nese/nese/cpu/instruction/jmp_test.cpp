#include "fixture/execute_fixture.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct jmp_fixture : execute_fixture
{
    struct absolute
    {
        static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(3);

        // clang-format off
        inline static const std::array behavior_scenarios = std::to_array<scenario>({
            {
                .initial_changes = {set_register_pc(0x0200), set_memory_word_value(0x0200, 0x0100)},
                .expected_changes = {set_register_pc(0x0100)},
                .base_cycle_cost = cycle_cost
            },
            {
                .initial_changes = {set_register_pc(0x0200), set_memory_word_value(0x0200, 0x8000)},
                .expected_changes = {set_register_pc(0x8000)},
                .base_cycle_cost = cycle_cost
            },
            
            {
                .initial_changes = {set_register_pc(0x01FF), set_memory_word_value(0x01FF, 0x0000)},
                .expected_changes = {set_register_pc(0x0000)},
                .base_cycle_cost = cycle_cost
            },
            {
                .initial_changes = {set_register_pc(0x0200), set_memory_word_value(0x0200, 0xFFFF)},
                .expected_changes = {set_register_pc(0xFFFF)},
                .base_cycle_cost = cycle_cost
            },
            {
                .initial_changes = {set_register_pc(0xF000), set_memory_word_value(0xF000, 0x4000)},
                .expected_changes = {set_register_pc(0x4000)},
                .base_cycle_cost = cycle_cost
            },

            {
                .initial_changes = {set_register_pc(0x0000), set_memory_word_value(0x0000, 0x0200)},
                .expected_changes = {set_register_pc(0x0200)},
                .base_cycle_cost = cycle_cost
            },
            {
                .initial_changes = {set_register_pc(0x8000), set_memory_word_value(0x8000, 0x0300)},
                .expected_changes = {set_register_pc(0x0300)},
                .base_cycle_cost = cycle_cost
            },
            {
                .initial_changes = {set_register_pc(0xFFFC), set_memory_word_value(0xFFFC, 0x0400)},
                .expected_changes = {set_register_pc(0x0400)},
                .base_cycle_cost = cycle_cost
            }
        });
        // clang-format on
    };

    struct indirect
    {
        static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(3);

        // clang-format off
        inline static const std::array behavior_scenarios = std::to_array<scenario>({
            {
                .initial_changes = {set_register_pc(0x0200), set_memory_word_value(0x0200, 0x0100), set_memory_word_value(0x0100, 0x0300)},
                .expected_changes = {set_register_pc(0x0300)},
                .base_cycle_cost = cycle_cost
            },
            {
                .initial_changes = {set_register_pc(0x0200), set_memory_word_value(0x0200, 0x01FF), set_memory_value(0x01FF, 0x00), set_memory_value(0x0100, 0x03)},
                .expected_changes = {set_register_pc(0x0300)},
                .description = "JMP hardware bug",
                .base_cycle_cost = cycle_cost
            },
        });
        // clang-format on
    };
};

TEST_CASE_METHOD(jmp_fixture, "jmp", "[cpu][instruction]")
{
    SECTION("absolute")
    {
        test_unspecified(opcode::jmp_absolute, absolute::behavior_scenarios);
    }

    SECTION("indirect")
    {
        test_unspecified(opcode::jmp_indirect, indirect::behavior_scenarios);
    }
}

} // namespace nese::cpu::instruction
