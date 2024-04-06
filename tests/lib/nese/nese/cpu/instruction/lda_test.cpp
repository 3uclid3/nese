#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/load_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(load_fixture, "lda", "[cpu][instruction]")
{
    test_immediate(opcode::lda_immediate, register_id::a);
    test_zero_page(opcode::lda_zero_page, register_id::a);
    test_zero_page_indexed(opcode::lda_zero_page_x, register_id::a, register_id::x);
    test_absolute(opcode::lda_absolute, register_id::a);
    test_absolute_indexed(opcode::lda_absolute_x, register_id::a, register_id::x);
    test_absolute_indexed(opcode::lda_absolute_y, register_id::a, register_id::y);
}

} // namespace nese::cpu::instruction