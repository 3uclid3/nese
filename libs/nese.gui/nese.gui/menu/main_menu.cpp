#include <nese.gui/menu/main_menu.hpp>

#include <nese.gui/imgui.hpp>

namespace nese::gui {

bool main_menu_bar_imgui_scope::begin()
{
    return ImGui::BeginMainMenuBar();
}

void main_menu_bar_imgui_scope::end()
{
    ImGui::EndMainMenuBar();
}

} // namespace nese::gui
