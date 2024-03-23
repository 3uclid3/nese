#include <nesesan.debugger/extension.hpp>

namespace nese::san::debugger {

void extension::install_impl(install_context& context)
{
    context.add_view("View/Debugger/CPU", "CPU Debugger", [this](f32_t dt, bool& is_visible) { _cpu_debugger_view.update(dt, is_visible); });
}

} // namespace nese::san::debugger
