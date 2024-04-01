#pragma once

#include <nese/emulator.hpp>
#include <nesesan/menu/main_menu.hpp>
#include <nesesan/view/view_registry.hpp>

namespace nese::san {

class application
{
public:
    application();

    void draw();

    [[nodiscard]] bool has_exit_requested() const;
    void exit();

    main_menu& get_main_menu();
    view_registry& get_views();

private:
    void initialize_docking();
    void begin_docking();
    void end_docking();

    void create_main_menu();
    void create_imgui_windows();

private:
    main_menu _main_menu;
    view_registry _views;
    emulator _emulator;
    bool _exit_requested{false};
};

inline bool application::has_exit_requested() const
{
    return _exit_requested;
}

inline main_menu& application::get_main_menu()
{
    return _main_menu;
}

inline view_registry& application::get_views()
{
    return _views;
}

} // namespace nese::san
