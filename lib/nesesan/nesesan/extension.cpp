#include <nesesan/extension.hpp>

namespace nese::san {

void extension::install(install_context& context)
{
    install_impl(context);
}

} // namespace nese::san
