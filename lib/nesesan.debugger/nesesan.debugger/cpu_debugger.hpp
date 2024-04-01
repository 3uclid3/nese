#pragma once

#include <nese.snapshot/snapshotter.hpp>
#include <nese/cpu/state.hpp>
#include <nese/memory/mapper.hpp>
#include <nese/memory/rom.hpp>

namespace nese::san::debugger {

class cpu_debugger
{
public:
    enum class mode
    {
        step,
        until_pc,
        until_off
    };

    enum class state
    {
        off,
        running,
        paused
    };

    [[nodiscard]] mode get_mode() const { return _mode; }
    void set_mode(mode mode) { _mode = mode; }

    [[nodiscard]] state get_state() const { return _state; }

    [[nodiscard]] addr_t get_until_pc() const { return _until_pc; }
    void set_until_pc(addr_t until_pc) { _until_pc = until_pc; }

    void update(f32_t dt);

    void power_on(memory::rom&& rom, bool start_running = false);
    void reset();

    void step();

    void pause();
    void unpause();

    [[nodiscard]] const snapshotter& get_snapshotter() const { return _snapshotter; }
    [[nodiscard]] const cpu::state& get_cpu_state() const { return _cpu_state; }
    [[nodiscard]] const memory::rom& get_rom() const { return _rom; }
    [[nodiscard]] const memory::mapper& get_memory() const { return _memory; }

private:
    memory::rom _rom;
    memory::mapper _memory;
    cpu::state _cpu_state;
    snapshotter _snapshotter;

    mode _mode{mode::step};
    state _state{state::off};
    addr_t _until_pc{0};
};

} // namespace nese::san::debugger
