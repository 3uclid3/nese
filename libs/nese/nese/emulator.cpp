#include <nese/emulator.hpp>

#include <nese/utility/log.hpp>
#include <nese/utility/nintendulator.hpp>

namespace nese {

void emulator::power_on()
{
    NESE_ASSERT(_state == state::off);
    NESE_TRACE("[emulator] power on");

    _state = state::on;
    _cycle = cycle_t{0};
    _bus.ppu.reset();
    _bus.cpu.reset();
    _bus.ram.fill(0);

    NESE_TRACE("{}", nintendulator::format(_bus));
}

void emulator::power_off()
{
    NESE_ASSERT(_state != state::off);
    NESE_TRACE("[emulator] power off");

    _state = state::off;
}

void emulator::load_cartridge(cartridge&& cartridge)
{
    NESE_TRACE("[emulator] load cartridge {}", cartridge);

    _bus.cartridge = std::move(cartridge);
}

void emulator::reset()
{
    NESE_ASSERT(_state != state::off);
    NESE_TRACE("[emulator] reset");

    _state = state::off;
    _cycle = cycle_t{0};
    _bus.cpu.reset();
    _bus.ram.fill(0);

    NESE_TRACE("{}", nintendulator::format(_bus));
}

void emulator::update(f32_t dt [[maybe_unused]])
{
    if (_state != state::on && _state != state::step && _state != state::step_to)
    {
        return;
    }

    ++_cycle;

    _bus.ppu.step(_cycle);

    if (_bus.cpu.step(_cycle))
    {
        NESE_TRACE("{}", nintendulator::format(_bus));

        if (_state == state::step || (_state == state::step_to && _step_to_addr == _bus.cpu.get_state().registers.pc))
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

void emulator::step_to(addr_t addr)
{
    NESE_ASSERT(_state == state::pause);
    NESE_TRACE("[emulator] step to 0x{:04X}", addr);

    _state = state::step_to;
    _step_to_addr = addr;
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
