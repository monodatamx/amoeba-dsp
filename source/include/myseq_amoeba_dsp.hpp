#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace myseq::amoeba {

constexpr std::size_t kMaxSpeakers = 18;
constexpr std::size_t kMaxAgents = 12;
constexpr std::size_t kParameterCount = 12;
constexpr double kPi = 3.14159265358979323846;
constexpr double kTwoPi = kPi * 2.0;

enum class Kind {
    Orbit,
    Swarm,
    Flow,
    Growth,
    Physarum,
    Membrane,
    Vortex,
    Brownian,
    Granular,
    Constellation
};

struct ModuleInfo {
    const char* title;
    const char* subtitle;
    std::array<const char*, 4> characters;
    std::size_t agents;
    std::array<double, 3> color;
};

struct ParameterSpec {
    const char* name;
    const char* help;
    double default_value;
};

struct Speaker {
    double azimuth = 0.0;
    double radius = 1.0;
    double elevation = 0.0;
};

struct Event {
    long pitch = 60;
    long velocity = 100;
    long duration_ms = 500;
    long channel = 1;
};

inline double clip(double value, double low, double high) noexcept
{
    return std::max(low, std::min(high, value));
}

inline double wrapPhase(double phase) noexcept
{
    phase = std::fmod(phase, kTwoPi);
    return phase < 0.0 ? phase + kTwoPi : phase;
}

inline ModuleInfo moduleInfo(Kind kind) noexcept
{
    switch (kind) {
        case Kind::Orbit: return {"AMOEBA ORBIT", "ELLIPTIC ORBITS / PRECESSION / GRAVITY WELLS", {"CIRCLE", "ELLIPSE", "COMET", "ECLIPSE"}, 3, {0.20, 0.78, 1.00}};
        case Kind::Swarm: return {"AMOEBA SWARM", "FLOCKING / COHESION / SEPARATION / EMERGENCE", {"FLOCK", "SCHOOL", "PANIC", "MURMUR"}, 10, {0.28, 0.90, 0.62}};
        case Kind::Flow: return {"AMOEBA FLOW", "CURL FIELD / ADVECTION / TURBULENT STREAMS", {"STREAM", "TIDE", "RAPIDS", "UNDERTOW"}, 6, {0.22, 0.74, 0.88}};
        case Kind::Growth: return {"AMOEBA GROWTH", "BRANCHING TIPS / L-SYSTEM PHYLLOTAXIS", {"SEED", "FERN", "CORAL", "BLOOM"}, 8, {0.74, 0.88, 0.28}};
        case Kind::Physarum: return {"AMOEBA PHYSARUM", "SLIME NETWORK / NUTRIENT SEEKING / TRAILS", {"SPORE", "FORAGE", "VEINS", "HUNGER"}, 9, {0.92, 0.68, 0.22}};
        case Kind::Membrane: return {"AMOEBA MEMBRANE", "ELASTIC BODY / WAVES / SURFACE TENSION", {"CELL", "PULSE", "STRETCH", "RUPTURE"}, 7, {0.84, 0.42, 0.76}};
        case Kind::Vortex: return {"AMOEBA VORTEX", "COUNTER-ROTATING VORTICES / SPIRAL ATTRACTORS", {"SPIN", "PAIR", "STORM", "SINGULAR"}, 6, {0.60, 0.48, 1.00}};
        case Kind::Brownian: return {"AMOEBA BROWNIAN", "INERTIAL PARTICLES / RANDOM WALK / COLLISIONS", {"DUST", "LIQUID", "GAS", "PLASMA"}, 8, {0.96, 0.52, 0.32}};
        case Kind::Granular: return {"AMOEBA GRANULAR", "MULTIHEAD TIME GRAINS / SPATIAL CLOUDS", {"MIST", "DROPS", "SHARDS", "STORM"}, 10, {0.32, 0.76, 1.00}};
        case Kind::Constellation: return {"AMOEBA CONSTELLATION", "SPEAKER-AWARE NODES / LISSAJOUS CONSTELLATIONS", {"POINT", "LINE", "WEB", "GALAXY"}, 12, {0.88, 0.72, 1.00}};
    }
    return {"AMOEBA OUTPUTS", "ORGANIC MULTICHANNEL SPATIALIZATION", {"A", "B", "C", "D"}, 4, {0.30, 0.80, 0.72}};
}

inline const std::array<ParameterSpec, kParameterCount>& parameterSpecs() noexcept
{
    static const std::array<ParameterSpec, kParameterCount> specs {{
        {"RATE", "Sets the time scale of the active amoeba behavior.", 0.28},
        {"SPREAD", "Controls the spatial width of each moving audio focus.", 0.34},
        {"DIFFUSION", "Leaks equal-power energy toward neighboring and distant speakers.", 0.12},
        {"INERTIA", "Controls resistance to changes in position and direction.", 0.62},
        {"ATTRACTION", "Pulls agents toward the colony center, nutrients, or orbital wells.", 0.48},
        {"REPULSION", "Separates agents and prevents the spatial body from collapsing.", 0.32},
        {"TURBULENCE", "Adds bounded chaotic force to the organic trajectory.", 0.24},
        {"ROTATION", "Adds global angular motion and handedness.", 0.56},
        {"ELEVATION", "Sets vertical movement used by elevated speaker layouts.", 0.28},
        {"DISTANCE", "Controls radial depth and proximity to the speaker perimeter.", 0.68},
        {"COLOR", "Shapes temporal brightness, grain history, and motion articulation.", 0.52},
        {"LEVEL", "Sets equal-power multichannel output level.", 0.72}
    }};
    return specs;
}

inline std::array<double, kParameterCount> defaultParameters() noexcept
{
    std::array<double, kParameterCount> values {};
    const auto& specs = parameterSpecs();
    for (std::size_t i = 0; i < values.size(); ++i) values[i] = specs[i].default_value;
    return values;
}

inline std::array<double, kParameterCount> characterParameters(Kind kind, long character) noexcept
{
    auto values = defaultParameters();
    const double c = clip(static_cast<double>(character), 0.0, 3.0);
    values[0] = clip(0.12 + c * 0.19, 0.0, 1.0);
    values[1] = clip(0.18 + c * 0.15, 0.0, 1.0);
    values[2] = clip(0.04 + c * 0.12, 0.0, 1.0);
    values[3] = clip(0.82 - c * 0.16, 0.0, 1.0);
    values[6] = clip(0.08 + c * 0.22, 0.0, 1.0);
    values[7] = clip(0.32 + c * 0.17, 0.0, 1.0);
    values[9] = clip(0.46 + c * 0.13, 0.0, 1.0);
    if (kind == Kind::Swarm || kind == Kind::Physarum) {
        values[4] = clip(0.34 + c * 0.12, 0.0, 1.0);
        values[5] = clip(0.22 + c * 0.16, 0.0, 1.0);
    }
    if (kind == Kind::Granular) values[10] = clip(0.26 + c * 0.20, 0.0, 1.0);
    if (kind == Kind::Membrane) values[8] = clip(0.18 + c * 0.21, 0.0, 1.0);
    return values;
}

class AmoebaEngine {
public:
    explicit AmoebaEngine(Kind kind, std::size_t speakers = 8)
    : kind_(kind)
    {
        parameters_ = defaultParameters();
        setSpeakerCount(speakers);
        reset();
    }

    void setSampleRate(double sample_rate)
    {
        sample_rate_ = sample_rate > 1000.0 ? sample_rate : 48000.0;
        buffer_.assign(static_cast<std::size_t>(sample_rate_ * 4.0), 0.0);
        write_index_ = 0;
        samples_written_ = 0;
    }

    void setSpeakerCount(std::size_t count)
    {
        speaker_count_ = std::clamp<std::size_t>(count, 1, kMaxSpeakers);
        for (std::size_t i = 0; i < kMaxSpeakers; ++i) {
            speakers_[i].azimuth = -kPi + kTwoPi * static_cast<double>(i) / static_cast<double>(speaker_count_);
            speakers_[i].radius = 1.0;
            speakers_[i].elevation = 0.0;
        }
    }

    [[nodiscard]] std::size_t speakerCount() const noexcept { return speaker_count_; }

    void setSpeaker(std::size_t index, double azimuth_degrees, double radius, double elevation_degrees) noexcept
    {
        if (index >= kMaxSpeakers) return;
        speakers_[index].azimuth = wrapPhase((azimuth_degrees + 180.0) * kPi / 180.0) - kPi;
        speakers_[index].radius = clip(radius, 0.15, 1.35);
        speakers_[index].elevation = clip(elevation_degrees / 90.0, -1.0, 1.0);
    }

    void setSpeakerCartesian(std::size_t index, double x, double y, double elevation = 0.0) noexcept
    {
        if (index >= kMaxSpeakers) return;
        speakers_[index].azimuth = std::atan2(y, x);
        speakers_[index].radius = clip(std::sqrt(x * x + y * y), 0.15, 1.35);
        speakers_[index].elevation = clip(elevation, -1.0, 1.0);
    }

    [[nodiscard]] Speaker speaker(std::size_t index) const noexcept
    {
        return index < kMaxSpeakers ? speakers_[index] : Speaker {};
    }

    void setParameters(const std::array<double, kParameterCount>& values) noexcept
    {
        for (std::size_t i = 0; i < parameters_.size(); ++i) parameters_[i] = clip(values[i], 0.0, 1.0);
    }

    [[nodiscard]] const std::array<double, kParameterCount>& parameters() const noexcept { return parameters_; }

    void setCenter(double x, double y) noexcept
    {
        center_x_ = clip(x, -0.85, 0.85);
        center_y_ = clip(y, -0.85, 0.85);
    }

    void note(const Event& event) noexcept
    {
        event_pitch_ = clip(static_cast<double>(event.pitch), 0.0, 127.0);
        event_velocity_ = clip(static_cast<double>(event.velocity) / 127.0, 0.0, 1.0);
        event_energy_ = std::max(event_energy_, event_velocity_);
        event_duration_ = std::max(1.0, static_cast<double>(event.duration_ms) * 0.001);
        phase_ += (event_pitch_ - 60.0) * 0.013;
        std::size_t index = static_cast<std::size_t>(std::max(0L, event.pitch)) % activeAgents();
        agents_[index].vx += randomSigned() * 0.28 * event_velocity_;
        agents_[index].vy += randomSigned() * 0.28 * event_velocity_;
    }

    void reset() noexcept
    {
        rng_ = 0x9e3779b9u ^ (static_cast<std::uint32_t>(kind_) * 0x85ebca6bu);
        phase_ = 0.0;
        event_energy_ = 0.0;
        event_pitch_ = 60.0;
        event_velocity_ = 0.0;
        event_duration_ = 0.5;
        lowpass_ = 0.0;
        control_counter_ = 0;
        trail_phase_ = 0.0;
        for (std::size_t i = 0; i < kMaxAgents; ++i) {
            const double angle = kTwoPi * static_cast<double>(i) / static_cast<double>(kMaxAgents);
            const double radius = 0.18 + 0.055 * static_cast<double>(i % 6);
            agents_[i] = {std::cos(angle) * radius, std::sin(angle) * radius, std::sin(angle * 2.0) * 0.12,
                -std::sin(angle) * 0.05, std::cos(angle) * 0.05, 0.0, angle, 1.0};
            agent_signal_[i] = 0.0;
        }
        std::fill(buffer_.begin(), buffer_.end(), 0.0);
        write_index_ = 0;
        samples_written_ = 0;
    }

    void process(const double* input, double** outputs, std::size_t frames, std::size_t output_count,
        bool input_connected, bool bypass, bool frozen) noexcept
    {
        if (!outputs) return;
        const std::size_t channels = std::clamp<std::size_t>(output_count, 1, speaker_count_);

        const double level = parameters_[11] * 1.3;
        const double bypass_gain = level / std::sqrt(static_cast<double>(channels));
        for (std::size_t frame = 0; frame < frames; ++frame) {
            // Read before clearing the current output frame. Max may alias the
            // input vector with an output vector when in-place DSP is allowed.
            const double dry = input_connected && input ? input[frame] : 0.0;
            for (std::size_t channel = 0; channel < channels; ++channel)
                if (outputs[channel]) outputs[channel][frame] = 0.0;
            if (bypass) {
                for (std::size_t channel = 0; channel < channels; ++channel)
                    if (outputs[channel]) outputs[channel][frame] = dry * bypass_gain;
                continue;
            }

            const double color_frequency = 100.0 + 16000.0 * parameters_[10] * parameters_[10];
            const double coefficient = 1.0 - std::exp(-kTwoPi * color_frequency / sample_rate_);
            lowpass_ += coefficient * (dry - lowpass_);
            const double colored = lowpass_ + (dry - lowpass_) * parameters_[10];

            if (!frozen && (++control_counter_ & 7u) == 0u) updateBehavior(8.0 / sample_rate_);
            if (!frozen && !buffer_.empty()) {
                buffer_[write_index_] = colored;
                write_index_ = (write_index_ + 1) % buffer_.size();
                samples_written_ = std::min(samples_written_ + 1, buffer_.size());
            }
            renderAgentSignals(colored);

            const std::size_t agents = activeAgents();
            // Agents usually carry correlated copies of the input. Dividing by
            // their count keeps colony size from becoming an unintended gain
            // control while each individual panning vector remains equal-power.
            const double agent_scale = level / static_cast<double>(agents);
            for (std::size_t agent = 0; agent < agents; ++agent) {
                std::array<double, kMaxSpeakers> gains {};
                computeGains(agents_[agent], gains, channels);
                const double signal = agent_signal_[agent] * agent_scale;
                for (std::size_t channel = 0; channel < channels; ++channel)
                    if (outputs[channel]) outputs[channel][frame] += signal * gains[channel];
            }
            event_energy_ *= std::exp(-1.0 / (sample_rate_ * std::max(0.03, event_duration_)));
        }
    }

    [[nodiscard]] std::array<double, 3> centroid() const noexcept
    {
        std::array<double, 3> value {};
        const std::size_t count = activeAgents();
        for (std::size_t i = 0; i < count; ++i) {
            value[0] += agents_[i].x + center_x_;
            value[1] += agents_[i].y + center_y_;
            value[2] += agents_[i].z;
        }
        const double inverse = 1.0 / static_cast<double>(count);
        for (double& component : value) component *= inverse;
        return value;
    }

    [[nodiscard]] std::array<double, 3> agentPosition(std::size_t index) const noexcept
    {
        if (index >= activeAgents()) return {};
        return {agents_[index].x + center_x_, agents_[index].y + center_y_, agents_[index].z};
    }

    [[nodiscard]] std::size_t agentCount() const noexcept { return activeAgents(); }

private:
    struct Agent {
        double x = 0.0, y = 0.0, z = 0.0;
        double vx = 0.0, vy = 0.0, vz = 0.0;
        double phase = 0.0, weight = 1.0;
    };

    [[nodiscard]] std::size_t activeAgents() const noexcept
    {
        return std::clamp<std::size_t>(moduleInfo(kind_).agents, 1, kMaxAgents);
    }

    std::uint32_t nextRandom() noexcept
    {
        std::uint32_t x = rng_ ? rng_ : 0x6d2b79f5u;
        x ^= x << 13; x ^= x >> 17; x ^= x << 5;
        rng_ = x;
        return x;
    }

    double randomSigned() noexcept
    {
        return (static_cast<double>(nextRandom()) / static_cast<double>(UINT32_MAX)) * 2.0 - 1.0;
    }

    void contain(Agent& agent) noexcept
    {
        const double radius = std::sqrt(agent.x * agent.x + agent.y * agent.y);
        const double limit = 0.12 + parameters_[9] * 0.86;
        if (radius > limit && radius > 0.0) {
            const double force = (radius - limit) * 3.2;
            agent.vx -= agent.x / radius * force;
            agent.vy -= agent.y / radius * force;
        }
        agent.x = clip(agent.x, -1.15, 1.15);
        agent.y = clip(agent.y, -1.15, 1.15);
        agent.z = clip(agent.z, -1.0, 1.0);
    }

    void updateBehavior(double dt) noexcept
    {
        const std::size_t count = activeAgents();
        const double rate = 0.08 * std::pow(55.0, parameters_[0]);
        const double rotation = (parameters_[7] * 2.0 - 1.0) * (0.25 + rate);
        const double turbulence = parameters_[6] * parameters_[6];
        const double inertia = 0.68 + parameters_[3] * 0.315;
        phase_ = wrapPhase(phase_ + dt * rate);
        trail_phase_ = wrapPhase(trail_phase_ + dt * rate * 0.37);

        double center_x = 0.0, center_y = 0.0;
        for (std::size_t i = 0; i < count; ++i) { center_x += agents_[i].x; center_y += agents_[i].y; }
        center_x /= static_cast<double>(count);
        center_y /= static_cast<double>(count);

        for (std::size_t i = 0; i < count; ++i) {
            Agent& a = agents_[i];
            const double index_phase = kTwoPi * static_cast<double>(i) / static_cast<double>(count);
            double ax = 0.0, ay = 0.0, az = 0.0;

            switch (kind_) {
                case Kind::Orbit: {
                    const double eccentricity = 0.18 + parameters_[4] * 0.62;
                    const double p = phase_ * (1.0 + 0.07 * static_cast<double>(i)) + index_phase;
                    const double target_x = std::cos(p) * (0.22 + parameters_[9] * 0.68);
                    const double target_y = std::sin(p) * (0.22 + parameters_[9] * 0.68) * (1.0 - eccentricity * 0.55);
                    ax = (target_x - a.x) * (2.0 + parameters_[4] * 5.0) - a.y * rotation;
                    ay = (target_y - a.y) * (2.0 + parameters_[4] * 5.0) + a.x * rotation;
                    az = std::sin(p * 0.5) * parameters_[8] - a.z;
                    break;
                }
                case Kind::Swarm: {
                    ax += (center_x - a.x) * parameters_[4] * 3.2;
                    ay += (center_y - a.y) * parameters_[4] * 3.2;
                    for (std::size_t j = 0; j < count; ++j) if (j != i) {
                        const double dx = a.x - agents_[j].x, dy = a.y - agents_[j].y;
                        const double d2 = dx * dx + dy * dy + 0.002;
                        if (d2 < 0.18) { ax += dx / d2 * parameters_[5] * 0.018; ay += dy / d2 * parameters_[5] * 0.018; }
                    }
                    ax += -a.y * rotation * 0.4;
                    ay += a.x * rotation * 0.4;
                    break;
                }
                case Kind::Flow: {
                    const double scale = 2.2 + parameters_[5] * 5.0;
                    ax = std::sin(a.y * scale + phase_) + std::cos(a.x * scale * 0.7 - phase_ * 0.4);
                    ay = -std::sin(a.x * scale - phase_ * 0.7) + std::cos(a.y * scale * 0.8 + phase_);
                    ax *= 0.6 + parameters_[4]; ay *= 0.6 + parameters_[4];
                    az = std::sin((a.x + a.y) * 3.0 + phase_) * parameters_[8] - a.z * 0.7;
                    break;
                }
                case Kind::Growth: {
                    const double generation = std::fmod(phase_ / kTwoPi + static_cast<double>(i) / static_cast<double>(count), 1.0);
                    const double golden = 2.399963229728653;
                    const double p = index_phase + std::floor(phase_ / kTwoPi * 7.0 + static_cast<double>(i)) * golden + rotation * phase_ * 0.1;
                    const double radius = (0.08 + generation * (0.22 + parameters_[9] * 0.72));
                    ax = (std::cos(p) * radius - a.x) * (2.0 + parameters_[4] * 4.0);
                    ay = (std::sin(p) * radius - a.y) * (2.0 + parameters_[4] * 4.0);
                    az = (generation * 2.0 - 1.0) * parameters_[8] - a.z;
                    break;
                }
                case Kind::Physarum: {
                    const double nutrient_angle = phase_ * 0.43 + index_phase * 2.0;
                    const double nx = std::cos(nutrient_angle) * (0.25 + parameters_[9] * 0.6);
                    const double ny = std::sin(nutrient_angle * 1.31) * (0.25 + parameters_[9] * 0.6);
                    ax = (nx - a.x) * parameters_[4] * 2.4 + (agents_[(i + count - 1) % count].x - a.x) * 0.8;
                    ay = (ny - a.y) * parameters_[4] * 2.4 + (agents_[(i + count - 1) % count].y - a.y) * 0.8;
                    ax -= a.y * rotation * 0.2; ay += a.x * rotation * 0.2;
                    break;
                }
                case Kind::Membrane: {
                    const double p = index_phase + std::sin(phase_ * 0.41) * parameters_[7];
                    const double wave = std::sin(phase_ * (1.0 + i * 0.11) - index_phase * 2.0);
                    const double radius = 0.34 + parameters_[9] * 0.45 + wave * (0.06 + event_energy_ * 0.24);
                    ax = (std::cos(p) * radius - a.x) * (3.0 + parameters_[4] * 4.0);
                    ay = (std::sin(p) * radius - a.y) * (3.0 + parameters_[4] * 4.0);
                    az = wave * parameters_[8] * (0.4 + event_energy_) - a.z;
                    break;
                }
                case Kind::Vortex: {
                    const double sign = i % 2 ? -1.0 : 1.0;
                    const double well_x = sign * (0.12 + parameters_[5] * 0.34);
                    const double dx = a.x - well_x, dy = a.y;
                    const double d2 = dx * dx + dy * dy + 0.025;
                    ax = -dx * parameters_[4] * 2.0 - sign * dy / d2 * (0.04 + std::abs(rotation) * 0.08);
                    ay = -dy * parameters_[4] * 2.0 + sign * dx / d2 * (0.04 + std::abs(rotation) * 0.08);
                    break;
                }
                case Kind::Brownian: {
                    ax = randomSigned() * (0.3 + turbulence * 4.0) - a.x * parameters_[4] * 0.45;
                    ay = randomSigned() * (0.3 + turbulence * 4.0) - a.y * parameters_[4] * 0.45;
                    az = randomSigned() * parameters_[8] - a.z * 0.4;
                    break;
                }
                case Kind::Granular: {
                    const double p = phase_ * (0.32 + i * 0.047) + index_phase;
                    const double radius = 0.16 + parameters_[9] * (0.25 + 0.55 * (0.5 + 0.5 * std::sin(p * 1.71)));
                    ax = (std::cos(p) * radius - a.x) * (1.5 + parameters_[4] * 3.0);
                    ay = (std::sin(p * 1.17) * radius - a.y) * (1.5 + parameters_[4] * 3.0);
                    az = std::sin(p * 0.73) * parameters_[8] - a.z;
                    break;
                }
                case Kind::Constellation: {
                    const double harmonic = 1.0 + static_cast<double>(i % 5);
                    const double target_x = std::sin(phase_ * (0.17 + harmonic * 0.03) + index_phase) * (0.2 + parameters_[9] * 0.72);
                    const double target_y = std::sin(phase_ * (0.23 + harmonic * 0.041) + index_phase * 1.618) * (0.2 + parameters_[9] * 0.72);
                    ax = (target_x - a.x) * (1.6 + parameters_[4] * 3.2);
                    ay = (target_y - a.y) * (1.6 + parameters_[4] * 3.2);
                    az = std::sin(phase_ * 0.19 + index_phase) * parameters_[8] - a.z;
                    break;
                }
            }

            ax += randomSigned() * turbulence * 1.8 + event_energy_ * randomSigned() * 0.6;
            ay += randomSigned() * turbulence * 1.8 + event_energy_ * randomSigned() * 0.6;
            a.vx = a.vx * inertia + ax * dt;
            a.vy = a.vy * inertia + ay * dt;
            a.vz = a.vz * inertia + az * dt;
            a.x += a.vx * dt * (8.0 + rate * 2.0);
            a.y += a.vy * dt * (8.0 + rate * 2.0);
            a.z += a.vz * dt * (4.0 + rate);
            contain(a);
        }
    }

    void renderAgentSignals(double input) noexcept
    {
        const std::size_t count = activeAgents();
        if (kind_ == Kind::Granular && !buffer_.empty()) {
            const double history = 0.015 + parameters_[10] * 2.7;
            for (std::size_t i = 0; i < count; ++i) {
                const double offset_seconds = history * (0.12 + 0.88 * static_cast<double>(i + 1) / static_cast<double>(count));
                const std::size_t offset = static_cast<std::size_t>(offset_seconds * sample_rate_) % buffer_.size();
                const std::size_t jitter = static_cast<std::size_t>((0.5 + 0.5 * std::sin(phase_ * (1.0 + i * 0.07))) * parameters_[6] * sample_rate_ * 0.08);
                const std::size_t read = (write_index_ + buffer_.size() - 1 - (offset + jitter) % buffer_.size()) % buffer_.size();
                agent_signal_[i] = offset + jitter < samples_written_ ? buffer_[read] : input;
            }
        }
        else {
            for (std::size_t i = 0; i < count; ++i) {
                const double articulation = 0.82 + 0.18 * std::sin(phase_ * (0.37 + i * 0.03) + agents_[i].phase);
                agent_signal_[i] = input * articulation;
            }
        }
    }

    void computeGains(const Agent& agent, std::array<double, kMaxSpeakers>& gains, std::size_t channels) const noexcept
    {
        const double spread = 0.055 + parameters_[1] * parameters_[1] * 0.95;
        const double diffuse = parameters_[2] * 0.28;
        double power = 0.0;
        for (std::size_t channel = 0; channel < channels; ++channel) {
            const Speaker& speaker = speakers_[channel];
            const double sx = std::cos(speaker.azimuth) * speaker.radius;
            const double sy = std::sin(speaker.azimuth) * speaker.radius;
            const double dx = agent.x + center_x_ - sx, dy = agent.y + center_y_ - sy;
            const double dz = (agent.z - speaker.elevation) * (0.25 + parameters_[8] * 0.75);
            const double d2 = dx * dx + dy * dy + dz * dz;
            const double focused = std::exp(-d2 / (2.0 * spread * spread));
            const double inverse = 1.0 / (1.0 + d2 * (2.0 + 10.0 * (1.0 - parameters_[1])));
            gains[channel] = diffuse + focused * 0.72 + inverse * 0.28;
            power += gains[channel] * gains[channel];
        }
        const double norm = power > 1.0e-12 ? 1.0 / std::sqrt(power) : 1.0 / std::sqrt(static_cast<double>(channels));
        for (std::size_t channel = 0; channel < channels; ++channel) gains[channel] *= norm;
    }

    Kind kind_;
    std::size_t speaker_count_ = 8;
    double sample_rate_ = 48000.0;
    std::array<double, kParameterCount> parameters_ {};
    std::array<Speaker, kMaxSpeakers> speakers_ {};
    std::array<Agent, kMaxAgents> agents_ {};
    std::array<double, kMaxAgents> agent_signal_ {};
    std::vector<double> buffer_;
    std::size_t write_index_ = 0;
    std::size_t samples_written_ = 0;
    std::uint32_t rng_ = 0x9e3779b9u;
    std::uint32_t control_counter_ = 0;
    double phase_ = 0.0;
    double trail_phase_ = 0.0;
    double event_energy_ = 0.0;
    double event_pitch_ = 60.0;
    double event_velocity_ = 0.0;
    double event_duration_ = 0.5;
    double lowpass_ = 0.0;
    double center_x_ = 0.0;
    double center_y_ = 0.0;
};

} // namespace myseq::amoeba
