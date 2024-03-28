#pragma once

#include <nese/basic_types.hpp>
#include <nesesan.debugger/cpu_debugger.hpp>

namespace nese::san::debugger {

class cpu_debugger_view
{
public:
    cpu_debugger_view();

    void update(f32_t dt, bool& is_visible);

    [[nodiscard]] const cpu_debugger& get_debugger() const { return _debugger; }

private:
    cpu_debugger _debugger;
    std::string _pc_input{"0x0000"};
};

} // namespace nese::san::debugger
