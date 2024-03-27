#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/state_mock.hpp>
#include <nese/memory/mapper.hpp>
#include <nese/utility/assert.hpp>

// clang-format off
#define GENERATE_ADDR() \
            GENERATE(0x0000, 0xFFFF,  \
                     0x00FF, 0x0100,  \
                     0x0001, 0x0010, 0x0100, 0x1000,  \
                     0x000F, 0x00F0, 0x0F00, 0xF000)

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
// clang-format on

namespace nese::cpu::instruction {

template<addr_mode AddrModeT>
void encode_operand_addr(state& state, word_t value);

template<>
void encode_operand_addr<addr_mode::zero_page>(state& state, word_t value)
{
    state.memory.get().set_byte(state.registers.pc, static_cast<byte_t>(value));
}

template<>
void encode_operand_addr<addr_mode::zero_page_x>(state& state, word_t value)
{
    state.memory.get().set_byte(state.registers.pc, (static_cast<byte_t>(value) + state.registers.x) & 0xff);
}

template<>
void encode_operand_addr<addr_mode::zero_page_y>(state& state, word_t value)
{
    state.memory.get().set_byte(state.registers.pc, (static_cast<byte_t>(value) + state.registers.y) & 0xff);
}

template<>
void encode_operand_addr<addr_mode::absolute>(state& state, word_t value)
{
    state.memory.get().set_word(state.registers.pc, value);
}

template<>
void encode_operand_addr<addr_mode::absolute_x>(state& state, word_t value)
{
    state.memory.get().set_word(state.registers.pc + state.registers.x, value);
}

template<>
void encode_operand_addr<addr_mode::absolute_y>(state& state, word_t value)
{
    state.memory.get().set_word(state.registers.pc + state.registers.y, value);
}

template<addr_mode AddrModeT>
void encode_operand(state& state, operand_t operand)
{
    if constexpr (AddrModeT == addr_mode::accumulator)
    {
        // Nothing to do
        // return 0;
    }
    else if constexpr (AddrModeT == addr_mode::immediate)
    {
        state.memory.get().set_byte(state.registers.pc, operand);
    }
    else
    {
        encode_operand_addr<AddrModeT>(state, operand);
    }
}

template<addr_mode AddrModeT>
void write_operand(state& state, byte_t value)
{
    if constexpr (AddrModeT == addr_mode::accumulator)
    {
        state.registers.a = value;
    }
    else if constexpr (AddrModeT == addr_mode::immediate)
    {
        // Nothing to do
        // return operand;
    }
    else
    {
        state.memory.get().set_byte(state.registers.pc + 1, value);
    }
}

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

state_mock create_default_state()
{
    state_mock state;

    byte_t value = 0;
    for (addr_t i = 0; i < static_cast<addr_t>(memory::mapper::capacity - 1); ++i)
    {
        state.owned_memory.set_byte(i, value);

        value = value == 0xFF ? 0x00 : value + 0x01;
    }

    return state;
}

state_mock default_state{create_default_state()};

struct fixture
{
    static constexpr auto minus_two = static_cast<byte_t>(-2);
    static constexpr auto minus_one = static_cast<byte_t>(-1);

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

    state_mock state{default_state};
};

struct in_fixture : fixture
{
    template<typename ExecuteFunctorT, typename SetRegisterFunctorT>
    void test_instruction(const char* section, const ExecuteFunctorT& execute, const SetRegisterFunctorT& set_register)
    {
        SECTION(section)
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
    }
};

TEST_CASE_METHOD(in_fixture, "inx", "[cpu][instruction]")
{
    test_instruction("implied", execute_inx<addr_mode::implied>, set_register_x);
}

TEST_CASE_METHOD(in_fixture, "iny", "[cpu][instruction]")
{
    test_instruction("implied", execute_iny<addr_mode::implied>, set_register_y);
}

struct jmp_fixture : fixture
{
    template<addr_mode AddrModeT>
    void test_instruction()
    {
        DYNAMIC_SECTION(to_string_view(AddrModeT))
        {
            const addr_t addr = GENERATE_ADDR();
            const addr_t addr_to = GENERATE_ADDR();

            state.registers.pc = addr;
            encode_operand_addr<AddrModeT>(state, addr_to);

            state_mock expected_state = state;
            expected_state.registers.pc = addr_to;

            execute_jmp<AddrModeT>(state);

            check_state(expected_state);
        }
    }
};

TEST_CASE_METHOD(jmp_fixture, "jmp", "[cpu][instruction]")
{
    test_instruction<addr_mode::absolute>();
}

struct ld_fixture : fixture
{
    template<addr_mode AddrModeT, typename ExecuteFunctorT, typename SetRegisterFunctorT>
    void test_instruction(const ExecuteFunctorT& execute, const SetRegisterFunctorT& set_register)
    {
        DYNAMIC_SECTION(to_string_view(AddrModeT))
        {
            const addr_t addr = GENERATE_ADDR();

            DYNAMIC_SECTION(fmt::format("load a zero pc = 0x{:04X}", addr))
            {
                state.registers.pc = addr;
                encode_operand<AddrModeT>(state, 0);
                write_operand<AddrModeT>(state, 0);

                state_mock expected_state = state;
                set_register(expected_state.registers, 0);
                expected_state.registers.pc = addr + get_addr_mode_operand_byte_count(AddrModeT);
                expected_state.registers.set_flag(status_flag::zero, true);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state);
            }

            DYNAMIC_SECTION(fmt::format("load a negative pc = 0x{:04X}", addr))
            {
                const byte_t value = GENERATE_NEGATIVE_BYTE();

                state.registers.pc = addr;
                write_operand<AddrModeT>(state, value);

                state_mock expected_state = state;
                set_register(expected_state.registers, value);

                expected_state.registers.pc = addr + get_addr_mode_operand_byte_count(AddrModeT);
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, true);

                execute(state);

                check_state(expected_state);
            }

            DYNAMIC_SECTION(fmt::format("load a positive pc = 0x{:04X}", addr))
            {
                const byte_t value = GENERATE_POSITIVE_BYTE();

                state.registers.pc = addr;
                write_operand<AddrModeT>(state, value);

                state_mock expected_state = state;
                set_register(expected_state.registers, value);
                expected_state.registers.pc = addr + get_addr_mode_operand_byte_count(AddrModeT);
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
    // test_instruction<addr_mode::immediate>(execute_lda<addr_mode::immediate>, set_register_a);
    // test_instruction<addr_mode::zero_page>(execute_lda<addr_mode::zero_page>, set_register_a);
    // test_instruction<addr_mode::zero_page_x>(execute_lda<addr_mode::zero_page_x>, set_register_a);
    // test_instruction<addr_mode::absolute>(execute_lda<addr_mode::absolute>, set_register_a);
    // test_instruction<addr_mode::absolute_x>(execute_lda<addr_mode::absolute_x>, set_register_a);
    // test_instruction<addr_mode::absolute_y>(execute_lda<addr_mode::absolute_y>, set_register_a);
    // test_instruction<addr_mode::indexed_indirect>(execute_lda<addr_mode::immediate>, set_register_a);
    // test_instruction<addr_mode::indirect_indexed>(execute_lda<addr_mode::immediate>, set_register_a);
}

TEST_CASE_METHOD(ld_fixture, "ldx", "[cpu][instruction][!mayfail]")
{
    // test_instruction<addr_mode::immediate>(execute_ldx<addr_mode::immediate>, set_register_x);
    // test_instruction<addr_mode::zero_page>(execute_ldx<addr_mode::zero_page>, set_register_x);
    // test_instruction<addr_mode::zero_page_y>(execute_ldx<addr_mode::zero_page_y>, set_register_x);
    // test_instruction<addr_mode::absolute>(execute_ldx<addr_mode::absolute>, set_register_x);
    // test_instruction<addr_mode::absolute_y>(execute_ldx<addr_mode::absolute_y>, set_register_x);
}

TEST_CASE_METHOD(ld_fixture, "ldy", "[cpu][instruction][!mayfail]")
{
    // test_instruction<addr_mode::immediate>(execute_ldy<addr_mode::immediate>, set_register_y);
    // test_instruction<addr_mode::zero_page>(execute_ldy<addr_mode::zero_page>, set_register_y);
    // test_instruction<addr_mode::zero_page_x>(execute_ldy<addr_mode::zero_page_x>, set_register_y);
    // test_instruction<addr_mode::absolute>(execute_ldy<addr_mode::absolute>, set_register_y);
    // test_instruction<addr_mode::absolute_x>(execute_ldy<addr_mode::absolute_x>, set_register_y);
}

struct st_fixture : fixture
{
    st_fixture()
    {
        state.registers.a = 0xA;
        state.registers.x = 0xB;
        state.registers.y = 0xC;
    }

    template<addr_mode AddrModeT, typename ExecuteFunctorT, typename SetRegisterFunctorT>
    void test_instruction(const ExecuteFunctorT& execute, const SetRegisterFunctorT& set_register)
    {
        DYNAMIC_SECTION(to_string_view(AddrModeT))
        {
            const addr_t addr = GENERATE_ADDR();

            DYNAMIC_SECTION(fmt::format("store a zero at address 0x{:04X}", addr))
            {
                state.registers.pc = addr;
                set_register(state.registers, 0);

                state_mock expected_state = state;
                encode_operand_addr<AddrModeT>(expected_state, 0);
                expected_state.registers.pc = addr + get_addr_mode_operand_byte_count(AddrModeT);

                execute(state);

                check_state(expected_state);
            }

            DYNAMIC_SECTION(fmt::format("store a negative at address 0x{:04X}", addr))
            {
                const byte_t value = GENERATE_NEGATIVE_BYTE();
            
                state.registers.pc = addr;
                set_register(state.registers, value);
            
                state_mock expected_state = state;
                encode_operand_addr<AddrModeT>(expected_state, value);
                expected_state.registers.pc = addr + get_addr_mode_operand_byte_count(AddrModeT);
            
                execute(state);
            
                check_state(expected_state);
            }
            
            DYNAMIC_SECTION(fmt::format("store a positive at address 0x{:04X}", addr))
            {
                const byte_t value = GENERATE_POSITIVE_BYTE();
            
                state.registers.pc = addr;
                set_register(state.registers, value);
            
                state_mock expected_state = state;
                encode_operand_addr<AddrModeT>(expected_state, value);
                expected_state.registers.pc = addr + get_addr_mode_operand_byte_count(AddrModeT);
            
                execute(state);
            
                check_state(expected_state);
            }
        }
    }
};

TEST_CASE_METHOD(st_fixture, "stx", "[cpu][instruction][!mayfail]")
{
    // test_instruction<addr_mode::zero_page>(execute_stx<addr_mode::zero_page>, set_register_x);
    // test_instruction<addr_mode::zero_page_y>(execute_stx<addr_mode::zero_page_y>, set_register_x);
    // test_instruction<addr_mode::absolute>(execute_stx<addr_mode::absolute>, set_register_x);
}

TEST_CASE_METHOD(st_fixture, "sty", "[cpu][instruction][!mayfail]")
{
    // test_instruction<addr_mode::zero_page>(execute_sty<addr_mode::zero_page>, set_register_x);
    // test_instruction<addr_mode::zero_page_x>(execute_sty<addr_mode::zero_page_x>, set_register_y);
    // test_instruction<addr_mode::absolute>(execute_sty<addr_mode::absolute>, set_register_y);
}

} // namespace nese::cpu::instruction