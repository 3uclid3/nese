#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction/fixture/execute_fixture.hpp>
#include <nese/cpu/instruction/opcode.hpp>
#include <nese/cpu/status_flag.hpp>
#include <nese/utility/format.hpp>
#include <nese/utility/hex.hpp>

namespace nese::cpu::instruction {

struct sbc_fixture : execute_fixture
{
    static constexpr std::array carry_flag_clear_scenarios = std::to_array<std::tuple<byte_x, byte_x, enum_hex<status_flags>>>(
        {
            {0x00, 0x00, status_flag::negative},                                      // 0 - 0 - 1 = -1 (0xFF), negative flag set
            {0x01, 0x00, status_flag::carry | status_flag::zero},                         // 1 - 0 - 1 = 0, carry flag set because 0 is "borrowed" from the carry
            {0x02, 0x01, status_flag::carry | status_flag::zero},                         // 2 - 1 - 1 = 0, no flags set
            {0x00, 0x01, status_flag::negative},                                      // 0 - 1 - 1 = -2 (0xFE), negative flag set
            {0x80, 0x7F, status_flag::zero | status_flag::carry | status_flag::overflow}, // 0x80 - 0x7F - 1 = 0, zero and carry flag set
            {0x80, 0x80, status_flag::negative},                                      // 0x80 - 0x80 - 1 = -1 (0xFF), negative flag set, carry flag remains clear due to the underflow

        });

    static constexpr std::array carry_flag_set_scenarios = std::to_array<std::tuple<byte_x, byte_x, enum_hex<status_flags>>>(
        {
            // {Initial A, Value from Memory, Expected Flags}
            {0x00, 0x00, status_flag::carry | status_flag::zero},
            {0x01, 0x01, status_flag::carry | status_flag::zero},
            {0x02, 0x01, status_flag::carry},
            {0x7F, 0x01, status_flag::carry},
            {0x80, 0x7F, status_flag::carry | status_flag::overflow},
            {0x80, 0x80, status_flag::carry | status_flag::zero},
            {0x7E, 0x7F, status_flag::negative},
            {0xFF, 0x7E, status_flag::carry | status_flag::negative},
            {0x00, 0xFF, status_flag::none},
        });

    void test_immediate(opcode opcode)
    {
        SECTION("immediate")
        {
            constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(2);

            SECTION("addressing")
            {
                const auto addr = GENERATE(as<addr_x>(), 0x0000, 0x7FFE, 0xFFFE);

                CAPTURE(addr);

                state().registers.pc = addr;
                state().registers.a = 0;
                state().registers.set_flag(status_flag::carry);
                memory().set_byte(addr, 0);

                expected_state().cycle = cycle_cost;
                expected_state().registers.pc = addr + 1;
                expected_state().registers.set_flag(status_flag::zero, true);

                execute_and_check(opcode);
            }

            SECTION("behavior")
            {
                state().registers.pc = default_pc_addr;
                state().registers.p = 0;

                SECTION("carry flag clear")
                {
                    auto [a, value, flags] = GENERATE(from_range(carry_flag_clear_scenarios));

                    CAPTURE(a);
                    CAPTURE(value);
                    CAPTURE(flags);

                    state().registers.a = a;
                    memory().set_byte(default_pc_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.pc = default_pc_addr + 1;
                    expected_state().registers.a = a - value - 1;
                    expected_state().registers.p = flags;

                    execute_and_check(opcode);
                }

                SECTION("carry flag set")
                {
                    state().registers.set_flag(status_flag::carry);

                    auto [a, value, flags] = GENERATE(from_range(carry_flag_set_scenarios));

                    CAPTURE(a);
                    CAPTURE(value);
                    CAPTURE(flags);

                    state().registers.a = a;
                    memory().set_byte(default_pc_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.pc = default_pc_addr + 1;
                    expected_state().registers.a = a - value;
                    expected_state().registers.p = flags;

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
                const auto [pc_addr, base_addr] = GENERATE_COPY(from_range(zero_page_scenarios));

                CAPTURE(pc_addr);
                CAPTURE(base_addr);

                state().registers.pc = pc_addr;
                state().registers.a = 0;
                state().registers.set_flag(status_flag::carry);
                memory().set_byte(pc_addr, base_addr);
                memory().set_byte(base_addr, 0);

                expected_state().cycle = cycle_cost;
                expected_state().registers.pc = pc_addr + 1;
                expected_state().registers.set_flag(status_flag::zero, true);

                execute_and_check(opcode);
            }

            SECTION("behavior")
            {
                state().registers.p = 0;
                state().registers.pc = default_pc_addr;
                memory().set_byte(default_pc_addr, zero_page_base_addr);

                SECTION("carry flag clear")
                {
                    auto [a, value, flags] = GENERATE(from_range(carry_flag_clear_scenarios));

                    CAPTURE(a);
                    CAPTURE(value);
                    CAPTURE(flags);

                    state().registers.a = a;
                    memory().set_byte(zero_page_base_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.pc = default_pc_addr + 1;
                    expected_state().registers.a = a - value - 1;
                    expected_state().registers.p = flags;

                    execute_and_check(opcode);
                }

                SECTION("carry flag set")
                {
                    state().registers.set_flag(status_flag::carry);

                    auto [a, value, flags] = GENERATE(from_range(carry_flag_set_scenarios));

                    CAPTURE(a);
                    CAPTURE(value);
                    CAPTURE(flags);

                    state().registers.a = a;
                    memory().set_byte(zero_page_base_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.pc = default_pc_addr + 1;
                    expected_state().registers.a = a - value;
                    expected_state().registers.p = flags;

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

                CAPTURE(pc_addr);
                CAPTURE(base_addr);
                CAPTURE(idx);
                CAPTURE(indexed_addr);

                state().registers.a = 0;
                state().registers.pc = pc_addr;
                state().registers.set_flag(status_flag::carry);
                memory().set_byte(pc_addr, base_addr);
                memory().set_byte(indexed_addr, 0);
                set_register(state().registers, index_register, idx);

                expected_state().cycle = cycle_cost;
                expected_state().registers.pc = pc_addr + 1;
                expected_state().registers.set_flag(status_flag::zero, true);

                execute_and_check(opcode);
            }

            SECTION("behavior")
            {
                constexpr byte_t indexed_addr = zero_page_base_addr + indexed_offset;

                state().registers.p = 0;
                state().registers.pc = default_pc_addr;
                memory().set_byte(default_pc_addr, zero_page_base_addr);
                set_register(state().registers, index_register, indexed_offset);

                SECTION("carry flag clear")
                {
                    auto [a, value, flags] = GENERATE(from_range(carry_flag_clear_scenarios));

                    CAPTURE(a);
                    CAPTURE(value);
                    CAPTURE(flags);

                    state().registers.a = a;
                    memory().set_byte(indexed_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.pc = default_pc_addr + 1;
                    expected_state().registers.a = a - value - 1;
                    expected_state().registers.p = flags;

                    execute_and_check(opcode);
                }

                SECTION("carry flag set")
                {
                    state().registers.set_flag(status_flag::carry);

                    auto [a, value, flags] = GENERATE(from_range(carry_flag_set_scenarios));

                    CAPTURE(a);
                    CAPTURE(value);
                    CAPTURE(flags);

                    CHECK(true);

                    state().registers.a = a;
                    memory().set_byte(indexed_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.pc = default_pc_addr + 1;
                    expected_state().registers.a = a - value;
                    expected_state().registers.p = flags;

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
                const auto [pc_addr, base_addr] = GENERATE_COPY(from_range(absolute_scenarios));

                CAPTURE(pc_addr);
                CAPTURE(base_addr);

                state().registers.pc = pc_addr;
                state().registers.a = 0;
                state().registers.set_flag(status_flag::carry);
                memory().set_word(pc_addr, base_addr);
                memory().set_byte(base_addr, 0);

                expected_state().cycle = cycle_cost;
                expected_state().registers.pc = pc_addr + 2;
                expected_state().registers.set_flag(status_flag::zero, true);

                execute_and_check(opcode);
            }

            SECTION("behavior")
            {
                state().registers.p = 0;
                state().registers.pc = default_pc_addr;
                memory().set_word(default_pc_addr, absolute_base_addr);

                SECTION("carry flag clear")
                {
                    auto [a, value, flags] = GENERATE(from_range(carry_flag_clear_scenarios));

                    CAPTURE(a);
                    CAPTURE(value);
                    CAPTURE(flags);

                    state().registers.a = a;
                    memory().set_byte(absolute_base_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.pc = default_pc_addr + 2;
                    expected_state().registers.a = a - value - 1;
                    expected_state().registers.p = flags;

                    execute_and_check(opcode);
                }

                SECTION("carry flag set")
                {
                    state().registers.set_flag(status_flag::carry);

                    auto [a, value, flags] = GENERATE(from_range(carry_flag_set_scenarios));

                    CAPTURE(a);
                    CAPTURE(value);
                    CAPTURE(flags);

                    state().registers.a = a;
                    memory().set_byte(absolute_base_addr, value);

                    expected_state().cycle = cycle_cost;
                    expected_state().registers.pc = default_pc_addr + 2;
                    expected_state().registers.a = a - value;
                    expected_state().registers.p = flags;

                    execute_and_check(opcode);
                }
            }
        }
    }

    void test_absolute_indexed(opcode opcode, register_id index_register)
    {
        DYNAMIC_SECTION(format("absolute_{}", index_register))
        {
            // constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(4);

            SECTION("addressing")
            {
                const auto [pc_addr, base_addr, idx] = GENERATE(from_range(absolute_indexed_scenarios));
                const addr_t indexed_addr = base_addr + idx;

                CAPTURE(pc_addr);
                CAPTURE(base_addr);
                CAPTURE(idx);
                CAPTURE(indexed_addr);

                state().registers.a = 0;
                state().registers.pc = pc_addr;
                state().registers.set_flag(status_flag::carry);
                memory().set_word(pc_addr, base_addr);
                memory().set_byte(indexed_addr, 0);
                set_register(state().registers, index_register, idx);

                // expected_state().cycle = cycle_cost;
                expected_state().registers.pc = pc_addr + 2;
                expected_state().registers.set_flag(status_flag::zero, true);

                execute_and_check(opcode, false);
            }

            SECTION("behavior")
            {
                constexpr addr_t indexed_addr = absolute_base_addr + indexed_offset;

                state().registers.p = 0;
                state().registers.pc = default_pc_addr;
                memory().set_word(default_pc_addr, absolute_base_addr);
                set_register(state().registers, index_register, indexed_offset);

                SECTION("carry flag clear")
                {
                    auto [a, value, flags] = GENERATE(from_range(carry_flag_clear_scenarios));

                    CAPTURE(a);
                    CAPTURE(value);
                    CAPTURE(flags);

                    state().registers.a = a;
                    memory().set_byte(indexed_addr, value);

                    // expected_state().cycle = cycle_cost;
                    expected_state().registers.pc = default_pc_addr + 2;
                    expected_state().registers.a = a - value - 1;
                    expected_state().registers.p = flags;

                    execute_and_check(opcode, false);
                }

                SECTION("carry flag set")
                {
                    state().registers.set_flag(status_flag::carry);

                    auto [a, value, flags] = GENERATE(from_range(carry_flag_set_scenarios));

                    CAPTURE(a);
                    CAPTURE(value);
                    CAPTURE(flags);

                    state().registers.a = a;
                    memory().set_byte(indexed_addr, value);

                    // expected_state().cycle = cycle_cost;
                    expected_state().registers.pc = default_pc_addr + 2;
                    expected_state().registers.a = a - value;
                    expected_state().registers.p = flags;

                    execute_and_check(opcode, false);
                }
            }
        }
    }
};

TEST_CASE_METHOD(sbc_fixture, "sbc", "[cpu][instruction]")
{
    test_immediate(opcode::sbc_immediate);
    test_zero_page(opcode::sbc_zero_page);
    test_zero_page_indexed(opcode::sbc_zero_page_x, register_id::x);
    test_absolute(opcode::sbc_absolute);
    test_absolute_indexed(opcode::sbc_absolute_x, register_id::x);
    test_absolute_indexed(opcode::sbc_absolute_y, register_id::y);
}

} // namespace nese::cpu::instruction
