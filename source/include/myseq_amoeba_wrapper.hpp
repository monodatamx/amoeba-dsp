#pragma once

#ifndef AMOEBA_OUTPUTS_CLASS
#error "AMOEBA_OUTPUTS_CLASS must be defined"
#endif
#ifndef AMOEBA_OUTPUTS_KIND
#error "AMOEBA_OUTPUTS_KIND must be defined"
#endif

#include "ext.h"
#include "ext_obex.h"
#include "jgraphics.h"
#include "jpatcher_api.h"
#include "z_dsp.h"

#include "myseq_amoeba_dsp.hpp"
#include "myseq_preset_bank.hpp"
#include "myseq_preset_ui.hpp"
#include "myseq_tooltip_ui.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <new>
#include <string>

namespace {

using myseq::amoeba::AmoebaEngine;
using myseq::amoeba::Event;
using myseq::amoeba::Kind;
using AmoebaPresetBank = myseq::presets::PresetBank<71>;

constexpr Kind kAmoebaKind = AMOEBA_OUTPUTS_KIND;
constexpr long kHeaderButtonCount = 8;
constexpr long kSpeakerControlCount = 3;
constexpr double kUiIntervalMs = 33.0;
constexpr std::size_t kEventQueueSize = 256;

enum HitKind : long {
    kHitNone = 0,
    kHitHeader,
    kHitPresetSlot,
    kHitPresetButton,
    kHitSpeaker,
    kHitCenter,
    kHitSpeakerControl,
    kHitCharacter,
    kHitParameter
};

enum DragKind : long {
    kDragNone = -1,
    kDragCenter = -2,
    kDragSpeakerOffset = 100,
    kDragSpeakerControlOffset = 200,
    kDragParameterOffset = 300
};

template <std::size_t Capacity>
class AmoebaEventQueue {
public:
    bool push(const Event& event) noexcept
    {
        const auto write = write_.load(std::memory_order_relaxed);
        const auto next = (write + 1) % Capacity;
        if (next == read_.load(std::memory_order_acquire)) return false;
        events_[write] = event;
        write_.store(next, std::memory_order_release);
        return true;
    }

    bool pop(Event& event) noexcept
    {
        const auto read = read_.load(std::memory_order_relaxed);
        if (read == write_.load(std::memory_order_acquire)) return false;
        event = events_[read];
        read_.store((read + 1) % Capacity, std::memory_order_release);
        return true;
    }

private:
    std::array<Event, Capacity> events_ {};
    std::atomic<std::size_t> write_ {0};
    std::atomic<std::size_t> read_ {0};
};

struct AmoebaRuntime {
    std::array<std::atomic<double>, myseq::amoeba::kParameterCount> parameters;
    std::array<std::atomic<double>, myseq::amoeba::kMaxSpeakers> azimuth;
    std::array<std::atomic<double>, myseq::amoeba::kMaxSpeakers> radius;
    std::array<std::atomic<double>, myseq::amoeba::kMaxSpeakers> elevation;
    std::array<std::atomic<double>, myseq::amoeba::kMaxSpeakers> peak;
    std::array<std::array<std::atomic<double>, 3>, myseq::amoeba::kMaxAgents> agents;
    std::atomic<long> agent_count {0};
    std::atomic<double> center_x {0.0};
    std::atomic<double> center_y {0.0};
    std::atomic<long> bypass {0};
    std::atomic<long> freeze {0};
    std::atomic<std::uint32_t> reset_serial {0};
};

typedef struct _amoeba_outputs {
    t_pxjbox object;
    long outlet_count;
    long output_attribute;
    long input_connected;
    long auto_enabled;
    long freeze_enabled;
    long bypass_enabled;
    long tooltips_enabled;
    long selected_speaker;
    long active_character;
    long active_preset;
    long preset_store_mode;
    long drag_target;
    long queue_warning_shown;
    double center_x;
    double center_y;
    double display_activity;
    std::array<double, myseq::amoeba::kParameterCount> parameters;
    std::array<double, myseq::amoeba::kMaxSpeakers> speaker_azimuth;
    std::array<double, myseq::amoeba::kMaxSpeakers> speaker_radius;
    std::array<double, myseq::amoeba::kMaxSpeakers> speaker_elevation;
    std::array<double, myseq::amoeba::kMaxSpeakers> display_peak;
    AmoebaEngine* engine;
    AmoebaRuntime* runtime;
    AmoebaEventQueue<kEventQueueSize>* events;
    AmoebaPresetBank* presets;
    void* ui_clock;
    std::uint32_t dsp_reset_serial;
    std::uint32_t rng;
    myseq::tooltipui::State tooltip;
} t_amoeba_outputs;

struct Layout {
    t_rect bounds;
    t_rect header;
    t_rect presets;
    t_rect radial;
    t_rect side;
    t_rect controls;
};

struct Hit { long kind = kHitNone; long index = -1; };

static t_class* s_amoeba_class = nullptr;

double uiClip(double value, double low, double high) { return std::max(low, std::min(high, value)); }
bool contains(t_pt point, const t_rect& rect) { return point.x >= rect.x && point.x <= rect.x + rect.width && point.y >= rect.y && point.y <= rect.y + rect.height; }
double distance(double x1, double y1, double x2, double y2) { const double x = x1 - x2, y = y1 - y2; return std::sqrt(x * x + y * y); }

void redraw(t_amoeba_outputs* x) { if (x) jbox_redraw(reinterpret_cast<t_jbox*>(x)); }

std::uint32_t nextRandom(t_amoeba_outputs* x)
{
    std::uint32_t value = x->rng ? x->rng : 0x6d2b79f5u;
    value ^= value << 13; value ^= value >> 17; value ^= value << 5;
    x->rng = value;
    return value;
}

double random01(t_amoeba_outputs* x) { return static_cast<double>(nextRandom(x)) / static_cast<double>(UINT32_MAX); }

void roundRect(t_jgraphics* g, const t_rect& rect, double radius, double red, double green, double blue, double alpha, bool fill, double width = 1.0)
{
    jgraphics_set_source_rgba(g, red, green, blue, alpha);
    jgraphics_rectangle_rounded(g, rect.x, rect.y, rect.width, rect.height, radius, radius);
    if (fill) jgraphics_fill(g); else { jgraphics_set_line_width(g, width); jgraphics_stroke(g); }
}

void circle(t_jgraphics* g, double x, double y, double radius, double red, double green, double blue, double alpha, bool fill, double width = 1.0)
{
    if (radius <= 0.0) return;
    jgraphics_set_source_rgba(g, red, green, blue, alpha);
    jgraphics_ellipse(g, x - radius, y - radius, radius * 2.0, radius * 2.0);
    if (fill) jgraphics_fill(g); else { jgraphics_set_line_width(g, width); jgraphics_stroke(g); }
}

void text(t_jgraphics* g, const char* value, double x, double y, double size, double red, double green, double blue, double alpha = 1.0)
{
    if (!value) return;
    jgraphics_set_font_size(g, size);
    jgraphics_set_source_rgba(g, red, green, blue, alpha);
    jgraphics_move_to(g, x, y);
    jgraphics_show_text(g, value);
}

void centeredText(t_jgraphics* g, const char* value, double center, double y, double size, double red, double green, double blue, double alpha = 1.0)
{
    text(g, value, center - static_cast<double>(std::strlen(value)) * size * 0.27, y, size, red, green, blue, alpha);
}

Layout makeLayout(const t_rect& bounds)
{
    Layout layout {};
    layout.bounds = bounds;
    const double margin = 10.0, gap = 6.0, header_h = 48.0, preset_h = 52.0;
    const double controls_h = uiClip(bounds.height * 0.225, 120.0, 170.0);
    layout.header = {margin, margin, std::max(100.0, bounds.width - margin * 2.0), header_h};
    layout.presets = {margin, layout.header.y + header_h + gap, layout.header.width, preset_h};
    const double main_y = layout.presets.y + preset_h + gap;
    const double main_h = std::max(180.0, bounds.height - main_y - controls_h - gap - margin);
    const double side_w = uiClip(bounds.width * 0.285, 240.0, 330.0);
    layout.radial = {margin, main_y, std::max(220.0, bounds.width - margin * 2.0 - side_w - gap), main_h};
    layout.side = {layout.radial.x + layout.radial.width + gap, main_y, side_w, main_h};
    layout.controls = {margin, main_y + main_h + gap, layout.header.width, controls_h};
    return layout;
}

t_rect headerButtonRect(const Layout& layout, long index)
{
    const double gap = 4.0;
    const double width = uiClip((layout.header.width - 320.0 - gap * 7.0) / 8.0, 46.0, 78.0);
    const double start = layout.header.x + layout.header.width - width * 8.0 - gap * 7.0 - 5.0;
    return {start + static_cast<double>(index) * (width + gap), layout.header.y + 8.0, width, layout.header.height - 16.0};
}

t_rect characterRect(const Layout& layout, long index)
{
    const double gap = 4.0;
    const double width = (layout.side.width - 18.0 - gap * 3.0) / 4.0;
    return {layout.side.x + 9.0 + index * (width + gap), layout.side.y + 50.0, width, 26.0};
}

t_rect speakerControlRect(const Layout& layout, long index)
{
    return {layout.side.x + 9.0, layout.side.y + 102.0 + index * 42.0, layout.side.width - 18.0, 31.0};
}

t_rect parameterRect(const Layout& layout, long index)
{
    const long column = index % 4, row = index / 4;
    const double gap = 5.0;
    const double width = (layout.controls.width - gap * 3.0) / 4.0;
    const double height = (layout.controls.height - gap * 2.0) / 3.0;
    return {layout.controls.x + column * (width + gap), layout.controls.y + row * (height + gap), width, height};
}

void radialGeometry(const Layout& layout, double* cx, double* cy, double* radius)
{
    *cx = layout.radial.x + layout.radial.width * 0.5;
    *cy = layout.radial.y + layout.radial.height * 0.5;
    *radius = std::max(40.0, std::min(layout.radial.width, layout.radial.height) * 0.42);
}

t_pt speakerPoint(const t_amoeba_outputs* x, const Layout& layout, long speaker)
{
    double cx, cy, radius;
    radialGeometry(layout, &cx, &cy, &radius);
    const double radial = x->speaker_radius[static_cast<std::size_t>(speaker)] / 1.35 * radius;
    const double angle = x->speaker_azimuth[static_cast<std::size_t>(speaker)];
    return {cx + std::cos(angle) * radial, cy + std::sin(angle) * radial};
}

t_pt centerPoint(const t_amoeba_outputs* x, const Layout& layout)
{
    double cx, cy, radius;
    radialGeometry(layout, &cx, &cy, &radius);
    return {cx + x->center_x / 0.85 * radius, cy + x->center_y / 0.85 * radius};
}

Hit hitTest(t_amoeba_outputs* x, const Layout& layout, t_pt point)
{
    for (long i = 0; i < kHeaderButtonCount; ++i) if (contains(point, headerButtonRect(layout, i))) return {kHitHeader, i};
    const long preset_button = myseq::presetui::hitButton(layout.presets, point);
    if (preset_button >= 0) return {kHitPresetButton, preset_button};
    const long preset_slot = myseq::presetui::hitSlot(layout.presets, point);
    if (preset_slot >= 0) return {kHitPresetSlot, preset_slot};
    for (long i = 0; i < x->outlet_count; ++i) {
        const t_pt p = speakerPoint(x, layout, i);
        if (distance(point.x, point.y, p.x, p.y) <= 12.0) return {kHitSpeaker, i};
    }
    const t_pt center = centerPoint(x, layout);
    if (distance(point.x, point.y, center.x, center.y) <= 13.0) return {kHitCenter, 0};
    for (long i = 0; i < 4; ++i) if (contains(point, characterRect(layout, i))) return {kHitCharacter, i};
    for (long i = 0; i < kSpeakerControlCount; ++i) if (contains(point, speakerControlRect(layout, i))) return {kHitSpeakerControl, i};
    for (long i = 0; i < static_cast<long>(myseq::amoeba::kParameterCount); ++i) if (contains(point, parameterRect(layout, i))) return {kHitParameter, i};
    return {};
}

void syncRuntime(t_amoeba_outputs* x)
{
    if (!x || !x->runtime) return;
    for (std::size_t i = 0; i < x->parameters.size(); ++i) x->runtime->parameters[i].store(uiClip(x->parameters[i], 0.0, 1.0), std::memory_order_relaxed);
    for (std::size_t i = 0; i < myseq::amoeba::kMaxSpeakers; ++i) {
        x->runtime->azimuth[i].store(x->speaker_azimuth[i], std::memory_order_relaxed);
        x->runtime->radius[i].store(x->speaker_radius[i], std::memory_order_relaxed);
        x->runtime->elevation[i].store(x->speaker_elevation[i], std::memory_order_relaxed);
    }
    x->runtime->center_x.store(x->center_x, std::memory_order_relaxed);
    x->runtime->center_y.store(x->center_y, std::memory_order_relaxed);
    x->runtime->bypass.store(x->bypass_enabled, std::memory_order_relaxed);
    x->runtime->freeze.store(x->freeze_enabled || !x->auto_enabled, std::memory_order_relaxed);
}

void setLayout(t_amoeba_outputs* x, long layout)
{
    if (!x) return;
    const long count = std::max(1L, x->outlet_count);
    for (long i = 0; i < count; ++i) {
        double angle = -myseq::amoeba::kPi + myseq::amoeba::kTwoPi * static_cast<double>(i) / static_cast<double>(count);
        double radius = 1.0, elevation = 0.0;
        if (layout == 1) angle = (-120.0 + 240.0 * static_cast<double>(i) / static_cast<double>(std::max(1L, count - 1))) * myseq::amoeba::kPi / 180.0;
        else if (layout == 2) elevation = (i % 3 == 0 ? 0.62 : i % 3 == 1 ? 0.0 : -0.42);
        else if (layout == 3) {
            angle = -myseq::amoeba::kPi + random01(x) * myseq::amoeba::kTwoPi;
            radius = 0.62 + random01(x) * 0.58;
            elevation = random01(x) * 1.4 - 0.7;
        }
        x->speaker_azimuth[static_cast<std::size_t>(i)] = angle;
        x->speaker_radius[static_cast<std::size_t>(i)] = radius;
        x->speaker_elevation[static_cast<std::size_t>(i)] = elevation;
    }
    syncRuntime(x);
    redraw(x);
}

AmoebaPresetBank::Values capturePreset(const t_amoeba_outputs* x)
{
    AmoebaPresetBank::Values values {};
    for (std::size_t i = 0; i < 12; ++i) values[i] = x->parameters[i];
    for (std::size_t i = 0; i < 18; ++i) {
        values[12 + i] = (x->speaker_azimuth[i] + myseq::amoeba::kPi) / myseq::amoeba::kTwoPi;
        values[30 + i] = x->speaker_radius[i] / 1.35;
        values[48 + i] = (x->speaker_elevation[i] + 1.0) * 0.5;
    }
    values[66] = (x->center_x + 0.85) / 1.7;
    values[67] = (x->center_y + 0.85) / 1.7;
    values[68] = static_cast<double>(x->auto_enabled);
    values[69] = static_cast<double>(x->freeze_enabled);
    values[70] = static_cast<double>(x->bypass_enabled);
    return values;
}

void applyPreset(t_amoeba_outputs* x, const AmoebaPresetBank::Values& values)
{
    for (std::size_t i = 0; i < 12; ++i) x->parameters[i] = uiClip(values[i], 0.0, 1.0);
    for (std::size_t i = 0; i < 18; ++i) {
        x->speaker_azimuth[i] = values[12 + i] * myseq::amoeba::kTwoPi - myseq::amoeba::kPi;
        x->speaker_radius[i] = uiClip(values[30 + i] * 1.35, 0.15, 1.35);
        x->speaker_elevation[i] = uiClip(values[48 + i] * 2.0 - 1.0, -1.0, 1.0);
    }
    x->center_x = uiClip(values[66] * 1.7 - 0.85, -0.85, 0.85);
    x->center_y = uiClip(values[67] * 1.7 - 0.85, -0.85, 0.85);
    x->auto_enabled = values[68] >= 0.5;
    x->freeze_enabled = values[69] >= 0.5;
    x->bypass_enabled = values[70] >= 0.5;
    syncRuntime(x);
    redraw(x);
}

std::filesystem::path presetPath(const t_amoeba_outputs* x)
{
    return AmoebaPresetBank::defaultPath(std::string(AMOEBA_OUTPUTS_CLASS) + "_" + std::to_string(x ? x->outlet_count : 8) + "ch");
}

void saveBank(t_amoeba_outputs* x)
{
    if (x && x->presets && !x->presets->save(presetPath(x))) object_error(reinterpret_cast<t_object*>(x), "could not save preset bank");
}

void storePreset(t_amoeba_outputs* x, long slot, const char* name = nullptr)
{
    if (!x || !x->presets || slot < 0 || slot >= 16) return;
    x->presets->store(static_cast<std::size_t>(slot), capturePreset(x), name ? name : "");
    x->active_preset = slot;
    x->preset_store_mode = 0;
    saveBank(x);
    redraw(x);
}

void recallPreset(t_amoeba_outputs* x, long slot)
{
    if (!x || !x->presets || slot < 0 || slot >= 16) return;
    AmoebaPresetBank::Values values {};
    if (x->presets->recall(static_cast<std::size_t>(slot), values)) { applyPreset(x, values); x->active_preset = slot; }
}

void randomize(t_amoeba_outputs* x)
{
    if (!x) return;
    for (double& value : x->parameters) value = uiClip(value + (random01(x) * 2.0 - 1.0) * 0.36, 0.0, 1.0);
    x->center_x = (random01(x) * 2.0 - 1.0) * 0.42;
    x->center_y = (random01(x) * 2.0 - 1.0) * 0.42;
    x->active_character = -1;
    x->active_preset = -1;
    syncRuntime(x);
    redraw(x);
}

void setCharacter(t_amoeba_outputs* x, long character)
{
    if (!x || character < 0 || character > 3) return;
    x->parameters = myseq::amoeba::characterParameters(kAmoebaKind, character);
    x->active_character = character;
    x->active_preset = -1;
    syncRuntime(x);
    redraw(x);
}

void drawButton(t_jgraphics* g, const t_rect& rect, const char* label, bool active, const std::array<double, 3>& color)
{
    roundRect(g, rect, 4.0, active ? color[0] * 0.42 : 0.045, active ? color[1] * 0.42 : 0.052, active ? color[2] * 0.42 : 0.064, 1.0, true);
    roundRect(g, rect, 4.0, color[0], color[1], color[2], active ? 0.98 : 0.38, false, active ? 1.5 : 1.0);
    centeredText(g, label, rect.x + rect.width * 0.5, rect.y + rect.height * 0.63, rect.width < 55.0 ? 5.5 : 6.5, 0.95, 0.97, 0.99, active ? 1.0 : 0.72);
}

void drawControl(t_jgraphics* g, const t_rect& rect, const char* label, double value, const std::array<double, 3>& color, bool active)
{
    roundRect(g, rect, 4.0, 0.035, 0.041, 0.052, 1.0, true);
    const t_rect fill {rect.x + 1.0, rect.y + 1.0, (rect.width - 2.0) * uiClip(value, 0.0, 1.0), rect.height - 2.0};
    roundRect(g, fill, 3.0, color[0], color[1], color[2], 0.18 + value * 0.22, true);
    roundRect(g, rect, 4.0, color[0], color[1], color[2], active ? 0.94 : 0.32, false, active ? 1.5 : 1.0);
    text(g, label, rect.x + 8.0, rect.y + 14.0, 7.0, 0.90, 0.93, 0.96, 0.94);
    char amount[16]; std::snprintf(amount, sizeof(amount), "%ld%%", static_cast<long>(value * 100.0 + 0.5));
    text(g, amount, rect.x + rect.width - 34.0, rect.y + rect.height - 8.0, 6.5, color[0], color[1], color[2], 0.94);
}

void drawRadial(t_amoeba_outputs* x, t_jgraphics* g, const Layout& layout, const std::array<double, 3>& color)
{
    roundRect(g, layout.radial, 8.0, 0.024, 0.030, 0.041, 1.0, true);
    roundRect(g, layout.radial, 8.0, color[0], color[1], color[2], 0.28, false);
    double cx, cy, radius; radialGeometry(layout, &cx, &cy, &radius);
    for (long ring = 1; ring <= 4; ++ring) circle(g, cx, cy, radius * static_cast<double>(ring) / 4.0, color[0], color[1], color[2], ring == 4 ? 0.30 : 0.10, false, ring == 4 ? 1.4 : 1.0);
    jgraphics_set_source_rgba(g, 1.0, 1.0, 1.0, 0.07);
    jgraphics_line_draw_fast(g, cx - radius, cy, cx + radius, cy, 1.0);
    jgraphics_line_draw_fast(g, cx, cy - radius, cx, cy + radius, 1.0);

    const long agents = x->runtime ? x->runtime->agent_count.load(std::memory_order_relaxed) : 0;
    for (long i = 0; i < agents; ++i) {
        const double ax = x->runtime->agents[static_cast<std::size_t>(i)][0].load(std::memory_order_relaxed);
        const double ay = x->runtime->agents[static_cast<std::size_t>(i)][1].load(std::memory_order_relaxed);
        const double px = cx + uiClip(ax / 1.35, -1.0, 1.0) * radius;
        const double py = cy + uiClip(ay / 1.35, -1.0, 1.0) * radius;
        const double halo = 12.0 + x->display_activity * 22.0;
        jgraphics_set_source_rgba(g, color[0], color[1], color[2], 0.12);
        jgraphics_line_draw_fast(g, cx, cy, px, py, 1.0);
        circle(g, px, py, halo, color[0], color[1], color[2], 0.035, true);
        circle(g, px, py, 3.0 + (i % 3), color[0], color[1], color[2], 0.64, true);
    }

    const t_pt colony = centerPoint(x, layout);
    circle(g, colony.x, colony.y, 10.0, color[0], color[1], color[2], 0.18, true);
    circle(g, colony.x, colony.y, 5.0, 0.96, 0.98, 1.0, 0.92, true);
    circle(g, colony.x, colony.y, 12.0, color[0], color[1], color[2], 0.74, false, 1.5);

    for (long i = 0; i < x->outlet_count; ++i) {
        const t_pt p = speakerPoint(x, layout, i);
        const double peak = uiClip(x->display_peak[static_cast<std::size_t>(i)], 0.0, 1.0);
        jgraphics_set_source_rgba(g, color[0], color[1], color[2], 0.08 + peak * 0.36);
        jgraphics_line_draw_fast(g, colony.x, colony.y, p.x, p.y, 0.8 + peak * 1.8);
        circle(g, p.x, p.y, 8.0 + peak * 8.0, color[0], color[1], color[2], 0.08 + peak * 0.25, true);
        circle(g, p.x, p.y, i == x->selected_speaker ? 8.0 : 6.0, color[0], color[1], color[2], i == x->selected_speaker ? 0.98 : 0.58, false, i == x->selected_speaker ? 2.0 : 1.0);
        char number[8]; std::snprintf(number, sizeof(number), "%ld", i + 1);
        centeredText(g, number, p.x, p.y + 2.5, 6.0, 0.96, 0.98, 1.0, 0.94);
    }
    text(g, "SPEAKER GEOMETRY / DRAG NODES / DRAG CORE", layout.radial.x + 10.0, layout.radial.y + 17.0, 7.0, 0.70, 0.75, 0.82, 0.84);
}

void drawSide(t_amoeba_outputs* x, t_jgraphics* g, const Layout& layout, const std::array<double, 3>& color)
{
    roundRect(g, layout.side, 8.0, 0.027, 0.033, 0.044, 1.0, true);
    roundRect(g, layout.side, 8.0, color[0], color[1], color[2], 0.26, false);
    const long selected = std::clamp(x->selected_speaker, 0L, std::max(0L, x->outlet_count - 1));
    char heading[64]; std::snprintf(heading, sizeof(heading), "SPEAKER %ld / OUTPUT %ld OF %ld", selected + 1, selected + 1, x->outlet_count);
    text(g, heading, layout.side.x + 10.0, layout.side.y + 20.0, 7.5, 0.92, 0.95, 0.98, 0.96);
    text(g, "BEHAVIOR", layout.side.x + 10.0, layout.side.y + 40.0, 6.5, color[0], color[1], color[2], 0.84);
    const auto info = myseq::amoeba::moduleInfo(kAmoebaKind);
    for (long i = 0; i < 4; ++i) drawButton(g, characterRect(layout, i), info.characters[static_cast<std::size_t>(i)], x->active_character == i, color);
    const char* labels[3] = {"AZIMUTH", "DISTANCE", "ELEVATION"};
    double values[3] = {
        (x->speaker_azimuth[static_cast<std::size_t>(selected)] + myseq::amoeba::kPi) / myseq::amoeba::kTwoPi,
        x->speaker_radius[static_cast<std::size_t>(selected)] / 1.35,
        (x->speaker_elevation[static_cast<std::size_t>(selected)] + 1.0) * 0.5
    };
    for (long i = 0; i < 3; ++i) drawControl(g, speakerControlRect(layout, i), labels[i], values[i], color, x->drag_target == kDragSpeakerControlOffset + i);
    const double meter_y = layout.side.y + 240.0;
    text(g, "OUTPUT ENERGY", layout.side.x + 10.0, meter_y, 6.5, color[0], color[1], color[2], 0.84);
    const double width = (layout.side.width - 20.0) / static_cast<double>(std::max(1L, x->outlet_count));
    for (long i = 0; i < x->outlet_count; ++i) {
        const double peak = uiClip(x->display_peak[static_cast<std::size_t>(i)], 0.0, 1.0);
        const double height = uiClip(layout.side.height - 270.0, 28.0, 100.0);
        const double x0 = layout.side.x + 10.0 + i * width;
        jgraphics_set_source_rgba(g, 0.05, 0.06, 0.075, 1.0);
        jgraphics_rectangle_fill_fast(g, x0 + 1.0, meter_y + 10.0, std::max(2.0, width - 2.0), height);
        jgraphics_set_source_rgba(g, color[0], color[1], color[2], 0.38 + peak * 0.5);
        jgraphics_rectangle_fill_fast(g, x0 + 1.0, meter_y + 10.0 + height * (1.0 - peak), std::max(2.0, width - 2.0), height * peak);
    }
}

void tooltipContent(t_amoeba_outputs* x, long kind, long index, char* title_value, std::size_t title_size, char* section, std::size_t section_size, char* body, std::size_t body_size)
{
    title_value[0] = section[0] = body[0] = 0;
    if (kind == kHitParameter && index >= 0 && index < 12) {
        const auto& spec = myseq::amoeba::parameterSpecs()[static_cast<std::size_t>(index)];
        std::snprintf(title_value, title_size, "%s  %ld%%", spec.name, static_cast<long>(x->parameters[static_cast<std::size_t>(index)] * 100.0 + 0.5));
        std::snprintf(section, section_size, "AMOEBA ENGINE");
        std::snprintf(body, body_size, "%s", spec.help);
    }
    else if (kind == kHitSpeaker) {
        std::snprintf(title_value, title_size, "SPEAKER %ld", index + 1); std::snprintf(section, section_size, "GEOMETRY");
        std::snprintf(body, body_size, "Drag to place this physical output by azimuth and distance.");
    }
    else if (kind == kHitCenter) {
        std::snprintf(title_value, title_size, "AMOEBA CORE"); std::snprintf(section, section_size, "POSITION");
        std::snprintf(body, body_size, "Drag the colony center while organic movement continues around it.");
    }
    else if (kind == kHitSpeakerControl) {
        const char* names[3] = {"AZIMUTH", "DISTANCE", "ELEVATION"};
        std::snprintf(title_value, title_size, "%s / SPEAKER %ld", names[index], x->selected_speaker + 1); std::snprintf(section, section_size, "GEOMETRY");
        std::snprintf(body, body_size, "Edits the selected speaker position used by equal-power distance panning.");
    }
    else if (kind == kHitCharacter) {
        const auto info = myseq::amoeba::moduleInfo(kAmoebaKind);
        std::snprintf(title_value, title_size, "%s BEHAVIOR", info.characters[static_cast<std::size_t>(index)]); std::snprintf(section, section_size, "CHARACTER");
        std::snprintf(body, body_size, "Loads a coherent starting state without changing speaker positions.");
    }
    else if (kind == kHitHeader) {
        const char* names[8] = {"BYPASS", "AUTO", "FREEZE", "CENTER", "CIRCLE", "FRONT", "DOME", "RANDOM"};
        const char* help[8] = {"Distributes bypass audio equally across all configured outputs.", "Runs or pauses the active organic trajectory.", "Stops motion and temporal recording at the current state.", "Returns the amoeba core to the center.", "Places speakers on an even circular ring.", "Places speakers across a frontal 240-degree stage.", "Builds an alternating elevated dome layout.", "Creates an irregular speaker geometry."};
        std::snprintf(title_value, title_size, "%s", names[index]); std::snprintf(section, section_size, "PERFORMANCE"); std::snprintf(body, body_size, "%s", help[index]);
    }
    else if (kind == kHitPresetSlot) {
        std::snprintf(title_value, title_size, "PRESET %ld", index + 1); std::snprintf(section, section_size, "MEMORY");
        std::snprintf(body, body_size, "Recalls parameters, behavior state, core position, and all 18 speaker coordinates.");
    }
    else if (kind == kHitPresetButton) {
        const char* names[3] = {"STORE", "SAVE", "LOAD"};
        std::snprintf(title_value, title_size, "%s PRESET BANK", names[index]); std::snprintf(section, section_size, "MEMORY");
        std::snprintf(body, body_size, "Each object and output count owns an independent persistent 16-slot bank.");
    }
}

void paint(t_amoeba_outputs* x, t_object* view)
{
    t_rect bounds; jbox_get_rect_for_view(reinterpret_cast<t_object*>(x), view, &bounds);
    auto* g = reinterpret_cast<t_jgraphics*>(patcherview_get_jgraphics(view));
    if (!g || bounds.width < 420.0 || bounds.height < 420.0) return;
    const Layout layout = makeLayout(bounds);
    const auto info = myseq::amoeba::moduleInfo(kAmoebaKind);
    const auto color = info.color;
    jgraphics_set_source_rgba(g, 0.014, 0.018, 0.026, 1.0); jgraphics_rectangle_fill_fast(g, 0.0, 0.0, bounds.width, bounds.height);
    roundRect(g, layout.header, 7.0, 0.025, 0.031, 0.042, 1.0, true); roundRect(g, layout.header, 7.0, color[0], color[1], color[2], 0.32, false);
    text(g, info.title, layout.header.x + 12.0, layout.header.y + 20.0, 11.0, 0.95, 0.97, 1.0, 0.98);
    char subtitle[160]; std::snprintf(subtitle, sizeof(subtitle), "%s / %ld DISCRETE MSP OUTPUTS", info.subtitle, x->outlet_count);
    text(g, subtitle, layout.header.x + 12.0, layout.header.y + 37.0, 6.3, color[0], color[1], color[2], 0.86);
    const char* labels[8] = {"BYPASS", "AUTO", "FREEZE", "CENTER", "CIRCLE", "FRONT", "DOME", "RANDOM"};
    for (long i = 0; i < 8; ++i) {
        const bool active = (i == 0 && x->bypass_enabled) || (i == 1 && x->auto_enabled) || (i == 2 && x->freeze_enabled);
        drawButton(g, headerButtonRect(layout, i), labels[i], active, color);
    }
    myseq::presetui::draw(g, layout.presets, x->presets, x->active_preset, x->preset_store_mode, color[0], color[1], color[2]);
    drawRadial(x, g, layout, color);
    drawSide(x, g, layout, color);
    for (long i = 0; i < 12; ++i) drawControl(g, parameterRect(layout, i), myseq::amoeba::parameterSpecs()[static_cast<std::size_t>(i)].name, x->parameters[static_cast<std::size_t>(i)], color, x->drag_target == kDragParameterOffset + i);
    char tip_title[128], tip_section[32], tip_body[192];
    tooltipContent(x, x->tooltip.kind, x->tooltip.index, tip_title, sizeof(tip_title), tip_section, sizeof(tip_section), tip_body, sizeof(tip_body));
    myseq::tooltipui::draw(g, bounds, x->tooltip, tip_title, tip_section, tip_body, color[0], color[1], color[2]);
}

void setParameterFromPoint(t_amoeba_outputs* x, const t_rect& rect, long index, t_pt point)
{
    if (index < 0 || index >= 12) return;
    x->parameters[static_cast<std::size_t>(index)] = uiClip((point.x - rect.x) / std::max(1.0, rect.width), 0.0, 1.0);
    x->active_character = x->active_preset = -1;
    syncRuntime(x); redraw(x);
}

void setSpeakerControlFromPoint(t_amoeba_outputs* x, const t_rect& rect, long control, t_pt point)
{
    const long index = std::clamp(x->selected_speaker, 0L, std::max(0L, x->outlet_count - 1));
    const double value = uiClip((point.x - rect.x) / std::max(1.0, rect.width), 0.0, 1.0);
    if (control == 0) x->speaker_azimuth[static_cast<std::size_t>(index)] = value * myseq::amoeba::kTwoPi - myseq::amoeba::kPi;
    else if (control == 1) x->speaker_radius[static_cast<std::size_t>(index)] = 0.15 + value * 1.20;
    else x->speaker_elevation[static_cast<std::size_t>(index)] = value * 2.0 - 1.0;
    syncRuntime(x); redraw(x);
}

void dragSpeaker(t_amoeba_outputs* x, const Layout& layout, long speaker, t_pt point)
{
    double cx, cy, radius; radialGeometry(layout, &cx, &cy, &radius);
    const double dx = point.x - cx, dy = point.y - cy;
    x->speaker_azimuth[static_cast<std::size_t>(speaker)] = std::atan2(dy, dx);
    x->speaker_radius[static_cast<std::size_t>(speaker)] = uiClip(std::sqrt(dx * dx + dy * dy) / radius * 1.35, 0.15, 1.35);
    syncRuntime(x); redraw(x);
}

void dragCenter(t_amoeba_outputs* x, const Layout& layout, t_pt point)
{
    double cx, cy, radius; radialGeometry(layout, &cx, &cy, &radius);
    x->center_x = uiClip((point.x - cx) / radius * 0.85, -0.85, 0.85);
    x->center_y = uiClip((point.y - cy) / radius * 0.85, -0.85, 0.85);
    syncRuntime(x); redraw(x);
}

void mouseDown(t_amoeba_outputs* x, t_object* view, t_pt point, long modifiers)
{
    t_rect bounds; jbox_get_rect_for_view(reinterpret_cast<t_object*>(x), view, &bounds); const Layout layout = makeLayout(bounds);
    myseq::tooltipui::hide(x->tooltip);
    const Hit hit = hitTest(x, layout, point);
    if (hit.kind == kHitHeader) {
        if (hit.index == 0) x->bypass_enabled = !x->bypass_enabled;
        else if (hit.index == 1) x->auto_enabled = !x->auto_enabled;
        else if (hit.index == 2) x->freeze_enabled = !x->freeze_enabled;
        else if (hit.index == 3) x->center_x = x->center_y = 0.0;
        else setLayout(x, hit.index - 4);
        syncRuntime(x); redraw(x); return;
    }
    if (hit.kind == kHitPresetButton) {
        if (hit.index == myseq::presetui::kStoreButton) x->preset_store_mode = !x->preset_store_mode;
        else if (hit.index == myseq::presetui::kSaveButton) saveBank(x);
        else if (x->presets && !x->presets->load(presetPath(x))) object_error(reinterpret_cast<t_object*>(x), "could not load preset bank");
        redraw(x); return;
    }
    if (hit.kind == kHitPresetSlot) {
        if (x->preset_store_mode || (modifiers & eAltKey)) storePreset(x, hit.index); else recallPreset(x, hit.index);
        return;
    }
    if (hit.kind == kHitSpeaker) { x->selected_speaker = hit.index; x->drag_target = kDragSpeakerOffset + hit.index; dragSpeaker(x, layout, hit.index, point); return; }
    if (hit.kind == kHitCenter) { x->drag_target = kDragCenter; dragCenter(x, layout, point); return; }
    if (hit.kind == kHitCharacter) { setCharacter(x, hit.index); return; }
    if (hit.kind == kHitSpeakerControl) { x->drag_target = kDragSpeakerControlOffset + hit.index; setSpeakerControlFromPoint(x, speakerControlRect(layout, hit.index), hit.index, point); return; }
    if (hit.kind == kHitParameter) { x->drag_target = kDragParameterOffset + hit.index; setParameterFromPoint(x, parameterRect(layout, hit.index), hit.index, point); }
}

void mouseDrag(t_amoeba_outputs* x, t_object* view, t_pt point, long)
{
    t_rect bounds; jbox_get_rect_for_view(reinterpret_cast<t_object*>(x), view, &bounds); const Layout layout = makeLayout(bounds);
    if (x->drag_target == kDragCenter) dragCenter(x, layout, point);
    else if (x->drag_target >= kDragSpeakerOffset && x->drag_target < kDragSpeakerOffset + 18) dragSpeaker(x, layout, x->drag_target - kDragSpeakerOffset, point);
    else if (x->drag_target >= kDragSpeakerControlOffset && x->drag_target < kDragSpeakerControlOffset + 3) {
        const long index = x->drag_target - kDragSpeakerControlOffset; setSpeakerControlFromPoint(x, speakerControlRect(layout, index), index, point);
    }
    else if (x->drag_target >= kDragParameterOffset && x->drag_target < kDragParameterOffset + 12) {
        const long index = x->drag_target - kDragParameterOffset; setParameterFromPoint(x, parameterRect(layout, index), index, point);
    }
}

void mouseUp(t_amoeba_outputs* x, t_object*, t_pt, long) { if (x) x->drag_target = kDragNone; }

void mouseMove(t_amoeba_outputs* x, t_object* view, t_pt point, long)
{
    if (!x || !x->tooltips_enabled || x->drag_target != kDragNone) return;
    t_rect bounds; jbox_get_rect_for_view(reinterpret_cast<t_object*>(x), view, &bounds); const Hit hit = hitTest(x, makeLayout(bounds), point);
    myseq::tooltipui::setTarget(x->tooltip, hit.kind, hit.index, point); redraw(x);
}

void mouseEnter(t_amoeba_outputs* x, t_object* view, t_pt point, long modifiers) { mouseMove(x, view, point, modifiers); }
void mouseLeave(t_amoeba_outputs* x, t_object*, t_pt, long) { myseq::tooltipui::hide(x->tooltip); redraw(x); }
long hitTestMethod(t_amoeba_outputs*, t_object*, t_pt, long) { return 1; }

void perform(t_object* object, t_object*, double** inputs, long num_inputs, double** outputs, long num_outputs, long frames, long, void*)
{
    auto* x = reinterpret_cast<t_amoeba_outputs*>(object);
    if (!x || !x->engine || !x->runtime || !outputs || frames <= 0) return;
    std::array<double, 12> params {};
    for (std::size_t i = 0; i < params.size(); ++i) params[i] = x->runtime->parameters[i].load(std::memory_order_relaxed);
    x->engine->setParameters(params);
    x->engine->setCenter(x->runtime->center_x.load(std::memory_order_relaxed), x->runtime->center_y.load(std::memory_order_relaxed));
    for (long i = 0; i < x->outlet_count; ++i) {
        const double azimuth = x->runtime->azimuth[static_cast<std::size_t>(i)].load(std::memory_order_relaxed) * 180.0 / myseq::amoeba::kPi;
        const double radius = x->runtime->radius[static_cast<std::size_t>(i)].load(std::memory_order_relaxed);
        const double elevation = x->runtime->elevation[static_cast<std::size_t>(i)].load(std::memory_order_relaxed) * 90.0;
        x->engine->setSpeaker(static_cast<std::size_t>(i), azimuth, radius, elevation);
    }
    Event event; while (x->events->pop(event)) x->engine->note(event);
    const std::uint32_t reset = x->runtime->reset_serial.load(std::memory_order_acquire);
    if (reset != x->dsp_reset_serial) { x->dsp_reset_serial = reset; x->engine->reset(); }
    x->engine->process(num_inputs > 0 ? inputs[0] : nullptr, outputs, static_cast<std::size_t>(frames), static_cast<std::size_t>(std::min(x->outlet_count, num_outputs)), x->input_connected != 0, x->runtime->bypass.load(std::memory_order_relaxed) != 0, x->runtime->freeze.load(std::memory_order_relaxed) != 0);
    for (long channel = 0; channel < std::min(x->outlet_count, num_outputs); ++channel) {
        double peak = 0.0; for (long frame = 0; frame < frames; ++frame) peak = std::max(peak, std::abs(outputs[channel][frame]));
        double current = x->runtime->peak[static_cast<std::size_t>(channel)].load(std::memory_order_relaxed);
        while (peak > current && !x->runtime->peak[static_cast<std::size_t>(channel)].compare_exchange_weak(current, peak, std::memory_order_release, std::memory_order_relaxed)) {}
    }
    const long agents = static_cast<long>(x->engine->agentCount()); x->runtime->agent_count.store(agents, std::memory_order_relaxed);
    for (long i = 0; i < agents; ++i) { const auto position = x->engine->agentPosition(static_cast<std::size_t>(i)); for (long axis = 0; axis < 3; ++axis) x->runtime->agents[static_cast<std::size_t>(i)][static_cast<std::size_t>(axis)].store(position[static_cast<std::size_t>(axis)], std::memory_order_relaxed); }
}

void dsp64(t_amoeba_outputs* x, t_object* chain, short* count, double sample_rate, long, long)
{
    if (!x || !x->engine) return;
    x->input_connected = count && count[0] != 0;
    x->engine->setSampleRate(sample_rate);
    x->dsp_reset_serial = x->runtime->reset_serial.load(std::memory_order_relaxed);
    dsp_add64(chain, reinterpret_cast<t_object*>(x), perform, 0, nullptr);
}

void uiTick(t_amoeba_outputs* x)
{
    if (!x) return;
    double activity = 0.0;
    for (long i = 0; i < x->outlet_count; ++i) {
        const double peak = x->runtime->peak[static_cast<std::size_t>(i)].exchange(0.0, std::memory_order_acq_rel);
        x->display_peak[static_cast<std::size_t>(i)] = std::max(peak, x->display_peak[static_cast<std::size_t>(i)] * 0.90);
        activity = std::max(activity, x->display_peak[static_cast<std::size_t>(i)]);
    }
    x->display_activity = x->display_activity * 0.86 + activity * 0.14;
    myseq::tooltipui::tick(x->tooltip, x->tooltips_enabled != 0);
    redraw(x);
    if (x->ui_clock) clock_fdelay(x->ui_clock, kUiIntervalMs);
}

void queueEvent(t_amoeba_outputs* x, Event event)
{
    if (!x || !x->events) return;
    event.pitch = std::clamp(event.pitch, 0L, 127L); event.velocity = std::clamp(event.velocity, 0L, 127L); event.duration_ms = std::clamp(event.duration_ms, 1L, 60000L); event.channel = std::clamp(event.channel, 1L, 16L);
    if (!x->events->push(event) && !x->queue_warning_shown) { object_error(reinterpret_cast<t_object*>(x), "event queue full; event dropped"); x->queue_warning_shown = 1; }
}

void list(t_amoeba_outputs* x, t_symbol*, long argc, t_atom* argv)
{
    if (!x || argc < 1) return;
    queueEvent(x, {atom_getlong(argv), argc > 1 ? atom_getlong(argv + 1) : 100, argc > 2 ? atom_getlong(argv + 2) : 500, argc > 3 ? atom_getlong(argv + 3) : 1});
}

void bang(t_amoeba_outputs* x) { queueEvent(x, {60, 110, 650, 1}); }
void bypass(t_amoeba_outputs* x, long state) { if (x) { x->bypass_enabled = state != 0; syncRuntime(x); redraw(x); } }
void freeze(t_amoeba_outputs* x, long state) { if (x) { x->freeze_enabled = state != 0; syncRuntime(x); redraw(x); } }
void automatic(t_amoeba_outputs* x, long state) { if (x) { x->auto_enabled = state != 0; syncRuntime(x); redraw(x); } }
void tooltips(t_amoeba_outputs* x, long state) { if (x) { x->tooltips_enabled = state != 0; if (!x->tooltips_enabled) myseq::tooltipui::hide(x->tooltip); redraw(x); } }

void center(t_amoeba_outputs* x, t_symbol*, long argc, t_atom* argv)
{
    if (!x) return; x->center_x = argc > 0 ? uiClip(atom_getfloat(argv), -0.85, 0.85) : 0.0; x->center_y = argc > 1 ? uiClip(atom_getfloat(argv + 1), -0.85, 0.85) : 0.0; syncRuntime(x); redraw(x);
}

void speaker(t_amoeba_outputs* x, t_symbol*, long argc, t_atom* argv)
{
    if (!x || argc < 3) return;
    const long index = atom_getlong(argv) - 1; if (index < 0 || index >= x->outlet_count) return;
    x->speaker_azimuth[static_cast<std::size_t>(index)] = uiClip(atom_getfloat(argv + 1), -180.0, 180.0) * myseq::amoeba::kPi / 180.0;
    x->speaker_radius[static_cast<std::size_t>(index)] = uiClip(atom_getfloat(argv + 2), 0.15, 1.35);
    if (argc > 3) x->speaker_elevation[static_cast<std::size_t>(index)] = uiClip(atom_getfloat(argv + 3) / 90.0, -1.0, 1.0);
    x->selected_speaker = index; syncRuntime(x); redraw(x);
}

void layoutMessage(t_amoeba_outputs* x, t_symbol* name)
{
    if (!x || !name || !name->s_name) return;
    if (!std::strcmp(name->s_name, "circle")) setLayout(x, 0); else if (!std::strcmp(name->s_name, "front")) setLayout(x, 1); else if (!std::strcmp(name->s_name, "dome")) setLayout(x, 2); else if (!std::strcmp(name->s_name, "random")) setLayout(x, 3);
}

void characterMessage(t_amoeba_outputs* x, t_symbol* name)
{
    if (!x || !name || !name->s_name) return; const auto info = myseq::amoeba::moduleInfo(kAmoebaKind);
    for (long i = 0; i < 4; ++i) if (!strcasecmp(name->s_name, info.characters[static_cast<std::size_t>(i)])) { setCharacter(x, i); return; }
    if (!std::strcmp(name->s_name, "random")) randomize(x);
}

void parameter(t_amoeba_outputs* x, long index, double value)
{
    if (!x || index < 1 || index > 12) return; x->parameters[static_cast<std::size_t>(index - 1)] = uiClip(value, 0.0, 1.0); x->active_character = x->active_preset = -1; syncRuntime(x); redraw(x);
}

void setMessage(t_amoeba_outputs* x, t_symbol*, long argc, t_atom* argv)
{
    if (!x || argc < 2 || atom_gettype(argv) != A_SYM) return; const char* name = atom_getsym(argv)->s_name;
    for (long i = 0; i < 12; ++i) if (!strcasecmp(name, myseq::amoeba::parameterSpecs()[static_cast<std::size_t>(i)].name)) { parameter(x, i + 1, atom_getfloat(argv + 1)); return; }
}

void resetMessage(t_amoeba_outputs* x) { if (x && x->runtime) { x->runtime->reset_serial.fetch_add(1, std::memory_order_release); redraw(x); } }
void randomizeMessage(t_amoeba_outputs* x) { randomize(x); }

void presetMessage(t_amoeba_outputs* x, t_symbol*, long argc, t_atom* argv)
{
    if (!x || !x->presets || argc < 1) return;
    if (atom_gettype(argv) == A_SYM) {
        const char* command = atom_getsym(argv)->s_name;
        if (!std::strcmp(command, "save")) { const auto path = argc > 1 && atom_gettype(argv + 1) == A_SYM ? std::filesystem::path(atom_getsym(argv + 1)->s_name) : presetPath(x); if (!x->presets->save(path)) object_error(reinterpret_cast<t_object*>(x), "could not save preset bank"); }
        else if (!std::strcmp(command, "load")) { const auto path = argc > 1 && atom_gettype(argv + 1) == A_SYM ? std::filesystem::path(atom_getsym(argv + 1)->s_name) : presetPath(x); if (!x->presets->load(path)) object_error(reinterpret_cast<t_object*>(x), "could not load preset bank"); }
        else if (!std::strcmp(command, "list")) for (long i = 0; i < 16; ++i) if (x->presets->valid(static_cast<std::size_t>(i))) object_post(reinterpret_cast<t_object*>(x), "preset %ld: %s", i + 1, x->presets->name(static_cast<std::size_t>(i)).c_str());
        return;
    }
    const long slot = atom_getlong(argv) - 1; if (slot < 0 || slot >= 16 || argc < 2 || atom_gettype(argv + 1) != A_SYM) return;
    const char* command = atom_getsym(argv + 1)->s_name;
    if (!std::strcmp(command, "store")) storePreset(x, slot, argc > 2 && atom_gettype(argv + 2) == A_SYM ? atom_getsym(argv + 2)->s_name : nullptr);
    else if (!std::strcmp(command, "recall")) recallPreset(x, slot);
    else if (!std::strcmp(command, "clear")) { x->presets->clear(static_cast<std::size_t>(slot)); saveBank(x); redraw(x); }
}

void dump(t_amoeba_outputs* x)
{
    if (!x) return; const auto info = myseq::amoeba::moduleInfo(kAmoebaKind);
    object_post(reinterpret_cast<t_object*>(x), "%s: %ld physical signal outputs, auto %ld, freeze %ld, center %.3f %.3f", info.title, x->outlet_count, x->auto_enabled, x->freeze_enabled, x->center_x, x->center_y);
    for (long i = 0; i < x->outlet_count; ++i) object_post(reinterpret_cast<t_object*>(x), "speaker %ld: azimuth %.2f distance %.3f elevation %.2f", i + 1, x->speaker_azimuth[static_cast<std::size_t>(i)] * 180.0 / myseq::amoeba::kPi, x->speaker_radius[static_cast<std::size_t>(i)], x->speaker_elevation[static_cast<std::size_t>(i)] * 90.0);
}

void assist(t_amoeba_outputs* x, void*, long message, long argument, char* output)
{
    if (message == ASSIST_INLET) std::snprintf(output, 256, "Signal input and control messages");
    else std::snprintf(output, 256, "Speaker %ld signal output (%ld configured)", argument + 1, x ? x->outlet_count : 0);
}

void freeObject(t_amoeba_outputs* x)
{
    if (!x) return; if (x->ui_clock) { clock_unset(x->ui_clock); object_free(reinterpret_cast<t_object*>(x->ui_clock)); }
    delete x->presets; delete x->events; delete x->runtime; delete x->engine; dsp_freejbox(reinterpret_cast<t_pxjbox*>(x)); jbox_free(reinterpret_cast<t_jbox*>(x));
}

void* newObject(t_symbol*, long argc, t_atom* argv)
{
    t_dictionary* dictionary = object_dictionaryarg(argc, argv); if (!dictionary) return nullptr;
    auto* x = reinterpret_cast<t_amoeba_outputs*>(object_alloc(s_amoeba_class)); if (!x) return nullptr;
    x->engine = nullptr; x->runtime = nullptr; x->events = nullptr; x->presets = nullptr; x->ui_clock = nullptr;
    long requested = 8; dictionary_getlong(dictionary, gensym("outputs"), &requested);
    for (long i = 0; i < argc; ++i) if (atom_gettype(argv + i) == A_LONG || atom_gettype(argv + i) == A_FLOAT) { requested = atom_getlong(argv + i); break; }
    x->outlet_count = x->output_attribute = std::clamp(requested, 1L, 18L);
    const long flags = JBOX_DRAWFIRSTIN | JBOX_NODRAWBOX | JBOX_DRAWINLAST | JBOX_TRANSPARENT | JBOX_GROWBOTH | JBOX_DRAWBACKGROUND;
    jbox_new(reinterpret_cast<t_jbox*>(x), flags, argc, argv); x->object.z_box.b_firstin = reinterpret_cast<t_object*>(x); dsp_setupjbox(reinterpret_cast<t_pxjbox*>(x), 1);
    for (long i = 0; i < x->outlet_count; ++i) outlet_new(reinterpret_cast<t_object*>(x), "signal");
    x->parameters = myseq::amoeba::defaultParameters(); x->center_x = x->center_y = 0.0; x->display_activity = 0.0; x->input_connected = 0; x->auto_enabled = 1; x->freeze_enabled = 0; x->bypass_enabled = 0; x->tooltips_enabled = 1; x->selected_speaker = 0; x->active_character = 0; x->active_preset = -1; x->preset_store_mode = 0; x->drag_target = kDragNone; x->queue_warning_shown = 0; x->rng = 0xa511e9b3u ^ static_cast<std::uint32_t>(kAmoebaKind); x->dsp_reset_serial = 0; x->tooltip = {};
    for (double& peak : x->display_peak) peak = 0.0;
    for (long i = 0; i < 18; ++i) { x->speaker_azimuth[static_cast<std::size_t>(i)] = -myseq::amoeba::kPi + myseq::amoeba::kTwoPi * static_cast<double>(i) / static_cast<double>(x->outlet_count); x->speaker_radius[static_cast<std::size_t>(i)] = 1.0; x->speaker_elevation[static_cast<std::size_t>(i)] = 0.0; }
    x->engine = new (std::nothrow) AmoebaEngine(kAmoebaKind, static_cast<std::size_t>(x->outlet_count)); x->runtime = new (std::nothrow) AmoebaRuntime(); x->events = new (std::nothrow) AmoebaEventQueue<kEventQueueSize>(); x->presets = new (std::nothrow) AmoebaPresetBank(); x->ui_clock = clock_new(reinterpret_cast<t_object*>(x), reinterpret_cast<method>(uiTick));
    if (!x->engine || !x->runtime || !x->events || !x->presets || !x->ui_clock) { object_error(reinterpret_cast<t_object*>(x), "could not allocate Amoeba Outputs state"); object_free(reinterpret_cast<t_object*>(x)); return nullptr; }
    x->engine->setSampleRate(sys_getsr() > 1000.0 ? sys_getsr() : 48000.0); x->presets->load(presetPath(x)); syncRuntime(x); attr_dictionary_process(x, dictionary); jbox_ready(reinterpret_cast<t_jbox*>(x)); clock_fdelay(x->ui_clock, kUiIntervalMs); return x;
}

} // namespace

extern "C" C74_EXPORT void ext_main(void*)
{
    t_class* c = class_new(AMOEBA_OUTPUTS_CLASS, reinterpret_cast<method>(newObject), reinterpret_cast<method>(freeObject), sizeof(t_amoeba_outputs), nullptr, A_GIMME, 0);
    c->c_flags |= CLASS_FLAG_NEWDICTIONARY; jbox_initclass(c, JBOX_COLOR); class_dspinitjbox(c);
    class_addmethod(c, reinterpret_cast<method>(dsp64), "dsp64", A_CANT, 0); class_addmethod(c, reinterpret_cast<method>(paint), "paint", A_CANT, 0);
    class_addmethod(c, reinterpret_cast<method>(mouseDown), "mousedown", A_CANT, 0); class_addmethod(c, reinterpret_cast<method>(mouseDrag), "mousedrag", A_CANT, 0); class_addmethod(c, reinterpret_cast<method>(mouseUp), "mouseup", A_CANT, 0); class_addmethod(c, reinterpret_cast<method>(mouseMove), "mousemove", A_CANT, 0); class_addmethod(c, reinterpret_cast<method>(mouseEnter), "mouseenter", A_CANT, 0); class_addmethod(c, reinterpret_cast<method>(mouseLeave), "mouseleave", A_CANT, 0); class_addmethod(c, reinterpret_cast<method>(hitTestMethod), "hittest", A_CANT, 0); class_addmethod(c, reinterpret_cast<method>(jbox_notify), "notify", A_CANT, 0);
    class_addmethod(c, reinterpret_cast<method>(list), "list", A_GIMME, 0); class_addmethod(c, reinterpret_cast<method>(list), "event", A_GIMME, 0); class_addmethod(c, reinterpret_cast<method>(list), "note", A_GIMME, 0); class_addmethod(c, reinterpret_cast<method>(bang), "bang", 0);
    class_addmethod(c, reinterpret_cast<method>(bypass), "bypass", A_LONG, 0); class_addmethod(c, reinterpret_cast<method>(freeze), "freeze", A_LONG, 0); class_addmethod(c, reinterpret_cast<method>(automatic), "auto", A_LONG, 0); class_addmethod(c, reinterpret_cast<method>(tooltips), "tooltips", A_LONG, 0);
    class_addmethod(c, reinterpret_cast<method>(center), "center", A_GIMME, 0); class_addmethod(c, reinterpret_cast<method>(speaker), "speaker", A_GIMME, 0); class_addmethod(c, reinterpret_cast<method>(layoutMessage), "layout", A_SYM, 0); class_addmethod(c, reinterpret_cast<method>(characterMessage), "character", A_SYM, 0); class_addmethod(c, reinterpret_cast<method>(parameter), "param", A_LONG, A_FLOAT, 0); class_addmethod(c, reinterpret_cast<method>(setMessage), "set", A_GIMME, 0); class_addmethod(c, reinterpret_cast<method>(resetMessage), "reset", 0); class_addmethod(c, reinterpret_cast<method>(randomizeMessage), "randomize", 0); class_addmethod(c, reinterpret_cast<method>(presetMessage), "preset", A_GIMME, 0); class_addmethod(c, reinterpret_cast<method>(dump), "dump", 0); class_addmethod(c, reinterpret_cast<method>(assist), "assist", A_CANT, 0);
    CLASS_ATTR_LONG(c, "outputs", 0, t_amoeba_outputs, output_attribute); CLASS_ATTR_FILTER_CLIP(c, "outputs", 1, 18); CLASS_ATTR_SAVE(c, "outputs", 0); CLASS_ATTR_DEFAULT(c, "outputs", 0, "8"); CLASS_ATTR_DEFAULT(c, "patching_rect", 0, "0. 0. 1040. 720.");
    class_register(CLASS_BOX, c); s_amoeba_class = c;
}
