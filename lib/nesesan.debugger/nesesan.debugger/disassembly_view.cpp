#include <nesesan.debugger/disassembly_view.hpp>

#include <nese/basic_types.hpp>
#include <nese/emulator.hpp>
#include <nesesan/imgui.hpp>
#include <nesesan/view/view_draw_context.hpp>

namespace nese::san {

void disassembly_view::draw(const view_draw_context& context)
{
    imgui::checkbox("Auto Scroll", _auto_scroll);

    imgui::separator();

    const emulator& emulator = context.get_emulator();
    const bus& bus = emulator.get_bus();

    if (_disassembly.is_empty())
    {
        if (bus.cartridge.is_valid())
        {
            _disassembly = disassembly::disassemble(bus);
        }

        if (_disassembly.is_empty())
        {
            return;
        }
    }

    imgui::begin_child("disassembly");

    constexpr addr_t min_addr = 0x8000;

    const addr_t pc_addr = std::max<addr_t>(bus.cpu.get_state().registers.pc, min_addr);

    // const auto height = ImGui::GetContentRegionAvail().y;
    // const auto line_height = ImGui::CalcTextSize("JMP").y;
    // const auto line_count = (std::ceil(height / line_height));
    // const auto line_half_count = line_count / 2 + 1;
    // const auto top_count = pc_addr - min_addr;
    // const auto right_count = line_count - top_count - 1;

    for (const disassembly::line& line : _disassembly)
    {
        if (line.addr == pc_addr)
        {
            if (_auto_scroll)
            {
                ImGui::SetScrollHereY();
            }

            imgui::text_colored(imgui::color(0, 0xFF, 0, 0xFF), line.to_string());
        }
        else
        {
            imgui::text(line.to_string());
        }
    }

    imgui::end_child();
}

} // namespace nese::san