#pragma once

#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/state_mock.hpp>

namespace nese::cpu::instruction {

static inline state_mock default_state_mock{[] {
    state_mock state;

    byte_t value = 0xFF;
    for (addr_t i = 0; i < static_cast<addr_t>(memory::mapper::capacity - 1); ++i)
    {
        state.owned_memory.set_byte(i, value);

        value = value == 0x01 ? 0xFF : value - 0x01;
    }

    return state;
}()};

struct fixture
{
    fixture()
    {
        state.registers.a = 0xA;
        state.registers.x = 0xB;
        state.registers.y = 0xC;
    }

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

    state_mock state{default_state_mock};
};

} // namespace nese::cpu::instruction
