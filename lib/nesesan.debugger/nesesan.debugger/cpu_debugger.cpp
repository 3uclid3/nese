#include <nese/utility/assert.hpp>
#include <nesesan.debugger/cpu_debugger.hpp>

namespace nese::san::debugger {

void cpu_debugger::update(f32_t dt)
{
    switch (_state)
    {
    case state::off:
    case state::paused:
    case state::stepped:
        break; // nothing to do

    case state::starting:
        update_starting(dt);
        break;

    case state::stopping:
        update_stopping(dt);
        break;

    case state::running:
        update_running(dt);
        break;

    case state::stepping:
        update_stepping(dt);
        break;
    }
}

void cpu_debugger::start(memory::rom&& rom)
{
    NESE_ASSERT(_state == state::off);

    _rom = std::move(rom);

    _state = state::starting;
}

void cpu_debugger::stop()
{
    NESE_ASSERT(_state != state::off);

    _state = state::stopping;
}

void cpu_debugger::step()
{
    NESE_ASSERT(_state == state::stepped);

    _state = state::stepping;
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

void cpu_debugger::update_starting(f32_t )
{
    _cpu.power_on();
    _ram.power_on();

    _cpu.set_code_addr(_rom.get_prg().size() == 0x4000 ? 0xc000 : 0x8000); // TODO Mapper

    _ram.set_bytes(0x8000, _rom.get_prg().data(), _rom.get_prg().size());

    if (_rom.get_prg().size() == 0x4000)
    {
        // "map" 0xC000 to 0x8000
        _ram.set_bytes(0xc000, _rom.get_prg().data(), _rom.get_prg().size());
    }

    _state = _mode == mode::step ? state::stepping : state::running;
}

void cpu_debugger::update_stopping(f32_t)
{
    _cpu.reset();
    _ram.reset();

    _state = state::off;
}

void cpu_debugger::update_running(f32_t)
{
    _cpu.step_to(_cpu.get_cycle() + cycle_t{1});
}

void cpu_debugger::update_stepping(f32_t)
{
    _cpu.step_to(_cpu.get_cycle() + cycle_t{1});

    _state = state::stepped;
}

} // namespace nese::san::debugger