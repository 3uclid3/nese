#include <nese/cpu/instruction/fixture/execute_fixture.hpp>

namespace nese::cpu::instruction {

void execute_fixture::execute_and_check(opcode code, bool should_check_cycle) const
{
    execute(execute_context(_context), code);

    check_registers();
    check_memory();

    if (should_check_cycle)
    {
        check_cycle();
    }
}

void execute_fixture::check_registers() const
{
    CHECK(_context.state().registers.a == _expected_context.state().registers.a);
    CHECK(_context.state().registers.x == _expected_context.state().registers.x);
    CHECK(_context.state().registers.y == _expected_context.state().registers.y);
    CHECK(_context.state().registers.pc == _expected_context.state().registers.pc);
    CHECK(_context.state().registers.s == _expected_context.state().registers.s);

    if (_context.state().registers.p != _expected_context.state().registers.p)
    {
        CHECK(_context.state().registers.is_flag_set(status_flag::carry) == _expected_context.state().registers.is_flag_set(status_flag::carry));
        CHECK(_context.state().registers.is_flag_set(status_flag::zero) == _expected_context.state().registers.is_flag_set(status_flag::zero));
        CHECK(_context.state().registers.is_flag_set(status_flag::interrupt) == _expected_context.state().registers.is_flag_set(status_flag::interrupt));
        CHECK(_context.state().registers.is_flag_set(status_flag::decimal) == _expected_context.state().registers.is_flag_set(status_flag::decimal));
        CHECK(_context.state().registers.is_flag_set(status_flag::break_cmd) == _expected_context.state().registers.is_flag_set(status_flag::break_cmd));
        CHECK(_context.state().registers.is_flag_set(status_flag::unused) == _expected_context.state().registers.is_flag_set(status_flag::unused));
        CHECK(_context.state().registers.is_flag_set(status_flag::overflow) == _expected_context.state().registers.is_flag_set(status_flag::overflow));
        CHECK(_context.state().registers.is_flag_set(status_flag::negative) == _expected_context.state().registers.is_flag_set(status_flag::negative));
    }
}

void execute_fixture::check_memory() const
{
    const auto& expected_memory_buffer = _expected_context.memory().get_bytes();
    const auto& memory_buffer = _context.memory().get_bytes();

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
    CHECK(_context.state().cycle == _expected_context.state().cycle);
}

cpu::state& execute_fixture::expected_state()
{
    return get_expected_context().state();
}

memory::mapper& execute_fixture::expected_memory()
{
    return get_expected_context().memory();
}

cpu::state& execute_fixture::state()
{
    NESE_ASSERT(!_expected_context_init);
    return _context.state();
}

memory::mapper& execute_fixture::memory()
{
    NESE_ASSERT(!_expected_context_init);
    return _context.memory();
}

execute_context_mock& execute_fixture::get_expected_context()
{
    if (!_expected_context_init)
    {
        _expected_context = _context;
        _expected_context_init = true;
    }

    return _expected_context;
}

} // namespace nese::cpu::instruction
