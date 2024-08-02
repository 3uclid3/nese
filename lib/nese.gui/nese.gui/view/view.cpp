#include <nese.gui/view/view.hpp>

namespace nese::gui {

view::view(string&& name)
    : _name(std::move(name))
{
}

void view::draw(view_draw_context& context)
{
    draw_impl(context);
}

} // namespace nese::gui
