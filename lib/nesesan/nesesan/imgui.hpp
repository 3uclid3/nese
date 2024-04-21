#pragma once

#include <imgui.h>
#include <imgui_stdlib.h>

#include <nese/basic_types.hpp>
#include <nese/utility/format.hpp>

namespace nese::san::imgui {

using vec2 = ImVec2;
using vec4 = ImVec4;
using color = vec4;

using style_var = ImGuiStyleVar;
using color_style_var = ImGuiCol;

struct color_scope
{
    color_scope(color_style_var var, u32_t color);
    color_scope(color_style_var var, color color);
    ~color_scope();
};

struct indent_scope
{
    explicit indent_scope(f32_t width = 0);
    ~indent_scope();

    f32_t indent_width;
};

struct disabled_scope
{
    explicit disabled_scope(bool disabled = true);
    ~disabled_scope();
};

struct style_var_scope
{
    style_var_scope(style_var var, f32_t value);
    style_var_scope(style_var var, vec2 value);
    ~style_var_scope();
};

inline bool begin_child(const char* str_id, vec2 size = {0,0})
{
    return ImGui::BeginChild(str_id, size);
}

inline void end_child()
{
    ImGui::EndChild();
}

inline void begin_group()
{
    ImGui::BeginGroup();
}

inline void end_group()
{
    ImGui::EndGroup();
}

inline void same_line(f32_t offset_from_start_x = 0.0f, f32_t spacing = -1.0f)
{
    ImGui::SameLine(offset_from_start_x, spacing);
}

inline void spacing()
{
    ImGui::Spacing();
}

inline void separator()
{
    ImGui::Separator();
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"

inline bool checkbox(const char* text, bool& is_checked)
{
    return ImGui::Checkbox(text, &is_checked);
}

inline void text(const char* text)
{
    ImGui::Text(text);
}

inline void text_colored(const color& color, const char* text)
{
    ImGui::TextColored(color, text);
}

template<typename... T>
bool button(fmt::format_string<T...> format, T&&... args)
{
    const char* result = v_format(format, fmt::make_format_args(args...));
    return ImGui::Button(result);
}

template<typename... T>
void text(fmt::format_string<T...> format, T&&... args)
{
    const char* result = v_format(format, fmt::make_format_args(args...));
    ImGui::Text(result);
}

template<typename... T>
void text_colored(const color& color, fmt::format_string<T...> format, T&&... args)
{
    const char* result = v_format(format, fmt::make_format_args(args...));
    ImGui::TextColored(color, result);
}

template<typename... T>
void text_disabled(fmt::format_string<T...> format, T&&... args)
{
    const char* result = v_format(format, fmt::make_format_args(args...));
    ImGui::TextDisabled(result);
}

template<typename... T>
void text_wrapped(fmt::format_string<T...> format, T&&... args)
{
    const char* result = v_format(format, fmt::make_format_args(args...));
    ImGui::TextWrapped(result);
}

template<typename... T>
void label_text(const char* label, fmt::format_string<T...> format, T&&... args)
{
    const char* result = v_format(format, fmt::make_format_args(args...));
    ImGui::LabelText(label, result);
}

template<typename... T>
void bullet_text(fmt::format_string<T...> format, T&&... args)
{
    const char* result = v_format(format, fmt::make_format_args(args...));
    ImGui::Text(result);
}

#pragma clang diagnostic pop

inline color_scope::color_scope(color_style_var var, u32_t color)
{
    ImGui::PushStyleColor(var, color);
}

inline color_scope::color_scope(color_style_var var, color color)
{
    ImGui::PushStyleColor(var, color);
}

inline color_scope::~color_scope()
{
    ImGui::PopStyleColor();
}

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

inline style_var_scope::style_var_scope(style_var var, f32_t value)
{
    ImGui::PushStyleVar(var, value);
}

inline style_var_scope::style_var_scope(style_var var, vec2 value)
{
    ImGui::PushStyleVar(var, value);
}

inline style_var_scope::~style_var_scope()
{
    ImGui::PopStyleVar();
}

} // namespace nese::san::imgui
