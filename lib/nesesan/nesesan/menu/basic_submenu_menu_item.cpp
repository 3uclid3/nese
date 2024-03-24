#include <nesesan/menu/basic_submenu_menu_item.hpp>

#include <nesesan/imgui/core.hpp>

namespace nese::san {

bool basic_submenu_menu_item_imgui_scope::begin(const char* name, bool enabled)
{
    return ImGui::BeginMenu(name, enabled);
}

void basic_submenu_menu_item_imgui_scope::end()
{
    ImGui::EndMenu();
}

} // namespace nese::san
