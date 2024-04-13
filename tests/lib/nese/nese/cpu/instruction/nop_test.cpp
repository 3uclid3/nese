#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct nop_fixture : execute_fixture
{
    static constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(2);

    // clang-format off
    inline static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial_changes = { set_register_pc(default_pc_addr)},
            .expected_changes = {},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = { set_register_pc(0x1234)},
            .expected_changes = {},
            .base_cycle_cost = cycle_cost
        },
        {
            .initial_changes = { set_register_pc(0x4321)},
            .expected_changes = {},
            .base_cycle_cost = cycle_cost
        },
    });
    // clang-format on
};

TEST_CASE_METHOD(nop_fixture, "nop", "[cpu][instruction]")
{
    test_implied(opcode::nop_implied, behavior_scenarios);
}

} // namespace nese::cpu::instruction
