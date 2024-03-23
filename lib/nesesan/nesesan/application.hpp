#pragma once

#include <nese/basic_types.hpp>

namespace nese::san {

class application
{
public:
    void update(f32_t dt);

    bool has_exit_requested() const;
    void exit();

private:
    void update_main_menu();


private:
    bool _exit_requested{false};
    bool _show_imgui_demo{true};
    bool _show_another_window{true};
};

inline bool application::has_exit_requested() const
{
    return _exit_requested;
}
} // namespace nese::san
