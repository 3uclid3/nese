#include <nesesan.debugger/ram_view.hpp>

#include <nese/memory/ram.hpp>
#include <nesesan/imgui/core.hpp>

namespace nese::san::debugger {

ram_view::ram_view(const memory::ram& ram)
    : _ram(ram)
{
}

void ram_view::update(f32_t, bool&)
{
    static char buffer[20] = {'\0'};
    ImGui::InputText("Address", &buffer[0], 20, ImGuiInputTextFlags_CharsHexadecimal);

    ImGui::SameLine();
    if (ImGui::BeginCombo("Columns", "2"))
    {
        ImGui::Selectable("2");
        ImGui::Selectable("4");
        ImGui::Selectable("8");
        ImGui::Selectable("16");

        ImGui::EndCombo();
    }

    ImGui::Separator();

    if (ImGui::BeginListBox("Data"))
    {
        for (size_t i = 0; i < memory::ram::max_size; ++i)
        {
            imgui::text_disabled("0x{:04X}", i);

            ImGui::SameLine();
            imgui::text("{:02X}", _ram.get_byte(static_cast<addr_t>(i)));
        }

        ImGui::EndListBox();
    }
}

} // namespace nese::san::debugger