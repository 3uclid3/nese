#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/bitwise_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct eor_fixture : bitwise_fixture
{
    // {Initial A, Value from Memory, Expected Resulting A}
    static constexpr std::array zero_scenarios = std::to_array<zero_scenario_t>(
        {
            {0x00, 0x00},
            {0x80, 0x80}
        });

    // {Initial A, Value from Memory, Expected Resulting A}
    static constexpr std::array negative_scenarios = std::to_array<std::tuple<byte_t, byte_t, byte_t>>(
        {
            {0x80, 0x00, 0x80}, // 0x80 EOR 0x00 results in 0x80
            {0x00, 0x80, 0x80}, // 0x00 EOR 0x80 results in 0x80
            {0xFF, 0x7F, 0x80}, // 0xFF EOR 0x7F results in 0x80
            {0x7F, 0xFF, 0x80}, // 0x7F EOR 0xFF results in 0x80
            {0xC0, 0x40, 0x80}, // 0xC0 EOR 0x40 results in 0x80
            {0x40, 0xC0, 0x80}, // 0x40 EOR 0xC0 results in 0x80
        });

    // {Initial A, Value from Memory, Expected Resulting A}
    static constexpr std::array positive_scenarios = std::to_array<std::tuple<byte_t, byte_t, byte_t>>(
        {
            {0x01, 0x02, 0x03}, // 0x01 EOR 0x02 results in 0x03
            {0x0F, 0x10, 0x1F}, // 0x0F EOR 0x10 results in 0x1F
            {0x20, 0x05, 0x25}, // 0x20 EOR 0x05 results in 0x25
            {0x4C, 0x33, 0x7F}, // 0x4C EOR 0x33 results in 0x7F, corrected for non-negative
            {0x22, 0x11, 0x33}, // 0x22 EOR 0x11 results in 0x33
            {0x64, 0x0A, 0x6E}, // 0x64 EOR 0x0A results in 0x6E
        });

    static constexpr behavior_scenarios behavior_scenarios = {zero_scenarios, negative_scenarios, positive_scenarios};
};

TEST_CASE_METHOD(eor_fixture, "eor", "[cpu][instruction]")
{
    test_immediate(opcode::eor_immediate, behavior_scenarios);
    test_zero_page(opcode::eor_zero_page, behavior_scenarios);
    test_zero_page_indexed(opcode::eor_zero_page_x, register_id::x, behavior_scenarios);
    test_absolute(opcode::eor_absolute, behavior_scenarios);
    test_absolute_indexed(opcode::eor_absolute_x, register_id::x, behavior_scenarios);
    test_absolute_indexed(opcode::eor_absolute_y, register_id::y, behavior_scenarios);
}

} // namespace nese::cpu::instruction
