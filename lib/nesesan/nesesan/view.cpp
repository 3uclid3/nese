#include <nesesan/view.hpp>

#include <nesesan/imgui/core.hpp>

namespace nese::san {

view::view(std::string name, update_callback callback)
    : _name(std::move(name))
    , _callback(std::move(callback))
{
}

void view::update(f32_t dt)
{
    if (_is_visible)
    {
        imgui::Begin(_name.c_str());

        _callback(dt, _is_visible);

        imgui::End();
    }
}

} // namespace nese::san
