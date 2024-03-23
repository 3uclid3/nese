#pragma once

#include <nesesan.debugger/cpu_debugger_view.hpp>
#include <nesesan/extension.hpp>

namespace nese::san::debugger {

class extension : public san::extension
{
private:
    void install_impl(install_context& context) override;

private:
    cpu_debugger_view _cpu_debugger_view;
};

} // namespace nese::san::debugger
