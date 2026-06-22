#pragma once

#include "jgraphics.h"

#include <algorithm>
#include <cstdio>

namespace myseq::presetui {

constexpr long kSlotCount = 16;
constexpr long kStoreButton = 0;
constexpr long kSaveButton = 1;
constexpr long kLoadButton = 2;

inline t_rect slotRect(const t_rect& strip, long slot)
{
    const double label_width = 58.0;
    const double right_width = 174.0;
    const double gap = 3.0;
    const double area_x = strip.x + label_width;
    const double area_width = std::max(80.0, strip.width - label_width - right_width);
    const double width = (area_width - gap * 7.0) / 8.0;
    const double height = (strip.height - 11.0) / 2.0;
    return {area_x + (width + gap) * (slot % 8), strip.y + 4.0 + (height + 3.0) * (slot / 8), width, height};
}

inline t_rect buttonRect(const t_rect& strip, long button)
{
    const double width = 54.0;
    const double gap = 3.0;
    const double start = strip.x + strip.width - (width * 3.0 + gap * 2.0) - 4.0;
    return {start + (width + gap) * button, strip.y + 4.0, width, strip.height - 8.0};
}

inline bool contains(t_pt point, const t_rect& rect)
{
    return point.x >= rect.x && point.x <= rect.x + rect.width && point.y >= rect.y && point.y <= rect.y + rect.height;
}

inline long hitSlot(const t_rect& strip, t_pt point)
{
    for (long slot = 0; slot < kSlotCount; ++slot) if (contains(point, slotRect(strip, slot))) return slot;
    return -1;
}

inline long hitButton(const t_rect& strip, t_pt point)
{
    for (long button = 0; button < 3; ++button) if (contains(point, buttonRect(strip, button))) return button;
    return -1;
}

inline void drawText(t_jgraphics* g, const char* text, double x, double y, double size, double r, double green, double b, double alpha = 1.0)
{
    jgraphics_set_font_size(g, size);
    jgraphics_set_source_rgba(g, r, green, b, alpha);
    jgraphics_move_to(g, x, y);
    jgraphics_show_text(g, text);
}

template <typename Bank>
void draw(t_jgraphics* g, const t_rect& strip, const Bank* bank, long active_slot, long store_mode, double red, double green, double blue)
{
    if (!g || !bank || strip.width < 360.0 || strip.height < 34.0) return;
    jgraphics_set_source_rgba(g, 0.030, 0.034, 0.041, 1.0);
    jgraphics_rectangle_fill_fast(g, strip.x, strip.y, strip.width, strip.height);
    jgraphics_set_source_rgba(g, red, green, blue, 0.52);
    jgraphics_rectangle_draw_fast(g, strip.x, strip.y, strip.width, strip.height, 1.0);
    drawText(g, "PRESETS", strip.x + 7.0, strip.y + 15.0, 7.0, 0.92, 0.94, 0.96);
    char status[16];
    if (active_slot >= 0) std::snprintf(status, sizeof(status), "USER %ld", active_slot + 1);
    else std::snprintf(status, sizeof(status), "NO SLOT");
    drawText(g, status, strip.x + 7.0, strip.y + 34.0, 6.5, red, green, blue, 0.92);

    for (long slot = 0; slot < kSlotCount; ++slot) {
        const t_rect r = slotRect(strip, slot);
        const bool valid = bank->valid(static_cast<std::size_t>(slot));
        const bool active = active_slot == slot;
        jgraphics_set_source_rgba(g, active ? red * 0.48 : valid ? 0.075 : 0.045, active ? green * 0.48 : valid ? 0.082 : 0.050, active ? blue * 0.48 : valid ? 0.095 : 0.058, 1.0);
        jgraphics_rectangle_fill_fast(g, r.x, r.y, r.width, r.height);
        jgraphics_set_source_rgba(g, red, green, blue, active ? 0.98 : valid ? 0.54 : 0.20);
        jgraphics_rectangle_draw_fast(g, r.x, r.y, r.width, r.height, active ? 1.5 : 1.0);
        char label[24];
        if (valid && r.width > 48.0) std::snprintf(label, sizeof(label), "%ld %.7s", slot + 1, bank->name(static_cast<std::size_t>(slot)).c_str());
        else std::snprintf(label, sizeof(label), "%ld", slot + 1);
        drawText(g, label, r.x + 4.0, r.y + r.height * 0.68, r.width < 42.0 ? 5.5 : 6.3, 0.94, 0.96, 0.98, valid ? 0.96 : 0.42);
    }

    static const char* labels[] = {"STORE", "SAVE", "LOAD"};
    for (long button = 0; button < 3; ++button) {
        const t_rect r = buttonRect(strip, button);
        const bool active = button == kStoreButton && store_mode;
        jgraphics_set_source_rgba(g, active ? red * 0.48 : 0.055, active ? green * 0.48 : 0.060, active ? blue * 0.48 : 0.070, 1.0);
        jgraphics_rectangle_fill_fast(g, r.x, r.y, r.width, r.height);
        jgraphics_set_source_rgba(g, red, green, blue, active ? 0.98 : 0.48);
        jgraphics_rectangle_draw_fast(g, r.x, r.y, r.width, r.height, active ? 1.5 : 1.0);
        drawText(g, labels[button], r.x + 7.0, r.y + r.height * 0.58, 6.7, 0.95, 0.97, 0.98, active ? 1.0 : 0.76);
    }
}

} // namespace myseq::presetui
