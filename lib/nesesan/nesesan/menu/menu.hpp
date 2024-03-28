#pragma once

#include <nesesan/menu/basic_menu.hpp>

namespace nese::san {

struct menu_bar_imgui_scope
{
    static bool begin();
    static void end();
};

template<typename... MenuItemsT>
using menu = basic_menu<menu_bar_imgui_scope, MenuItemsT...>;

} // namespace nese::san
