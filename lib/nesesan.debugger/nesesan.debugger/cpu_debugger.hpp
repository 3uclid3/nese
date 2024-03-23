#pragma once

#include <nese/cpu/processor.hpp>
#include <nese/memory/ram.hpp>
#include <nese/memory/rom.hpp>

namespace nese::san::debugger {

class cpu_debugger
{
public:
    enum class mode
    {
        step,
        until_done
    };

    enum class state
    {
        off,
        starting,
        stopping,
        running,
        stepping,
        stepped,
        paused
    };

    [[nodiscard]] state get_state() const { return _state; }

    void update(f32_t dt);

    void start(memory::rom&& rom);
    void stop();

    void step();

    void pause();
    void unpause();

    [[nodiscard]] const cpu::processor& get_cpu() const { return _cpu; }

private:
    void update_starting(f32_t dt);
    void update_stopping(f32_t dt);
    void update_running(f32_t dt);
    void update_stepping(f32_t dt);

    memory::rom _rom;
    memory::ram _ram;
    cpu::processor _cpu{_ram};

    mode _mode{mode::step};
    state _state{state::off};
};

} // namespace nese::san::debugger
