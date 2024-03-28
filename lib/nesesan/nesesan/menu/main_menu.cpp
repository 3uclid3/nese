#include <nesesan/menu/main_menu.hpp>

#include <nesesan/imgui/core.hpp>

namespace nese::san {

bool main_menu_bar_imgui_scope::begin()
{
    return ImGui::BeginMainMenuBar();
}

void main_menu_bar_imgui_scope::end()
{
    ImGui::EndMainMenuBar();
}

} // namespace nese::san
