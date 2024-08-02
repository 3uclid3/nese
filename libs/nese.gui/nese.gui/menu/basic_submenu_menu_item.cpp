#include <nese.gui/menu/basic_submenu_menu_item.hpp>

#include <nese.gui/imgui.hpp>

namespace nese::gui {

bool basic_submenu_menu_item_imgui_scope::begin(const char* name, bool enabled)
{
    return ImGui::BeginMenu(name, enabled);
}

void basic_submenu_menu_item_imgui_scope::end()
{
    ImGui::EndMenu();
}

} // namespace nese::gui
