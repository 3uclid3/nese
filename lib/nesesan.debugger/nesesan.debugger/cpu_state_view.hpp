#pragma once

#include <nese/basic_types.hpp>
#include <nesesan/view/window_view.hpp>

namespace nese::san {

class view_draw_context;

class cpu_state_view
{
public:
    void draw(const view_draw_context& context);
};

} // namespace nese::san
