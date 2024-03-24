#include <nesesan.debugger/cpu_debugger_view.hpp>

#include <nesesan/imgui/core.hpp>

namespace nese::san::debugger {

void cpu_debugger_view::update(f32_t dt, bool&)
{
    _debugger.update(dt);

    {
        imgui::disabled_scope disabled(_debugger.get_state() != cpu_debugger::state::off);
        if (ImGui::Button("Start"))
        {
            _debugger.start(memory::rom::from_file("D:\\nese\\tests\\lib\\nese\\test_roms\\nestest.nes"));
        }
    }

    {
        ImGui::SameLine();
        imgui::disabled_scope disabled(_debugger.get_state() == cpu_debugger::state::off);
        if (ImGui::Button("Stop"))
        {
            _debugger.stop();
        }
    }

    {
        ImGui::SameLine();
        imgui::disabled_scope disabled(_debugger.get_state() != cpu_debugger::state::stepped);
        if (ImGui::Button("Step"))
        {
            _debugger.step();
        }
    }

    ImGui::Separator();

    {
        imgui::text("Cycle");
        {
            imgui::indent_scope indent;
            imgui::text("{}", _debugger.get_cpu().get_cycle().count());            
        }

        imgui::text("Registers");
        {
            imgui::indent_scope indent;

            const auto& registers = _debugger.get_cpu().get_registers();

            imgui::label_text("A", "{:02X}", registers.a);
            imgui::label_text("X", "{:02X}", registers.x);
            imgui::label_text("Y", "{:02X}", registers.y);
            imgui::label_text("PC", "{:04X}", registers.pc);
            imgui::label_text("S", "{:02X}", registers.s);
            imgui::label_text("A", "{:02X}", registers.p);
        }
    }
}

} // namespace nese::san::debugger