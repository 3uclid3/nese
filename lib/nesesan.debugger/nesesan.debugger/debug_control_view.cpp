#include <nesesan.debugger/debug_control_view.hpp>

#include <magic_enum.hpp>

#include <nese/emulator.hpp>
#include <nese/memory/rom.hpp>
#include <nesesan/imgui.hpp>
#include <nesesan/view/view_draw_context.hpp>

namespace nese::san {

void debug_control_view::draw(view_draw_context& context)
{
    // TODO implement file explorer
    static memory::rom loaded_rom = memory::rom::from_file(R"(D:\nese\tests\lib\nese\test_roms\nestest.nes)");

    emulator& emulator = context.get_emulator();
    switch (emulator.get_state())
    {
    case emulator::state::off:
        if (imgui::button("Power ON"))
        {
            emulator.power_on();
            emulator.pause();
            emulator.load_rom(loaded_rom);
        }
        break;

    case emulator::state::on:
    case emulator::state::pause:
        if (imgui::button("Power OFF"))
        {
            emulator.power_off();
        }
        break;
    }

    imgui::disabled_scope disabled(emulator.get_state() == emulator::state::off);

    imgui::same_line();
    if (imgui::button("Reset"))
    {
        emulator.reset();
        emulator.load_rom(loaded_rom);
    }

    imgui::same_line(0.0f, 15.0f);
    switch (emulator.get_state())
    {
    case emulator::state::pause:
        if (imgui::button("Unpause"))
        {
            emulator.unpause();
        }
        break;

    case emulator::state::off:
    case emulator::state::on:
        if (imgui::button("Pause"))
        {
            emulator.pause();
        }
        break;
    }

    imgui::same_line();
    imgui::disabled_scope step_disabled(emulator.get_state() == emulator::state::on);
    if (imgui::button("Step"))
    {
        emulator.step();
    }
}

} // namespace nese::san
