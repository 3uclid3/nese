#pragma once

#include <array>

#include <nese/basic_types.hpp>

namespace nese::memory {

// 2KB
using ram = std::array<byte_t, 2048>;

} // namespace nese::memory
