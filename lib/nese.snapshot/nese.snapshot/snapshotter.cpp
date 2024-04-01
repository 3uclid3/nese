#include <nese.snapshot/snapshotter.hpp>

#include <nese.snapshot/snapshot.hpp>
#include <nese/cpu/state.hpp>

namespace nese {

snapshotter::snapshotter() = default;
snapshotter::~snapshotter() = default;

void snapshotter::take_snapshot(snapshot_context context)
{
    snapshot& new_snapshot = *_snapshots.emplace_back(std::make_unique<snapshot>());
    new_snapshot.memory = context.memory();
    new_snapshot.cpu_state = context.cpu_state();
}

void snapshotter::reset()
{
    _snapshots.clear();
}

} // namespace nese::cpu
