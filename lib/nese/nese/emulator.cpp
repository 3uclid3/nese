#include <nese/emulator.hpp>

#include <nese/utility/log.hpp>

namespace nese {

void emulator::power_on()
{
    NESE_TRACE("[emulator] power on");

    _cycle = {};

    _ram.power_on();
    _cpu.power_on();
}

void emulator::reset()
{
    NESE_TRACE("[emulator] reset");

    _cycle = {};

    _ram.reset();
    _cpu.reset();
}

void emulator::step(cycle_t count)
{
    NESE_TRACE("[emulator] step {}", count);

    const cycle_t end = _cycle + count;

    for (;_cycle < end; _cycle += cycle_t{1})
    {
        // TODO Review
        _cpu.step();
    }
}

bool emulator::has_stop_requested() const
{
    return _cpu.has_stop_requested();
}

void emulator::stop()
{
    NESE_TRACE("[emulator] stop");

    _cpu.stop();
}

} // namespace nese
