#include <nese/cpu/state_mock.hpp>

namespace nese::cpu {

void state_mock::set_register(register_id id, byte_t value)
{
    switch (id)
    {
    case register_id::a:
        registers.a = value;
        break;

    case register_id::x:
        registers.x = value;
        break;

    case register_id::y:
        registers.y = value;
        break;

    case register_id::s:
        registers.s = value;
        break;

    case register_id::p:
        registers.p = value;
        break;

    case register_id::pc:
        registers.pc = value;
        break;
    }
}

void state_mock::set_register(register_id id, word_t value)
{
    switch (id)
    {
    case register_id::a:
        registers.a = value;
        break;

    case register_id::x:
        registers.x = value;
        break;

    case register_id::y:
        registers.y = value;
        break;

    case register_id::s:
        registers.s = value;
        break;

    case register_id::p:
        registers.p = value;
        break;

    case register_id::pc:
        registers.pc = value;
        break;
    }
}

} // namespace nese::cpu