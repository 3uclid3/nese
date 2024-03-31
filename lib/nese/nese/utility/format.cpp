#include <nese/utility/format.hpp>

#include <array>
#include <span>

namespace nese {

namespace details {

struct local_data
{
    static constexpr size_t last_buffer_index = format_circular_buffer::count - 1;

    std::span<char> next_buffer()
    {
        const size_t offset = next_buffer_index * format_circular_buffer::size;

        next_buffer_index = next_buffer_index == last_buffer_index ? 0 : next_buffer_index + 1;

        return {&buffer[offset], format_circular_buffer::size};
    }

    std::array<char, format_circular_buffer::size * format_circular_buffer::count> buffer{};
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

    std::span<char> buffer{};
    size_t size{0};
};

thread_local local_data data{};

} // namespace details

const char* v_format(fmt::string_view format, fmt::format_args args)
{
    details::output out(details::data.next_buffer());

    out = fmt::vformat_to(out, format, args);

    out = '\0';

    return out.buffer.data();
}

} // namespace nese
