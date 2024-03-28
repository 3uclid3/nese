#pragma once

#include <nesesan/extension.hpp>

namespace nese::san::imgui {

class extension : public san::extension
{
private:
    void install_impl(install_context& context) override;
};

} // namespace nese::san::imgui
