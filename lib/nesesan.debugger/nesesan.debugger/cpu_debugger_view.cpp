#include <nesesan.debugger/cpu_debugger_view.hpp>

#include <nesesan/imgui/core.hpp>

namespace nese::san::debugger {

void cpu_debugger_view::update(f32_t dt, bool&)
{
    _debugger.update(dt);

    {
        imgui::disabled_scope disabled(_debugger.get_state() != cpu_debugger::state::off);
        if (imgui::Button("Start"))
        {
            _debugger.start(memory::rom::from_file("D:\\nese\\tests\\lib\\nese\\test_roms\\nestest.nes"));
        }
    }

    {
        imgui::SameLine();
        imgui::disabled_scope disabled(_debugger.get_state() == cpu_debugger::state::off);
        if (imgui::Button("Stop"))
        {
            _debugger.stop();
        }
    }

    {
        imgui::SameLine();
        imgui::disabled_scope disabled(_debugger.get_state() != cpu_debugger::state::stepped);
        if (imgui::Button("Step"))
        {
            _debugger.step();
        }
    }

    imgui::Separator();

    {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
        imgui::Text("Cycle");
        {
            imgui::indent_scope indent;

            const auto cycle_str = fmt::format("{}", _debugger.get_cpu().get_cycle().count());
            imgui::Text(cycle_str.c_str());            
        }

        imgui::Text("Registers");
        {
            imgui::indent_scope indent;

            const auto& registers = _debugger.get_cpu().get_registers();
            const auto a_str = fmt::format("{:02X}", registers.a);
            imgui::LabelText("A", a_str.c_str());

            const auto x_str = fmt::format("{:02X}", registers.x);
            imgui::LabelText("X", x_str.c_str());

            const auto y_str = fmt::format("{:02X}", registers.y);
            imgui::LabelText("Y", y_str.c_str());

            const auto pc_str = fmt::format("{:04X}", registers.pc);
            imgui::LabelText("PC", pc_str.c_str());

            const auto s_str = fmt::format("{:02X}", registers.s);
            imgui::LabelText("S", s_str.c_str());

            const auto p_str = fmt::format("{:02X}", registers.p);
            imgui::LabelText("P", p_str.c_str());
#pragma clang diagnostic pop
        }
    }
}

} // namespace nese::san::debugger