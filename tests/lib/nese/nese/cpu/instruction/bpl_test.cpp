#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/branch_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>
#include <nese/cpu/status_flag.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(branch_fixture, "bpl", "[cpu][instruction]")
{
    test_relative(opcode::bpl_relative, status_flag::negative, branch_when::is_clear);
}

} // namespace nese::cpu::instruction
