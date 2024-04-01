#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

#include <catch2/catch_test_macros.hpp>

#include <nese/cpu/instruction/execute_context.hpp>

namespace nese::cpu::instruction {

void execute_fixture::execute_and_check(opcode code, bool should_check_cycle)
{
    REQUIRE(execute(execute_context(_state, _memory), code));

    check_registers();
    check_memory();

    if (should_check_cycle)
    {
        check_cycle();
    }
}

void execute_fixture::check_registers() const
{
    CHECK(_state.registers.a == _expected_state.registers.a);
    CHECK(_state.registers.x == _expected_state.registers.x);
    CHECK(_state.registers.y == _expected_state.registers.y);
    CHECK(_state.registers.pc == _expected_state.registers.pc);
    CHECK(_state.registers.s == _expected_state.registers.s);

    if (_state.registers.p != _expected_state.registers.p)
    {
        CHECK(_state.registers.is_flag_set(status_flag::carry) == _expected_state.registers.is_flag_set(status_flag::carry));
        CHECK(_state.registers.is_flag_set(status_flag::zero) == _expected_state.registers.is_flag_set(status_flag::zero));
        CHECK(_state.registers.is_flag_set(status_flag::interrupt) == _expected_state.registers.is_flag_set(status_flag::interrupt));
        CHECK(_state.registers.is_flag_set(status_flag::decimal) == _expected_state.registers.is_flag_set(status_flag::decimal));
        CHECK(_state.registers.is_flag_set(status_flag::break_cmd) == _expected_state.registers.is_flag_set(status_flag::break_cmd));
        CHECK(_state.registers.is_flag_set(status_flag::unused) == _expected_state.registers.is_flag_set(status_flag::unused));
        CHECK(_state.registers.is_flag_set(status_flag::overflow) == _expected_state.registers.is_flag_set(status_flag::overflow));
        CHECK(_state.registers.is_flag_set(status_flag::negative) == _expected_state.registers.is_flag_set(status_flag::negative));
    }
}

void execute_fixture::check_memory() const
{
    const auto& expected_memory_buffer = _expected_memory.get_bytes();
    const auto& memory_buffer = _memory.get_bytes();

    CHECK(expected_memory_buffer.size() == memory_buffer.size());

    if (std::memcmp(memory_buffer.data(), expected_memory_buffer.data(), memory_buffer.size()) != 0)
    {
        std::string fail_message = "expected_state.memory != state.memory\n";

        for (size_t addr = 0; addr < memory_buffer.size() - 1; ++addr)
        {
            if (memory_buffer[addr] != expected_memory_buffer[addr])
            {
                fmt::format_to(std::back_inserter(fail_message), "  Mismatch at address 0x{:04X}: expected 0x{:02X}, actual 0x{:02X}\n", static_cast<addr_t>(addr), expected_memory_buffer[addr], memory_buffer[addr]);
            }
        }

        FAIL_CHECK(fail_message.c_str());
    }
}

void execute_fixture::check_cycle() const
{
    CHECK(_state.cycle == _expected_state.cycle);
}

cpu::state& execute_fixture::expected_state()
{
    init_expected();
    return _expected_state;
}

memory::mapper& execute_fixture::expected_memory()
{
    init_expected();
    return _expected_memory;
}

cpu::state& execute_fixture::state()
{
    NESE_ASSERT(!_expected_init);
    return _state;
}

memory::mapper& execute_fixture::memory()
{
    NESE_ASSERT(!_expected_init);
    return _memory;
}

void execute_fixture::init_expected()
{
    if (!_expected_init)
    {
        _expected_state = _state;
        _expected_memory = _memory;
        _expected_init = true;
    }
}

} // namespace nese::cpu::instruction
