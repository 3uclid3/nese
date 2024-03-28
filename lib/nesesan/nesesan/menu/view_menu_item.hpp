#pragma once

#include <functional>
#include <string>

#include <nesesan/view.hpp>

namespace nese::san {

struct view_menu_item
{
    using is_enabled_callback = std::function<bool()>;

    void update() const;

    std::string name;
    std::reference_wrapper<view> view;
    is_enabled_callback is_enabled{};
};

} // namespace nese::san
