#pragma once

#include <nese/basic_types.hpp>

namespace nese {

namespace cpu {
struct state;
}

namespace memory {
class mapper;
}

namespace nintendulator {

const char* format(const cpu::state& cpu_state, const memory::mapper& memory_mapper);

}

} // namespace nese
