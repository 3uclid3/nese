#include <nesesan/menu/menu.hpp>

#include <nesesan/imgui.hpp>

namespace nese::san {

bool menu_bar_imgui_scope::begin()
{
    return ImGui::BeginMenuBar();
}

void menu_bar_imgui_scope::end()
{
    ImGui::EndMenuBar();
}

} // namespace nese::san
