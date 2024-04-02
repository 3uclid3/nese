#include <nese/emulator.hpp>

#include <nese/cpu/instruction/execute.hpp>
#include <nese/cpu/instruction/execute_context.hpp>
#include <nese/memory/rom.hpp>
#include <nese/utility/log.hpp>
#include <nese/utility/nintendulator.hpp>

namespace nese {

void emulator::power_on()
{
    NESE_ASSERT(_state == state::off);
    NESE_TRACE("[emulator] power on");

    _state = state::on;
    _cycle = cycle_t{0};
    _memory.set_zero();
    _cpu_state = {};

    NESE_TRACE("{}", nintendulator::format(_cpu_state, _memory));
}

void emulator::power_off()
{
    NESE_ASSERT(_state != state::off);
    NESE_TRACE("[emulator] power off");

    _state = state::off;
}

void emulator::load_rom(const memory::rom& rom)
{
    NESE_TRACE("[emulator] load rom {}", rom);

    // TODO Mapper
    _cpu_state.registers.pc = rom.get_prg().size() == 0x4000 ? 0xc000 : 0x8000;

    _memory.set_bytes(0x8000, rom.get_prg().data(), rom.get_prg().size());

    if (rom.get_prg().size() == 0x4000)
    {
        // "map" 0xC000 to 0x8000
        _memory.set_bytes(0xc000, rom.get_prg().data(), rom.get_prg().size());
    }
}

void emulator::reset()
{
    NESE_ASSERT(_state != state::off);
    NESE_TRACE("[emulator] reset");

    _state = state::on;
    _cycle = cycle_t{0};
    _memory.set_zero();
    _cpu_state = {};

    NESE_TRACE("{}", nintendulator::format(_cpu_state, _memory));
}

void emulator::update(f32_t dt [[maybe_unused]])
{
    if (_state != state::on && _state != state::step)
    {
        return;
    }

    cpu::instruction::execute_context context(_cpu_state, _memory);
    if (cpu::instruction::execute(context))
    {
        NESE_TRACE("{}", nintendulator::format(_cpu_state, _memory));

        if (_state == state::step)
        {
            _state = state::pause;
        }
    }
    else
    {
        _state = state::error;
    }
}

void emulator::step()
{
    NESE_ASSERT(_state == state::pause);
    NESE_TRACE("[emulator] step");

    _state = state::step;
}

void emulator::pause()
{
    NESE_ASSERT(_state == state::on);
    NESE_TRACE("[emulator] pause");

    _state = state::pause;
}

void emulator::unpause()
{
    NESE_ASSERT(_state == state::pause);
    NESE_TRACE("[emulator] unpause");

    _state = state::on;
}

} // namespace nese
