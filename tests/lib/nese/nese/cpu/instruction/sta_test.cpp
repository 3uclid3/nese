#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/store_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(store_fixture, "sta", "[cpu][instruction]")
{
    const auto& addr_mode_scenario = store_fixture::addr_mode_scenario<register_id::a>;
    const auto& behavior_scenarios = store_fixture::behavior_scenarios<register_id::a>;

    test_zero_page(opcode::sta_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(opcode::sta_zero_page_x, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(opcode::sta_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::sta_absolute_x, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::sta_absolute_y, register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(opcode::sta_indexed_indirect, addr_mode_scenario, behavior_scenarios);
}

} // namespace nese::cpu::instruction
