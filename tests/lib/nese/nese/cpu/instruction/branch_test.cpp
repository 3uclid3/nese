#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/fixture.hpp>
#include <nese/cpu/status_flag.hpp>
#include <nese/utility/format.hpp>

namespace nese::cpu::instruction {

struct branch_fixture : fixture
{
    enum class branch_when
    {
        is_set = 1,
        is_clear = 0
    };

    template<typename ExecuteFunctorT>
    void test_relative(const ExecuteFunctorT& execute, status_flag flag, branch_when branch)
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

            INFO(nese::format("addr = 0x{:04X} offset = 0x{:02X} {} page_crossing", addr, offset, page_crossing ? "is" : "is not"));

            state.registers.pc = addr;
            state.owned_memory.set_byte(state.registers.pc, offset);

            DYNAMIC_SECTION(nese::format("branch taken when {} is set", to_string_view(flag)))
            {
                state.registers.set_flag(flag);

                expected_state = state;
                expected_state.registers.pc = branch == branch_when::is_set ? addr + offset + 1 : addr + 1;
                expected_state.cycle = cpu_cycle_t(branch == branch_when::is_set ? (page_crossing ? 4 : 3) : 2);

                execute(state);

                check_state();
            }

            DYNAMIC_SECTION(nese::format("branch taken when {} is clear", to_string_view(flag)))
            {
                state.registers.clear_flag(flag);

                expected_state = state;
                expected_state.registers.pc = branch == branch_when::is_clear ? addr + offset + 1 : addr + 1;
                expected_state.cycle = cpu_cycle_t(branch == branch_when::is_clear ? (page_crossing ? 4 : 3) : 2);

                execute(state);

                check_state();
            }
        }
    }
};

TEST_CASE_METHOD(branch_fixture, "bcc", "[cpu][instruction]")
{
    test_relative(execute_bcc<addr_mode::relative>, status_flag::carry, branch_when::is_clear);
}

TEST_CASE_METHOD(branch_fixture, "bcs", "[cpu][instruction]")
{
    test_relative(execute_bcs<addr_mode::relative>, status_flag::carry, branch_when::is_set);
}

TEST_CASE_METHOD(branch_fixture, "beq", "[cpu][instruction]")
{
    test_relative(execute_beq<addr_mode::relative>, status_flag::zero, branch_when::is_set);
}

TEST_CASE_METHOD(branch_fixture, "bne", "[cpu][instruction]")
{
    test_relative(execute_bne<addr_mode::relative>, status_flag::zero, branch_when::is_clear);
}

TEST_CASE_METHOD(branch_fixture, "bpl", "[cpu][instruction]")
{
    test_relative(execute_bpl<addr_mode::relative>, status_flag::negative, branch_when::is_clear);
}

TEST_CASE_METHOD(branch_fixture, "bvc", "[cpu][instruction]")
{
    test_relative(execute_bvc<addr_mode::relative>, status_flag::overflow, branch_when::is_clear);
}

TEST_CASE_METHOD(branch_fixture, "bvs", "[cpu][instruction]")
{
    test_relative(execute_bvs<addr_mode::relative>, status_flag::overflow, branch_when::is_set);
}

} // namespace nese::cpu::instruction
