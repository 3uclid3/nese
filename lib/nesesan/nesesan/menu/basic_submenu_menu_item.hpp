#pragma once

#include <functional>
#include <string>
#include <variant>
#include <vector>

namespace nese::san {

struct basic_submenu_menu_item_imgui_scope
{
    static bool begin(const char* name, bool enabled);
    static void end();
};

template<typename... MenuItemsT>
struct basic_submenu_menu_item
{
    using menu_item = std::variant<basic_submenu_menu_item, MenuItemsT...>;

    using is_enabled_callback = std::function<bool()>;

    void update() const;

    const menu_item* find_item(std::string_view name) const;

    std::string name;
    std::vector<menu_item> items;
    is_enabled_callback is_enabled;
};

template<typename... MenuItemsT>
void basic_submenu_menu_item<MenuItemsT...>::update() const
{
    const bool enabled = !is_enabled || is_enabled();

    if (basic_submenu_menu_item_imgui_scope::begin(name.c_str(), enabled))
    {
        for (auto& i : items)
        {
            std::visit([](auto&& item) { item.update(); }, i);
        }

        basic_submenu_menu_item_imgui_scope::end();
    }
}

template<typename... MenuItemsT>
const basic_submenu_menu_item<MenuItemsT...>::menu_item* basic_submenu_menu_item<MenuItemsT...>::find_item(std::string_view name) const
{
    auto it = std::find_if(
        items.begin(),
        items.end(),
        [name](const menu_item& i)
        {
            bool is = false;
            std::visit([&is, name](const auto& item) { is = item.name == name; }, i);
            return is;
        }
    );

    return it != items.end() ? &*it : nullptr;
}

} // namespace nese::san
