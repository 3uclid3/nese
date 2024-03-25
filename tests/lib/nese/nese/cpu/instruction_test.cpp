#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/state_mock.hpp>
#include <nese/memory/mapper.hpp>
#include <nese/utility/assert.hpp>

namespace nese::cpu::instruction {

struct fixture
{
    static constexpr auto minus_two = static_cast<byte_t>(-2);
    static constexpr auto minus_one = static_cast<byte_t>(-1);

    void check_state(const state& expected_state) const
    {
        check_registers(expected_state);
        check_memory(expected_state);
    }

    void check_registers(const state& expected_state) const
    {
        CHECK(state.registers.a == expected_state.registers.a);
        CHECK(state.registers.x == expected_state.registers.x);
        CHECK(state.registers.y == expected_state.registers.y);
        CHECK(state.registers.pc == expected_state.registers.pc);
        CHECK(state.registers.s == expected_state.registers.s);

        if (state.registers.p != expected_state.registers.p)
        {
            CHECK(state.registers.has_flag(status_flag::carry) == expected_state.registers.has_flag(status_flag::carry));
            CHECK(state.registers.has_flag(status_flag::zero) == expected_state.registers.has_flag(status_flag::zero));
            CHECK(state.registers.has_flag(status_flag::interrupt) == expected_state.registers.has_flag(status_flag::interrupt));
            CHECK(state.registers.has_flag(status_flag::decimal) == expected_state.registers.has_flag(status_flag::decimal));
            CHECK(state.registers.has_flag(status_flag::break_cmd) == expected_state.registers.has_flag(status_flag::break_cmd));
            CHECK(state.registers.has_flag(status_flag::unused) == expected_state.registers.has_flag(status_flag::unused));
            CHECK(state.registers.has_flag(status_flag::overflow) == expected_state.registers.has_flag(status_flag::overflow));
            CHECK(state.registers.has_flag(status_flag::negative) == expected_state.registers.has_flag(status_flag::negative));
        }
    }

    void check_memory(const state& expected_state) const
    {
        const auto& expected_memory_buffer = expected_state.memory.get().get_bytes();
        const auto& memory_buffer = state.memory.get().get_bytes();

        CHECK(expected_memory_buffer.size() == memory_buffer.size());

        if (std::memcmp(memory_buffer.data(), expected_memory_buffer.data(), memory_buffer.size()) != 0)
        {
            std::string fail_message = "expected_state.memory != state.memory\n";

            for (addr_t addr = 0; addr < memory_buffer.size() - 1; ++addr)
            {
                if (memory_buffer[addr] != expected_memory_buffer[addr])
                {
                    fmt::format_to(std::back_inserter(fail_message), "  Mismatch at address 0x{:04X}: expected 0x{:02X}, actual 0x{:02X}\n", addr, expected_memory_buffer[addr], memory_buffer[addr]);
                }
            }

            FAIL_CHECK(fail_message.c_str());
        }
    }

    static void set_register_a(registers& r, byte_t a)
    {
        r.a = a;
    }

    static void set_register_x(registers& r, byte_t x)
    {
        r.x = x;
    }

    static void set_register_y(registers& r, byte_t y)
    {
        r.y = y;
    }

    state_mock state{};
};

struct in_fixture : fixture
{
    template<typename ExecuteFunctorT, typename SetRegisterFunctorT>
    void test_addr_mode_immediate(const ExecuteFunctorT& execute, const SetRegisterFunctorT& set_register)
    {
        SECTION("increment to negative")
        {
            set_register(state.registers, minus_two);

            state_mock expected_state = state;
            set_register(expected_state.registers, minus_one);
            expected_state.registers.set_alu_flag(minus_one);

            execute(state);

            check_state(expected_state);
        }

        SECTION("increment to zero")
        {
            set_register(state.registers, minus_one);

            state_mock expected_state = state;
            set_register(expected_state.registers, 0);
            expected_state.registers.set_alu_flag(0);

            execute(state);

            check_state(expected_state);
        }

        SECTION("increment to positive")
        {
            set_register(state.registers, 0);

            state_mock expected_state = state;
            set_register(expected_state.registers, 1);
            expected_state.registers.set_alu_flag(1);

            execute(state);

            check_state(expected_state);
        }
    }
};

TEST_CASE_METHOD(in_fixture, "inx::implied", "[cpu][instruction]")
{
    test_addr_mode_immediate(execute_inx<addr_mode::implied> , set_register_x);
}

TEST_CASE_METHOD(in_fixture, "iny::implied", "[cpu][instruction]")
{
    test_addr_mode_immediate(execute_iny<addr_mode::implied> , set_register_y);
}

TEST_CASE_METHOD(fixture, "jmp::absolute", "[cpu][instruction]")
{
    for (addr_t addr = 0x1000; addr < 0x8008; addr += 0x1001)
    {
        const addr_t jmp_to = addr + 0x0010;

        state.memory.get().set_word(addr, jmp_to);
        state.registers.pc = addr;

        state_mock expected_state = state;
        expected_state.registers.pc = state.memory.get().get_word(addr);

        execute_jmp<addr_mode::absolute>(state);

        check_state(expected_state);
    }
}

struct ld_fixture : fixture
{
    template<typename ExecuteFunctorT, typename SetRegisterFunctorT>
    void test_addr_mode_immediate(const ExecuteFunctorT& execute, const SetRegisterFunctorT& set_register)
    {
        for (addr_t addr = 0x10; addr < 0x80; addr += 0x10)
        {
            SECTION("load a negative")
            {
                state.memory.get().set_byte(addr, minus_one);
                state.registers.pc = addr;

                state_mock expected_state = state;
                set_register(expected_state.registers, minus_one);
                expected_state.registers.pc = addr + 1;
                expected_state.registers.set_alu_flag(minus_one);

                execute(state);

                check_state(expected_state);
            }

            SECTION("load a zero")
            {
                state.memory.get().set_byte(addr, 0);
                state.registers.pc = addr;

                state_mock expected_state = state;
                set_register(expected_state.registers, 0);
                expected_state.registers.pc = addr + 1;
                expected_state.registers.set_alu_flag(0);

                execute(state);

                check_state(expected_state);
            }

            SECTION("load a positive")
            {
                state.memory.get().set_byte(addr, 1);
                state.registers.pc = addr;

                state_mock expected_state = state;
                set_register(expected_state.registers, 1);
                expected_state.registers.pc = addr + 1;
                expected_state.registers.set_alu_flag(1);

                execute(state);

                check_state(expected_state);
            }
        }
    }
};

TEST_CASE_METHOD(ld_fixture, "lda::immediate", "[cpu][instruction]")
{
    test_addr_mode_immediate(execute_lda<addr_mode::immediate> ,set_register_a);
}

} // namespace nese::cpu::instruction