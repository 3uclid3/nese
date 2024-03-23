#pragma once

#include <memory>
#include <vector>

#include <nese/basic_types.hpp>
#include <nesesan/menu/main_menu.hpp>
#include <nesesan/view.hpp>

namespace nese::san {

class application
{
public:
    application();

    void update(f32_t dt);

    bool has_exit_requested() const;
    void exit();

private:
    template<typename F>
    void add_view(std::string_view path, F func);

    template<typename F>
    void add_callback(std::string_view path, F func);

private:
    using views = std::vector<std::unique_ptr<view>>;

private:
    main_menu _main_menu;
    views _views;
    bool _exit_requested{false};
};

inline bool application::has_exit_requested() const
{
    return _exit_requested;
}
} // namespace nese::san
