#include <nesesan.debugger/cpu_state_view.hpp>

#include <nese/emulator.hpp>
#include <nesesan/imgui.hpp>
#include <nesesan/view/view_draw_context.hpp>

namespace nese::san {

void cpu_state_view::draw(const view_draw_context& context)
{
    const cpu::registers& registers = context.get_emulator().get_cpu_state().registers;

    auto get_color = [&registers](cpu::status_flag flag) -> imgui::vec4 {
        return registers.is_flag_set(flag) ? imgui::vec4{.0f, 1.f, .0f, 1.f} : imgui::vec4{1.f, .0f, .0f, 1.f};
    };

    imgui::text("status: ");

    imgui::same_line();
    imgui::text_colored(get_color(cpu::status_flag::negative), "N");

    imgui::same_line();
    imgui::text_colored(get_color(cpu::status_flag::overflow), "O");

    imgui::same_line();
    imgui::text_colored(imgui::vec4{1.f, .0f, .0f, 1.f}, "-");

    imgui::same_line();
    imgui::text_colored(get_color(cpu::status_flag::break_cmd), "B");

    imgui::same_line();
    imgui::text_colored(get_color(cpu::status_flag::decimal), "D");

    imgui::same_line();
    imgui::text_colored(get_color(cpu::status_flag::interrupt), "I");

    imgui::same_line();
    imgui::text_colored(get_color(cpu::status_flag::zero), "Z");

    imgui::same_line();
    imgui::text_colored(get_color(cpu::status_flag::carry), "C");

    imgui::text("A: ${0:02X} [{0:}]", registers.a);
    imgui::text("Y: ${0:02X} [{0:}]", registers.x);
    imgui::text("X: ${0:02X} [{0:}]", registers.y);

    imgui::text("S: {:02X}", registers.s);
    imgui::text("PC: {:04X}", registers.pc);
}

} // namespace nese::san