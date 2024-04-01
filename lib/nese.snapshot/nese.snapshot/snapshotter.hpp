#pragma once

#include <memory>
#include <span>
#include <vector>
#include <windows.h>

#include <nese/cpu/registers.hpp>
#include <nese/cpu/state.hpp>
#include <nese/memory/mapper.hpp>

namespace nese {

struct state;
struct snapshot;

class snapshot_context
{
public:
    snapshot_context(cpu::state& cpu_state, memory::mapper& memory)
        : _cpu_state(cpu_state)
        , _memory(memory)
    {        
    }

    [[nodiscard]] constexpr memory::mapper& memory() const { return _memory; }
    [[nodiscard]] constexpr cpu::registers& cpu_regisers() const { return cpu_state().registers; }
    [[nodiscard]] constexpr cpu::state& cpu_state() const { return _cpu_state; }

private:
    ref_wrap<cpu::state> _cpu_state;
    ref_wrap<memory::mapper> _memory;
};

class snapshotter
{
public:
    using snapshot_ptr = std::unique_ptr<snapshot>;

public:
    snapshotter();
    ~snapshotter();

    void take_snapshot(snapshot_context context);

    void reset();

    [[nodiscard]] std::span<const snapshot_ptr> get_snapshots() const { return {_snapshots}; }

private:
    std::vector<snapshot_ptr> _snapshots;
};

} // namespace nese
