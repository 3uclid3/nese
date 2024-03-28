#include <nesesan/imgui/extension.hpp>

#include <nesesan/imgui/core.hpp>

namespace nese::san::imgui {

void extension::install_impl(install_context& context)
{
    context.add_view_no_scope("View/ImGui/Demo", "ImGui Demo", [](f32_t, bool& is_visible) { ImGui::ShowDemoWindow(&is_visible); });
    context.add_view_no_scope("View/ImGui/Metrics", "ImGui Metrics", [](f32_t, bool& is_visible) { ImGui::ShowMetricsWindow(&is_visible); });
    context.add_view_no_scope("View/ImGui/Stack Tool", "ImGui Stack Tool", [](f32_t, bool& is_visible) { ImGui::ShowStackToolWindow(&is_visible); });
    context.add_view_no_scope("View/ImGui/Debug Log", "ImGui Debug Log", [](f32_t, bool& is_visible) { ImGui::ShowDebugLogWindow(&is_visible); });
    context.add_view_no_scope("View/ImGui/About", "ImGui About", [](f32_t, bool& is_visible) { ImGui::ShowAboutWindow(&is_visible); });
}

} // namespace nese::san::imgui
