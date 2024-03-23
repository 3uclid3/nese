#pragma once

#include <imgui.h>

#include <nese/basic_types.hpp>

namespace nese::san::imgui {

using namespace ImGui;

using vec2 = ImVec2;

struct indent_scope
{
    indent_scope(f32_t width = 0);
    ~indent_scope();

    f32_t indent_width;
};

struct disabled_scope
{
    disabled_scope(bool disabled = true);
    ~disabled_scope();
};

struct style_var_scope
{
    style_var_scope(int style_var, f32_t value);
    style_var_scope(int style_var, vec2 value);
    ~style_var_scope();
};

inline indent_scope::indent_scope(f32_t width)
    : indent_width(width)
{
    Indent(indent_width);
}

inline indent_scope::~indent_scope()
{
    Unindent(indent_width);
}

inline disabled_scope::disabled_scope(bool disabled)
{
    BeginDisabled(disabled);
}

inline disabled_scope::~disabled_scope()
{
    EndDisabled();
}

inline style_var_scope::style_var_scope(int style_var, f32_t value)
{
    PushStyleVar(style_var, value);
}

inline style_var_scope::style_var_scope(int style_var, vec2 value)
{
    PushStyleVar(style_var, value);
}

inline style_var_scope::~style_var_scope()
{
    PopStyleVar();
}

}
