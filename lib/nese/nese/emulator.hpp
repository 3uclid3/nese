#pragma once

#include <nese/cpu/state.hpp>
#include <nese/cycle.hpp>
#include <nese/memory/mapper.hpp>

namespace nese {

namespace memory {
class rom;
}

class emulator
{
public:
    enum class state
    {
        off,
        on,
        pause,
        step,
        error
    };

public:
    void power_on();
    void power_off();

    void load_rom(const memory::rom& rom);

    void reset();

    void update(f32_t dt);
    void step();

    void pause();
    void unpause();

    [[nodiscard]] const cpu::state& get_cpu_state() const;
    [[nodiscard]] const memory::mapper& get_memory() const;
    [[nodiscard]] state get_state() const;

private:
    cpu::state _cpu_state;
    memory::mapper _memory;
    state _state{state::off};
    cycle_t _cycle{0};
};

inline const cpu::state& emulator::get_cpu_state() const
{
    return _cpu_state;
}

inline const memory::mapper& emulator::get_memory() const
{
    return _memory;
}

inline emulator::state emulator::get_state() const
{
    return _state;
}

} // namespace nese
