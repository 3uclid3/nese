#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/load_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(load_fixture, "ldy", "[cpu][instruction]")
{
    test_immediate(opcode::ldy_immediate, register_id::y);
    test_zero_page(opcode::ldy_zero_page, register_id::y);
    test_zero_page_indexed(opcode::ldy_zero_page_x, register_id::y, register_id::x);
    test_absolute(opcode::ldy_absolute, register_id::y);
    test_absolute_indexed(opcode::ldy_absolute_x, register_id::y, register_id::x);
}

} // namespace nese::cpu::instruction
