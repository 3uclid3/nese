#include <nese.gui/view/window_view.hpp>

#include <nese.gui/imgui.hpp>

namespace nese::gui {

base_window_view::base_window_view(string&& name, flags flags)
    : view(std::move(name))
    , _flags(flags)
{
}

void base_window_view::begin()
{
    const ImGuiWindowFlags imgui_flags = static_cast<ImGuiWindowFlags>(_flags);
    ImGui::Begin(_name.c_str(), &_visible, imgui_flags);
}

void base_window_view::end()
{
    ImGui::End();
}

} // namespace nese::gui
