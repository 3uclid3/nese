#pragma once

#include <array>
#include <tuple>

#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/execute.hpp>
#include <nese/cpu/instruction/execute_context_mock.hpp>
#include <nese/cpu/instruction/opcode.hpp>
#include <nese/utility/assert.hpp>

namespace nese::cpu::instruction {

static inline execute_context_mock default_execute_context_mock{[] {
    execute_context_mock ctx;

    ctx.registers().a = 0xA;
    ctx.registers().x = 0xB;
    ctx.registers().y = 0xC;

    byte_t value = 0xFF;

    for (std::size_t i = 0; i < memory::mapper::capacity; ++i)
    {
        ctx.memory().set_byte(static_cast<addr_t>(i), value);

        value = value == 0x01 ? 0xFF : value - 0x01;
    }

    return ctx;
}()};

class execute_fixture
{
public:
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

    static void set_register(registers& registers, register_id type, byte_t value)
    {
        switch (type)
        {
        case register_id::a:
            registers.a = value;
            break;

        case register_id::x:
            registers.x = value;
            break;

        case register_id::y:
            registers.y = value;
            break;
        }
    }

    void execute_and_check(opcode code, bool should_check_cycle = true) const;

    void check_registers() const;
    void check_memory() const;
    void check_cycle() const;

    [[nodiscard]] cpu::state& expected_state();
    [[nodiscard]] memory::mapper& expected_memory();

    [[nodiscard]] cpu::state& state();
    [[nodiscard]] memory::mapper& memory();

private:
    execute_context_mock& get_expected_context();

    execute_context_mock _context{default_execute_context_mock};
    execute_context_mock _expected_context{default_execute_context_mock};

    bool _expected_context_init{false};
};

constexpr std::string_view format_as(execute_fixture::register_id type)
{
    switch (type)
    {
    case execute_fixture::register_id::a:
        return "a";

    case execute_fixture::register_id::x:
        return "x";

    case execute_fixture::register_id::y:
        return "y";
    }

    NESE_ASSUME(false);
}

} // namespace nese::cpu::instruction
