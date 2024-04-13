#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/compare_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(compare_fixture, "cpx", "[cpu][instruction]")
{
    const auto& addr_mode_scenario = compare_fixture::addr_mode_scenario<register_id::x>;
    const auto& behavior_scenarios = compare_fixture::behavior_scenarios<register_id::x>;

    test_immediate(opcode::cpx_immediate, addr_mode_scenario, behavior_scenarios);
    test_zero_page(opcode::cpx_zero_page, addr_mode_scenario, behavior_scenarios);
    test_absolute(opcode::cpx_absolute, addr_mode_scenario, behavior_scenarios);
}

} // namespace nese::cpu::instruction
