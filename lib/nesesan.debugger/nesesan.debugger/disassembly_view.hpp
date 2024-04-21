#pragma once

#include <nese/disassembly.hpp>

namespace nese::san {

class view_draw_context;

class disassembly_view
{
public:
    void draw(const view_draw_context& context);

private:
    disassembly _disassembly;

    bool _auto_scroll{true};
};

} // namespace nese::san
