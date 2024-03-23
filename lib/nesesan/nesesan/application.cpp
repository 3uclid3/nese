#include <begin_code.h>
#include <nesesan/application.hpp>

#include <nese/utility/assert.hpp>
#include <nesesan/imgui/core.hpp>
#include <nesesan/menu/callback_menu_item.hpp>

namespace nese::san {

application::application()
{
    initialize_docking();
    create_main_menu();
    create_views();
}

void application::update(f32_t dt)
{
    begin_docking();

    _main_menu.update();

    for (const auto& view : _views)
    {
        if (view->is_visible())
        {
            view->update(dt);
        }
    }

    end_docking();
}

void application::exit()
{
    _exit_requested = true;
}

void application::initialize_docking()
{
    imgui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}
void application::begin_docking()
{
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        imgui::SetNextWindowPos(viewport->Pos);
        imgui::SetNextWindowSize(viewport->Size);
        imgui::SetNextWindowViewport(viewport->ID);

        imgui::style_var_scope window_rounding_scope{ImGuiStyleVar_WindowRounding, 0.0f};
        imgui::style_var_scope window_border_size_scope{ImGuiStyleVar_WindowBorderSize, 0.0f};
        imgui::style_var_scope window_padding_scope{ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)};

        constexpr ImGuiWindowFlags window_flags = 
            ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | 
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | 
            ImGuiWindowFlags_NoBackground;

        imgui::Begin("Main DockSpace", nullptr, window_flags);
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    constexpr ImGuiDockNodeFlags dock_space_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    imgui::DockSpace(ImGui::GetID("MainDockSpace"), ImVec2(0.0f, 0.0f), dock_space_flags);
}

void application::end_docking()
{
    imgui::End();
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
