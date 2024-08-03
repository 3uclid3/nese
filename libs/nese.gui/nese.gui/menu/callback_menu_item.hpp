#pragma once

#include <functional>
#include <string>

namespace nese::gui {

struct callback_menu_item
{
    using is_enabled_callback = std::function<bool()>;
    using is_checked_callback = std::function<bool()>;
    using execute_callback = std::function<void()>;

    void update() const;

    std::string name;
    execute_callback execute{};
    is_enabled_callback is_enabled{};
    is_checked_callback is_checked{};
};

} // namespace nese::gui
