#pragma once

#include <nese/basic_types.hpp>
#include <nese/ppu.hpp>
#include <nesesan/texture.hpp>

namespace nese::san {

class view_draw_context;

class game_view
{
public:
    game_view() = default;

    game_view(game_view&&) = default;
    game_view& operator=(game_view&&) = default;

    game_view(const game_view&) = delete;
    game_view& operator=(const game_view&) = delete;

    ~game_view();

    void draw(const view_draw_context& context);

private:
    texture::id _texture_id{texture::invalid_id};
    const ppu_frame_buffer* _previous_frame_buffer{nullptr};
};

} // namespace nese::san