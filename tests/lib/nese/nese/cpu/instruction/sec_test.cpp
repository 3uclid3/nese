#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/set_status_flag_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(set_status_flag_fixture, "sec", "[cpu][instruction]")
{
    test_implied(opcode::sec_implied, behavior_scenarios<status_flag::carry>);
}

} // namespace nese::cpu::instruction
