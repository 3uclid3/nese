#include <nesesan/menu/main_menu.hpp>

#include <nesesan/imgui/core.hpp>

namespace nese::san {

bool main_menu_bar_imgui_scope::begin()
{
    return imgui::BeginMainMenuBar();
}

void main_menu_bar_imgui_scope::end()
{
    imgui::EndMainMenuBar();
}

} // namespace nese::san
