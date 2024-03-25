#include <nese.debug/cpu/snapshotter.hpp>

#include <nese.debug/cpu/snapshot.hpp>
#include <nese/cpu/processor.hpp>
#include <nese/cpu/state.hpp>

namespace nese::cpu {

snapshotter::snapshotter() = default;
snapshotter::~snapshotter() = default;

void snapshotter::take_snapshot(const state& state)
{
    _snapshots.emplace_back(std::make_unique<snapshot>());

    snapshot& new_snapshot = *_snapshots.back();

    new_snapshot.memory = state.memory.get();
    new_snapshot.registers = state.registers;
    new_snapshot.cycle = state.cycle;
}

void snapshotter::reset()
{
    _snapshots.clear();
}

} // namespace nese::cpu
