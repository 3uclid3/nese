#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

namespace nese::cpu::instruction {

struct bit_fixture : execute_fixture
{
    void test_zero_page(opcode opcode)
    {
        SECTION("zero_page")
        {
            constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(3);

            // Clear all flags
            state().registers.p = 0;

            SECTION("addressing")
            {
                const auto [pc, val_addr] = GENERATE(from_range(zero_page_scenarios));

                state().registers.pc = pc;
                state().registers.a = 0xFF;
                memory().set_byte(pc, val_addr);
                memory().set_byte(val_addr, 0x01);

                expected_state().cycle = cycle_cost;
                expected_state().registers.pc = pc + 1;

                execute_and_check(opcode, false);
            }

            SECTION("value")
            {
                state().registers.pc = default_pc_addr;
                memory().set_byte(default_pc_addr, zero_page_base_addr);

                SECTION("zero")
                {
                    SECTION("set")
                    {
                        const auto [bit, a] = GENERATE(table<byte_t, byte_t>(
                            {{0x00, 0xFF},
                             {0x0F, 0xF0},
                             {0x3F, 0x40},
                             {0x1F, 0x20}}));

                        memory().set_byte(zero_page_base_addr, bit);
                        state().registers.a = a;

                        expected_state().cycle = cycle_cost;
                        expected_state().registers.pc = default_pc_addr + 1;
                        expected_state().registers.set_flag(status_flag::zero);

                        execute_and_check(opcode, false);
                    }

                    SECTION("not set")
                    {
                        const auto [bit, a] = GENERATE(table<byte_t, byte_t>(
                            {{0x3F, 0xBF},
                             {0x1F, 0x9F}}));

                        memory().set_byte(zero_page_base_addr, bit);
                        state().registers.a = a;

                        expected_state().cycle = cycle_cost;
                        expected_state().registers.pc = default_pc_addr + 1;
                        expected_state().registers.clear_flag(status_flag::zero);

                        execute_and_check(opcode, false);
                    }
                }

                SECTION("overflow")
                {
                    SECTION("set")
                    {
                        const auto [bit, a] = GENERATE(table<byte_t, byte_t>(
                            {{0x40, 0xFF},
                             {0x41, 0xFF},
                             {0x40, 0x7F},
                             {0x42, 0xBF}}));

                        memory().set_byte(zero_page_base_addr, bit);
                        state().registers.a = a;

                        expected_state().cycle = cycle_cost;
                        expected_state().registers.pc = default_pc_addr + 1;
                        expected_state().registers.set_flag(status_flag::overflow);

                        execute_and_check(opcode, false);
                    }

                    SECTION("not set")
                    {
                        const auto [bit, a] = GENERATE(table<byte_t, byte_t>(
                            {{0x01, 0x01},
                             {0x02, 0x03},
                             {0x08, 0x0C},
                             {0x10, 0x10}}));

                        memory().set_byte(zero_page_base_addr, bit);
                        state().registers.a = a;

                        expected_state().cycle = cycle_cost;
                        expected_state().registers.pc = default_pc_addr + 1;
                        expected_state().registers.clear_flag(status_flag::overflow);

                        execute_and_check(opcode, false);
                    }
                }

                SECTION("negative")
                {
                    SECTION("set")
                    {
                        const auto [bit, a] = GENERATE(table<byte_t, byte_t>(
                            {{0x80, 0xFF},
                             {0x81, 0xFF},
                             {0x80, 0x80},
                             {0x82, 0xBF}}));

                        memory().set_byte(zero_page_base_addr, bit);
                        state().registers.a = a;

                        expected_state().cycle = cycle_cost;
                        expected_state().registers.pc = default_pc_addr + 1;
                        expected_state().registers.set_flag(status_flag::negative);

                        execute_and_check(opcode, false);
                    }

                    SECTION("not set")
                    {
                        const auto [bit, a] = GENERATE(table<byte_t, byte_t>(
                            {{0x3F, 0xFF},
                             {0x20, 0xEF},
                             {0x1F, 0xF7}}));

                        memory().set_byte(zero_page_base_addr, bit);
                        state().registers.a = a;

                        expected_state().cycle = cycle_cost;
                        expected_state().registers.pc = default_pc_addr + 1;
                        expected_state().registers.clear_flag(status_flag::negative);

                        execute_and_check(opcode, false);
                    }
                }
            }
        }
    }
};

TEST_CASE_METHOD(bit_fixture, "bit", "[cpu][instruction]")
{
    test_zero_page(opcode::bit_zero_page);
}

} // namespace nese::cpu::instruction
