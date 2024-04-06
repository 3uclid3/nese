#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/bitwise_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct and_fixture : bitwise_fixture
{
    // {Initial A, Value from Memory, Expected Resulting A}
    static constexpr std::array zero_scenarios = std::to_array<zero_scenario_t>(
        {{0x00, 0xFF},
         {0xFF, 0x00},
         {0x0F, 0xF0}});

    // {Initial A, Value from Memory, Expected Resulting A}
    static constexpr std::array negative_scenarios = std::to_array<negative_scenario_t>(
        {
            {0x80, 0xFF, 0x80}, // 0x80 AND anything with the MSB set results in 0x80 or more, which is negative
            {0xFF, 0x80, 0x80}, // Similar as above, operands reversed
            {0xC0, 0xF0, 0xC0}, // Both operands have their MSB set, ensuring a negative result
            {0xF0, 0xC0, 0xC0}, // Similar as above, operands reversed
            {0x80, 0x80, 0x80}, // Both operands are the same, both negative
            {0xFF, 0xC0, 0xC0}  // Memory value has bits set that align with A's MSB, ensuring negativity
        });

    // {Initial A, Value from Memory, Expected Resulting A}
    static constexpr std::array positive_scenarios = std::to_array<positive_scenario_t>(
        {
            {0x7F, 0xFF, 0x7F}, // 0x7F AND anything without affecting the MSB results in a positive value
            {0x3F, 0x7F, 0x3F}, // Both operands without MSB set, ensuring a positive result
            {0x1F, 0x2F, 0x0F}, // Both operands ensure bits do not align to set MSB
            {0x0F, 0x8F, 0x0F}, // AND with a negative operand, but result is positive due to alignment
            {0x7F, 0x3F, 0x3F}, // Memory value has no MSB set, ensuring a positive result
            {0x2F, 0x1F, 0x0F}, // Overlapping set bits, but MSB remains clear
            {0x01, 0x01, 0x01}  // Minimal positive example
        });

    static constexpr behavior_scenarios behavior_scenarios = {zero_scenarios, negative_scenarios, positive_scenarios};
};

TEST_CASE_METHOD(and_fixture, "and", "[cpu][instruction]")
{
    test_immediate(opcode::and_immediate, behavior_scenarios);
    test_zero_page(opcode::and_zero_page, behavior_scenarios);
    test_zero_page_indexed(opcode::and_zero_page_x, register_id::x, behavior_scenarios);
    test_absolute(opcode::and_absolute, behavior_scenarios);
    test_absolute_indexed(opcode::and_absolute_x, register_id::x, behavior_scenarios);
    test_absolute_indexed(opcode::and_absolute_y, register_id::y, behavior_scenarios);
}

} // namespace nese::cpu::instruction
