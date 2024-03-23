#include <nesesan/menu/basic_submenu_menu_item.hpp>

#include <nesesan/imgui.hpp>

namespace nese::san {

bool basic_submenu_menu_item_imgui_scope::begin(const char* name, bool enabled)
{
    return imgui::BeginMenu(name, enabled);
}

void basic_submenu_menu_item_imgui_scope::end()
{
    imgui::EndMenu();
}

} // namespace nese::san
