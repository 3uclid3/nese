#pragma once

#include <nese/basic_types.hpp>
#include <nesesan.debugger/cpu_debugger.hpp>

namespace nese::memory {

class ram;

}

namespace nese::san::debugger {

class ram_view
{
public:
    explicit ram_view(const memory::ram& ram);

    void update(f32_t dt, bool& is_visible);

private:
    const memory::ram& _ram;
};

} // namespace nese::san::debugger
