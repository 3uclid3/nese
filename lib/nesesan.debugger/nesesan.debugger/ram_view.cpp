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
    static char buffer[20]={'\0'};
    imgui::InputText("Address", &buffer[0], 20, ImGuiInputTextFlags_CharsHexadecimal);

    imgui::SameLine();
    if (imgui::BeginCombo("Columns", "2"))
    {
        imgui::Selectable("2");
        imgui::Selectable("4");
        imgui::Selectable("8");
        imgui::Selectable("16");

        imgui::EndCombo();    
    }

    imgui::Separator();

    if (imgui::BeginListBox("Data"))
    {
        for (size_t i = 0; i < memory::ram::max_size; ++i)
        {
            const auto addr_str  = fmt::format("0x{:04X}", i);

            imgui::TextDisabled("%s", addr_str.c_str());

            auto value_str = fmt::format("{:02X}", _ram.get_byte(i));

            ImGui::SameLine();
            imgui::Text("%s", value_str.c_str());
        }
        imgui::EndListBox();
    }
}

} // namespace nese::san::debugger