#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/increment_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(increment_fixture, "inx", "[cpu][instruction]")
{
    test_inc(opcode::inx_implied, register_id::x);
}

} // namespace nese::cpu::instruction