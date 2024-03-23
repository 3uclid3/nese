#include <nesesan/application.hpp>

#include <nese/utility/assert.hpp>
#include <nesesan/menu/callback_menu_item.hpp>

namespace nese::san {

application::application()
{
    _main_menu.add<callback_menu_item>("File/Exit").execute = [this] { exit(); };
    _main_menu.add<main_menu::submenu_item>("View");
    _main_menu.add<main_menu::submenu_item>("Help");
}

void application::update(f32_t dt)
{
    _main_menu.update();

    for (const auto& view : _views)
    {
        if (view->is_visible())
        {
            view->update(dt);
        }
    }
}

void application::exit()
{
    _exit_requested = true;
}

} // namespace nese::san
