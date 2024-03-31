#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>
#include <nese/cpu/stack.hpp>

namespace nese::cpu::instruction {

TEST_CASE_METHOD(execute_fixture, "rts", "[cpu][instruction]")
{
    constexpr cycle_t cycle_cost = cpu_cycle_t(6);

    auto [pc_addr, return_addr, s] = GENERATE(table<addr_t, addr_t, u8_t>(
        {// Test with initial SP away from wrap-around, typical usage scenario
         {0x8000, 0x4000, 0xFD},

         // Test with initial SP at FE to simulate edge case of wrapping around from 0x01FF to 0x0100
         {0xC000, 0x1234, 0xFE},

         // Test with SP at FF, demonstrating a pop operation wrapping from 0x0100 back to 0x01FF
         {0xFFFC, 0x0200, 0xFF},

         // Edge case: Test with SP at the edge of the stack page, to ensure correct wrap-around behavior
         {0xC000, 0x1234, 0x01},

         // Typical case: Test with SP in the middle of the stack page, far from the wrap-around edges
         {0xC000, 0x1234, 0x80}}));

    state().registers.pc = pc_addr;
    state().registers.s = s;

    const byte_t s0 = s + 1;
    const byte_t s1 = s + 2;

    memory().set_byte(stack_offset + s0, return_addr & 0xFF);
    memory().set_byte(stack_offset + s1, return_addr >> 8);

    expected_state().cycle = cycle_cost;
    expected_state().registers.pc = return_addr + 1;
    expected_state().registers.s = s + 2;

    execute_and_check(opcode::rts_implied);
}

} // namespace nese::cpu::instruction
