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

    [[nodiscard]] const ppu_frame_buffer& frame_buffer() const;

private:
    [[maybe_unused]] void swap_buffers();

private:
    ref_wrap<BusT> _bus;

    array<ppu_frame_buffer, 2> _frame_buffers;
    ref_wrap<ppu_frame_buffer> _front_buffer{_frame_buffers[0]};
    ref_wrap<ppu_frame_buffer> _back_buffer{_frame_buffers[1]};
};

template<typename BusT>
ppu<BusT>::ppu(BusT& bus)
    : _bus(bus)
{
    for (size_t y = 0; y < screen_height; ++y)
    {
        for (size_t x = 0; x < screen_width; ++x)
        {
            _front_buffer.get()[y * screen_width + x] = 0xFFFF0000;
        }
    }
}

template<typename BusT>
const ppu_frame_buffer& ppu<BusT>::frame_buffer() const
{
    return _front_buffer;
}

template<typename BusT>
void ppu<BusT>::swap_buffers()
{
    std::swap(_front_buffer, _back_buffer);
}

} // namespace nese
