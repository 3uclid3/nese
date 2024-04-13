#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

TEST_CASE("mnemonics", "[cpu][instruction]")
{
    CHECK(mnemonics[opcode::adc_absolute] == "adc");
    CHECK(mnemonics[opcode::nop_implied] == "nop");
}

TEST_CASE("addr_modes", "[cpu][instruction]")
{
    CHECK(addr_modes[opcode::nop_implied] == addr_mode::implied);
    CHECK(addr_modes[opcode::asl_accumulator] == addr_mode::accumulator);
    CHECK(addr_modes[opcode::adc_immediate] == addr_mode::immediate);
    CHECK(addr_modes[opcode::adc_zero_page] == addr_mode::zero_page);
    CHECK(addr_modes[opcode::adc_zero_page_x] == addr_mode::zero_page_x);
    CHECK(addr_modes[opcode::ldx_zero_page_y] == addr_mode::zero_page_y);
    CHECK(addr_modes[opcode::adc_absolute] == addr_mode::absolute);
    CHECK(addr_modes[opcode::adc_absolute_x] == addr_mode::absolute_x);
    CHECK(addr_modes[opcode::adc_absolute_y] == addr_mode::absolute_y);
    CHECK(addr_modes[opcode::bcc_relative] == addr_mode::relative);
    CHECK(addr_modes[opcode::jmp_indirect] == addr_mode::indirect);
    CHECK(addr_modes[opcode::adc_indexed_indirect] == addr_mode::indexed_indirect);
    CHECK(addr_modes[opcode::adc_indirect_indexed] == addr_mode::indirect_indexed);

#if NESE_UNOFFICIAL_INSTRUCTIONS_ENABLED
    CHECK(addr_modes[opcode::nop_immediate_unofficial_80] == addr_mode::immediate);
    CHECK(addr_modes[opcode::nop_zero_page_unofficial_44] == addr_mode::zero_page);
    CHECK(addr_modes[opcode::nop_absolute_unofficial_0C] == addr_mode::absolute);
#endif // NESE_UNOFFICIAL_INSTRUCTIONS_ENABLED
}

TEST_CASE("is_officials", "[cpu][instruction]")
{
    CHECK(is_officials[opcode::adc_immediate]);
    CHECK(is_officials[opcode::adc_zero_page]);
    CHECK(is_officials[opcode::adc_zero_page_x]);
    CHECK(is_officials[opcode::adc_absolute]);
    CHECK(is_officials[opcode::adc_absolute_x]);
    CHECK(is_officials[opcode::adc_absolute_y]);

#if NESE_UNOFFICIAL_INSTRUCTIONS_ENABLED
    CHECK_FALSE(is_officials[opcode::nop_immediate_unofficial_80]);
    CHECK_FALSE(is_officials[opcode::nop_zero_page_unofficial_44]);
    CHECK_FALSE(is_officials[opcode::nop_absolute_unofficial_0C]);
#endif // NESE_UNOFFICIAL_INSTRUCTIONS_ENABLED
}

} // namespace nese::cpu::instruction
