#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/load_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(load_fixture, "ldx", "[cpu][instruction]")
{
    const auto& addr_mode_scenario = load_fixture::addr_mode_scenario<register_id::x>;
    const auto& behavior_scenarios = load_fixture::behavior_scenarios<register_id::x>;

    test_immediate(opcode::ldx_immediate, addr_mode_scenario, behavior_scenarios);
    test_zero_page(opcode::ldx_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(opcode::ldx_zero_page_y, register_id::y, addr_mode_scenario, behavior_scenarios);
    test_absolute(opcode::ldx_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::ldx_absolute_y, register_id::y, addr_mode_scenario, behavior_scenarios);
}

} // namespace nese::cpu::instruction
