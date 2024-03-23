#include <nesesan/menu/callback_menu_item.hpp>

#include <nesesan/imgui.hpp>

namespace nese::san {

void callback_menu_item::update() const
{
    const bool enabled = !is_enabled || is_enabled();
    const bool checked = is_checked && is_checked();

    if (imgui::MenuItem(name.c_str(), nullptr, checked, enabled))
    {
        execute();
    }
}

} // namespace nesa::san
