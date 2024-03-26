#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/cpu/state_mock.hpp>
#include <nese/memory/mapper.hpp>
#include <nese/utility/assert.hpp>

namespace nese::cpu::instruction {

template<addr_mode AddrModeT>
void set_operand_addr(state& state, word_t value);

template<>
void set_operand_addr<addr_mode::zero_page>(state& state, word_t value)
{
    state.memory.get().set_byte(state.registers.pc, static_cast<byte_t>(value));
}

template<>
void set_operand_addr<addr_mode::zero_page_x>(state& state, word_t value)
{
    state.memory.get().set_byte(state.registers.pc + state.registers.x, static_cast<byte_t>(value));
}

template<>
void set_operand_addr<addr_mode::zero_page_y>(state& state, word_t value)
{
    state.memory.get().set_byte(state.registers.pc + state.registers.y, static_cast<byte_t>(value));
}

template<>
void set_operand_addr<addr_mode::absolute>(state& state, word_t value)
{
    state.memory.get().set_word(state.registers.pc, value);
}

template<>
void set_operand_addr<addr_mode::absolute_x>(state& state, word_t value)
{
    state.memory.get().set_word(state.registers.pc + state.registers.x, value);
}

template<>
void set_operand_addr<addr_mode::absolute_y>(state& state, word_t value)
{
    state.memory.get().set_word(state.registers.pc + state.registers.y, value);
}

template<addr_mode AddrModeT>
void set_operand(state& state, operand_t value)
{
    if constexpr (AddrModeT == addr_mode::accumulator)
    {
        state.registers.a = value;
    }
    else if constexpr (AddrModeT == addr_mode::immediate)
    {
        state.memory.get().set_byte(state.registers.pc, value);
    }
    else
    {
        set_operand_addr<AddrModeT>(state, value);
    }
}

// template<>
// void set_operand_addr<addr_mode::indexed_indirect>(state& state, word_t value)
// {
//     const memory::mapper& memory = state.memory;
//     const byte_t addr = decode_byte(state);
//     return memory.get_byte((addr + state.registers.x) & 0xff) + static_cast<uint16_t>((memory.get_byte((addr + state.registers.x + 1) & 0xff)) << 8);
// }
//
// template<>
// void set_operand_addr<addr_mode::indirect_indexed>(state& state, word_t value)
// {
//     const memory::mapper& memory = state.memory;
//     const byte_t addr = decode_byte(state);
//     return memory.get_byte(addr) + (static_cast<uint16_t>((memory.get_byte(addr + 1) & 0xff)) << 8) + state.registers.y;
// }

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

    state_mock state{};
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
    void test_instruction(const char* name, addr_t addr, addr_t jmp_to)
    {
        SECTION(name)
        {
            state.registers.pc = addr;
            set_operand_addr<AddrModeT>(state, jmp_to);

            state_mock expected_state = state;
            expected_state.registers.pc = jmp_to;

            execute_jmp<AddrModeT>(state);

            check_state(expected_state);
        }
    }
};

TEST_CASE_METHOD(jmp_fixture, "jmp", "[cpu][instruction]")
{
    SECTION("absolute")
    {
        test_instruction<addr_mode::absolute>("pc = 0x0000 to 0xFFFF", 0x0000, 0xFFFF);
        test_instruction<addr_mode::absolute>("pc = 0xFFFF to 0x0000", 0xFFFF, 0x0000);

        test_instruction<addr_mode::absolute>("pc = 0x0001 to 0xDEAD", 0x0001, 0xDEAD);
        test_instruction<addr_mode::absolute>("pc = 0x0010 to 0xDEAD", 0x0010, 0xDEAD);
        test_instruction<addr_mode::absolute>("pc = 0x0100 to 0xDEAD", 0x0100, 0xDEAD);
        test_instruction<addr_mode::absolute>("pc = 0x1000 to 0xDEAD", 0x1000, 0xDEAD);

        test_instruction<addr_mode::absolute>("pc = 0x000F to 0xDEAD", 0x000F, 0xDEAD);
        test_instruction<addr_mode::absolute>("pc = 0x00F0 to 0xDEAD", 0x00F0, 0xDEAD);
        test_instruction<addr_mode::absolute>("pc = 0x0F00 to 0xDEAD", 0x0F00, 0xDEAD);
        test_instruction<addr_mode::absolute>("pc = 0xF000 to 0xDEAD", 0xF000, 0xDEAD);
    }
}

struct ld_fixture : fixture
{
    template<addr_mode AddrModeT, typename ExecuteFunctorT, typename SetRegisterFunctorT>
    void test_instruction(const ExecuteFunctorT& execute, const SetRegisterFunctorT& set_register)
    {
        DYNAMIC_SECTION(to_string_view(AddrModeT))
        {
            // clang-format off
            const addr_t addr = GENERATE(0x0000, 0xFFFF,
                                         0x0001, 0x0010, 0x0100, 0x1000,
                                         0x000F, 0x00F0, 0x0F00, 0xF000);
            // clang-format on

            SECTION("load a negative")
            {
                state.registers.pc = addr;
                set_operand<AddrModeT>(state, minus_one);

                state_mock expected_state = state;
                set_register(expected_state.registers, minus_one);

                expected_state.registers.pc = addr + get_addr_mode_operand_byte_count(AddrModeT);
                expected_state.registers.set_flag(status_flag::zero, false);
                expected_state.registers.set_flag(status_flag::negative, true);

                execute(state);

                check_state(expected_state);
            }

            SECTION("load a zero")
            {
                state.registers.pc = addr;
                set_operand<AddrModeT>(state, 0);

                state_mock expected_state = state;
                set_register(expected_state.registers, 0);
                expected_state.registers.pc = addr + get_addr_mode_operand_byte_count(AddrModeT);
                expected_state.registers.set_flag(status_flag::zero, true);
                expected_state.registers.set_flag(status_flag::negative, false);

                execute(state);

                check_state(expected_state);
            }

            SECTION("load a positive")
            {
                state.registers.pc = addr;
                set_operand<AddrModeT>(state, 1);

                state_mock expected_state = state;
                set_register(expected_state.registers, 1);
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
    test_instruction<addr_mode::immediate>(execute_lda<addr_mode::immediate>, set_register_a);
    test_instruction<addr_mode::zero_page>(execute_lda<addr_mode::zero_page>, set_register_a);
    test_instruction<addr_mode::zero_page_x>(execute_lda<addr_mode::zero_page_x>, set_register_a);
    test_instruction<addr_mode::absolute>(execute_lda<addr_mode::absolute>, set_register_a);
    test_instruction<addr_mode::absolute_x>(execute_lda<addr_mode::absolute_x>, set_register_a);
    test_instruction<addr_mode::absolute_y>(execute_lda<addr_mode::absolute_y>, set_register_a);
    // test_instruction<addr_mode::indexed_indirect>(execute_lda<addr_mode::immediate>, set_register_a);
    // test_instruction<addr_mode::indirect_indexed>(execute_lda<addr_mode::immediate>, set_register_a);
}

TEST_CASE_METHOD(ld_fixture, "ldx", "[cpu][instruction]")
{
    test_instruction<addr_mode::immediate>(execute_ldx<addr_mode::immediate>, set_register_x);
    test_instruction<addr_mode::zero_page>(execute_ldx<addr_mode::zero_page>, set_register_x);
    test_instruction<addr_mode::zero_page_y>(execute_ldx<addr_mode::zero_page_y>, set_register_x);
    test_instruction<addr_mode::absolute>(execute_ldx<addr_mode::absolute>, set_register_x);
    test_instruction<addr_mode::absolute_y>(execute_ldx<addr_mode::absolute_y>, set_register_x);
}

TEST_CASE_METHOD(ld_fixture, "ldy", "[cpu][instruction]")
{
    test_instruction<addr_mode::immediate>(execute_ldy<addr_mode::immediate>, set_register_y);
    test_instruction<addr_mode::zero_page>(execute_ldy<addr_mode::zero_page>, set_register_y);
    test_instruction<addr_mode::zero_page_x>(execute_ldy<addr_mode::zero_page_x>, set_register_y);
    test_instruction<addr_mode::absolute>(execute_ldy<addr_mode::absolute>, set_register_y);
    test_instruction<addr_mode::absolute_x>(execute_ldy<addr_mode::absolute_x>, set_register_y);
}

} // namespace nese::cpu::instruction