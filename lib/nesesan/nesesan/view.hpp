#pragma once

#include <functional>
#include <string>

#include <nese/basic_types.hpp>

namespace nese::san {

class view
{
public:
    using update_callback = std::function<void(f32_t, bool&)>;

public:
    explicit view(std::string name, update_callback callback);

    void update(f32_t dt);

    [[nodiscard]] const std::string& get_name() const;

    [[nodiscard]] bool is_visible() const;
    void toggle_visible();

private:
    std::string _name;
    update_callback _callback;
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

inline void view::toggle_visible()
{
    _is_visible = !_is_visible;
}

} // namespace nese::san
