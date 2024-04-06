#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/fixture/bitwise_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>

namespace nese::cpu::instruction {

struct ora_fixture : bitwise_fixture
{
    // {Initial A, Value from Memory, Expected Resulting A}
    static constexpr std::array zero_scenarios = std::to_array<zero_scenario_t>(
        {{0x00, 0x00}});

    // {Initial A, Value from Memory, Expected Resulting A}
    static constexpr std::array negative_scenarios = std::to_array<std::tuple<byte_t, byte_t, byte_t>>(
        {
            {0x80, 0x01, 0x81},
            {0x01, 0x80, 0x81},
            {0x80, 0x80, 0x80},
            {0xFF, 0x00, 0xFF},
            {0x00, 0xFF, 0xFF},
            {0xC0, 0x3F, 0xFF},
            {0x3F, 0xC0, 0xFF}
        });

    // {Initial A, Value from Memory, Expected Resulting A}
    static constexpr std::array positive_scenarios = std::to_array<std::tuple<byte_t, byte_t, byte_t>>(
        {
            {0x7F, 0x00, 0x7F},
            {0x00, 0x7F, 0x7F},
            {0x3F, 0x40, 0x7F},
            {0x40, 0x3F, 0x7F},
            {0x1F, 0x20, 0x3F},
            {0x20, 0x1F, 0x3F}
        });

    static constexpr behavior_scenarios behavior_scenarios = {zero_scenarios, negative_scenarios, positive_scenarios};
};

TEST_CASE_METHOD(ora_fixture, "ora", "[cpu][instruction]")
{
    test_immediate(opcode::ora_immediate, behavior_scenarios);
    test_zero_page(opcode::ora_zero_page, behavior_scenarios);
    test_zero_page_indexed(opcode::ora_zero_page_x, register_id::x, behavior_scenarios);
    test_absolute(opcode::ora_absolute, behavior_scenarios);
    test_absolute_indexed(opcode::ora_absolute_x, register_id::x, behavior_scenarios);
    test_absolute_indexed(opcode::ora_absolute_y, register_id::y, behavior_scenarios);
}

} // namespace nese::cpu::instruction
