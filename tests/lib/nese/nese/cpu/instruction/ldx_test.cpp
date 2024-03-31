#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/load_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(load_fixture, "ldx", "[cpu][instruction]")
{
    test_immediate(opcode::ldx_immediate, register_id::x);
    test_zero_page(opcode::ldx_zero_page, register_id::x);
    test_zero_page_indexed(opcode::ldx_zero_page_y, register_id::x, register_id::y);
    test_absolute(opcode::ldx_absolute, register_id::x);
    test_absolute_indexed(opcode::ldx_absolute_y, register_id::x, register_id::y);
}

} // namespace nese::cpu::instruction
