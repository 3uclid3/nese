#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/se_fixture.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(se_fixture, "sec", "[cpu][instruction]")
{
    test_implied(execute_sec<addr_mode::implied>, status_flag::carry);
}

} // namespace nese::cpu::instruction
