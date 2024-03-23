#pragma once

#include <imgui.h>

#include <nese/basic_types.hpp>

namespace nese::san::imgui {

using namespace ImGui;

using vec2 = ImVec2;

struct style_var_scope
{
    style_var_scope(int style_var, f32_t value);
    style_var_scope(int style_var, vec2 value);
    ~style_var_scope();
};

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
