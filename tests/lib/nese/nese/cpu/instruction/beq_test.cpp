#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/branch_fixture.hpp>
#include <nese/cpu/status_flag.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(branch_fixture, "beq", "[cpu][instruction]")
{
    test_relative(execute_beq<addr_mode::relative>, status_flag::zero, branch_when::is_set);
}

} // namespace nese::cpu::instruction