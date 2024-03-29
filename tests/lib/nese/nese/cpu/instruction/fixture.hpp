#pragma once

#include <array>
#include <tuple>

#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/state_mock.hpp>
#include <nese/utility/assert.hpp>

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
    enum class register_id
    {
        a,
        x,
        y
    };

    // Standard PC address
    static constexpr addr_t default_pc_addr = 0x0200;

    // Small offset
    static constexpr addr_t indexed_offset = 0x0020;

    // Well within the zero page
    static constexpr byte_t zero_page_base_addr = 0x20;

    // Well within the zero page
    static constexpr addr_t absolute_base_addr = 0x0300;

    static constexpr std::array zero_page_scenarios = std::to_array<std::tuple<addr_t, byte_t>>(
        {
            /* Simple zero-page tests */
            {0x0200, 0x00}, /* PC in a common code area, val_addr at zero page start */
            {0x0200, 0xFF}, /* PC in a common code area, val_addr at zero page end */
            /* Boundary conditions */
            {0x01FF, 0x00}, /* PC just before zero page, val_addr at zero page start */
            {0x0200, 0x01}, /* PC in a common code area, val_addr just into zero page */
            {0x01FF, 0xFF}, /* PC just before zero page, val_addr at zero page end */
            {0xF000, 0x80}, /* Higher PC value, val_addr in middle of zero page */
            /* Testing PC at various points */
            {0x0000, 0x02}, /* PC at start of memory, testing very early execution */
            {0x8000, 0x04}, /* PC in a typical ROM area, val_addr early in zero page */
            {0xFFFC, 0xFE}, /* PC at the very end of memory space */
            /* Varied val_addr values to test LDA, LDX, LDY behavior */
            {0x0300, 0x10}, /* Common code area, testing nonzero page value */
            {0x0400, 0x20}  /* Another common code area, testing nonzero page value */
        });

    static constexpr std::array zero_page_indexed_scenarios = std::to_array<std::tuple<addr_t, byte_t, byte_t>>(
        {
            // Offset within zero-page without wrap: Tests basic indexed addressing within the zero page without crossing the zero-page boundary.
            {0x0200, 0x00, 0x01}, // PC in common area, base at start of zero page, offset +1.
            {0x0200, 0x10, 0x0F}, // PC in common area, base within zero page, offset to edge without wrapping.

            // Offset causing wrap-around: Validates correct handling of address wrap-around within the zero-page (255 + offset wraps to start of zero page).
            {0x0200, 0xFE, 0x01}, // PC in common area, base near end of zero page, +1 offset wraps.
            {0x0200, 0xFD, 0x03}, // PC in common area, base within zero page, +3 offset wraps.

            // Varied PC values with offset: Tests how indexed zero-page addressing behaves with different PC values, ensuring PC location doesn't affect indexed addressing calculation.
            {0x0000, 0x02, 0x01}, // PC at memory start, base early in zero page, small offset.
            {0x8000, 0x03, 0x02}, // PC in typical ROM area, base early in zero page, small offset.
            {0xFFFC, 0xFD, 0x02}  // PC at end of memory space, base in zero page, offset without wrap.
        });

    static constexpr std::array absolute_scenarios = std::to_array<std::tuple<addr_t, addr_t>>(
        {
            /* Absolute addressing tests in different memory regions */
            {0x0200, 0x0100}, /* PC in common code area, absolute address in lower memory */
            {0x0200, 0x8000}, /* PC in common code area, absolute address in upper memory */
            /* Boundary conditions */
            {0x01FF, 0x0000}, /* PC just before zero page, testing absolute address at memory start */
            {0x0200, 0xFFFF}, /* PC in common code area, testing absolute address at memory end */
            {0xF000, 0x4000}, /* Higher PC value, absolute address in the middle of memory */
            /* Testing PC at various points */
            {0x0000, 0x0200}, /* PC at start of memory, absolute address in common code area */
            {0x8000, 0x0300}, /* PC in a typical ROM area, absolute address in common code area */
            {0xFFFC, 0x0400}  /* PC at the very end of memory space, absolute address in common code area */
        });

    static constexpr std::array absolute_indexed_scenarios = std::to_array<std::tuple<addr_t, addr_t, byte_t>>(
        {
            /* Conceptual tests with "offsets" for absolute addressing */
            {0x0200, 0x0100, 0x01}, /* Common code area, lower memory with a byte offset */
            {0x0200, 0x8000, 0x10}, /* Common code area, upper memory with a byte offset */
            /* Varied PC values with "offsets" */
            {0x0000, 0x0200, 0x20}, /* PC at start, absolute address in common code area with a byte offset */
            {0x8000, 0x0300, 0x30}, /* PC in ROM area, absolute address in common code area with a byte offset */
            {0xFFFC, 0x0400, 0x40}  /* PC at end, absolute address in common code area with a byte offset */
        });

    static constexpr std::array positive_byte_values = std::to_array<byte_t>(
        {
            0x01, // The smallest value
            0x10, // A low value
            0x40, // A mid-range value
            0x7F  // The largest value
        });

    static constexpr std::array negative_byte_values = std::to_array<byte_t>(
        {
            0x80, // The smallest negative
            0x90, // A low negative value
            0xC0, // A mid-range value
            0xFF  // The largest value
        });

    fixture()
    {
        state.registers.a = 0xA;
        state.registers.x = 0xB;
        state.registers.y = 0xC;
    }

    static void set_register(state& state, register_id type, byte_t value)
    {
        switch (type)
        {
        case register_id::a:
            state.registers.a = value;
            break;

        case register_id::x:
            state.registers.x = value;
            break;

        case register_id::y:
            state.registers.y = value;
            break;
        }
    }

    void check_state(const state& expected_state, bool should_check_cycle = true) const
    {
        check_registers(expected_state);
        check_memory(expected_state);

        if (should_check_cycle)
            check_cycle(expected_state);
    }

    void check_registers(const state& expected_state) const
    {
        if (std::memcmp(&state.registers, &expected_state.registers, sizeof(registers)) != 0)
        {
            CHECK(state.registers.a == expected_state.registers.a);
            CHECK(state.registers.x == expected_state.registers.x);
            CHECK(state.registers.y == expected_state.registers.y);
            CHECK(state.registers.pc == expected_state.registers.pc);
            CHECK(state.registers.s == expected_state.registers.s);

            if (state.registers.p != expected_state.registers.p)
            {
                CHECK(state.registers.is_flag_set(status_flag::carry) == expected_state.registers.is_flag_set(status_flag::carry));
                CHECK(state.registers.is_flag_set(status_flag::zero) == expected_state.registers.is_flag_set(status_flag::zero));
                CHECK(state.registers.is_flag_set(status_flag::interrupt) == expected_state.registers.is_flag_set(status_flag::interrupt));
                CHECK(state.registers.is_flag_set(status_flag::decimal) == expected_state.registers.is_flag_set(status_flag::decimal));
                CHECK(state.registers.is_flag_set(status_flag::break_cmd) == expected_state.registers.is_flag_set(status_flag::break_cmd));
                CHECK(state.registers.is_flag_set(status_flag::unused) == expected_state.registers.is_flag_set(status_flag::unused));
                CHECK(state.registers.is_flag_set(status_flag::overflow) == expected_state.registers.is_flag_set(status_flag::overflow));
                CHECK(state.registers.is_flag_set(status_flag::negative) == expected_state.registers.is_flag_set(status_flag::negative));
            }
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

    void check_cycle(const state& expected_state) const
    {
        CHECK(state.cycle == expected_state.cycle);
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

constexpr std::string_view format_as(fixture::register_id type)
{
    switch (type)
    {
    case fixture::register_id::a:
        return "a";

    case fixture::register_id::x:
        return "x";

    case fixture::register_id::y:
        return "y";
    }

    NESE_ASSUME(false);
}

} // namespace nese::cpu::instruction
