#pragma once

#include <memory>

#include <nese/basic_types.hpp>
#include <nese/cartridge/cartridge_mapper.hpp>

namespace nese {

class cartridge
{
public:
    static cartridge from_file(const char* filepath);

public:
    cartridge() = default;
    explicit cartridge(std::unique_ptr<cartridge_mapper>&& mapper);

    [[nodiscard]] const std::vector<byte_t>& get_prg() const;
    [[nodiscard]] const std::vector<byte_t>& get_chr() const;

    [[nodiscard]] byte_t read(addr_t addr) const;
    void write(addr_t addr, byte_t value);

    [[nodiscard]] bool is_valid() const;

private:
    std::unique_ptr<cartridge_mapper> _mapper;
};

inline const std::vector<byte_t>& cartridge::get_prg() const
{
    return _mapper->get_prg();
}

inline const std::vector<byte_t>& cartridge::get_chr() const
{
    return _mapper->get_chr();
}

inline bool cartridge::is_valid() const
{
    return _mapper != nullptr;
}

} // namespace nese
