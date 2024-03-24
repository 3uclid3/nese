#pragma once

#include <memory>
#include <vector>

namespace nese::cpu {

class processor;
struct snapshot;

class snapshotter
{
public:
    snapshotter();
    ~snapshotter();

    void take_snapshot(const processor& cpu);

    void reset();

    [[nodiscard]] const std::vector<std::unique_ptr<snapshot>>& get_snapshots() const { return _snapshots; }

private:
    std::vector<std::unique_ptr<snapshot>> _snapshots;
};

} // namespace nese::cpu
