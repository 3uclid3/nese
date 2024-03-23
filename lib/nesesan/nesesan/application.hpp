#pragma once

namespace nesesan {

class application
{
public:
    void update();

private:
    bool _show_imgui_demo{true};
    bool _show_another_window{true};
};

} // namespace nesesan
