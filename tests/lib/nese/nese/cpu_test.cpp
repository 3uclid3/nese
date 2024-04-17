#include <catch2/catch_test_macros.hpp>

#include <nese/cpu.hpp>
#include <nese/cpu_fixture.hpp>

namespace nese::v2 {

TEST_CASE_METHOD(cpu_fixture, "nop v2", "[cpu][instruction]")
{
    static constexpr auto cycle_cost = cpu_cycle_t(2);

    // clang-format off
    static const std::array behavior_scenarios = std::to_array<scenario>({
        {
            .initial = {set_pc(default_pc_addr)},
            .expected = {}
        },
        {
            .initial = { set_pc(0x1234)},
            .expected = {}
        },
        {
            .initial = { set_pc(0x4321)},
            .expected = {}
        },
    });
    // clang-format on

    test_implied(cpu_opcode::nop_implied, behavior_scenarios, cycle_cost);
}

} // namespace nese::v2
