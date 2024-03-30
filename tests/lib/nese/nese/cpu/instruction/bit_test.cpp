#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/fixture.hpp>

namespace nese::cpu::instruction {

struct bit_fixture : fixture
{
    template<typename ExecuteFunctorT>
    void test_zero_page(const ExecuteFunctorT& execute)
    {
        SECTION("zero_page")
        {
            // Clear all flags
            state.registers.p = 0;

            SECTION("addressing")
            {
                const auto [pc, val_addr] = GENERATE(from_range(zero_page_scenarios));

                INFO(format("pc 0x{:04X} val_addr 0x{:02X}", pc, val_addr));

                state.registers.pc = pc;
                state.owned_memory.set_byte(pc, val_addr);

                constexpr byte_t bit = 0x01;

                state.owned_memory.set_byte(val_addr, bit);

                state.registers.a = 0xFF;

                expected_state = state;
                expected_state.registers.pc = pc + 1;

                execute(state);

                check_state();
            }

            SECTION("value")
            {
                state.registers.pc = default_pc_addr;
                state.owned_memory.set_byte(default_pc_addr, zero_page_base_addr);

                SECTION("zero")
                {
                    SECTION("set")
                    {
                        const auto [bit, a] = GENERATE(table<byte_t, byte_t>(
                            {{0x00, 0xFF},
                             {0x0F, 0xF0},
                             {0x3F, 0x40},
                             {0x1F, 0x20}}));

                        INFO(format("bit 0x{:02X} a 0x{:02X}", bit, a));

                        state.owned_memory.set_byte(zero_page_base_addr, bit);
                        state.registers.a = a;

                        expected_state = state;
                        expected_state.registers.pc = default_pc_addr + 1;
                        expected_state.registers.set_flag(status_flag::zero);

                        execute(state);

                        check_state();
                    }

                    SECTION("not set")
                    {
                        const auto [bit, a] = GENERATE(table<byte_t, byte_t>(
                            {{0x3F, 0xBF},
                             {0x1F, 0x9F}}));

                        INFO(format("bit 0x{:02X} a 0x{:02X}", bit, a));

                        state.owned_memory.set_byte(zero_page_base_addr, bit);
                        state.registers.a = a;

                        expected_state = state;
                        expected_state.registers.pc = default_pc_addr + 1;
                        expected_state.registers.clear_flag(status_flag::zero);

                        execute(state);

                        check_state();
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

                        INFO(format("bit 0x{:02X} a 0x{:02X}", bit, a));

                        state.owned_memory.set_byte(zero_page_base_addr, bit);
                        state.registers.a = a;

                        expected_state = state;
                        expected_state.registers.pc = default_pc_addr + 1;
                        expected_state.registers.set_flag(status_flag::overflow);

                        execute(state);

                        check_state();
                    }

                    SECTION("not set")
                    {
                        const auto [bit, a] = GENERATE(table<byte_t, byte_t>(
                            {{0x01, 0x01},
                             {0x02, 0x03},
                             {0x08, 0x0C},
                             {0x10, 0x10}}));

                        INFO(format("bit 0x{:02X} a 0x{:02X}", bit, a));

                        state.owned_memory.set_byte(zero_page_base_addr, bit);
                        state.registers.a = a;

                        expected_state = state;
                        expected_state.registers.pc = default_pc_addr + 1;
                        expected_state.registers.clear_flag(status_flag::overflow);

                        execute(state);

                        check_state();
                    }
                }

                SECTION("negative")
                {
                    SECTION("set")
                    {
                        const auto [bit, a] = GENERATE(table<byte_t, byte_t>(
                            {
                                {0x80, 0xFF},
                                {0x81, 0xFF},
                                {0x80, 0x80},
                                {0x82, 0xBF} 
                            }));

                        INFO(format("bit 0x{:02X} a 0x{:02X}", bit, a));

                        state.owned_memory.set_byte(zero_page_base_addr, bit);
                        state.registers.a = a;

                        expected_state = state;
                        expected_state.registers.pc = default_pc_addr + 1;
                        expected_state.registers.set_flag(status_flag::negative);

                        execute(state);

                        check_state();
                    }

                    SECTION("not set")
                    {
                        const auto [bit, a] = GENERATE(table<byte_t, byte_t>(
                            {{0x3F, 0xFF},
                             {0x20, 0xEF},
                             {0x1F, 0xF7}}));

                        INFO(format("bit 0x{:02X} a 0x{:02X}", bit, a));

                        state.owned_memory.set_byte(zero_page_base_addr, bit);
                        state.registers.a = a;

                        expected_state = state;
                        expected_state.registers.pc = default_pc_addr + 1;
                        expected_state.registers.clear_flag(status_flag::negative);

                        execute(state);

                        check_state();
                    }
                }
            }
        }
    }
};

TEST_CASE_METHOD(bit_fixture, "bit", "[cpu][instruction]")
{
    test_zero_page(execute_bit<addr_mode::zero_page>);
}

} // namespace nese::cpu::instruction
