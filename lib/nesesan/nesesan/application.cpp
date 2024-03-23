#include <nesesan/application.hpp>

#include <nese/utility/assert.hpp>
#include <nesesan/menu/callback_menu_item.hpp>
#include <nesesan/imgui/core.hpp>

namespace nese::san {

application::application()
{
    create_main_menu();
    create_views();
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

void application::create_main_menu()
{
    _main_menu.add<callback_menu_item>("File/Exit").execute = [this] { exit(); };
    _main_menu.add<main_menu::submenu_item>("View");
    _main_menu.add<main_menu::submenu_item>("Help");
}

void application::create_views()
{
    create_views_imgui();
}

void application::create_views_imgui()
{
    _views.emplace_back(view::make_unique_no_scope("ImGui Demo", [](f32_t, bool& is_visible) { imgui::ShowDemoWindow(&is_visible); }));
    _main_menu.add<view_menu_item>("View/ImGui/Demo", std::ref(*_views.back()));

    _views.emplace_back(view::make_unique_no_scope("ImGui Metrics", [](f32_t, bool& is_visible) { imgui::ShowMetricsWindow(&is_visible); }));
    _main_menu.add<view_menu_item>("View/ImGui/Metrics", std::ref(*_views.back()));

    _views.emplace_back(view::make_unique_no_scope("ImGui Stack Tool", [](f32_t, bool& is_visible) { imgui::ShowStackToolWindow(&is_visible); }));
    _main_menu.add<view_menu_item>("View/ImGui/Stack Tool", std::ref(*_views.back()));

    _views.emplace_back(view::make_unique_no_scope("ImGui Debug Log", [](f32_t, bool& is_visible) { imgui::ShowDebugLogWindow(&is_visible); }));
    _main_menu.add<view_menu_item>("View/ImGui/Debug Log", std::ref(*_views.back()));

    _views.emplace_back(view::make_unique_no_scope("ImGui About", [](f32_t, bool& is_visible) { imgui::ShowAboutWindow(&is_visible); }));
    _main_menu.add<view_menu_item>("View/ImGui/About", std::ref(*_views.back()));
}

} // namespace nese::san
