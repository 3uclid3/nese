#pragma once

#include <imgui.h>

#include <nese/basic_types.hpp>
#include <nesesan/imgui/icons.hpp>
#include <nesesan/imgui/format.hpp>

namespace nese::san::imgui {

using vec2 = ImVec2;
using vec4 = ImVec4;
using color = vec4;

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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"

template<typename... T>
void text(fmt::format_string<T...> format, T&&... args)
{
    const char* result = formatv(format, fmt::make_format_args(args...));
    ImGui::Text(result);
}

template<typename... T>
void text_colored(const color& color, fmt::format_string<T...> format, T&&... args)
{
    const char* result = formatv(format, fmt::make_format_args(args...));
    ImGui::TextColored(color, result);
}

template<typename... T>
void text_disabled(fmt::format_string<T...> format, T&&... args)
{
    const char* result = formatv(format, fmt::make_format_args(args...));
    ImGui::TextDisabled(result);
}

template<typename... T>
void text_wrapped(fmt::format_string<T...> format, T&&... args)
{
    const char* result = formatv(format, fmt::make_format_args(args...));
    ImGui::TextWrapped(result);
}

template<typename... T>
void label_text(const char* label, fmt::format_string<T...> format, T&&... args)
{
    const char* result = formatv(format, fmt::make_format_args(args...));
    ImGui::LabelText(label, result);
}

template<typename... T>
void bullet_text(fmt::format_string<T...> format, T&&... args)
{
    const char* result = formatv(format, fmt::make_format_args(args...));
    ImGui::Text(result);
}

#pragma clang diagnostic pop

inline indent_scope::indent_scope(f32_t width)
    : indent_width(width)
{
    ImGui::Indent(indent_width);
}

inline indent_scope::~indent_scope()
{
    ImGui::Unindent(indent_width);
}

inline disabled_scope::disabled_scope(bool disabled)
{
    ImGui::BeginDisabled(disabled);
}

inline disabled_scope::~disabled_scope()
{
    ImGui::EndDisabled();
}

inline style_var_scope::style_var_scope(int style_var, f32_t value)
{
    ImGui::PushStyleVar(style_var, value);
}

inline style_var_scope::style_var_scope(int style_var, vec2 value)
{
    ImGui::PushStyleVar(style_var, value);
}

inline style_var_scope::~style_var_scope()
{
    ImGui::PopStyleVar();
}

} // namespace nese::san::imgui
