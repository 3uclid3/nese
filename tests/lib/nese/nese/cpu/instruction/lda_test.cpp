#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/load_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(load_fixture, "lda", "[cpu][instruction]")
{
    const auto& addr_mode_scenario = load_fixture::addr_mode_scenario<register_id::a>;
    const auto& behavior_scenarios = load_fixture::behavior_scenarios<register_id::a>;

    test_immediate(opcode::lda_immediate, addr_mode_scenario, behavior_scenarios);
    test_zero_page(opcode::lda_zero_page, addr_mode_scenario, behavior_scenarios);
    test_zero_page_indexed(opcode::lda_zero_page_x, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute(opcode::lda_absolute, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::lda_absolute_x, register_id::x, addr_mode_scenario, behavior_scenarios);
    test_absolute_indexed(opcode::lda_absolute_y, register_id::y, addr_mode_scenario, behavior_scenarios);
    test_indexed_indirect(opcode::lda_indexed_indirect, addr_mode_scenario, behavior_scenarios);
}

} // namespace nese::cpu::instruction
