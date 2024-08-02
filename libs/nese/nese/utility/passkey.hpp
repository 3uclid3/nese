#pragma once

namespace nese {

template<typename T>
class passkey
{
    friend T;

    constexpr passkey() {}
    constexpr passkey(const passkey&) {}
    constexpr passkey& operator=(const passkey&) = delete;
};

} // namespace nese
