#pragma once

#include <memory>

#include <nese/cycle.hpp>

namespace nese {

class cpu;
class ram;

class emulator
{
public:
    emulator();
    ~emulator();

    void power_on();
    void reset();

    void step(cycle_t count);

    bool has_stop_requested() const;
    void stop();

    const cpu& get_cpu() const;
    const ram& get_ram() const;


private:
    std::unique_ptr<ram> _ram{std::make_unique<ram>()};
    std::unique_ptr<cpu> _cpu{std::make_unique<cpu>(*_ram.get())};

    cycle_t _cycle{0};
};

inline const cpu& emulator::get_cpu() const
{
    return *_cpu;
}

inline const ram& emulator::get_ram() const
{
    return *_ram;
}

} // namespace nese
