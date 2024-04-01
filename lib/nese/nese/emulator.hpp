#pragma once

#include <nese/cpu/state.hpp>
#include <nese/cycle.hpp>
#include <nese/memory/mapper.hpp>

namespace nese {

class emulator
{
public:
    void power_on();
    void reset();

    void step();

    [[nodiscard]] const cpu::state& get_cpu_state() const;
    [[nodiscard]] const memory::mapper& get_memory() const;

private:
    cpu::state _cpu_state;
    memory::mapper _memory;

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

} // namespace nese
