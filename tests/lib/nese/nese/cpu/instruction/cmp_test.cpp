#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction/fixture/compare_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(compare_fixture, "cmp", "[cpu][instruction]")
{
    test_immediate(opcode::cmp_immediate, register_id::a);
    test_zero_page(opcode::cmp_zero_page, register_id::a);
    test_zero_page_indexed(opcode::cmp_zero_page_x, register_id::a, register_id::x);
    test_absolute(opcode::cmp_absolute, register_id::a);
    test_absolute_indexed(opcode::cmp_absolute_x, register_id::a, register_id::x);
    test_absolute_indexed(opcode::cmp_absolute_y, register_id::a, register_id::y);
}

} // namespace nese::cpu::instruction
