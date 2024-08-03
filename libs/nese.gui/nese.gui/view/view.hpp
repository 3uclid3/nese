#pragma once

#include <nese/basic_types.hpp>

namespace nese::gui {

class view_draw_context;

class view
{
public:
    view(const view&) = delete;
    view(view&&) = delete;
    view& operator=(const view&) = delete;
    view& operator=(view&&) = delete;

    virtual ~view() = default;

    void draw(view_draw_context& context);

    [[nodiscard]] const string& get_name() const;

    [[nodiscard]] bool is_visible() const;
    void set_visible(bool value);
    void toggle_visible();

protected:
    explicit view(string&& name);

    virtual void draw_impl(view_draw_context& context) = 0;

    string _name;
    bool _visible{true};
};

inline const string& view::get_name() const
{
    return _name;
}

inline bool view::is_visible() const
{
    return _visible;
}

inline void view::set_visible(bool value)
{
    _visible = value;
}

inline void view::toggle_visible()
{
    _visible = !_visible;
}

} // namespace nese::gui