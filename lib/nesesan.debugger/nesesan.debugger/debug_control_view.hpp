#pragma once

#include <nese/basic_types.hpp>

namespace nese::san {

class view_draw_context;

class debug_control_view
{
public:
    void draw(view_draw_context& context);

private:
    addr_t _to_addr{0x0000};
    string _to_addr_edit{"0000"};
};

} // namespace nese::san
