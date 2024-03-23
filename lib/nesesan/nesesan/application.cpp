#include <nesesan/application.hpp>

#include <nese/utility/assert.hpp>
#include <nesesan/menu/callback_menu_item.hpp>

namespace nese::san {

application::application()
{
    _main_menu.add<callback_menu_item>("File/Exit").execute = [this] { exit(); };

    // add_callback("File/Exit", [this]() { exit(); });

    // add_view("Help/Dear ImGui Demo", [=](f32_t, bool& is_visible) { imgui::ShowDemoWindow(&is_visible); });
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
