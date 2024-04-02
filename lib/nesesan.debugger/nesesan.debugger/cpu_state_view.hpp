#pragma once

namespace nese::san {

class view_draw_context;

class cpu_state_view
{
public:
    void draw(const view_draw_context& context);
};

} // namespace nese::san
