#include <imgui.h>
#include <nesesan.debugger/extension.hpp>

#include <nesesan.debugger/cpu_debugger_view.hpp>

namespace nese::san::debugger {

extension::extension()
    : _cpu_debugger_view(std::make_unique<cpu_debugger_view>())
{
}

extension::~extension() = default;

void extension::install_impl(install_context& context)
{
    context.add_view(
        "View/Debugger/CPU",
        "CPU Debugger",
        [this](f32_t dt, bool& is_visible) { _cpu_debugger_view->update(dt, is_visible); },
        true,
        ImGuiWindowFlags_NoCollapse);
    // context.add_view("View/Debugger/RAM", "RAM Inspector", [this](f32_t dt, bool& is_visible) { _ram_view.update(dt, is_visible); });
}

} // namespace nese::san::debugger
