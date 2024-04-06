#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>
#include <nese/cpu/status_flag.hpp>
#include <nese/utility/format.hpp>

namespace nese::cpu::instruction {

struct and_fixture : execute_fixture
{
    // {Initial A, Value from Memory, Expected Resulting A}
    static constexpr std::array zero_scenarios = std::to_array<std::tuple<byte_t, byte_t>>(
        {{0x00, 0xFF},
         {0xFF, 0x00},
         {0x0F, 0xF0}});

    // {Initial A, Value from Memory, Expected Resulting A}
    static constexpr std::array negative_scenarios = std::to_array<std::tuple<byte_t, byte_t, byte_t>>(
        {
            {0x80, 0xFF, 0x80}, // 0x80 AND anything with the MSB set results in 0x80 or more, which is negative
            {0xFF, 0x80, 0x80}, // Similar as above, operands reversed
            {0xC0, 0xF0, 0xC0}, // Both operands have their MSB set, ensuring a negative result
            {0xF0, 0xC0, 0xC0}, // Similar as above, operands reversed
            {0x80, 0x80, 0x80}, // Both operands are the same, both negative
            {0xFF, 0xC0, 0xC0}  // Memory value has bits set that align with A's MSB, ensuring negativity
        });

    // {Initial A, Value from Memory, Expected Resulting A}
    static constexpr std::array positive_scenarios = std::to_array<std::tuple<byte_t, byte_t, byte_t>>(
        {
            {0x7F, 0xFF, 0x7F}, // 0x7F AND anything without affecting the MSB results in a positive value
            {0x3F, 0x7F, 0x3F}, // Both operands without MSB set, ensuring a positive result
            {0x1F, 0x2F, 0x0F}, // Both operands ensure bits do not align to set MSB
            {0x0F, 0x8F, 0x0F}, // AND with a negative operand, but result is positive due to alignment
            {0x7F, 0x3F, 0x3F}, // Memory value has no MSB set, ensuring a positive result
            {0x2F, 0x1F, 0x0F}, // Overlapping set bits, but MSB remains clear
            {0x01, 0x01, 0x01}  // Minimal positive example
        });

    void test_immediate(opcode opcode)
    {
        SECTION("immediate")
        {
            constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(2);

            SECTION("addressing")
            {
                const addr_t addr = GENERATE(as<addr_t>(), 0x0000, 0x7FFE, 0xFFFE);

                state().registers.pc = addr;
                state().registers.a = 1;
                memory().set_byte(addr, 1);

                expected_state().cycle = cycle_cost;
                expected_state().registers.pc = addr + 1;

                execute_and_check(opcode);
            }

            SECTION("behavior")
            {
                state().registers.pc = default_pc_addr;

                SECTION("result in zero")
                {
                    auto [a, value] = GENERATE(from_range(zero_scenarios));

                    state().registers.a = a;
                    memory().set_byte(default_pc_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.a = 0x00;
                    expected_state().registers.pc = default_pc_addr + 1;
                    expected_state().registers.set_flag(status_flag::zero, true);
                    expected_state().registers.set_flag(status_flag::negative, false);

                    execute_and_check(opcode);
                }

                SECTION("result in negative")
                {
                    auto [a, value, expected_a] = GENERATE(from_range(negative_scenarios));

                    state().registers.a = a;
                    memory().set_byte(default_pc_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.a = expected_a;
                    expected_state().registers.pc = default_pc_addr + 1;
                    expected_state().registers.set_flag(status_flag::zero, false);
                    expected_state().registers.set_flag(status_flag::negative, true);

                    execute_and_check(opcode);
                }

                SECTION("result in positive")
                {
                    auto [a, value, expected_a] = GENERATE(from_range(positive_scenarios));

                    state().registers.a = a;
                    memory().set_byte(default_pc_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.a = expected_a;
                    expected_state().registers.pc = default_pc_addr + 1;
                    expected_state().registers.set_flag(status_flag::zero, false);
                    expected_state().registers.set_flag(status_flag::negative, false);

                    execute_and_check(opcode);
                }
            }
        }
    }

    void test_zero_page(opcode opcode)
    {
        SECTION("zero_page")
        {
            constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(3);

            SECTION("addressing")
            {
                const auto [pc_addr, base_addr] = GENERATE(from_range(zero_page_scenarios));

                state().registers.a = 1;
                state().registers.pc = pc_addr;
                memory().set_byte(pc_addr, base_addr);
                memory().set_byte(base_addr, 1);

                expected_state().cycle = cycle_cost;
                expected_state().registers.pc = pc_addr + 1;

                execute_and_check(opcode);
            }

            SECTION("behavior")
            {
                state().registers.pc = default_pc_addr;
                memory().set_byte(default_pc_addr, zero_page_base_addr);

                SECTION("result in zero")
                {
                    auto [a, value] = GENERATE(from_range(zero_scenarios));

                    state().registers.a = a;
                    memory().set_byte(zero_page_base_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.a = 0x00;
                    expected_state().registers.pc = default_pc_addr + 1;
                    expected_state().registers.set_flag(status_flag::zero, true);
                    expected_state().registers.set_flag(status_flag::negative, false);

                    execute_and_check(opcode);
                }

                SECTION("result in negative")
                {
                    auto [a, value, expected_a] = GENERATE(from_range(negative_scenarios));

                    state().registers.a = a;
                    memory().set_byte(zero_page_base_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.a = expected_a;
                    expected_state().registers.pc = default_pc_addr + 1;
                    expected_state().registers.set_flag(status_flag::zero, false);
                    expected_state().registers.set_flag(status_flag::negative, true);

                    execute_and_check(opcode);
                }

                SECTION("result in positive")
                {
                    auto [a, value, expected_a] = GENERATE(from_range(positive_scenarios));

                    state().registers.a = a;
                    memory().set_byte(zero_page_base_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.a = expected_a;
                    expected_state().registers.pc = default_pc_addr + 1;
                    expected_state().registers.set_flag(status_flag::zero, false);
                    expected_state().registers.set_flag(status_flag::negative, false);

                    execute_and_check(opcode);
                }
            }
        }
    }

    void test_zero_page_indexed(opcode opcode, register_id index_register)
    {
        DYNAMIC_SECTION(format("zero_page_{}", index_register))
        {
            constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(4);

            SECTION("addressing")
            {
                const auto [pc_addr, base_addr, idx] = GENERATE(from_range(zero_page_indexed_scenarios));
                const byte_t indexed_addr = base_addr + idx & 0xff;

                state().registers.a = 1;
                state().registers.pc = pc_addr;
                memory().set_byte(pc_addr, base_addr);
                memory().set_byte(indexed_addr, 1);
                set_register(state().registers, index_register, idx);

                expected_state().cycle = cycle_cost;
                expected_state().registers.pc = pc_addr + 1;

                execute_and_check(opcode);
            }

            SECTION("behavior")
            {
                constexpr byte_t indexed_addr = zero_page_base_addr + indexed_offset;

                state().registers.pc = default_pc_addr;
                memory().set_byte(default_pc_addr, zero_page_base_addr);
                set_register(state().registers, index_register, indexed_offset);

                SECTION("result in zero")
                {
                    auto [a, value] = GENERATE(from_range(zero_scenarios));

                    state().registers.a = a;
                    memory().set_byte(indexed_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.a = 0x00;
                    expected_state().registers.pc = default_pc_addr + 1;
                    expected_state().registers.set_flag(status_flag::zero, true);
                    expected_state().registers.set_flag(status_flag::negative, false);

                    execute_and_check(opcode);
                }

                SECTION("result in negative")
                {
                    auto [a, value, expected_a] = GENERATE(from_range(negative_scenarios));

                    state().registers.a = a;
                    memory().set_byte(indexed_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.a = expected_a;
                    expected_state().registers.pc = default_pc_addr + 1;
                    expected_state().registers.set_flag(status_flag::zero, false);
                    expected_state().registers.set_flag(status_flag::negative, true);

                    execute_and_check(opcode);
                }

                SECTION("result in positive")
                {
                    auto [a, value, expected_a] = GENERATE(from_range(positive_scenarios));

                    state().registers.a = a;
                    memory().set_byte(indexed_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.a = expected_a;
                    expected_state().registers.pc = default_pc_addr + 1;
                    expected_state().registers.set_flag(status_flag::zero, false);
                    expected_state().registers.set_flag(status_flag::negative, false);

                    execute_and_check(opcode);
                }
            }
        }
    }

    void test_absolute(opcode opcode)
    {
        SECTION("absolute")
        {
            constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(4);

            SECTION("addressing")
            {
                const auto [pc_addr, base_addr] = GENERATE(from_range(absolute_scenarios));

                state().registers.a = 1;
                state().registers.pc = pc_addr;
                memory().set_word(pc_addr, base_addr);
                memory().set_byte(base_addr, 1);

                expected_state().cycle = cycle_cost;
                expected_state().registers.pc = pc_addr + 2;

                execute_and_check(opcode);
            }

            SECTION("behavior")
            {
                state().registers.pc = default_pc_addr;
                memory().set_word(default_pc_addr, absolute_base_addr);

                SECTION("result in zero")
                {
                    auto [a, value] = GENERATE(from_range(zero_scenarios));

                    state().registers.a = a;
                    memory().set_byte(absolute_base_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.a = 0x00;
                    expected_state().registers.pc = default_pc_addr + 2;
                    expected_state().registers.set_flag(status_flag::zero, true);
                    expected_state().registers.set_flag(status_flag::negative, false);

                    execute_and_check(opcode);
                }

                SECTION("result in negative")
                {
                    auto [a, value, expected_a] = GENERATE(from_range(negative_scenarios));

                    state().registers.a = a;
                    memory().set_byte(absolute_base_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.a = expected_a;
                    expected_state().registers.pc = default_pc_addr + 2;
                    expected_state().registers.set_flag(status_flag::zero, false);
                    expected_state().registers.set_flag(status_flag::negative, true);

                    execute_and_check(opcode);
                }

                SECTION("result in positive")
                {
                    auto [a, value, expected_a] = GENERATE(from_range(positive_scenarios));

                    state().registers.a = a;
                    memory().set_byte(absolute_base_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.a = expected_a;
                    expected_state().registers.pc = default_pc_addr + 2;
                    expected_state().registers.set_flag(status_flag::zero, false);
                    expected_state().registers.set_flag(status_flag::negative, false);

                    execute_and_check(opcode);
                }
            }
        }
    }

    void test_absolute_indexed(opcode opcode, register_id index_register)
    {
        DYNAMIC_SECTION(format("absolute_{}", index_register))
        {
            SECTION("addressing")
            {
                const auto [pc_addr, base_addr, idx] = GENERATE(from_range(absolute_indexed_scenarios));
                const addr_t indexed_addr = base_addr + idx;

                state().registers.a = 1;
                state().registers.pc = pc_addr;
                memory().set_word(pc_addr, base_addr);
                memory().set_byte(indexed_addr, 1);
                set_register(state().registers, index_register, idx);

                // expected_state().cycle = cycle_cost;
                expected_state().registers.pc = pc_addr + 2;

                execute_and_check(opcode, false);
            }

            SECTION("behavior")
            {
                constexpr addr_t indexed_addr = absolute_base_addr + indexed_offset;

                state().registers.pc = default_pc_addr;
                memory().set_word(default_pc_addr, absolute_base_addr);
                set_register(state().registers, index_register, indexed_offset);

                SECTION("result in zero")
                {
                    auto [a, value] = GENERATE(from_range(zero_scenarios));

                    state().registers.a = a;
                    memory().set_byte(indexed_addr, value);

                    // expected_state().cycle = cycle_cost;
                    expected_state().registers.a = 0x00;
                    expected_state().registers.pc = default_pc_addr + 2;
                    expected_state().registers.set_flag(status_flag::zero, true);
                    expected_state().registers.set_flag(status_flag::negative, false);

                    execute_and_check(opcode, false);
                }

                SECTION("result in negative")
                {
                    auto [a, value, expected_a] = GENERATE(from_range(negative_scenarios));

                    state().registers.a = a;
                    memory().set_byte(indexed_addr, value);

                    // expected_state().cycle = cycle_cost;
                    expected_state().registers.a = expected_a;
                    expected_state().registers.pc = default_pc_addr + 2;
                    expected_state().registers.set_flag(status_flag::zero, false);
                    expected_state().registers.set_flag(status_flag::negative, true);

                    execute_and_check(opcode, false);
                }

                SECTION("result in positive")
                {
                    auto [a, value, expected_a] = GENERATE(from_range(positive_scenarios));

                    state().registers.a = a;
                    memory().set_byte(indexed_addr, value);

                    // expected_state().cycle = cycle_cost;
                    expected_state().registers.a = expected_a;
                    expected_state().registers.pc = default_pc_addr + 2;
                    expected_state().registers.set_flag(status_flag::zero, false);
                    expected_state().registers.set_flag(status_flag::negative, false);

                    execute_and_check(opcode, false);
                }
            }
        }
    }
};

TEST_CASE_METHOD(and_fixture, "and", "[cpu][instruction]")
{
    test_immediate(opcode::and_immediate);
    test_zero_page(opcode::and_zero_page);
    test_zero_page_indexed(opcode::and_zero_page_x, register_id::x);
    test_absolute(opcode::and_absolute);
    test_absolute_indexed(opcode::and_absolute_x, register_id::x);
    test_absolute_indexed(opcode::and_absolute_y, register_id::y);
}

} // namespace nese::cpu::instruction
