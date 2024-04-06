#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/store_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(store_fixture, "sta", "[cpu][instruction]")
{
    test_zero_page(opcode::sta_zero_page, register_id::a);
    test_zero_page_indexed(opcode::sta_zero_page_x, register_id::a, register_id::x);
    test_absolute(opcode::sta_absolute, register_id::a);
    test_absolute_indexed(opcode::sta_absolute_x, register_id::a, register_id::x);
    test_absolute_indexed(opcode::sta_absolute_y, register_id::a, register_id::y);
}

} // namespace nese::cpu::instruction
