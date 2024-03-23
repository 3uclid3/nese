#pragma once

#include <memory>
#include <type_traits>
#include <vector>

#include <nesesan/menu/main_menu.hpp>
#include <nesesan/view.hpp>

namespace nese::san {

class extension
{
public:
    class install_context
    {
    public:
        using views = std::vector<std::unique_ptr<view>>;

        install_context(main_menu& main_menu, views& views);

        template<typename F>
        void add_main_menu_callback(std::string_view path, F&& update_callback);

        template<typename F>
        void add_view(std::string_view path, std::string name, F&& update_callback);

        template<typename F>
        void add_view_no_scope(std::string_view path, std::string name, F&& update_callback);

    private:
        main_menu& _main_menu;
        views& _views;
    };

public:
    virtual ~extension() = default;

    void install(install_context& context);

private:
    virtual void install_impl(install_context& context) = 0;
};

template<typename T>
concept extensible = std::is_base_of_v<extension, T>;

inline extension::install_context::install_context(main_menu& main_menu, views& views)
    : _main_menu(main_menu)
    , _views(views)
{
}

template<typename F>
void extension::install_context::add_main_menu_callback(std::string_view path, F&& update_callback)
{
    _main_menu.add<callback_menu_item>(path).execute = std::forward<F>(update_callback);
}

template<typename F>
void extension::install_context::add_view(std::string_view path, std::string name, F&& update_callback)
{
    _views.emplace_back(view::make_unique(std::move(name), std::forward<F>(update_callback)));

    _main_menu.add<view_menu_item>(path, std::ref(*_views.back()));
}

template<typename F>
void extension::install_context::add_view_no_scope(std::string_view path, std::string name, F&& update_callback)
{
    _views.emplace_back(view::make_unique_no_scope(std::move(name), std::forward<F>(update_callback)));

    _main_menu.add<view_menu_item>(path, std::ref(*_views.back()));
}

} // namespace nese::san
