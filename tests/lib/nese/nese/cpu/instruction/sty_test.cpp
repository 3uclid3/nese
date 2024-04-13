#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/store_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(store_fixture, "sty", "[cpu][instruction]")
{
    const auto& addr_mode_scenario = store_fixture::addr_mode_scenario<register_id::y>;
    const auto& behavior_scenarios = store_fixture::behavior_scenarios<register_id::y>;

    test_zero_page(opcode::sty_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(opcode::sty_zero_page_x, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(opcode::sty_absolute, addr_mode_scenario, behavior_scenarios);
}

} // namespace nese::cpu::instruction
