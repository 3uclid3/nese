#include <nesesan/view.hpp>

#include <nesesan/imgui/core.hpp>

namespace nese::san {

view::view(std::string&& name, update_callback callback)
    : _name(std::move(name))
    , _callback(std::move(callback))
{
}

void view::update(f32_t dt)
{
    if (_is_visible)
    {
        _callback(dt, _is_visible);
    }
}

void view::begin(const char* name, bool& is_visible, flags flags)
{
    ImGui::Begin(name, &is_visible, flags);
}

void view::end()
{
    ImGui::End();
}

} // namespace nese::san
