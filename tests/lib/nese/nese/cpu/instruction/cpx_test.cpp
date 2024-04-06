#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/compare_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(compare_fixture, "cpx", "[cpu][instruction]")
{
    test_immediate(opcode::cpx_immediate, register_id::x);
    test_zero_page(opcode::cpx_zero_page, register_id::x);
    test_absolute(opcode::cpx_absolute, register_id::x);
}

} // namespace nese::cpu::instruction
