#pragma once

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/status_flag.hpp>

namespace nese::cpu {

namespace instruction {

struct branch_fixture : execute_fixture
{
    static constexpr cpu_cycle_t no_branching_cycle_cost = cpu_cycle_t(2);
    static constexpr cpu_cycle_t no_page_crossing_cycle_cost = cpu_cycle_t(3);
    static constexpr cpu_cycle_t page_crossing_cycle_cost = cpu_cycle_t(4);
};

struct branch_if_set_fixture : branch_fixture
{
    // clang-format off
    template<status_flag StatusFlagT>
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
         // no page crossing
        {
            .initial_changes = {set_status_flag<StatusFlagT>(), set_register_pc(0x0000), set_memory_value(0x0000, 0x10)},
            .expected_changes = {set_register_pc(0x0011)},
            .description = "within the first page",
            .base_cycle_cost = no_page_crossing_cycle_cost
        },
        {
            .initial_changes = {set_register_pc(0x0000), set_memory_value(0x0000, 0x10)},
            .expected_changes = {set_register_pc(0x0001)},
            .description = "no branching within the first page",
            .base_cycle_cost = no_branching_cycle_cost
        },
        {
            .initial_changes = {set_status_flag<StatusFlagT>(), set_register_pc(0x0080), set_memory_value(0x0080, 0x7E)},
            .expected_changes = {set_register_pc(0x0081 + 0x7E)},
            .description = "near the middle of the first page, large offset but no crossing",
            .base_cycle_cost = no_page_crossing_cycle_cost
        },
        {
            .initial_changes = {set_register_pc(0x0080), set_memory_value(0x0080, 0x7E)},
            .expected_changes = {set_register_pc(0x0081)},
            .description = "no branching near the middle of the first page, large offset but no crossing",
            .base_cycle_cost = no_branching_cycle_cost
        },

         // page crossing
        {
            .initial_changes = {set_status_flag<StatusFlagT>(), set_register_pc(0x00F0), set_memory_value(0x00F0, 0x0F)},
            .expected_changes = {set_register_pc(0x0100)},
            .description = "crosses from 0x00xx to 0x01xx page",
            .base_cycle_cost = page_crossing_cycle_cost
        },
        {
            .initial_changes = {set_register_pc(0x00F0), set_memory_value(0x00F0, 0x0F)},
            .expected_changes = {set_register_pc(0x00F1)},
            .description = "no branching crosses from 0x00xx to 0x01xx page",
            .base_cycle_cost = no_branching_cycle_cost
        },
        {
            .initial_changes = {set_status_flag<StatusFlagT>(), set_register_pc(0x01FD), set_memory_value(0x01FD, 0x02)},
            .expected_changes = {set_register_pc(0x0200)},
            .description = "crosses from 0x01xx to 0x02xx page",
            .base_cycle_cost = page_crossing_cycle_cost
        },
        {
            .initial_changes = {set_register_pc(0x01FD), set_memory_value(0x01FD, 0x02)},
            .expected_changes = {set_register_pc(0x01FE)},
            .description = "no branching crosses from 0x01xx to 0x02xx page",
            .base_cycle_cost = no_branching_cycle_cost
        },
        {
            .initial_changes = {set_status_flag<StatusFlagT>(), set_register_pc(0x0FFF), set_memory_value(0x0FFF, 0x01)},
            .expected_changes = {set_register_pc(0x1001)},
            .description = "crosses from 0x0Fxx to 0x10xx page",
            .base_cycle_cost = page_crossing_cycle_cost
        }
    });
    // clang-format on
};

struct branch_if_clear_fixture : branch_fixture
{
    // clang-format off
    template<status_flag StatusFlagT>
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        // no page crossing
        {
            .initial_changes = {set_register_pc(0x0000), set_memory_value(0x0000, 0x10)},
            .expected_changes = {set_register_pc(0x0011)},
            .description = "within the first page",
            .base_cycle_cost = no_page_crossing_cycle_cost
        },
        {
            .initial_changes = {set_status_flag<StatusFlagT>(), set_register_pc(0x0000), set_memory_value(0x0000, 0x10)},
            .expected_changes = {set_register_pc(0x0001)},
            .description = "no branching within the first page",
            .base_cycle_cost = no_branching_cycle_cost
        },
        {
            .initial_changes = {set_register_pc(0x0080), set_memory_value(0x0080, 0x7E)},
            .expected_changes = {set_register_pc(0x0081 + 0x7E)},
            .description = "near the middle of the first page, large offset but no crossing",
            .base_cycle_cost = no_page_crossing_cycle_cost
        },
        {
            .initial_changes = {set_status_flag<StatusFlagT>(), set_register_pc(0x0080), set_memory_value(0x0080, 0x7E)},
            .expected_changes = {set_register_pc(0x0081)},
            .description = "no branching near the middle of the first page, large offset but no crossing",
            .base_cycle_cost = no_branching_cycle_cost
        },

         // page crossing
        {
            .initial_changes = {set_register_pc(0x00F0), set_memory_value(0x00F0, 0x0F)},
            .expected_changes = {set_register_pc(0x0100)},
            .description = "crosses from 0x00xx to 0x01xx page",
            .base_cycle_cost = page_crossing_cycle_cost
        },
        {
            .initial_changes = {set_status_flag<StatusFlagT>(), set_register_pc(0x00F0), set_memory_value(0x00F0, 0x0F)},
            .expected_changes = {set_register_pc(0x00F1)},
            .description = "no branching crosses from 0x00xx to 0x01xx page",
            .base_cycle_cost = no_branching_cycle_cost
        },
        {
            .initial_changes = {set_register_pc(0x01FD), set_memory_value(0x01FD, 0x02)},
            .expected_changes = {set_register_pc(0x0200)},
            .description = "crosses from 0x01xx to 0x02xx page",
            .base_cycle_cost = page_crossing_cycle_cost
        },
        {
            .initial_changes = {set_status_flag<StatusFlagT>(), set_register_pc(0x01FD), set_memory_value(0x01FD, 0x02)},
            .expected_changes = {set_register_pc(0x01FE)},
            .description = "no branching crosses from 0x01xx to 0x02xx page",
            .base_cycle_cost = no_branching_cycle_cost
        },
        {
            .initial_changes = {set_register_pc(0x0FFF), set_memory_value(0x0FFF, 0x01)},
            .expected_changes = {set_register_pc(0x1001)},
            .description = "crosses from 0x0Fxx to 0x10xx page",
            .base_cycle_cost = page_crossing_cycle_cost
        }
    });
    // clang-format on
};

} // namespace instruction
} // namespace nese::cpu
