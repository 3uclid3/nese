#pragma once

#include <memory>

#include <nese/cycle.hpp>

namespace nese {

namespace cpu { class processor; }
namespace memory { class ram; }

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

    const cpu::processor& get_cpu() const;
    const memory::ram& get_ram() const;


private:
    std::unique_ptr<memory::ram> _ram{std::make_unique<memory::ram>()};
    std::unique_ptr<cpu::processor> _cpu{std::make_unique<cpu::processor>(*_ram.get())};

    cycle_t _cycle{0};
};

inline const cpu::processor& emulator::get_cpu() const
{
    return *_cpu;
}

inline const memory::ram& emulator::get_ram() const
{
    return *_ram;
}

} // namespace nese
