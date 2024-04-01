#include <nesesan/view/window_view.hpp>

#include <nesesan/imgui.hpp>

namespace nese::san {

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

} // namespace nese::san
