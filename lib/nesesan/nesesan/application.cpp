#include <nesesan/application.hpp>

#include <nesesan/imgui.hpp>
#include <nesesan/menu/callback_menu_item.hpp>
#include <nesesan/view/view.hpp>
#include <nesesan/view/view_draw_context.hpp>

namespace nese::san {

class imgui_debug_window final : public view
{
public:
    using imgui_callback = void (*)(bool*);

    imgui_debug_window(const char* name, imgui_callback callback)
        : view(name)
        , _callback(callback)
    {
        _visible = false;
    }

private:
    void draw_impl(view_draw_context& context [[maybe_unused]]) override
    {
        _callback(&_visible);
    }

    imgui_callback _callback;
};

application::application()
{
    initialize_docking();

    create_main_menu();
    create_imgui_windows();
}

void application::update(f32_t dt)
{
    _emulator.update(dt);
}

void application::draw()
{
    begin_docking();

    _main_menu.update();

    _views.for_each([this](view& view) {
        if (view.is_visible())
        {
            view_draw_context context(_emulator);
            view.draw(context);
        }
    });

    end_docking();
}

void application::exit()
{
    _exit_requested = true;
}

void application::initialize_docking()
{
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}
void application::begin_docking()
{
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        imgui::style_var_scope window_rounding_scope{ImGuiStyleVar_WindowRounding, 0.0f};
        imgui::style_var_scope window_border_size_scope{ImGuiStyleVar_WindowBorderSize, 0.0f};
        imgui::style_var_scope window_padding_scope{ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)};

        constexpr ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

        ImGui::Begin("Main DockSpace", nullptr, window_flags);
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    constexpr ImGuiDockNodeFlags dock_space_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    ImGui::DockSpace(ImGui::GetID("MainDockSpace"), ImVec2(0.0f, 0.0f), dock_space_flags);
}

void application::end_docking()
{
    ImGui::End();
}

void application::create_main_menu()
{
    _main_menu.add<callback_menu_item>("File/Exit").execute = [this] { exit(); };
    _main_menu.add<main_menu::submenu_item>("View");
}

void application::create_imgui_windows()
{
    auto create = [this](const char* name, const char* menu_path, auto callback) {
        auto& window = _views.add<imgui_debug_window>(name, callback);
        auto& menu = _main_menu.add<callback_menu_item>(menu_path);
        menu.is_checked = [&window] { return window.is_visible(); };
        menu.execute = [&window] { window.toggle_visible(); };
    };

    create("Dear ImGui Demo", "View/ImGui/Demo", &ImGui::ShowDemoWindow);
    create("Dear ImGui Metrics", "View/ImGui/Metrics", &ImGui::ShowMetricsWindow);
    create("Dear ImGui Stack Tool", "View/ImGui/Stack Tool", &ImGui::ShowStackToolWindow);
    create("Dear ImGui Debug Log", "View/ImGui/Debug Log", &ImGui::ShowDebugLogWindow);
    create("Dear ImGui About", "View/ImGui/About", &ImGui::ShowAboutWindow);
}

} // namespace nese::san
