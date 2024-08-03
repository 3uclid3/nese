#pragma once

#include <nese.gui/view/view.hpp>
#include <nese/basic_types.hpp>

namespace nese::gui {

enum class window_view_flag : int
{
    none = 0,
    disable_resize = 1 << 1,
    auto_resize = 1 << 6,
};

using window_view_flags = window_view_flag;

class base_window_view : public view
{
public:
    using flag = window_view_flag;
    using flags = window_view_flags;

protected:
    explicit base_window_view(string&& name, flags flags = flag::none);

    void begin();
    void end();

private:
    flags _flags{flag::none};
};

template<typename ContentT>
class window_view final : public base_window_view
{
public:
    explicit window_view(string&& name, flags flags = flag::none);

private:
    void draw_impl(view_draw_context& context) override;

    ContentT _content{};
};

constexpr window_view_flag operator|(window_view_flag lhs, window_view_flag rhs)
{
    // every static_cast is intentional
    return static_cast<window_view_flag>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

template<typename ContentT>
window_view<ContentT>::window_view(string&& name, flags flags)
    : base_window_view(std::move(name), flags)
{
}

template<typename ContentT>
void window_view<ContentT>::draw_impl(view_draw_context& context)
{
    if (is_visible())
    {
        begin();

        _content.draw(context);

        end();
    }
}

} // namespace nese::gui
