#include <catch2/catch_test_macros.hpp>

#include <array>

#include <nese/basic_types.hpp>
#include <nese/utility/format.hpp>

namespace nese {

TEST_CASE("Concurrent format strings", "[utility]")
{
    std::array<const char*, format_circular_buffer::count> buffers;

    for (size_t i = 0; i < buffers.size(); ++i)
    {
        buffers[i] = format("format #{}", i);
    }

    for (size_t i = 0; i < buffers.size(); ++i)
    {
        CHECK(strcmp(buffers[i], format("format #{}", i)) == 0);
    }

    CHECK(strcmp(buffers[0], format("format #{}", 12)) == 0);
}

} // namespace nese