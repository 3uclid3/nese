#pragma once

#include <memory>
#include <vector>

#include <nese/basic_types.hpp>
#include <nesesan/menu/main_menu.hpp>
#include <nesesan/extension.hpp>
#include <nesesan/view.hpp>

namespace nese::san {

class application
{
public:
    application();

    template<extensible ExtensionT>
    void install_extension();

    void update(f32_t dt);

    [[nodiscard]] bool has_exit_requested() const;
    void exit();

private:
    void initialize_docking();
    void begin_docking();
    void end_docking();

    void create_main_menu_base();

private:
    using extensions = std::vector<std::unique_ptr<extension>>;
    using views = std::vector<std::unique_ptr<view>>;

private:
    extensions _extensions;
    main_menu _main_menu;
    views _views;
    bool _exit_requested{false};
};

template<extensible ExtensionT>
void application::install_extension()
{
    auto extension = std::make_unique<ExtensionT>();

    extension::install_context install_context{_main_menu, _views};
    extension->install(install_context);

    _extensions.emplace_back(std::move(extension));
}

inline bool application::has_exit_requested() const
{
    return _exit_requested;
}
} // namespace nese::san
