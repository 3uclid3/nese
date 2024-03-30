#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/se_fixture.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(se_fixture, "sei", "[cpu][instruction]")
{
    test_implied(execute_sei<addr_mode::implied>, status_flag::interrupt);
}

} // namespace nese::cpu::instruction
