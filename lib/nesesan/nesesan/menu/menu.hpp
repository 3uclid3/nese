#pragma once

#include <nesesan/menu/basic_menu.hpp>
#include <nesesan/menu/callback_menu_item.hpp>

namespace nese::san {

struct menu_bar_imgui_scope
{
    static bool begin();
    static void end();
};

using menu = basic_menu<menu_bar_imgui_scope, callback_menu_item>;

} // namespace nese::san
