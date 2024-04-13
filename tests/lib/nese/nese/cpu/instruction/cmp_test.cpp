#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/compare_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(compare_fixture, "cmp", "[cpu][instruction]")
{
    const auto& addr_mode_scenario = compare_fixture::addr_mode_scenario<register_id::a>;
    const auto& behavior_scenarios = compare_fixture::behavior_scenarios<register_id::a>;

    test_immediate(opcode::cmp_immediate, addr_mode_scenario, behavior_scenarios);
    test_zero_page(opcode::cmp_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(opcode::cmp_zero_page_x, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(opcode::cmp_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::cmp_absolute_x, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::cmp_absolute_y, register_id::y, addr_mode_scenario, behavior_scenarios);
}

} // namespace nese::cpu::instruction
