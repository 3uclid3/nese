#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE("get_mnemonic", "[cpu][instruction]")
{
    CHECK(get_mnemonic(opcode::adc_absolute) == "adc");
    CHECK(get_mnemonic(opcode::nop_implied) == "nop");
}

TEST_CASE("get_addr_mode_from_name", "[cpu][instruction]")
{
    CHECK(get_addr_mode_from_name(opcode::nop_implied) == addr_mode::implied);
    CHECK(get_addr_mode_from_name(opcode::asl_accumulator) == addr_mode::accumulator);
    CHECK(get_addr_mode_from_name(opcode::adc_immediate) == addr_mode::immediate);
    CHECK(get_addr_mode_from_name(opcode::adc_zero_page) == addr_mode::zero_page);
    CHECK(get_addr_mode_from_name(opcode::adc_zero_page_x) == addr_mode::zero_page_x);
    CHECK(get_addr_mode_from_name(opcode::ldx_zero_page_y) == addr_mode::zero_page_y);
    CHECK(get_addr_mode_from_name(opcode::adc_absolute) == addr_mode::absolute);
    CHECK(get_addr_mode_from_name(opcode::adc_absolute_x) == addr_mode::absolute_x);
    CHECK(get_addr_mode_from_name(opcode::adc_absolute_y) == addr_mode::absolute_y);
    CHECK(get_addr_mode_from_name(opcode::bcc_relative) == addr_mode::relative);
    CHECK(get_addr_mode_from_name(opcode::jmp_indirect) == addr_mode::indirect);
    CHECK(get_addr_mode_from_name(opcode::adc_indexed_indirect) == addr_mode::indexed_indirect);
    CHECK(get_addr_mode_from_name(opcode::adc_indirect_indexed) == addr_mode::indirect_indexed);
}

} // namespace nese::cpu::instruction
