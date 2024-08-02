#include <nese.gui/menu/menu.hpp>

#include <nese.gui/imgui.hpp>

namespace nese::gui {

bool menu_bar_imgui_scope::begin()
{
    return ImGui::BeginMenuBar();
}

void menu_bar_imgui_scope::end()
{
    ImGui::EndMenuBar();
}

} // namespace nese::gui
