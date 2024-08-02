#pragma once

#include <nese.gui/menu/basic_menu.hpp>
#include <nese.gui/menu/callback_menu_item.hpp>

namespace nese::gui {

struct main_menu_bar_imgui_scope
{
    static bool begin();
    static void end();
};

using main_menu = basic_menu<main_menu_bar_imgui_scope, callback_menu_item>;

} // namespace nese::gui
