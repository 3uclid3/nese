#include <nese.debug/cpu/snapshotter.hpp>

#include <nese.debug/cpu/snapshot.hpp>
#include <nese/cpu/processor.hpp>

namespace nese::cpu {

snapshotter::snapshotter() = default;
snapshotter::~snapshotter() = default;

void snapshotter::take_snapshot(const processor& cpu)
{
    _snapshots.emplace_back(std::make_unique<snapshot>());

    snapshot& new_snapshot = *_snapshots.back();

    new_snapshot.ram = cpu.get_ram();
    new_snapshot.registers = cpu.get_registers();
    new_snapshot.cycle = cpu.get_cycle();    
}

void snapshotter::reset()
{
    _snapshots.clear();
}

} // namespace nese::cpu
