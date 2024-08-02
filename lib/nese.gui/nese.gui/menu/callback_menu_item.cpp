#include <nese.gui/menu/callback_menu_item.hpp>

#include <nese.gui/imgui.hpp>

namespace nese::gui {

void callback_menu_item::update() const
{
    const bool enabled = !is_enabled || is_enabled();
    const bool checked = is_checked && is_checked();

    if (ImGui::MenuItem(name.c_str(), nullptr, checked, enabled))
    {
        execute();
    }
}

} // namespace nese::gui
