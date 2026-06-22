#pragma once

#include "jgraphics.h"

#include <algorithm>
#include <cstring>

namespace myseq::tooltipui {

enum Kind : long {
    kNone = 0,
    kHeaderButton,
    kPresetSlot,
    kPresetButton,
    kMorphField,
    kCharacterButton,
    kControl,
    kFooter
};

struct State {
    long kind = kNone;
    long index = -1;
    long age = 0;
    t_pt point {0.0, 0.0};
};

inline void setTarget(State& state, long kind, long index, t_pt point)
{
    if (state.kind != kind || state.index != index) {
        state.kind = kind;
        state.index = index;
        state.age = 0;
    }
    state.point = point;
}

inline void hide(State& state)
{
    state.kind = kNone;
    state.index = -1;
    state.age = 0;
}

inline void tick(State& state)
{
    if (state.kind != kNone && state.age < 30) ++state.age;
}

inline void tick(State& state, bool enabled)
{
    if (enabled)
        tick(state);
    else
        hide(state);
}

inline void drawText(t_jgraphics* g, const char* value, double x, double y, double size, double red, double green, double blue, double alpha = 1.0)
{
    jgraphics_set_font_size(g, size);
    jgraphics_set_source_rgba(g, red, green, blue, alpha);
    jgraphics_move_to(g, x, y);
    jgraphics_show_text(g, value);
}

inline void draw(t_jgraphics* g, const t_rect& bounds, const State& state, const char* title, const char* section, const char* body, double red, double green, double blue)
{
    if (!g || state.kind == kNone || state.age < 8 || !title || !*title || bounds.width < 350.0) return;
    const double width = std::min(430.0, std::max(330.0, bounds.width - 20.0));
    const double height = 56.0;
    double x = state.point.x + 16.0;
    double y = state.point.y + 20.0;
    if (x + width > bounds.width - 6.0) x = state.point.x - width - 16.0;
    if (y + height > bounds.height - 6.0) y = state.point.y - height - 16.0;
    x = std::max(6.0, std::min(x, bounds.width - width - 6.0));
    y = std::max(6.0, std::min(y, bounds.height - height - 6.0));

    jgraphics_set_source_rgba(g, 0.0, 0.0, 0.0, 0.38);
    jgraphics_rectangle_fill_fast(g, x + 3.0, y + 3.0, width, height);
    jgraphics_set_source_rgba(g, 0.025, 0.030, 0.038, 0.985);
    jgraphics_rectangle_fill_fast(g, x, y, width, height);
    jgraphics_set_source_rgba(g, red, green, blue, 0.96);
    jgraphics_rectangle_fill_fast(g, x, y, 4.0, height);
    jgraphics_rectangle_draw_fast(g, x, y, width, height, 1.0);

    drawText(g, title, x + 12.0, y + 18.0, 9.0, 0.96, 0.98, 1.0);
    if (section && *section) {
        const double sx = x + width - 12.0 - static_cast<double>(std::strlen(section)) * 4.7;
        drawText(g, section, sx, y + 17.0, 7.2, red, green, blue, 0.96);
    }
    drawText(g, body ? body : "", x + 12.0, y + 40.0, 7.8, 0.76, 0.81, 0.85, 0.98);
}

} // namespace myseq::tooltipui
