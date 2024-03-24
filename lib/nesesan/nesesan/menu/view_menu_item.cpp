#include <nesesan/menu/view_menu_item.hpp>

#include <nesesan/imgui/core.hpp>

namespace nese::san {

void view_menu_item::update() const
{
    const bool enabled = !is_enabled || is_enabled();

    if (ImGui::MenuItem(name.c_str(), nullptr, view.get().is_visible(), enabled))
    {
        view.get().toggle_visible();
    }
}

} // namespace nese::san
