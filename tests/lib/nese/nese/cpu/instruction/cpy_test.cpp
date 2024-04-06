#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/compare_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(compare_fixture, "cpy", "[cpu][instruction]")
{
    test_immediate(opcode::cpy_immediate, register_id::y);
    test_zero_page(opcode::cpy_zero_page, register_id::y);
    test_absolute(opcode::cpy_absolute, register_id::y);
}

} // namespace nese::cpu::instruction
