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
        if (ImGui::Button("Start"))
        {
            _debugger.power_on(memory::rom::from_file("D:\\nese\\tests\\lib\\nese\\test_roms\\nestest.nes"));
        }
    }
    else
    {
        imgui::color_scope color(ImGuiCol_Button, IM_COL32(255, 0, 0, 255));
        if (ImGui::Button("Stop"))
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
            if (ImGui::Button("Continue"))
            {
                _debugger.set_mode(_debugger.get_until_pc() != 0 ? cpu_debugger::mode::until_pc : cpu_debugger::mode::until_off);
                _debugger.unpause();
            }
        }
    }

    {
        imgui::same_line();
        imgui::disabled_scope disabled(_debugger.get_state() == cpu_debugger::state::running);
        
        ImGui::SetNextItemWidth(200.0f);

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

    if (ImGui::BeginTable("#Instructions", 12, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody))
    {
        const f32_t hex_width = ImGui::CalcTextSize("X").x;

        // Step #
        ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthStretch);

        // PC
        ImGui::TableSetupColumn("PC", ImGuiTableColumnFlags_WidthFixed, hex_width * 4.f);

        // instruction byte 0
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, hex_width * 2.f);

        // instruction byte 1
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, hex_width * 2.f);

        // instruction byte 2
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, hex_width * 2.f);

        // instruction str
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, hex_width * 4.f);

        // operand
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, hex_width * 28.f);

        // a
        ImGui::TableSetupColumn("A", ImGuiTableColumnFlags_WidthFixed, hex_width * 2.f);

        // x
        ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, hex_width * 2.f);

        // y
        ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, hex_width * 2.f);

        // p
        ImGui::TableSetupColumn("P", ImGuiTableColumnFlags_WidthFixed, hex_width * 2.f);

        // sp
        ImGui::TableSetupColumn("SP", ImGuiTableColumnFlags_WidthFixed, hex_width * 2.f);

        ImGui::TableHeadersRow();

        imgui::begin_group();
        {
                
            // Step #
            ImGui::TableNextColumn();
            imgui::text("0");

            // PC
            ImGui::TableNextColumn();
            imgui::text("C000");

            // instruction byte 0
            ImGui::TableNextColumn();
            imgui::text("4C");

            // instruction byte 1
            ImGui::TableNextColumn();
            imgui::text("F5");

            // instruction byte 2
            ImGui::TableNextColumn();
            imgui::text("C5");

            // instruction str
            ImGui::TableNextColumn();
            imgui::text(" JMP");

            // operand
            ImGui::TableNextColumn();
            imgui::text("$C5F5");

            // a
            ImGui::TableNextColumn();
            imgui::text("00");

            // x
            ImGui::TableNextColumn();
            imgui::text("00");

            // y
            ImGui::TableNextColumn();
            imgui::text("00");

            // p
            ImGui::TableNextColumn();
            imgui::text("24");

            // sp
            ImGui::TableNextColumn();
            imgui::text("FD");
        }
        imgui::end_group();

        ImGui::EndTable();
    }
}

} // namespace nese::san::debugger