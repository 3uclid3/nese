#include <nese/utility/crc32.hpp>

namespace nese {

static_assert(crc32("toto") == 0x10CCA4F1);
static_assert(crc32("short") == 0x8F2890A2);

} // namespace nese