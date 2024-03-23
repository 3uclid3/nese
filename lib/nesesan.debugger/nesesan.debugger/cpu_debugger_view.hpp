#pragma once

#include <nese/basic_types.hpp>
#include <nesesan.debugger/cpu_debugger.hpp>

namespace nese::san::debugger {

class cpu_debugger_view
{
public:
    void update(f32_t dt, bool& is_visible);

private:
    cpu_debugger _debugger;
};

} // namespace nese::san::debugger
