#include <nese/emulator.hpp>

#include <nese/cpu/instruction/execute.hpp>
#include <nese/cpu/instruction/execute_context.hpp>
#include <nese/nintendulator.hpp>
#include <nese/utility/log.hpp>

namespace nese {

void emulator::power_on()
{
    NESE_TRACE("[emulator] power on");

    _cycle = cycle_t{0};
    _memory.set_zero();
    _cpu_state = {};

    NESE_TRACE("{}", nintendulator::format(_cpu_state, _memory));
}

void emulator::reset()
{
    NESE_TRACE("[emulator] reset");

    _cycle = cycle_t{0};
    _memory.set_zero();
    _cpu_state = {};

    NESE_TRACE("{}", nintendulator::format(_cpu_state, _memory));
}

void emulator::step()
{
    NESE_TRACE("[emulator] step");

    cpu::instruction::execute_context context(_cpu_state, _memory);
    cpu::instruction::execute(context);

    NESE_TRACE("{}", nintendulator::format(_cpu_state, _memory));
}

} // namespace nese
