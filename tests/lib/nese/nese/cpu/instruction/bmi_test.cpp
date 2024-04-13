#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/branch_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>
#include <nese/cpu/status_flag.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(branch_if_set_fixture, "bmi", "[cpu][instruction]")
{
    test_unspecified(opcode::bmi_relative, behavior_scenarios<status_flag::negative>);
}

} // namespace nese::cpu::instruction
