#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/status_flag_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(status_flag_fixture, "clc", "[cpu][instruction]")
{
    test_clear(opcode::clc_implied, status_flag::carry);
}

} // namespace nese::cpu::instruction
