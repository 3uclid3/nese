#pragma once

#include <functional>
#include <memory>
#include <string>

#include <nese/basic_types.hpp>

namespace nese::san {

class view
{
public:
    using flags = int;

public:
    template<typename F>
    static std::unique_ptr<view> make_unique(std::string name, F&& func);

    template<typename F>
    static std::unique_ptr<view> make_unique_no_scope(std::string name, F&& func);

public:
    using update_callback = std::function<void(f32_t, bool&)>;

public:
    void update(f32_t dt);

    [[nodiscard]] const std::string& get_name() const;

    [[nodiscard]] bool is_visible() const;
    void set_visible(bool value);
    void toggle_visible();

    [[nodiscard]] flags get_flags() const;
    void set_flags(flags flags);

private:
    static void begin(const char* name, bool& is_visible, flags flags);
    static void end();

    explicit view(std::string&& name, update_callback callback = {});

private:
    std::string _name;
    update_callback _callback;
    flags _flags{0};
    bool _is_visible{false};
};

inline const std::string& view::get_name() const
{
    return _name;
}

inline bool view::is_visible() const
{
    return _is_visible;
}

inline void view::set_visible(bool value)
{
    _is_visible = value;
}

inline void view::toggle_visible()
{
    _is_visible = !_is_visible;
}

inline view::flags view::get_flags() const
{
    return _flags;
}

inline void view::set_flags(flags flags)
{
    _flags = flags;
}

template<typename F>
std::unique_ptr<view> view::make_unique(std::string name, F&& func)
{
    auto new_view = new view(std::move(name));

    new_view->_callback = [viewPtr = new_view, f = std::forward<F>(func)](f32_t dt, bool& is_visible) {
        begin(viewPtr->get_name().c_str(), is_visible, viewPtr->get_flags());

        f(dt, is_visible);

        end();
    };

    return std::unique_ptr<view>(new_view);
}

template<typename F>
std::unique_ptr<view> view::make_unique_no_scope(std::string name, F&& func)
{
    auto new_view = new view(std::move(name), std::forward<F>(func));

    return std::unique_ptr<view>(new_view);
}

} // namespace nese::san
