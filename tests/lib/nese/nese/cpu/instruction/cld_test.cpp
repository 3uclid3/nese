#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/clear_status_flag_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(clear_status_flag_fixture, "cld", "[cpu][instruction]")
{
    test_implied(opcode::cld_implied, behavior_scenarios<status_flag::decimal>);
}

} // namespace nese::cpu::instruction
