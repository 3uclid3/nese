#pragma once

#include <nesesan/menu/basic_menu.hpp>
#include <nesesan/menu/callback_menu_item.hpp>

namespace nese::san {

struct main_menu_bar_imgui_scope
{
    static bool begin();
    static void end();
};

using main_menu = basic_menu<main_menu_bar_imgui_scope, callback_menu_item>;

} // namespace nese::san
