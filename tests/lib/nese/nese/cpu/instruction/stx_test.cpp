#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/store_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(store_fixture, "stx", "[cpu][instruction]")
{
    test_zero_page(opcode::stx_zero_page, register_id::x);
    test_zero_page_indexed(opcode::stx_zero_page_y, register_id::x, register_id::y);
    test_absolute(opcode::stx_absolute, register_id::x);
}

} // namespace nese::cpu::instruction
