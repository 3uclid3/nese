#pragma once

#include <memory>

#include <nesesan/extension.hpp>

namespace nese::san::debugger {

class cpu_debugger_view;

class extension : public san::extension
{
public:
    extension();
    ~extension() override;

private:
    void install_impl(install_context& context) override;

private:
    std::unique_ptr<cpu_debugger_view> _cpu_debugger_view;
    // ram_view _ram_view{_cpu_debugger_view.get_debugger().get_ram()};
};

} // namespace nese::san::debugger
