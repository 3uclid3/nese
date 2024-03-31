#include <nese/cpu/instruction/fixture/store_fixture.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <nese/utility/format.hpp>

namespace nese::cpu::instruction {

void store_fixture::test_zero_page(opcode opcode, register_id store_register)
{
    SECTION("zero_page")
    {
        constexpr cycle_t cycle_cost = cpu_cycle_t(3);

        SECTION("addressing")
        {
            const auto [pc_addr, base_addr] = GENERATE(from_range(zero_page_scenarios));

            state().registers.pc = pc_addr;
            memory().set_byte(pc_addr, base_addr);
            set_register(state().registers, store_register, 1);

            expected_state().cycle = cycle_cost;
            expected_memory().set_byte(base_addr, 1);
            expected_state().registers.pc = pc_addr + 1;

            execute_and_check(opcode);
        }

        SECTION("value")
        {
            const byte_t value = GENERATE(byte_t{0x00}, byte_t{0xC0}, byte_t{0xFF});

            state().registers.pc = default_pc_addr;
            memory().set_byte(default_pc_addr, zero_page_base_addr);
            set_register(state().registers, store_register, value);

            expected_state().cycle = cycle_cost;
            expected_memory().set_byte(zero_page_base_addr, value);
            expected_state().registers.pc = default_pc_addr + 1;

            execute_and_check(opcode);
        }
    }
}

void store_fixture::test_zero_page_indexed(opcode opcode, register_id store_register, register_id index_register)
{
    DYNAMIC_SECTION(format("zero_page_{}", index_register))
    {
        constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(4);

        SECTION("addressing")
        {
            const auto [pc_addr, base_addr, idx] = GENERATE(from_range(zero_page_indexed_scenarios));
            const byte_t indexed_addr = base_addr + idx & 0xff;

            state().registers.pc = pc_addr;
            memory().set_byte(pc_addr, base_addr);
            set_register(state().registers, index_register, idx);
            set_register(state().registers, store_register, 1);

            expected_state().cycle = cycle_cost;
            expected_state().registers.pc = pc_addr + 1;
            expected_memory().set_byte(indexed_addr, 1);

            execute_and_check(opcode);
        }

        SECTION("value")
        {
            constexpr byte_t indexed_addr = zero_page_base_addr + indexed_offset;

            const byte_t value = GENERATE(byte_t{0x00}, byte_t{0xC0}, byte_t{0xFF});

            state().registers.pc = default_pc_addr;
            memory().set_byte(default_pc_addr, zero_page_base_addr);
            set_register(state().registers, index_register, indexed_offset);
            set_register(state().registers, store_register, value);

            expected_state().cycle = cycle_cost;
            expected_state().registers.pc = default_pc_addr + 1;
            expected_memory().set_byte(indexed_addr, value);

            execute_and_check(opcode);
        }
    }
}

void store_fixture::test_absolute(opcode opcode, register_id store_register)
{
    SECTION("absolute")
    {
        constexpr cpu_cycle_t cycle_cost = cpu_cycle_t(4);

        SECTION("addressing")
        {
            const auto [pc_addr, base_addr] = GENERATE(from_range(absolute_scenarios));

            state().registers.pc = pc_addr;
            memory().set_word(pc_addr, base_addr);
            set_register(state().registers, store_register, 1);

            expected_state().cycle = cycle_cost;
            expected_memory().set_byte(base_addr, 1);
            expected_state().registers.pc = pc_addr + 2;

            execute_and_check(opcode);
        }

        SECTION("value")
        {
            const byte_t value = GENERATE(byte_t{0x00}, byte_t{0xC0}, byte_t{0xFF});

            state().registers.pc = default_pc_addr;
            memory().set_word(default_pc_addr, zero_page_base_addr);
            set_register(state().registers, store_register, value);

            expected_state().cycle = cycle_cost;
            expected_memory().set_byte(zero_page_base_addr, value);
            expected_state().registers.pc = default_pc_addr + 2;

            execute_and_check(opcode);
        }
    }
}

}; // namespace nese::cpu::instruction
