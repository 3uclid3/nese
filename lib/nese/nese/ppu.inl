namespace nese {

template<typename BusT>
ppu<BusT>::ppu(BusT& bus)
    : _bus(bus)
{
    for (size_t y = 0; y < screen_height; ++y)
    {
        for (size_t x = 0; x < screen_width; ++x)
        {
            const size_t index = y * screen_width + x;
            _frame_buffers[0][index] = pal_screen[rand() % 2 ? 0x3F : 0x30];
            _frame_buffers[1][index] = _frame_buffers[0][index];
        }
    }
}

template<typename BusT>
void ppu<BusT>::reset()
{
}

template<typename BusT>
void ppu<BusT>::step()
{
    ++_cycle;
    ++_scanline_cycle;

    if (_scanline_cycle >= scanline_max_cycle)
    {
        _scanline_cycle %= scanline_max_cycle;

        ++_scanline;

        if (_scanline >= scanline_count)
        {
            _scanline %= scanline_count;
            swap_buffers();
        }
    }

    const size_t index = _scanline * screen_width + _scanline_cycle.count();
    
    if (ppu_frame_buffer& fb = _back_frame_buffer; index < fb.size())
    {
        fb[index] = pal_screen[rand() % 2 ? 0x3F : 0x30];
    }
}

template<typename BusT>
void ppu<BusT>::step(ppu_cycle_t to_cycle)
{
    while (_cycle < to_cycle)
    {
        NESE_ASSERT_CODE(const ppu_cycle_t pre_step_cycle = _cycle);

        step();

        NESE_ASSERT(_cycle > pre_step_cycle);
    }
}

template<typename BusT>
const ppu_frame_buffer& ppu<BusT>::frame_buffer() const
{
    return _front_frame_buffer;
}

template<typename BusT>
void ppu<BusT>::swap_buffers()
{
    std::swap(_front_frame_buffer, _back_frame_buffer);
}

} // namespace nese
