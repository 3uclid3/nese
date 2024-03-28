#pragma once

#include <nese/cpu/processor.hpp>
#include <nese/cycle.hpp>
#include <nese/memory/ram.hpp>

namespace nese {

class emulator
{
public:
    void power_on();
    void reset();

    void step(cycle_t count);

    [[nodiscard]] bool has_stop_requested() const;
    void stop();

    [[nodiscard]] const cpu::processor& get_cpu() const;
    [[nodiscard]] const memory::ram& get_ram() const;

private:
    memory::ram _ram{};
    cpu::processor _cpu{_ram};

    cycle_t _cycle{0};
};

inline const cpu::processor& emulator::get_cpu() const
{
    return _cpu;
}

inline const memory::ram& emulator::get_ram() const
{
    return _ram;
}

} // namespace nese
