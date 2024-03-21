#include <nese/emulator.hpp>

#include <nese/cpu.hpp>
#include <nese/log.hpp>
#include <nese/ram.hpp>

namespace nese {

emulator::emulator() = default;
emulator::~emulator() = default;

void emulator::power_on()
{
    NESE_TRACE("[emulator] power on");

    _cycle = {};

    _ram->power_on();
    _cpu->power_on();
}

void emulator::reset()
{
    NESE_TRACE("[emulator] reset");

    _cycle = {};

    _ram->reset();
    _cpu->reset();
}

void emulator::step(cycle_t count)
{
    NESE_TRACE("[emulator] step {}", count);

    _cycle += count;

    _cpu->step_to(_cycle);
}

bool emulator::has_stop_requested() const
{
    return _cpu->has_stop_requested();
}

void emulator::stop()
{
    NESE_TRACE("[emulator] stop");

    _cpu->stop();
}

} // namespace nese
