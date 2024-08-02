#pragma once

#include <nese/basic_types.hpp>
#include <nese/utility/assert.hpp>

namespace nese {

inline static constexpr size_t screen_height = 240;
inline static constexpr size_t screen_width = 256;

using ppu_frame_buffer = array<u32_t, screen_height * screen_width>;

template<typename BusT>
class ppu
{
public:
    explicit ppu(BusT& bus);

public:
    void step();
    void step(ppu_cycle_t to_cycle);
    void reset();

public:
    [[nodiscard]] const ppu_frame_buffer& frame_buffer() const;

    [[nodiscard]] ppu_cycle_t get_cycle() const;
    [[nodiscard]] ppu_cycle_t get_scanline_cycle() const;

    [[nodiscard]] size_t get_scanline() const;

private:
    [[maybe_unused]] void swap_buffers();

private:
    // clang-format off
    // Colors are in format ARGB
    // Table taken from https://wiki.nesdev.com/w/index.php/PPU_palettes
    static constexpr array<u32_t, 0x40> pal_screen{
        0xFF545454, 0xFF001E74, 0xFF081090, 0xFF300088, 0xFF440064, 0xFF5C0030, 0xFF540400, 0xFF3C1800, 0xFF202A00, 0xFF083A00, 0xFF004000, 0xFF003C00, 0xFF00323C, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF989698, 0xFF084CC4, 0xFF3032EC, 0xFF5C1EE4, 0xFF8814B0, 0xFFA01464, 0xFF982220, 0xFF783C00, 0xFF545A00, 0xFF287200, 0xFF087C00, 0xFF007628, 0xFF006678, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFFECEEEC, 0xFF4C9AEC, 0xFF787CEC, 0xFFB062EC, 0xFFE454EC, 0xFFEC58B4, 0xFFEC6A64, 0xFFD48820, 0xFFA0AA00, 0xFF74C400, 0xFF4CD020, 0xFF38CC6C, 0xFF38B4CC, 0xFF3C3C3C, 0xFF000000, 0xFF000000,
        0xFFECEEEC, 0xFFA8CCEC, 0xFFBCBCEC, 0xFFD4B2EC, 0xFFECAEEC, 0xFFECAED4, 0xFFECD4AE, 0xFFE4C490, 0xFFCCD278, 0xFFB4DE78, 0xFFA8E290, 0xFF98E2B4, 0xFFA0D6E4, 0xFFA0A2A0, 0xFF000000, 0xFF000000
    };
    // clang-format on

    static constexpr ppu_cycle_t scanline_max_cycle{341};
    static constexpr size_t scanline_count{362};

public:
    ref_wrap<BusT> _bus;

    array<ppu_frame_buffer, 2> _frame_buffers;
    ref_wrap<ppu_frame_buffer> _front_frame_buffer{_frame_buffers[0]};
    ref_wrap<ppu_frame_buffer> _back_frame_buffer{_frame_buffers[1]};

    array<array<byte_t, 4096>, 2> _pattern_tables;
    array<byte_t, 32> _palette_table;

    ppu_cycle_t _cycle{0};
    ppu_cycle_t _scanline_cycle{0};

    size_t _scanline{0};
};

} // namespace nese

#include <nese/ppu.inl>
