#include <catch2/catch_test_macros.hpp>

#include <string>

#include <nese.gui/menu/basic_menu.hpp>
#include <nese/basic_types.hpp>

namespace nese::gui {

struct null_scope
{
    static bool begin()
    {
        return false;
    }

    static void end()
    {
    }
};

struct leaf_menu_item_mock
{
    std::string name;
};

struct leaf_with_arg_menu_item_mock
{
    std::string name;
    size_t size{};
};

using basic_menu_mock = basic_menu<null_scope, leaf_menu_item_mock, leaf_with_arg_menu_item_mock>;
using basic_menu_item_mock = basic_menu_mock::menu_item;
using basic_submenu_item_mock = basic_menu_mock::submenu_item;

bool is_branch(const basic_menu_item_mock& item)
{
    return std::holds_alternative<basic_submenu_item_mock>(item);
}

bool is_leaf(const basic_menu_item_mock& item)
{
    return std::holds_alternative<leaf_menu_item_mock>(item);
}

bool has_branch(const basic_menu_item_mock& menu, std::string_view name)
{
    if (!std::holds_alternative<basic_submenu_item_mock>(menu))
    {
        return false;
    }

    const auto* item = std::get<basic_submenu_item_mock>(menu).find_item(name);

    return item && is_branch(*item);
}

bool has_leaf(const basic_menu_item_mock& menu, std::string_view name)
{
    if (!std::holds_alternative<basic_submenu_item_mock>(menu))
    {
        return false;
    }

    const auto* item = std::get<basic_submenu_item_mock>(menu).find_item(name);

    return item && is_leaf(*item);
}

bool has_branch(const basic_submenu_item_mock& menu, std::string_view name)
{
    const auto* item = menu.find_item(name);

    return item && is_branch(*item);
}

bool has_leaf(const basic_submenu_item_mock& menu, std::string_view name)
{
    const auto* item = menu.find_item(name);

    return item && is_leaf(*item);
}

bool has_branch(const basic_menu_mock& menu, std::string_view name)
{
    const auto* item = menu.find_item(name);

    return item && is_branch(*item);
}

bool has_leaf(const basic_menu_mock& menu, std::string_view name)
{
    const auto* item = menu.find_item(name);

    return item && is_leaf(*item);
}

TEST_CASE("basic_menu add", "[imgui]")
{
    basic_menu_mock menu;

    menu.add<leaf_menu_item_mock>("root/child/subchild/leaf");
    menu.add<leaf_menu_item_mock>("root/child/subchild/leaf 2");
    menu.add<leaf_menu_item_mock>("root/child/subchild 2/leaf");
    menu.add<leaf_menu_item_mock>("root/child/subchild 2/leaf 2");
    menu.add<leaf_menu_item_mock>("root/child/leaf");
    menu.add<leaf_menu_item_mock>("root/child/leaf 2");
    menu.add<leaf_menu_item_mock>("root/child 2/leaf");
    menu.add<leaf_menu_item_mock>("root/child 2/leaf 2");
    menu.add<leaf_menu_item_mock>("root/leaf");
    menu.add<leaf_menu_item_mock>("root/leaf 2");
    menu.add<leaf_menu_item_mock>("root 2/leaf");
    menu.add<leaf_menu_item_mock>("root 2/leaf 2");
    menu.add<leaf_menu_item_mock>("leaf");
    menu.add<leaf_menu_item_mock>("leaf 2");

    CHECK(has_leaf(menu, "leaf"));
    CHECK(has_leaf(menu, "leaf 2"));

    auto* root_item = menu.find_item("root");
    REQUIRE(root_item);

    CHECK(has_leaf(*root_item, "leaf"));
    CHECK(has_leaf(*root_item, "leaf 2"));

    auto* child = std::get<basic_submenu_item_mock>(*root_item).find_item("child");
    REQUIRE(child);

    CHECK(has_leaf(*child, "leaf"));
    CHECK(has_leaf(*child, "leaf 2"));

    auto* subchild = std::get<basic_submenu_item_mock>(*child).find_item("subchild");
    REQUIRE(subchild);

    CHECK(has_leaf(*subchild, "leaf"));
    CHECK(has_leaf(*subchild, "leaf 2"));

    auto* subchild2 = std::get<basic_submenu_item_mock>(*child).find_item("subchild 2");
    REQUIRE(subchild2);

    CHECK(has_leaf(*subchild2, "leaf"));
    CHECK(has_leaf(*subchild2, "leaf 2"));
}

TEST_CASE("basic_menu add a submenu", "[imgui]")
{
    basic_menu_mock menu;

    menu.add<basic_submenu_item_mock>("root");

    CHECK(has_branch(menu, "root"));
}

TEST_CASE("basic_menu add with arg", "[imgui]")
{
    basic_menu_mock menu;

    const leaf_with_arg_menu_item_mock& menu_item = menu.add<leaf_with_arg_menu_item_mock>("root", size_t{42});

    CHECK(menu_item.size == 42);
}

} // namespace nese::gui