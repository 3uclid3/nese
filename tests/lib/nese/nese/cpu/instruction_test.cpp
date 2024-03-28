#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <nese/cpu/fixture.hpp>
#include <nese/cpu/instruction.hpp>
#include <nese/cpu/state_mock.hpp>
#include <nese/memory/mapper.hpp>
#include <nese/utility/assert.hpp>

// clang-format off
#define GENERATE_ADDR() \
            GENERATE(0x0000, 0xFFFF, \
                     0x00FE, 0x01FE, \
                     0x7FFE, 0xFFFE, \
                     0x00FF, 0x0100)

#define GENERATE_NEGATIVE_BYTE() \
            GENERATE(0x80, \
                     0x81, \
                     0xC0, \
                     0xFE, \
                     0xFF)

#define GENERATE_POSITIVE_BYTE() \
            GENERATE(0x01, \
                     0x10, \
                     0x40, \
                     0x7E, \
                     0x7F)

#define GENERATE_ADDR_FOR_ZERO_PAGE() \
     GENERATE(table<addr_t, byte_t>({ \
                /* Simple zero-page tests */ \
                {0x0200, 0x00}, /* PC in a common code area, val_addr at zero page start */ \
                {0x0200, 0xFF}, /* PC in a common code area, val_addr at zero page end */ \
                /* Boundary conditions */ \
                {0x01FF, 0x00}, /* PC just before zero page, val_addr at zero page start */ \
                {0x0200, 0x01}, /* PC in a common code area, val_addr just into zero page */ \
                {0x01FF, 0xFF}, /* PC just before zero page, val_addr at zero page end */ \
                {0xF000, 0x80}, /* Higher PC value, val_addr in middle of zero page */ \
                /* Testing PC at various points */ \
                {0x0000, 0x02}, /* PC at start of memory, testing very early execution */ \
                {0x8000, 0x04}, /* PC in a typical ROM area, val_addr early in zero page */ \
                {0xFFFC, 0xFE}, /* PC at the very end of memory space */ \
                /* Varied val_addr values to test LDA, LDX, LDY behavior */ \
                {0x0300, 0x10}, /* Common code area, testing nonzero page value */ \
                {0x0400, 0x20}  /* Another common code area, testing nonzero page value */ \
            }))


#define GENERATE_ADDR_AND_OFFSET_FOR_ZERO_PAGE() \
     GENERATE(table<addr_t, byte_t, byte_t>({ \
                /* Offset within zero-page without wrap */ \
                {0x0200, 0x00, 0x01}, \
                {0x0200, 0x10, 0x0F}, \
                /* Offset causing wrap-around */ \
                {0x0200, 0xFE, 0x01}, \
                {0x0200, 0xFD, 0x03}, \
                /* Varied PC values with offset */ \
                {0x0000, 0x02, 0x01}, \
                {0x8000, 0x03, 0x02}, \
                {0xFFFC, 0xFD, 0x02} \
            }))

#define GENERATE_ADDR_FOR_ABSOLUTE() \
     GENERATE(table<addr_t, addr_t>({ \
                /* Absolute addressing tests in different memory regions */ \
                {0x0200, 0x0100}, /* PC in common code area, absolute address in lower memory */ \
                {0x0200, 0x8000}, /* PC in common code area, absolute address in upper memory */ \
                /* Boundary conditions */ \
                {0x01FF, 0x0000}, /* PC just before zero page, testing absolute address at memory start */ \
                {0x0200, 0xFFFF}, /* PC in common code area, testing absolute address at memory end */ \
                {0xF000, 0x4000}, /* Higher PC value, absolute address in the middle of memory */ \
                /* Testing PC at various points */ \
                {0x0000, 0x0200}, /* PC at start of memory, absolute address in common code area */ \
                {0x8000, 0x0300}, /* PC in a typical ROM area, absolute address in common code area */ \
                {0xFFFC, 0x0400}  /* PC at the very end of memory space, absolute address in common code area */ \
            }))

#define GENERATE_ADDR_AND_OFFSET_FOR_ABSOLUTE() \
     GENERATE(table<addr_t, addr_t, byte_t>({ \
                /* Conceptual tests with "offsets" for absolute addressing */ \
                {0x0200, 0x0100, 0x01}, /* Common code area, lower memory with a byte offset */ \
                {0x0200, 0x8000, 0x10}, /* Common code area, upper memory with a byte offset */ \
                /* Varied PC values with "offsets" */ \
                {0x0000, 0x0200, 0x20}, /* PC at start, absolute address in common code area with a byte offset */ \
                {0x8000, 0x0300, 0x30}, /* PC in ROM area, absolute address in common code area with a byte offset */ \
                {0xFFFC, 0x0400, 0x40}  /* PC at end, absolute address in common code area with a byte offset */ \
            }))

// clang-format on

namespace nese::cpu::instruction {

void set_register_a(registers& r, byte_t a)
{
    r.a = a;
}

void set_register_x(registers& r, byte_t x)
{
    r.x = x;
}

void set_register_y(registers& r, byte_t y)
{
    r.y = y;
}

struct in_fixture : fixture
{
    template<typename InstructionExecuteT, typename SetRegisterFunctorT>
    void test_implied(const InstructionExecuteT& execute, const SetRegisterFunctorT& set_register)
    {
        SECTION("implied")
        {
            SECTION("increment 0xFF results in overflow to 0x00")
            {
                set_register(state.registers, 0xFF);

                state_mock expected_state = state;
                set_register(expected_state.registers, 0x00);
                expected_state.registers.set_flag(status_flag::zero, true);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state);
            }

            SECTION("increment 0x7F results in overflow to 0x80")
            {
                set_register(state.registers, 0x7F);

                state_mock expected_state = state;
                set_register(expected_state.registers, 0x80);
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, true);

                execute(state);

                check_state(expected_state);
            }

            SECTION("increment negative")
            {
                const byte_t value = GENERATE(0x80,
                                              0xC0,
                                              0xFE);

                set_register(state.registers, value);

                state_mock expected_state = state;
                set_register(expected_state.registers, value + 1);
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, true);

                execute(state);

                check_state(expected_state);
            }

            SECTION("increment positive")
            {
                const byte_t value = GENERATE(0x00,
                                              0x10,
                                              0x40,
                                              0x7E);

                set_register(state.registers, value);

                state_mock expected_state = state;
                set_register(expected_state.registers, value + 1);
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state);
            }
        }
    }
};

TEST_CASE_METHOD(in_fixture, "inx", "[cpu][instruction]")
{
    test_implied(execute_inx<addr_mode::implied>, set_register_x);
}

TEST_CASE_METHOD(in_fixture, "iny", "[cpu][instruction]")
{
    test_implied(execute_iny<addr_mode::implied>, set_register_y);
}

struct jmp_fixture : fixture
{
    template<typename ExecuteFunctorT>
    void test_absolute(const ExecuteFunctorT& execute)
    {
        SECTION("absolute")
        {
            const auto [addr, addr_to] = GENERATE_ADDR_FOR_ABSOLUTE();

            state.registers.pc = addr;
            state.owned_memory.set_word(addr, addr_to);

            state_mock expected_state = state;
            expected_state.registers.pc = addr_to;

            execute(state);

            check_state(expected_state);
        }
    }
};

TEST_CASE_METHOD(jmp_fixture, "jmp", "[cpu][instruction]")
{
    test_absolute(execute_jmp<addr_mode::absolute>);
}

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

                check_state(expected_state);
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

                check_state(expected_state);
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

                check_state(expected_state);
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

                check_state(expected_state);
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

                check_state(expected_state);
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

                check_state(expected_state);
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
};

TEST_CASE_METHOD(st_fixture, "stx", "[cpu][instruction]")
{
    test_zero_page(execute_stx<addr_mode::zero_page>, set_register_x);
    test_zero_page_y(execute_stx<addr_mode::zero_page_y>, set_register_x);
}

TEST_CASE_METHOD(st_fixture, "sty", "[cpu][instruction]")
{
    test_zero_page(execute_sty<addr_mode::zero_page>, set_register_y);
    test_zero_page_x(execute_sty<addr_mode::zero_page_x>, set_register_y);
}

} // namespace nese::cpu::instruction