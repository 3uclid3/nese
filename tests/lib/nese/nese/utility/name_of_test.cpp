#include <nese/utility/name_of.hpp>

namespace nese {

struct test_a_type
{
};

struct test_b_type
{
};

using alias_test_a_type = test_a_type;
using alias_test_b_type = test_b_type;

static_assert(name_of<bool>() == "bool");
static_assert(name_of<char>() == "char");

static_assert(name_of<test_a_type>() == "nese::test_a_type");
static_assert(name_of<test_b_type>() == "nese::test_b_type");
static_assert(name_of<alias_test_a_type>() == "nese::test_a_type");
static_assert(name_of<alias_test_b_type>() == "nese::test_b_type");

} // namespace nese
