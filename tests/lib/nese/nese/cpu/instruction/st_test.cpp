#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/catch_generators.hpp>
#include <nese/cpu/instruction/fixture.hpp>

namespace nese::cpu::instruction {

struct st_fixture : fixture
{
    template<typename ExecuteFunctorT>
    void test_zero_page(const ExecuteFunctorT& execute, register_id store_register)
    {
        SECTION("zero_page")
        {
            constexpr cycle_t cycle_cost = cpu_cycle_t(3);

            SECTION("addressing")
            {
                const auto [pc_addr, base_addr] = GENERATE(from_range(zero_page_scenarios));

                state.registers.pc = pc_addr;
                state.owned_memory.set_byte(pc_addr, base_addr);
                set_register(state, store_register, 1);

                expected_state = state;
                expected_state.cycle = cycle_cost;
                expected_state.owned_memory.set_byte(base_addr, 1);
                expected_state.registers.pc = pc_addr + 1;

                execute(state);

                check_state();
            }

            SECTION("value")
            {
                const byte_t value = GENERATE(byte_t{0x00}, byte_t{0xC0}, byte_t{0xFF});

                state.registers.pc = default_pc_addr;
                state.owned_memory.set_byte(default_pc_addr, zero_page_base_addr);
                set_register(state, store_register, value);

                expected_state = state;
                expected_state.cycle = cycle_cost;
                expected_state.owned_memory.set_byte(zero_page_base_addr, value);
                expected_state.registers.pc = default_pc_addr + 1;

                execute(state);

                check_state();
            }
        }
    }

    template<typename ExecuteFunctorT>
    void test_zero_page_indexed(const ExecuteFunctorT& execute, register_id store_register, register_id index_register)
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
                set_register(state, index_register, idx);
                set_register(state, store_register, 1);

                expected_state = state;
                expected_state.cycle = cycle_cost;
                expected_state.registers.pc = pc_addr + 1;
                expected_state.owned_memory.set_byte(indexed_addr, 1);

                execute(state);

                check_state();
            }

            SECTION("value")
            {
                constexpr byte_t indexed_addr = zero_page_base_addr + indexed_offset;

                const byte_t value = GENERATE(byte_t{0x00}, byte_t{0xC0}, byte_t{0xFF});

                state.registers.pc = default_pc_addr;
                state.owned_memory.set_byte(default_pc_addr, zero_page_base_addr);
                set_register(state, index_register, indexed_offset);
                set_register(state, store_register, value);

                expected_state = state;
                expected_state.cycle = cycle_cost;
                expected_state.registers.pc = default_pc_addr + 1;
                expected_state.owned_memory.set_byte(indexed_addr, value);

                execute(state);

                check_state();
            }
        }
    }

    template<typename ExecuteFunctorT>
    void test_absolute(const ExecuteFunctorT& execute, register_id store_register)
    {
        SECTION("absolute")
        {
            constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(4);

            SECTION("addressing")
            {
                const auto [pc_addr, base_addr] = GENERATE(from_range(absolute_scenarios));

                state.registers.pc = pc_addr;
                state.owned_memory.set_word(pc_addr, base_addr);
                set_register(state, store_register, 1);

                expected_state = state;
                expected_state.cycle = cycle_cost;
                expected_state.owned_memory.set_byte(base_addr, 1);
                expected_state.registers.pc = pc_addr + 2;

                execute(state);

                check_state();
            }

            SECTION("value")
            {
                const byte_t value = GENERATE(byte_t{0x00}, byte_t{0xC0}, byte_t{0xFF});

                state.registers.pc = default_pc_addr;
                state.owned_memory.set_word(default_pc_addr, zero_page_base_addr);
                set_register(state, store_register, value);

                expected_state = state;
                expected_state.cycle = cycle_cost;
                expected_state.owned_memory.set_byte(zero_page_base_addr, value);
                expected_state.registers.pc = default_pc_addr + 2;

                execute(state);

                check_state();
            }
        }
    }
};

TEST_CASE_METHOD(st_fixture, "stx", "[cpu][instruction]")
{
    test_zero_page(execute_stx<addr_mode::zero_page>, register_id::x);
    test_zero_page_indexed(execute_stx<addr_mode::zero_page_y>, register_id::x, register_id::y);
    test_absolute(execute_stx<addr_mode::absolute>, register_id::x);
}

TEST_CASE_METHOD(st_fixture, "sty", "[cpu][instruction]")
{
    test_zero_page(execute_sty<addr_mode::zero_page>, register_id::y);
    test_zero_page_indexed(execute_sty<addr_mode::zero_page_x>, register_id::y, register_id::x);
    test_absolute(execute_sty<addr_mode::absolute>, register_id::y);
}

} // namespace nese::cpu::instruction
