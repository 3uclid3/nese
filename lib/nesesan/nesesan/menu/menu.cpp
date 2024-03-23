#include <nesesan/menu/menu.hpp>

#include <nesesan/imgui/core.hpp>

namespace nese::san {

bool menu_bar_imgui_scope::begin()
{
    return imgui::BeginMenuBar();
}

void menu_bar_imgui_scope::end()
{
    imgui::EndMenuBar();
}

} // namespace nese::san
