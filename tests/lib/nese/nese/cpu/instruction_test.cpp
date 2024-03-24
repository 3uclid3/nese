#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/state.hpp>
#include <nese/memory/mapper.hpp>
#include <nese/utility/assert.hpp>

namespace nese::cpu::instruction {

struct state_mock : state
{
    state_mock()
        : state{.memory = owned_memory}
    {
    }

    explicit state_mock(cpu::registers new_registers)
        : state_mock()
    {
        registers = new_registers;
    }

    memory::mapper owned_memory{};
};

struct fixture
{
    void check_state() const
    {
        check_registers();
        check_memory();
    }

    void check_registers() const
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

    void check_memory() const
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

    state_mock state{};
    state_mock expected_state{};
};

constexpr auto minus_two = static_cast<byte_t>(-2);
constexpr auto minus_one = static_cast<byte_t>(-1);

TEST_CASE_METHOD(fixture, "inx with addr_mode implied")
{
    SECTION("increment to negative")
    {
        state.registers.x = minus_two;

        expected_state.registers.x = minus_one;
        expected_state.registers.set_alu_flag(minus_one);

        execute_inx<addr_mode::implied>(state);

        check_state();
    }

    SECTION("increment to zero")
    {
        state.registers.x = minus_one;

        expected_state.registers.x = 0;
        expected_state.registers.set_alu_flag(0);

        execute_inx<addr_mode::implied>(state);

        check_state();
    }

    SECTION("increment to positive")
    {
        state.registers.x = 0;

        expected_state.registers.x = 1;
        expected_state.registers.set_alu_flag(1);

        execute_inx<addr_mode::implied>(state);

        check_state();
    }
}

TEST_CASE_METHOD(fixture, "iny with addr_mode implied")
{
    SECTION("increment to negative")
    {
        state.registers.y = minus_two;

        expected_state.registers.y = minus_one;
        expected_state.registers.set_alu_flag(minus_one);

        execute_iny<addr_mode::implied>(state);

        check_state();
    }

    SECTION("increment to zero")
    {
        state.registers.y = minus_one;

        expected_state.registers.y = 0;
        expected_state.registers.set_alu_flag(0);

        execute_iny<addr_mode::implied>(state);

        check_state();
    }

    SECTION("increment to positive")
    {
        state.registers.y = 0;

        expected_state.registers.y = 1;
        expected_state.registers.set_alu_flag(1);

        execute_iny<addr_mode::implied>(state);

        check_state();
    }
}

TEST_CASE_METHOD(fixture, "lda with addr_mode immediate")
{
    constexpr addr_t addr = 0x10;

    state.memory.get().set_byte(addr, static_cast<byte_t>(-1));
    state.registers.pc = addr;

    execute_lda<addr_mode::immediate>(state);

    CHECK(state.registers.pc == addr + 1);
    CHECK(state.registers.a == static_cast<byte_t>(-1));
    CHECK_FALSE(state.registers.has_flag(status_flag::zero));
    CHECK(state.registers.has_flag(status_flag::negative));
}

TEST_CASE_METHOD(fixture, "lda with addr_mode zero_page")
{
}

TEST_CASE_METHOD(fixture, "lda with addr_mode zero_page_x")
{
}

TEST_CASE_METHOD(fixture, "lda with addr_mode absolute")
{
}

TEST_CASE_METHOD(fixture, "lda with addr_mode absolute_x")
{
}

TEST_CASE_METHOD(fixture, "lda with addr_mode absolute_y")
{
}

TEST_CASE_METHOD(fixture, "lda with addr_mode indexed_indirect")
{
}

TEST_CASE_METHOD(fixture, "lda with addr_mode indirect_indexed")
{
}

} // namespace nese::cpu::instruction