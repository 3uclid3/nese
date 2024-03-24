#include <nesesan/imgui/format.hpp>

#include <array>
#include <span>
#include <windows.h>

namespace nese::san::imgui {

namespace details {

struct local_data
{
    static constexpr size_t BufferSize = 8192;
    static constexpr size_t BufferCount = 16;
    static constexpr size_t LastBufferIndex = BufferCount - 1;

    std::span<char> next_buffer()
    {
        const size_t offset = next_buffer_index * BufferSize;

        next_buffer_index = next_buffer_index == LastBufferIndex ? 0 : next_buffer_index + 1;

        return {&buffer[offset], BufferSize};
    }

    std::array<char, BufferSize * BufferCount> buffer;
    size_t next_buffer_index{0};
};

struct output
{
    explicit output(std::span<char> buffer, size_t size = 0)
        : buffer(buffer)
        , size(size)
    {
    }

    output& operator=(char v)
    {
        buffer[size++] = v;
        return *this;
    }

    [[nodiscard]] char& operator*()
    {
        return buffer[size];
    }

    output& operator++()
    {
        ++size;
        return *this;
    }

    output operator++(int)
    {
        output out(buffer, size);

        ++size;

        return out;
    }

    std::span<char> buffer;
    size_t size{0};
};

thread_local local_data data;

} // namespace details

const char* vformat(fmt::string_view format, fmt::format_args args)
{
    details::output out(details::data.next_buffer());

    out = fmt::vformat_to(out, format, args);

    out = '\0';

    return out.buffer.data();
}

} // namespace nese::san::imgui
