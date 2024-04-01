#pragma once

#include <nese/basic_types.hpp>
#include <nesesan/view/view.hpp>

namespace nese::san {

class base_window_view : public view
{
public:
    enum class flag : int
    {
        none = 0
    };

    using flags = flag;

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

constexpr base_window_view::flag operator|(base_window_view::flag lhs, base_window_view::flag rhs)
{
    // every static_cast is intentional
    return static_cast<base_window_view::flag>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
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

} // namespace nese::san
