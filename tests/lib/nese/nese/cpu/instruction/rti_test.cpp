#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>
#include <nese/cpu/stack.hpp>
#include <nese/utility/format.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(execute_fixture, "rti", "[cpu][instruction]")
{
    constexpr cycle_t cycle_cost = cpu_cycle_t(6);

    // Clear all flags
    state().registers.p = 0;

    SECTION("addressing")
    {
        auto [pc_addr, return_addr, s] = GENERATE(table<addr_t, addr_t, u8_t>(
            {
                {0x8000, 0x4000, 0xFD}, // Test with initial SP away from wrap-around, typical usage scenario
                {0xC000, 0x1234, 0xFE}, // Test with initial SP at FE to simulate edge case of wrapping around from 0x01FF to 0x0100
                {0xFFFC, 0x0200, 0xFF}, // Test with SP at FF, demonstrating a pop operation wrapping from 0x0100 back to 0x01FF
                {0xC000, 0x1234, 0x01}, // Edge case: Test with SP at the edge of the stack page, to ensure correct wrap-around behavior
                {0xC000, 0x1234, 0x80}  // Typical case: Test with SP in the middle of the stack page, far from the wrap-around edges
            }));

        state().registers.pc = pc_addr;
        state().registers.s = s;

        const byte_t s0 = s + 1;
        const byte_t s1 = s + 2;
        const byte_t s2 = s + 3;

        memory().set_byte(stack_offset + s0, 0x00);
        memory().set_byte(stack_offset + s1, return_addr & 0xFF);
        memory().set_byte(stack_offset + s2, return_addr >> 8);

        expected_state().cycle = cycle_cost;
        expected_state().registers.pc = return_addr;
        expected_state().registers.p = static_cast<u8_t>(status_flag::unused);
        expected_state().registers.s = s + 3;

        execute_and_check(opcode::rti_implied);
    }

    SECTION("flag")
    {
        constexpr addr_t return_addr = 0x4000;

        const byte_t s0 = state().registers.s + 1;
        const byte_t s1 = state().registers.s + 2;
        const byte_t s2 = state().registers.s + 3;

        memory().set_byte(stack_offset + s1, return_addr & 0xFF);
        memory().set_byte(stack_offset + s2, return_addr >> 8);

        SECTION("set")
        {
            auto flag_set = GENERATE(
                as<status_flag>(),
                status_flag::carry,
                status_flag::zero,
                status_flag::interrupt,
                status_flag::decimal,
                status_flag::unused,
                status_flag::overflow,
                status_flag::negative);

            DYNAMIC_SECTION(format("{}", flag_set))
            {
                memory().set_byte(stack_offset + s0, static_cast<byte_t>(flag_set));

                expected_state().cycle = cycle_cost;
                expected_state().registers.pc = return_addr;
                expected_state().registers.s += 3;
                expected_state().registers.p = static_cast<byte_t>(flag_set | status_flag::unused); // unused always set

                execute_and_check(opcode::rti_implied);
            }
        }

        SECTION("ignored")
        {
            auto flag_ignored = GENERATE(
                as<status_flag>(),
                status_flag::break_cmd);

            DYNAMIC_SECTION(format("{}", flag_ignored))
            {
                state().registers.p = static_cast<byte_t>(flag_ignored);
                memory().set_byte(stack_offset + s0, 0x00);

                expected_state().cycle = cycle_cost;
                expected_state().registers.pc = return_addr;
                expected_state().registers.s += 3;
                expected_state().registers.p = static_cast<byte_t>(flag_ignored | status_flag::unused); // unused always set

                execute_and_check(opcode::rti_implied);
            }
        }
    }
}

} // namespace nese::cpu::instruction
