#pragma once

#include <nese/cycle.hpp>
#include <nese/bus.hpp>

namespace nese {

class cartridge;

class emulator
{
public:
    enum class state
    {
        off,
        on,
        pause,
        step,
        step_to,
        error
    };

public:
    void power_on();
    void power_off();

    void load_cartridge(cartridge&& cartridge);

    void reset();

    void update(f32_t dt);
    void step();
    void step_to(addr_t addr);

    void pause();
    void unpause();

    [[nodiscard]] const bus& get_bus() const;
    [[nodiscard]] state get_state() const;

private:
    bus _bus;
    state _state{state::off};
    cycle_t _cycle{0};
    addr_t _step_to_addr{0};
};

inline const bus& emulator::get_bus() const
{
    return _bus;
}

inline emulator::state emulator::get_state() const
{
    return _state;
}

} // namespace nese
