#pragma once

#include <nese/basic_types.hpp>

namespace nese {

template<typename T>
consteval string_view name_of()
{
#ifndef __PRETTY_FUNCTION__

    constexpr size_t size = sizeof(__PRETTY_FUNCTION__);
    constexpr size_t prefix_size = sizeof("string_view nese::name_of() [T = ");
    constexpr size_t suffix_size = sizeof("]");
    constexpr size_t type_size = size - prefix_size - suffix_size + 1;

    return string_view(__PRETTY_FUNCTION__ + prefix_size - 1, type_size);

#else

#error "Unsupported __PRETTY_FUNCTION__"

#endif
}

} // namespace nese
