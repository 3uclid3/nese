#pragma once

namespace nese {

class emulator;

namespace san {

class view_draw_context
{
public:
    explicit view_draw_context(emulator& emulator);

    const emulator& get_emulator() const;
    emulator& get_emulator();

private:
    emulator& _emulator;
};

inline view_draw_context::view_draw_context(emulator& emulator)
    : _emulator(emulator)
{
}

inline const emulator& view_draw_context::get_emulator() const
{
    return _emulator;
}

inline emulator& view_draw_context::get_emulator()
{
    return _emulator;
}

} // namespace san
} // namespace nese