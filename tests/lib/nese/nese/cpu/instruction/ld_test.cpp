#include <catch2/catch_test_macros.hpp>

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

    template<typename ExecuteFunctorT, typename SetRegisterFunctorT>
    void test_immediate(const ExecuteFunctorT& execute, const SetRegisterFunctorT& set_register)
    {
        SECTION("immediate")
        {
            const addr_t addr = GENERATE_ADDR();

            state.registers.pc = addr;

            SECTION("load zero")
            {
                state.owned_memory.set_byte(addr, 0);

                state_mock expected_state = state;
                expected_state.cycle = cpu_cycle_t(2);
                set_register(expected_state.registers, 0);
                expected_state.registers.pc = addr + 1;
                expected_state.registers.set_flag(status_flag::zero, true);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state);
            }

            SECTION("load a negative")
            {
                const byte_t value = GENERATE_NEGATIVE_BYTE();

                state.owned_memory.set_byte(addr, value);

                state_mock expected_state = state;
                expected_state.cycle = cpu_cycle_t(2);
                set_register(expected_state.registers, value);

                expected_state.registers.pc = addr + 1;
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, true);

                execute(state);

                check_state(expected_state);
            }

            SECTION("load a positive")
            {
                const byte_t value = GENERATE_POSITIVE_BYTE();

                state.owned_memory.set_byte(addr, value);

                state_mock expected_state = state;
                expected_state.cycle = cpu_cycle_t(2);
                set_register(expected_state.registers, value);
                expected_state.registers.pc = addr + 1;
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state);
            }
        }
    }

    template<typename ExecuteFunctorT, typename SetRegisterFunctorT>
    void test_zero_page(const ExecuteFunctorT& execute, const SetRegisterFunctorT& set_register)
    {
        SECTION("zero_page")
        {
            const auto [pc, val_addr] = GENERATE_ADDR_FOR_ZERO_PAGE();

            state.registers.pc = pc;
            state.owned_memory.set_byte(pc, val_addr);

            SECTION("load zero")
            {
                state.owned_memory.set_byte(val_addr, 0);

                state_mock expected_state = state;
                expected_state.cycle = cpu_cycle_t(3);
                set_register(expected_state.registers, 0);
                expected_state.registers.pc = pc + 1;
                expected_state.registers.set_flag(status_flag::zero, true);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state);
            }

            SECTION("load a negative")
            {
                const byte_t value = GENERATE_NEGATIVE_BYTE();

                state.owned_memory.set_byte(val_addr, value);

                state_mock expected_state = state;
                expected_state.cycle = cpu_cycle_t(3);
                set_register(expected_state.registers, value);

                expected_state.registers.pc = pc + 1;
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, true);

                execute(state);

                check_state(expected_state);
            }

            SECTION("load a positive")
            {
                const byte_t value = GENERATE_POSITIVE_BYTE();

                state.owned_memory.set_byte(val_addr, value);

                state_mock expected_state = state;
                expected_state.cycle = cpu_cycle_t(3);
                set_register(expected_state.registers, value);
                expected_state.registers.pc = pc + 1;
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state);
            }
        }
    }

    template<typename ExecuteFunctorT, typename SetRegisterFunctorT>
    void test_zero_page_x(const ExecuteFunctorT& execute, const SetRegisterFunctorT& set_register)
    {
        SECTION("zero_page_x")
        {
            const auto [pc, val_addr, x] = GENERATE_ADDR_AND_OFFSET_FOR_ZERO_PAGE();

            const byte_t val_addr_x = val_addr + x & 0xff;

            state.registers.pc = pc;
            state.registers.x = x;
            state.owned_memory.set_byte(pc, val_addr);

            SECTION("load zero")
            {
                state.owned_memory.set_byte(val_addr_x, 0);

                state_mock expected_state = state;
                expected_state.cycle = cpu_cycle_t(4);
                set_register(expected_state.registers, 0);
                expected_state.registers.pc = pc + 1;
                expected_state.registers.set_flag(status_flag::zero, true);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state);
            }

            SECTION("load a negative")
            {
                const byte_t value = GENERATE_NEGATIVE_BYTE();

                state.owned_memory.set_byte(val_addr_x, value);

                state_mock expected_state = state;
                expected_state.cycle = cpu_cycle_t(4);
                set_register(expected_state.registers, value);

                expected_state.registers.pc = pc + 1;
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, true);

                execute(state);

                check_state(expected_state);
            }

            SECTION("load a positive")
            {
                const byte_t value = GENERATE_POSITIVE_BYTE();

                state.owned_memory.set_byte(val_addr_x, value);

                state_mock expected_state = state;
                expected_state.cycle = cpu_cycle_t(4);
                set_register(expected_state.registers, value);
                expected_state.registers.pc = pc + 1;
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state);
            }
        }
    }

    template<typename ExecuteFunctorT, typename SetRegisterFunctorT>
    void test_zero_page_y(const ExecuteFunctorT& execute, const SetRegisterFunctorT& set_register)
    {
        SECTION("zero_page_y")
        {
            const auto [pc, val_addr, y] = GENERATE_ADDR_AND_OFFSET_FOR_ZERO_PAGE();

            const byte_t val_addr_y = val_addr + y & 0xff;

            state.registers.pc = pc;
            state.registers.y = y;
            state.owned_memory.set_byte(pc, val_addr);

            SECTION("load zero")
            {
                state.owned_memory.set_byte(val_addr_y, 0);

                state_mock expected_state = state;
                expected_state.cycle = cpu_cycle_t(4);
                set_register(expected_state.registers, 0);
                expected_state.registers.pc = pc + 1;
                expected_state.registers.set_flag(status_flag::zero, true);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state);
            }

            SECTION("load a negative")
            {
                const byte_t value = GENERATE_NEGATIVE_BYTE();

                state.owned_memory.set_byte(val_addr_y, value);

                state_mock expected_state = state;
                expected_state.cycle = cpu_cycle_t(4);
                set_register(expected_state.registers, value);

                expected_state.registers.pc = pc + 1;
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, true);

                execute(state);

                check_state(expected_state);
            }

            SECTION("load a positive")
            {
                const byte_t value = GENERATE_POSITIVE_BYTE();

                state.owned_memory.set_byte(val_addr_y, value);

                state_mock expected_state = state;
                expected_state.cycle = cpu_cycle_t(4);
                set_register(expected_state.registers, value);
                expected_state.registers.pc = pc + 1;
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state);
            }
        }
    }

    template<typename ExecuteFunctorT, typename SetRegisterFunctorT>
    void test_absolute(const ExecuteFunctorT& execute, const SetRegisterFunctorT& set_register)
    {
        SECTION("absolute")
        {
            const auto [pc, val_addr] = GENERATE_ADDR_FOR_ABSOLUTE();

            INFO(fmt::format("pc = 0x{:04X} val_addr = 0x{:04X}", pc, val_addr));

            state.registers.pc = pc;
            state.owned_memory.set_word(pc, val_addr);

            SECTION("load zero")
            {
                state.owned_memory.set_byte(val_addr, 0);

                state_mock expected_state = state;
                expected_state.cycle = cpu_cycle_t(4);
                set_register(expected_state.registers, 0);
                expected_state.registers.pc = pc + 2;
                expected_state.registers.set_flag(status_flag::zero, true);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state);
            }

            SECTION("load a negative")
            {
                const byte_t value = GENERATE_NEGATIVE_BYTE();

                state.owned_memory.set_byte(val_addr, value);

                state_mock expected_state = state;
                expected_state.cycle = cpu_cycle_t(4);
                set_register(expected_state.registers, value);

                expected_state.registers.pc = pc + 2;
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, true);

                execute(state);

                check_state(expected_state);
            }

            SECTION("load a positive")
            {
                const byte_t value = GENERATE_POSITIVE_BYTE();

                state.owned_memory.set_byte(val_addr, value);

                state_mock expected_state = state;
                expected_state.cycle = cpu_cycle_t(4);
                set_register(expected_state.registers, value);
                expected_state.registers.pc = pc + 2;
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state);
            }
        }
    }

    template<typename ExecuteFunctorT, typename SetRegisterFunctorT>
    void test_absolute_x(const ExecuteFunctorT& execute, const SetRegisterFunctorT& set_register)
    {
        SECTION("absolute_x")
        {
            const auto [pc, val_addr, x] = GENERATE_ADDR_AND_OFFSET_FOR_ABSOLUTE();
            const addr_t val_addr_x = val_addr + x;

            INFO(fmt::format("pc = 0x{:04X}; value addr = 0x{:02X}); x = 0x{:02X}", pc, val_addr, x));

            state.registers.pc = pc;
            state.registers.x = x;
            state.owned_memory.set_word(pc, val_addr);

            SECTION("load zero")
            {
                state.owned_memory.set_byte(val_addr_x, 0);

                state_mock expected_state = state;
                set_register(expected_state.registers, 0);
                expected_state.registers.pc = pc + 2;
                expected_state.registers.set_flag(status_flag::zero, true);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state, false);
            }

            SECTION("load a negative")
            {
                const byte_t value = GENERATE_NEGATIVE_BYTE();

                state.owned_memory.set_byte(val_addr_x, value);

                state_mock expected_state = state;
                set_register(expected_state.registers, value);

                expected_state.registers.pc = pc + 2;
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, true);

                execute(state);

                check_state(expected_state, false);
            }

            SECTION("load a positive")
            {
                const byte_t value = GENERATE_POSITIVE_BYTE();

                state.owned_memory.set_byte(val_addr_x, value);

                state_mock expected_state = state;
                set_register(expected_state.registers, value);
                expected_state.registers.pc = pc + 2;
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state, false);
            }
        }
    }

    template<typename ExecuteFunctorT, typename SetRegisterFunctorT>
    void test_absolute_y(const ExecuteFunctorT& execute, const SetRegisterFunctorT& set_register)
    {
        SECTION("absolute_y")
        {
            const auto [pc, val_addr, y] = GENERATE_ADDR_AND_OFFSET_FOR_ABSOLUTE();
            const addr_t val_addr_y = val_addr + y;

            INFO(fmt::format("pc = 0x{:04X}; value addr = 0x{:02X}); y = 0x{:02X}", pc, val_addr, y));

            state.registers.pc = pc;
            state.registers.y = y;
            state.owned_memory.set_word(pc, val_addr);

            SECTION("load zero")
            {
                state.owned_memory.set_byte(val_addr_y, 0);

                state_mock expected_state = state;
                set_register(expected_state.registers, 0);
                expected_state.registers.pc = pc + 2;
                expected_state.registers.set_flag(status_flag::zero, true);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state, false);
            }

            SECTION("load a negative")
            {
                const byte_t value = GENERATE_NEGATIVE_BYTE();

                state.owned_memory.set_byte(val_addr_y, value);

                state_mock expected_state = state;
                set_register(expected_state.registers, value);

                expected_state.registers.pc = pc + 2;
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, true);

                execute(state);

                check_state(expected_state, false);
            }

            SECTION("load a positive")
            {
                const byte_t value = GENERATE_POSITIVE_BYTE();

                state.owned_memory.set_byte(val_addr_y, value);

                state_mock expected_state = state;
                set_register(expected_state.registers, value);
                expected_state.registers.pc = pc + 2;
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state, false);
            }
        }
    }
};

TEST_CASE_METHOD(ld_fixture, "lda", "[cpu][instruction]")
{
    test_immediate(execute_lda<addr_mode::immediate>, set_register_a);
    test_zero_page(execute_lda<addr_mode::zero_page>, set_register_a);
    test_zero_page_x(execute_lda<addr_mode::zero_page_x>, set_register_a);
    test_absolute(execute_lda<addr_mode::absolute>, set_register_a);
    test_absolute_x(execute_lda<addr_mode::absolute_x>, set_register_a);
    test_absolute_y(execute_lda<addr_mode::absolute_y>, set_register_a);
}

TEST_CASE_METHOD(ld_fixture, "ldx", "[cpu][instruction]")
{
    test_immediate(execute_ldx<addr_mode::immediate>, set_register_x);
    test_zero_page(execute_ldx<addr_mode::zero_page>, set_register_x);
    test_zero_page_y(execute_ldx<addr_mode::zero_page_y>, set_register_x);
    test_absolute(execute_ldx<addr_mode::absolute>, set_register_x);
    test_absolute_y(execute_ldx<addr_mode::absolute_y>, set_register_x);
}

TEST_CASE_METHOD(ld_fixture, "ldy", "[cpu][instruction]")
{
    test_immediate(execute_ldy<addr_mode::immediate>, set_register_y);
    test_zero_page(execute_ldy<addr_mode::zero_page>, set_register_y);
    test_zero_page_x(execute_ldy<addr_mode::zero_page_x>, set_register_y);
    test_absolute(execute_ldy<addr_mode::absolute>, set_register_y);
    test_absolute_x(execute_ldy<addr_mode::absolute_x>, set_register_y);
}
} // namespace nese::cpu::instruction
