#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/compare_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(compare_fixture, "cpy", "[cpu][instruction]")
{
    const auto& addr_mode_scenario = compare_fixture::addr_mode_scenario<register_id::y>;
    const auto& behavior_scenarios = compare_fixture::behavior_scenarios<register_id::y>;

    test_immediate(opcode::cpy_immediate, addr_mode_scenario, behavior_scenarios);
    test_zero_page(opcode::cpy_zero_page, addr_mode_scenario, behavior_scenarios);
    test_absolute(opcode::cpy_absolute, addr_mode_scenario, behavior_scenarios);
}

} // namespace nese::cpu::instruction
