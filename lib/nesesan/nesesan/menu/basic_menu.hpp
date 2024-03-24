#pragma once

#include <algorithm>
#include <concepts>
#include <nese/utility/assert.hpp>

#include <nesesan/menu/basic_submenu_menu_item.hpp>

namespace nese::san {
    
template<typename T>
concept basic_menu_bar_imgui_scope = requires(T a)
{
    { T::begin() } -> std::same_as<bool>;
    { T::end() } -> std::same_as<void>;
};

template<basic_menu_bar_imgui_scope ScopeT, typename... MenuItemsT>
class basic_menu
{
public:
    using submenu_item = basic_submenu_menu_item<MenuItemsT...>;
    using menu_item = typename submenu_item::menu_item;

    template<typename T, typename... ArgsT>
    T& add(std::string_view path, ArgsT&&... args);

    void update() const;

    const menu_item* find_item(std::string_view name) const;

private:
    submenu_item _root;
};

template<basic_menu_bar_imgui_scope ScopeT, typename... MenuItemsT>
template<typename T, typename... ArgsT>
T& basic_menu<ScopeT, MenuItemsT...>::add(std::string_view path, ArgsT&&... args)
{
    submenu_item* current_item = &_root;

    for (size_t i = 0; i < path.size();)
    {
        const auto end = path.find_first_of('/', i);
        const auto name = end == std::string::npos ? path.substr(i) : path.substr(i, end - i);

        auto is_item = [name](const auto& i) {
            bool is = false;
            std::visit([&is, name](const auto& item) { is = item.name == name; }, i);
            return is;
        };

        i = end;

        if (end == std::string::npos)
        {
            NESE_ASSERT(std::none_of(current_item->items.begin(), current_item->items.end(), is_item));

            current_item->items.emplace_back(T{std::string(name), std::forward<ArgsT>(args)...});

            return std::get<T>(current_item->items.back());
        }
        else
        {
            ++i; // skip the '/'

            auto it = std::find_if(current_item->items.begin(), current_item->items.end(), is_item);

            if (it == current_item->items.end())
            {
                current_item->items.emplace_back(submenu_item{.name = std::string(name), .items = {}, .is_enabled = {}});

                current_item = &std::get<submenu_item>(current_item->items.back());
            }
            else
            {
                NESE_ASSERT(std::holds_alternative<submenu_item>(*it));

                current_item = &std::get<submenu_item>(*it);
            }
        }
    }

    NESE_ASSERT(false);
    __assume(false);
}

template<basic_menu_bar_imgui_scope ScopeT, typename... MenuItemsT>
void basic_menu<ScopeT, MenuItemsT...>::update() const
{
    if (ScopeT::begin())
    {
        for (auto& item : _root.items)
        {
            std::visit([](auto&& arg) { arg.update(); }, item);
        }

        ScopeT::end();
    }
}

template<basic_menu_bar_imgui_scope ScopeT, typename... MenuItemsT>
const typename basic_menu<ScopeT, MenuItemsT...>::menu_item* basic_menu<ScopeT, MenuItemsT...>::find_item(std::string_view name) const
{
    return _root.find_item(name);
}

} // namespace nese::san
