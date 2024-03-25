#include <nesesan.debugger/cpu_debugger.hpp>

#include <nese/cpu/instruction.hpp>
#include <nese/utility/assert.hpp>

namespace nese::san::debugger {

void cpu_debugger::update(f32_t)
{
    if (_state != state::running)
    {
        return;
    }

    if (_mode == mode::until_pc && _until_pc == _cpu_state.registers.pc)
    {
        pause();
        return;
    }

    step();
}

void cpu_debugger::power_on(memory::rom&& rom, bool start_running)
{
    NESE_ASSERT(_state == state::off);

    _rom = std::move(rom);

    _memory.set_zero();
    _cpu_state = cpu::state{.memory = _memory};

    _cpu_state.registers.pc = _rom.get_prg().size() == 0x4000 ? 0xc000 : 0x8000; // TODO Mapper

    _memory.set_bytes(0x8000, _rom.get_prg().data(), _rom.get_prg().size());

    if (_rom.get_prg().size() == 0x4000)
    {
        // "map" 0xC000 to 0x8000
        _memory.set_bytes(0xc000, _rom.get_prg().data(), _rom.get_prg().size());
    }

    // initial state after loading
    _cpu_snapshotter.reset();
    _cpu_snapshotter.take_snapshot(_cpu_state);

    _state = start_running ? state::running : state::paused;
}

void cpu_debugger::reset()
{
    NESE_ASSERT(_state != state::off);

    _memory.set_zero();
    _cpu_state = cpu::state{.memory = _memory};

    _state = state::off;
}

void cpu_debugger::step()
{
    const auto opcode = _memory.get_byte(_cpu_state.registers.pc++);
    cpu::instruction::execute(opcode, _cpu_state);

    _cpu_snapshotter.take_snapshot(_cpu_state);
}

void cpu_debugger::pause()
{
    NESE_ASSERT(_state == state::running);

    _state = state::paused;
}

void cpu_debugger::unpause()
{
    NESE_ASSERT(_state == state::paused);

    _state = state::running;
}

} // namespace nese::san::debugger