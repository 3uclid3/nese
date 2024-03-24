#include <filesystem>
#include <nese/utility/assert.hpp>
#include <nesesan.debugger/cpu_debugger_view.hpp>

#include <nesesan/imgui/core.hpp>

namespace nese::san::debugger {

cpu_debugger_view::cpu_debugger_view()
{
}

void cpu_debugger_view::update(f32_t dt, bool&)
{
    _debugger.update(dt);

    if (_debugger.get_state() == cpu_debugger::state::off)
    {
        if (ImGui::Button("Power ON"))
        {
            _debugger.power_on(memory::rom::from_file("D:\\nese\\tests\\lib\\nese\\test_roms\\nestest.nes"));
        }
    }
    else
    {
        imgui::color_scope color(ImGuiCol_Button, IM_COL32(255, 0, 0, 255));
        if (ImGui::Button("Power OFF"))
        {
            _debugger.reset();
        }
    }

    {
        imgui::same_line();
        imgui::disabled_scope disabled(_debugger.get_state() != cpu_debugger::state::paused);
        if (ImGui::Button("Step"))
        {
            _debugger.step();
        }
    }

    {
        imgui::same_line();
        imgui::disabled_scope disabled(_debugger.get_state() == cpu_debugger::state::off);

        if (_debugger.get_state() == cpu_debugger::state::running)
        {
            if (ImGui::Button("Pause"))
            {
                _debugger.pause();
            }
        }
        else
        {
            if (ImGui::Button(_debugger.get_until_pc() != 0 ? "Continue Until" : "Continue"))
            {
                _debugger.pause();
            }
        }
    }

    {
        imgui::same_line();
        imgui::disabled_scope disabled(_debugger.get_state() != cpu_debugger::state::paused);

        if (ImGui::InputText("Address", &_pc_input))
        {
            NESE_ASSERT(_pc_input.find_first_of("0x") == 0);

            if (_pc_input.size() > 6)
            {
                _pc_input.reserve(6);
            }

            addr_t addr = 0;
            for (size_t i = 2; i < _pc_input.size(); ++i)
            {
                char byte = _pc_input[i];

                // get current character then increment
                // transform hex character to the 4bit equivalent number, using the ascii table indexes
                if (byte >= '0' && byte <= '9')
                    byte = byte - '0';
                else if (byte >= 'a' && byte <= 'f')
                    byte = byte - 'a' + 10;
                else if (byte >= 'A' && byte <= 'F')
                    byte = byte - 'A' + 10;

                // shift 4 to make space for new digit, and add the 4 bits of the new digit
                addr = static_cast<addr_t>(addr << 4 | (byte & 0xF));
            }

            _pc_input = imgui::format("0x{:04X}", addr);

            _debugger.set_until_pc(addr);
        }
    }
}

} // namespace nese::san::debugger