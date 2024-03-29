#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/catch_generators.hpp>
#include <nese/cpu/instruction/fixture.hpp>

namespace nese::cpu::instruction {

struct ld_fixture : fixture
{
    ld_fixture()
    {
        state.registers.a = 0xA;
        state.registers.x = 0xB;
        state.registers.y = 0xC;
    }

    template<typename ExecuteFunctorT>
    void test_immediate(const ExecuteFunctorT& execute, register_id load_register)
    {
        SECTION("immediate")
        {
            constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(2);

            SECTION("addressing")
            {
                const addr_t addr = GENERATE(as<addr_t>(), 0x0000, 0x7FFE, 0xFFFE);

                state.registers.pc = addr;
                state.owned_memory.set_byte(addr, 1);

                state_mock expected_state = state;
                expected_state.cycle = cycle_cost;
                expected_state.registers.pc = addr + 1;
                set_register(expected_state, load_register, 1);

                execute(state);

                check_state(expected_state);
            }

            SECTION("flags")
            {
                state.registers.pc = default_pc_addr;

                SECTION("load a zero")
                {
                    state.owned_memory.set_byte(default_pc_addr, 0);

                    state_mock expected_state = state;
                    expected_state.cycle = cycle_cost;
                    expected_state.registers.pc = default_pc_addr + 1;
                    expected_state.registers.set_flag(status_flag::zero, true);
                    expected_state.registers.set_flag(status_flag::negative, false);
                    set_register(expected_state, load_register, 0);

                    execute(state);

                    check_state(expected_state);
                }

                SECTION("load a negative")
                {
                    const byte_t value = GENERATE(from_range(negative_byte_values));
                    state.owned_memory.set_byte(default_pc_addr, value);

                    state_mock expected_state = state;
                    expected_state.cycle = cycle_cost;
                    expected_state.registers.pc = default_pc_addr + 1;
                    expected_state.registers.set_flag(status_flag::zero, false);
                    expected_state.registers.set_flag(status_flag::negative, true);
                    set_register(expected_state, load_register, value);

                    execute(state);

                    check_state(expected_state);
                }

                SECTION("load a positive")
                {
                    const byte_t value = GENERATE(from_range(positive_byte_values));
                    state.owned_memory.set_byte(default_pc_addr, value);

                    state_mock expected_state = state;
                    expected_state.cycle = cycle_cost;
                    expected_state.registers.pc = default_pc_addr + 1;
                    expected_state.registers.set_flag(status_flag::zero, false);
                    expected_state.registers.set_flag(status_flag::negative, false);
                    set_register(expected_state, load_register, value);

                    execute(state);

                    check_state(expected_state);
                }
            }
        }
    }

    template<typename ExecuteFunctorT>
    void test_zero_page(const ExecuteFunctorT& execute, register_id load_register)
    {
        SECTION("zero_page")
        {
            constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(3);

            SECTION("addressing")
            {
                const auto [pc_addr, base_addr] = GENERATE(from_range(zero_page_scenarios));

                state.registers.pc = pc_addr;
                state.owned_memory.set_byte(pc_addr, base_addr);
                state.owned_memory.set_byte(base_addr, 1);

                state_mock expected_state = state;
                expected_state.cycle = cycle_cost;
                expected_state.registers.pc = pc_addr + 1;
                set_register(expected_state, load_register, 1);

                execute(state);

                check_state(expected_state);
            }

            SECTION("flags")
            {
                state.registers.pc = default_pc_addr;
                state.owned_memory.set_byte(default_pc_addr, zero_page_base_addr);

                SECTION("load a zero")
                {
                    state.owned_memory.set_byte(zero_page_base_addr, 0);

                    state_mock expected_state = state;
                    expected_state.cycle = cycle_cost;
                    expected_state.registers.pc = default_pc_addr + 1;
                    expected_state.registers.set_flag(status_flag::zero, true);
                    expected_state.registers.set_flag(status_flag::negative, false);
                    set_register(expected_state, load_register, 0);

                    execute(state);

                    check_state(expected_state);
                }

                SECTION("load a negative")
                {
                    const byte_t value = GENERATE(from_range(negative_byte_values));

                    state.owned_memory.set_byte(zero_page_base_addr, value);

                    state_mock expected_state = state;
                    expected_state.cycle = cycle_cost;
                    expected_state.registers.pc = default_pc_addr + 1;
                    expected_state.registers.set_flag(status_flag::zero, false);
                    expected_state.registers.set_flag(status_flag::negative, true);
                    set_register(expected_state, load_register, value);

                    execute(state);

                    check_state(expected_state);
                }

                SECTION("load a positive")
                {
                    const byte_t value = GENERATE(from_range(positive_byte_values));

                    state.owned_memory.set_byte(zero_page_base_addr, value);

                    state_mock expected_state = state;
                    expected_state.cycle = cycle_cost;
                    expected_state.registers.pc = default_pc_addr + 1;
                    expected_state.registers.set_flag(status_flag::zero, false);
                    expected_state.registers.set_flag(status_flag::negative, false);
                    set_register(expected_state, load_register, value);

                    execute(state);

                    check_state(expected_state);
                }
            }
        }
    }

    template<typename ExecuteFunctorT>
    void test_zero_page_indexed(const ExecuteFunctorT& execute, register_id load_register, register_id index_register)
    {
        DYNAMIC_SECTION(format("zero_page_{}", index_register))
        {
            constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(4);

            SECTION("addressing")
            {
                const auto [pc_addr, base_addr, idx] = GENERATE(from_range(zero_page_indexed_scenarios));
                const byte_t indexed_addr = base_addr + idx & 0xff;

                state.registers.pc = pc_addr;
                state.owned_memory.set_byte(pc_addr, base_addr);
                state.owned_memory.set_byte(indexed_addr, 1);
                set_register(state, index_register, idx);

                state_mock expected_state = state;
                expected_state.cycle = cycle_cost;
                expected_state.registers.pc = pc_addr + 1;
                set_register(expected_state, load_register, 1);

                execute(state);

                check_state(expected_state);
            }

            SECTION("flags")
            {
                constexpr byte_t indexed_addr = zero_page_base_addr + indexed_offset;

                state.registers.pc = default_pc_addr;
                state.owned_memory.set_byte(default_pc_addr, zero_page_base_addr);
                set_register(state, index_register, indexed_offset);

                SECTION("load a zero")
                {
                    state.owned_memory.set_byte(indexed_addr, 0);

                    state_mock expected_state = state;
                    expected_state.cycle = cycle_cost;
                    expected_state.registers.pc = default_pc_addr + 1;
                    expected_state.registers.set_flag(status_flag::zero, true);
                    expected_state.registers.set_flag(status_flag::negative, false);
                    set_register(expected_state, load_register, 0);

                    execute(state);

                    check_state(expected_state);
                }

                SECTION("load a negative")
                {
                    const byte_t value = GENERATE(from_range(negative_byte_values));

                    state.owned_memory.set_byte(indexed_addr, value);

                    state_mock expected_state = state;
                    expected_state.cycle = cycle_cost;
                    expected_state.registers.pc = default_pc_addr + 1;
                    expected_state.registers.set_flag(status_flag::zero, false);
                    expected_state.registers.set_flag(status_flag::negative, true);
                    set_register(expected_state, load_register, value);

                    execute(state);

                    check_state(expected_state);
                }

                SECTION("load a positive")
                {
                    const byte_t value = GENERATE(from_range(positive_byte_values));

                    state.owned_memory.set_byte(indexed_addr, value);

                    state_mock expected_state = state;
                    expected_state.cycle = cpu_cycle_t(4);
                    expected_state.registers.pc = default_pc_addr + 1;
                    expected_state.registers.set_flag(status_flag::zero, false);
                    expected_state.registers.set_flag(status_flag::negative, false);
                    set_register(expected_state, load_register, value);

                    execute(state);

                    check_state(expected_state);
                }
            }
        }
    }

    template<typename ExecuteFunctorT>
    void test_absolute(const ExecuteFunctorT& execute, register_id load_register)
    {
        SECTION("absolute")
        {
            constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(4);

            SECTION("addressing")
            {
                const auto [pc_addr, base_addr] = GENERATE(from_range(absolute_scenarios));

                state.registers.pc = pc_addr;
                state.owned_memory.set_word(pc_addr, base_addr);
                state.owned_memory.set_byte(base_addr, 1);

                state_mock expected_state = state;
                expected_state.cycle = cycle_cost;
                expected_state.registers.pc = pc_addr + 2;
                set_register(expected_state, load_register, 1);

                execute(state);

                check_state(expected_state);
            }

            SECTION("flags")
            {
                state.registers.pc = default_pc_addr;
                state.owned_memory.set_word(default_pc_addr, absolute_base_addr);

                SECTION("load zero")
                {
                    state.owned_memory.set_byte(absolute_base_addr, 0);

                    state_mock expected_state = state;
                    expected_state.cycle = cycle_cost;
                    expected_state.registers.pc = default_pc_addr + 2;
                    expected_state.registers.set_flag(status_flag::zero, true);
                    expected_state.registers.set_flag(status_flag::negative, false);
                    set_register(expected_state, load_register, 0);

                    execute(state);

                    check_state(expected_state);
                }

                SECTION("load a negative")
                {
                    const byte_t value = GENERATE(from_range(negative_byte_values));
                    state.owned_memory.set_byte(absolute_base_addr, value);

                    state_mock expected_state = state;
                    expected_state.cycle = cycle_cost;
                    expected_state.registers.pc = default_pc_addr + 2;
                    expected_state.registers.set_flag(status_flag::zero, false);
                    expected_state.registers.set_flag(status_flag::negative, true);
                    set_register(expected_state, load_register, value);

                    execute(state);

                    check_state(expected_state);
                }

                SECTION("load a positive")
                {
                    const byte_t value = GENERATE(from_range(positive_byte_values));
                    state.owned_memory.set_byte(absolute_base_addr, value);

                    state_mock expected_state = state;
                    expected_state.cycle = cycle_cost;
                    expected_state.registers.pc = default_pc_addr + 2;
                    expected_state.registers.set_flag(status_flag::zero, false);
                    expected_state.registers.set_flag(status_flag::negative, false);
                    set_register(expected_state, load_register, value);

                    execute(state);

                    check_state(expected_state);
                }
            }
        }
    }

    template<typename ExecuteFunctorT>
    void test_absolute_indexed(const ExecuteFunctorT& execute, register_id load_register, register_id index_register)
    {
        DYNAMIC_SECTION(format("absolute_{}", index_register))
        {
            SECTION("addressing")
            {
                const auto [pc_addr, base_addr, idx] = GENERATE(from_range(absolute_indexed_scenarios));
                const addr_t indexed_addr = base_addr + idx;

                state.registers.pc = pc_addr;
                state.owned_memory.set_word(pc_addr, base_addr);
                state.owned_memory.set_byte(indexed_addr, 1);
                set_register(state, index_register, idx);

                state_mock expected_state = state;
                // expected_state.cycle = cycle_cost;
                expected_state.registers.pc = pc_addr + 2;
                set_register(expected_state, load_register, 1);

                execute(state);

                check_state(expected_state, false);
            }

            SECTION("flags")
            {
                constexpr addr_t indexed_addr = absolute_base_addr + indexed_offset;

                state.registers.pc = default_pc_addr;
                state.owned_memory.set_word(default_pc_addr, absolute_base_addr);
                set_register(state, index_register, indexed_offset);

                SECTION("load zero")
                {
                    state.owned_memory.set_byte(indexed_addr, 0);

                    state_mock expected_state = state;
                    // expected_state.cycle = cycle_cost;
                    expected_state.registers.pc = default_pc_addr + 2;
                    expected_state.registers.set_flag(status_flag::zero, true);
                    expected_state.registers.set_flag(status_flag::negative, false);
                    set_register(expected_state, load_register, 0);

                    execute(state);

                    check_state(expected_state, false);
                }

                SECTION("load a negative")
                {
                    const byte_t value = GENERATE(from_range(negative_byte_values));
                    state.owned_memory.set_byte(indexed_addr, value);

                    state_mock expected_state = state;
                    // expected_state.cycle = cycle_cost;
                    expected_state.registers.pc = default_pc_addr + 2;
                    expected_state.registers.set_flag(status_flag::zero, false);
                    expected_state.registers.set_flag(status_flag::negative, true);
                    set_register(expected_state, load_register, value);

                    execute(state);

                    check_state(expected_state, false);
                }

                SECTION("load a positive")
                {
                    const byte_t value = GENERATE(from_range(positive_byte_values));
                    state.owned_memory.set_byte(indexed_addr, value);

                    state_mock expected_state = state;
                    // expected_state.cycle = cycle_cost;
                    expected_state.registers.pc = default_pc_addr + 2;
                    expected_state.registers.set_flag(status_flag::zero, false);
                    expected_state.registers.set_flag(status_flag::negative, false);
                    set_register(expected_state, load_register, value);

                    execute(state);

                    check_state(expected_state, false);
                }
            }
        }
    }
};

TEST_CASE_METHOD(ld_fixture, "lda", "[cpu][instruction]")
{
    test_immediate(execute_lda<addr_mode::immediate>, register_id::a);
    test_zero_page(execute_lda<addr_mode::zero_page>, register_id::a);
    test_zero_page_indexed(execute_lda<addr_mode::zero_page_x>, register_id::a, register_id::x);
    test_absolute(execute_lda<addr_mode::absolute>, register_id::a);
    test_absolute_indexed(execute_lda<addr_mode::absolute_x>, register_id::a, register_id::x);
    test_absolute_indexed(execute_lda<addr_mode::absolute_y>, register_id::a, register_id::y);
}

TEST_CASE_METHOD(ld_fixture, "ldx", "[cpu][instruction]")
{
    test_immediate(execute_ldx<addr_mode::immediate>, register_id::x);
    test_zero_page(execute_ldx<addr_mode::zero_page>, register_id::x);
    test_zero_page_indexed(execute_ldx<addr_mode::zero_page_y>, register_id::x, register_id::y);
    test_absolute(execute_ldx<addr_mode::absolute>, register_id::x);
    test_absolute_indexed(execute_ldx<addr_mode::absolute_y>, register_id::x, register_id::y);
}

TEST_CASE_METHOD(ld_fixture, "ldy", "[cpu][instruction]")
{
    test_immediate(execute_ldy<addr_mode::immediate>, register_id::y);
    test_zero_page(execute_ldy<addr_mode::zero_page>, register_id::y);
    test_zero_page_indexed(execute_ldy<addr_mode::zero_page_x>, register_id::y, register_id::x);
    test_absolute(execute_ldy<addr_mode::absolute>, register_id::y);
    test_absolute_indexed(execute_ldy<addr_mode::absolute_x>, register_id::y, register_id::x);
}

} // namespace nese::cpu::instruction
