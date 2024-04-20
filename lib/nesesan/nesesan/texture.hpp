#pragma once

#include <memory>
#include <span>

#include <nese/basic_types.hpp>

namespace nese::san::texture {

using id = u64_t;

inline static constexpr id invalid_id{0};

id create(u32_t width, u32_t height, std::span<const u32_t> data);
void update(id id, u32_t width, u32_t height, std::span<const u32_t> data);
void destroy(id id);

class broker
{
public:
    virtual ~broker() = default;
    virtual id create(u32_t width, u32_t height, std::span<const u32_t> data) = 0;
    virtual void update(id id, u32_t width, u32_t height, std::span<const u32_t> data) = 0;
    virtual void destroy(id id) = 0;
};

void set_broker(std::unique_ptr<broker>&& broker);

} // namespace nese::san::texture
