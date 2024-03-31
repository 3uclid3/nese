#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/store_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(store_fixture, "sty", "[cpu][instruction]")
{
    test_zero_page(opcode::sty_zero_page, register_id::y);
    test_zero_page_indexed(opcode::sty_zero_page_x, register_id::y, register_id::x);
    test_absolute(opcode::sty_absolute, register_id::y);
}

} // namespace nese::cpu::instruction
