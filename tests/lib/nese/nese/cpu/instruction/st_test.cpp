#include <catch2/catch_template_test_macros.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/instruction/catch_generators.hpp>
#include <nese/cpu/instruction/fixture.hpp>

namespace nese::cpu::instruction {

struct st_fixture : fixture
{
    st_fixture()
    {
        state.registers.a = 0xA;
        state.registers.x = 0xB;
        state.registers.y = 0xC;
    }

    template<typename ExecuteFunctorT, typename SetRegisterFunctorT>
    void test_zero_page(const ExecuteFunctorT& execute, const SetRegisterFunctorT& set_register)
    {
        auto [pc, val_addr] = GENERATE_ADDR_FOR_ZERO_PAGE();

        const byte_t val = GENERATE(0x00, 0xC0, 0xFF);

        SECTION("zero_page")
        {
            INFO(fmt::format("pc = 0x{:04X}; val addr = 0x{:02X}); value = 0x{:02X}", pc, val_addr, val));

            state.registers.pc = pc;
            state.owned_memory.set_byte(pc, val_addr);
            set_register(state.registers, val);

            state_mock expected_state = state;
            expected_state.owned_memory.set_byte(val_addr, val);
            expected_state.registers.pc = pc + 1;

            execute(state);

            check_state(expected_state);
        }
    }

    template<typename ExecuteFunctorT, typename SetRegisterFunctorT>
    void test_zero_page_x(const ExecuteFunctorT& execute, const SetRegisterFunctorT& set_register)
    {
        auto [pc, val_addr, x] = GENERATE_ADDR_AND_OFFSET_FOR_ZERO_PAGE();
        const byte_t val_addr_x = val_addr + x & 0xff;

        const byte_t val = GENERATE(0x00, 0xC0, 0xFF);

        if (static_cast<byte_t>(pc) != val_addr_x)
        {
            SECTION("zero_page_x")
            {
                INFO(fmt::format("pc = 0x{:04X}; value addr = 0x{:02X}); x = 0x{:02X}; value = 0x{:02X}", pc, val_addr, x, val));

                state.registers.pc = pc;
                state.registers.x = x;
                state.owned_memory.set_byte(pc, val_addr);
                set_register(state.registers, val);

                state_mock expected_state = state;
                expected_state.owned_memory.set_byte(val_addr_x, val);
                expected_state.registers.pc = pc + 1;

                execute(state);

                check_state(expected_state);
            }
        }
    }

    template<typename ExecuteFunctorT, typename SetRegisterFunctorT>
    void test_zero_page_y(const ExecuteFunctorT& execute, const SetRegisterFunctorT& set_register)
    {
        auto [pc, val_addr, y] = GENERATE_ADDR_AND_OFFSET_FOR_ZERO_PAGE();
        const byte_t val_addr_y = val_addr + y & 0xff;

        const byte_t val = GENERATE(0x00, 0xC0, 0xFF);

        if (static_cast<byte_t>(pc) != val_addr_y)
        {
            SECTION("zero_page_y")
            {
                INFO(fmt::format("pc = 0x{:04X}; value addr = 0x{:02X}); y = 0x{:02X}; value = 0x{:02X}", pc, val_addr, y, val));

                state.registers.pc = pc;
                state.registers.y = y;
                state.owned_memory.set_byte(pc, val_addr);
                set_register(state.registers, val);

                state_mock expected_state = state;
                expected_state.owned_memory.set_byte(val_addr_y, val);
                expected_state.registers.pc = pc + 1;

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
            auto [pc, val_addr] = GENERATE_ADDR_FOR_ABSOLUTE();

            const byte_t val = GENERATE(0x00, 0xC0, 0xFF);

            INFO(fmt::format("pc = 0x{:04X}; val addr = 0x{:04X}); value = 0x{:02X}", pc, val_addr, val));

            state.registers.pc = pc;
            state.owned_memory.set_word(pc, val_addr);

            set_register(state.registers, val);

            state_mock expected_state = state;
            expected_state.owned_memory.set_byte(val_addr, val);
            expected_state.registers.pc = pc + 2;

            execute(state);

            check_state(expected_state);
        }
    }
};

TEST_CASE_METHOD(st_fixture, "stx", "[cpu][instruction]")
{
    test_zero_page(execute_stx<addr_mode::zero_page>, set_register_x);
    test_zero_page_y(execute_stx<addr_mode::zero_page_y>, set_register_x);
    test_absolute(execute_stx<addr_mode::absolute>, set_register_x);
}

TEST_CASE_METHOD(st_fixture, "sty", "[cpu][instruction]")
{
    test_zero_page(execute_sty<addr_mode::zero_page>, set_register_y);
    test_zero_page_x(execute_sty<addr_mode::zero_page_x>, set_register_y);
    test_absolute(execute_sty<addr_mode::absolute>, set_register_y);
}

} // namespace nese::cpu::instruction
