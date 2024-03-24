#include <nese/utility/assert.hpp>
#include <nesesan.debugger/cpu_debugger.hpp>

namespace nese::san::debugger {

void cpu_debugger::update(f32_t)
{
    if (_state != state::running)
    {
        return;
    }

    if (_mode == mode::until_pc && _until_pc == _cpu.get_registers().pc)
    {
        pause();
        return;
    }

    _cpu.step();
    _cpu_snapshotter.take_snapshot(_cpu);
}

void cpu_debugger::power_on(memory::rom&& rom, bool start_running)
{
    NESE_ASSERT(_state == state::off);

    _rom = std::move(rom);

    _cpu.power_on();
    _ram.power_on();

    _cpu.set_code_addr(_rom.get_prg().size() == 0x4000 ? 0xc000 : 0x8000); // TODO Mapper

    _ram.set_bytes(0x8000, _rom.get_prg().data(), _rom.get_prg().size());

    if (_rom.get_prg().size() == 0x4000)
    {
        // "map" 0xC000 to 0x8000
        _ram.set_bytes(0xc000, _rom.get_prg().data(), _rom.get_prg().size());
    }

    // initial state after loading
    _cpu_snapshotter.reset();
    _cpu_snapshotter.take_snapshot(_cpu);

    _state = start_running ? state::running : state::paused;
}

void cpu_debugger::reset()
{
    NESE_ASSERT(_state != state::off);

    _cpu.reset();
    _ram.reset();

    _state = state::off;
}

void cpu_debugger::step()
{
    NESE_ASSERT(_state == state::paused);

    _cpu.step();
    _cpu_snapshotter.take_snapshot(_cpu);
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