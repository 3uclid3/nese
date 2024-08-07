#include <nese.debugger/debug_control_view.hpp>

#include <magic_enum.hpp>

#include <nese.gui/imgui.hpp>
#include <nese.gui/view/view_draw_context.hpp>
#include <nese/cartridge.hpp>
#include <nese/emulator.hpp>

namespace nese::gui {

void debug_control_view::draw(view_draw_context& context)
{
    // TODO implement file explorer
    auto load_cartridge = [] {
        return cartridge::from_file(R"(X:\nese\tests\lib\nese\test_roms\nestest.nes)");
    };

    emulator& emulator = context.get_emulator();

    if (emulator.get_state() == emulator::state::off)
    {
        if (imgui::button("Power ON"))
        {
            emulator.load_cartridge(load_cartridge());
            emulator.power_on();

            // power on paused for debugging
            emulator.pause();
        }
    }
    else
    {
        if (imgui::button("Power OFF"))
        {
            emulator.power_off();
        }
    }

    imgui::disabled_scope disabled(emulator.get_state() == emulator::state::off);

    imgui::same_line();
    if (imgui::button("Reset"))
    {
        emulator.load_cartridge(load_cartridge());
        emulator.reset();
    }

    imgui::disabled_scope error_disabled(emulator.get_state() == emulator::state::error);

    imgui::same_line(0.0f, 15.0f);
    if (emulator.get_state() == emulator::state::pause)
    {
        if (imgui::button("Unpause"))
        {
            emulator.unpause();
        }
    }
    else
    {
        if (imgui::button("Pause"))
        {
            emulator.pause();
        }
    }

    imgui::same_line();
    imgui::disabled_scope step_disabled(emulator.get_state() == emulator::state::on);
    if (imgui::button("Step"))
    {
        emulator.step();
    }

    imgui::same_line();
    if (imgui::button("Step To"))
    {
        emulator.step_to(_to_addr);
    }

    imgui::same_line();
    ImGui::SetNextItemWidth(50.0f);
    if (ImGui::InputText("PC", _to_addr_edit.data(), 5, ImGuiInputTextFlags_CharsHexadecimal))
    {
        const addr_t addr = static_cast<addr_t>(std::stoi(_to_addr_edit, nullptr, 16));

        _to_addr_edit = format("{:04X}", addr);
        _to_addr = addr;
    }
}

} // namespace nese::gui
