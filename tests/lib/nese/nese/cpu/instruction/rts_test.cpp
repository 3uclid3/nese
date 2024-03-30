#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/fixture.hpp>

namespace nese::cpu::instruction {

struct rts_fixture : fixture
{
    template<typename ExecuteFunctorT>
    void test_implied(const ExecuteFunctorT& execute)
    {
        constexpr cycle_t cycle_cost = cpu_cycle_t(6);

        SECTION("implied")
        {
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

            state.registers.pc = pc_addr;
            state.registers.s = s;

            const byte_t s0 = s + 1;
            const byte_t s1 = s + 2;

            state.memory.get().set_byte(stack_offset + s0, return_addr & 0xFF);
            state.memory.get().set_byte(stack_offset + s1, return_addr >> 8);

            expected_state = state;
            expected_state.cycle = cycle_cost;
            expected_state.registers.pc = return_addr + 1;
            expected_state.registers.s = s + 2;

            execute(state);

            check_state();
        }
    }
};

TEST_CASE_METHOD(rts_fixture, "rts", "[cpu][instruction]")
{
    test_implied(execute_rts<addr_mode::implied>);
}

} // namespace nese::cpu::instruction
