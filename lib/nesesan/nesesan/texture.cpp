#include <nesesan/texture.hpp>

#include <nese/utility/assert.hpp>

namespace nese::san::texture {

namespace {

std::unique_ptr<broker> current_broker;

} // namespace details

id create(u32_t width, u32_t height, std::span<const u32_t> data)
{
    NESE_ASSERT(current_broker);
    return current_broker->create(width, height, data);
}

void update(id id, u32_t width, u32_t height, std::span<const u32_t> data)
{
    NESE_ASSERT(current_broker);
    current_broker->update(id, width, height, data);
}

void destroy(id id)
{
    NESE_ASSERT(current_broker);
    current_broker->destroy(id);
}

void set_broker(std::unique_ptr<broker>&& broker)
{
    current_broker = std::move(broker);
}

} // namespace nese::san::texture