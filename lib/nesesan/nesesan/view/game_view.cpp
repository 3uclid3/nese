#include <nesesan/view/game_view.hpp>

#include <nese/emulator.hpp>
#include <nesesan/imgui.hpp>
#include <nesesan/view/view_draw_context.hpp>

namespace nese::san {

game_view::~game_view()
{
    if (_texture_id != texture::invalid_id)
    {
        texture::destroy(_texture_id);
    }
}

void game_view::draw(const view_draw_context& context)
{
    const emulator& emulator = context.get_emulator();
    const bus& bus = emulator.get_bus();
    const ppu_frame_buffer& frame_buffer = bus.ppu.frame_buffer();

    if (_texture_id == texture::invalid_id)
    {
        _texture_id = texture::create(screen_width, screen_height, frame_buffer);

        _previous_frame_buffer = &frame_buffer;
    }
    else if (_previous_frame_buffer != &frame_buffer)
    {
        texture::update(_texture_id, screen_width, screen_height, frame_buffer);

        _previous_frame_buffer = &frame_buffer;
    }

    const auto region_avail = ImGui::GetContentRegionAvail();

    float factor = 1.f;

    if (region_avail.x > region_avail.y)
    {
        factor = region_avail.y / screen_height;
    }
    else
    {
        factor = region_avail.x / screen_width;
    }

    factor = std::max(1.f, factor);

    const auto size = ImVec2(screen_width * factor, screen_height * factor);
    const auto diff_x = std::abs(region_avail.x - size.x);

    ImGui::SetCursorPosX(diff_x / 2.f);

    ImGui::Image(reinterpret_cast<ImTextureID>(_texture_id), size);
}

} // namespace nese::san
