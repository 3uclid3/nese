#include <nesesan.debugger/cpu_state_view.hpp>

#include <nese/emulator.hpp>
#include <nesesan/imgui.hpp>
#include <nesesan/view/view_draw_context.hpp>

namespace nese::san {

void cpu_state_view::draw(const view_draw_context& context)
{
    const cpu_registers& registers = context.get_emulator().get_bus().cpu.get_state().registers;

    auto get_color = [&registers](cpu_status flag) -> imgui::vec4 {
        return registers.is_status_set(flag) ? imgui::vec4{.0f, 1.f, .0f, 1.f} : imgui::vec4{1.f, .0f, .0f, 1.f};
    };

    imgui::text("Status: ");

    imgui::same_line();
    imgui::text_colored(get_color(cpu_status::negative), "N");

    imgui::same_line();
    imgui::text_colored(get_color(cpu_status::overflow), "O");

    imgui::same_line();
    imgui::text_colored(imgui::vec4{1.f, .0f, .0f, 1.f}, "-");

    imgui::same_line();
    imgui::text_colored(get_color(cpu_status::break_cmd), "B");

    imgui::same_line();
    imgui::text_colored(get_color(cpu_status::decimal), "D");

    imgui::same_line();
    imgui::text_colored(get_color(cpu_status::interrupt), "I");

    imgui::same_line();
    imgui::text_colored(get_color(cpu_status::zero), "Z");

    imgui::same_line();
    imgui::text_colored(get_color(cpu_status::carry), "C");

    imgui::text("A: ${0:02X} [{0:}]", registers.a);
    imgui::text("Y: ${0:02X} [{0:}]", registers.x);
    imgui::text("X: ${0:02X} [{0:}]", registers.y);

    const addr_t stack_pointer = registers.sp + cpu_stack_offset;
    imgui::text("Stack Pointer: ${:04X}", stack_pointer);
    imgui::text("PC: ${:04X}", registers.pc);
}

} // namespace nese::san