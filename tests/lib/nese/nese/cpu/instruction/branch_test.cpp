#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/fixture.hpp>
#include <nese/utility/format.hpp>

namespace nese::cpu::instruction {

struct branch_fixture : fixture
{
    template<typename ExecuteFunctorT>
    void test_relative(const ExecuteFunctorT& execute)
    {
        SECTION("relative")
        {
            auto [addr, offset, page_crossing] = GENERATE(table<addr_t, byte_t, bool>(
                {
                    // No page crossing examples
                    {0x0000, 0x10, false}, // Stays within the first page
                    {0x0080, 0x7E, false}, // Near the middle of the first page, large offset but no crossing

                    // These entries need correction based on the understanding of page crossing
                    {0x00F0, 0x0F, true}, // Crosses from 0x00xx to 0x01xx page

                    // Page crossing examples
                    {0x00FE, 0x01, true}, // Crosses from 0x00xx to 0x01xx page
                    {0x01FD, 0x02, true}, // Crosses from 0x01xx to 0x02xx page
                    {0x0FFF, 0x01, true}  // Crosses from 0x0Fxx to 0x10xx page
                }));

            INFO(format("addr = 0x{:04X} offset = 0x{:02X} {} page_crossing", addr, offset, page_crossing ? "is" : "is not"));

            state.registers.pc = addr;
            state.owned_memory.set_byte(state.registers.pc, offset);

            SECTION("branch taken when carry is set")
            {
                state.registers.set_flag(status_flag::carry);

                state_mock expected_state = state;
                expected_state.registers.pc = addr + offset + 1;
                expected_state.cycle = cpu_cycle_t(page_crossing ? 4 : 3);

                execute(state);

                check_state(expected_state);
            }

            SECTION("branch not taken when carry is clear")
            {
                state.registers.unset_flag(status_flag::carry);

                state_mock expected_state = state;
                expected_state.registers.pc = addr + 1;
                expected_state.cycle = cpu_cycle_t(2);

                execute(state);

                check_state(expected_state);
            }
        }
    }
};

TEST_CASE_METHOD(branch_fixture, "bcs", "[cpu][instruction]")
{
    test_relative(execute_bcs<addr_mode::relative>);
}

} // namespace nese::cpu::instruction
