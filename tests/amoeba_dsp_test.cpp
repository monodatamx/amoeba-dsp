#include "myseq_amoeba_dsp.hpp"

#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

using myseq::amoeba::AmoebaEngine;
using myseq::amoeba::Kind;

int main()
{
    const auto require = [](bool condition, const char* message) {
        if (!condition) {
            std::cerr << "FAILED: " << message << '\n';
            std::exit(1);
        }
    };
    constexpr std::size_t frames = 4096;
    constexpr std::size_t channels = 18;
    const std::array<Kind, 10> kinds {{
        Kind::Orbit, Kind::Swarm, Kind::Flow, Kind::Growth, Kind::Physarum,
        Kind::Membrane, Kind::Vortex, Kind::Brownian, Kind::Granular, Kind::Constellation
    }};

    std::vector<double> input(frames);
    for (std::size_t i = 0; i < frames; ++i)
        input[i] = std::sin(2.0 * myseq::amoeba::kPi * 220.0 * static_cast<double>(i) / 48000.0) * 0.25;

    for (Kind kind : kinds) {
        AmoebaEngine engine(kind, channels);
        engine.setSampleRate(48000.0);
        require(engine.speakerCount() == channels, "speaker count");
        require(engine.agentCount() >= 1 && engine.agentCount() <= myseq::amoeba::kMaxAgents, "agent count");
        engine.setSpeaker(0, -35.0, 0.82, 22.0);
        const auto speaker = engine.speaker(0);
        require(std::abs(speaker.radius - 0.82) < 1.0e-12, "speaker placement");
        engine.note({64, 118, 900, 3});

        std::array<std::vector<double>, channels> output;
        std::array<double*, channels> pointers {};
        for (std::size_t channel = 0; channel < channels; ++channel) {
            output[channel].resize(frames);
            pointers[channel] = output[channel].data();
        }
        engine.process(input.data(), pointers.data(), frames, channels, true, false, false);

        double total_energy = 0.0;
        std::size_t active_channels = 0;
        for (const auto& channel : output) {
            double energy = 0.0;
            for (double sample : channel) {
                require(std::isfinite(sample), "finite output");
                energy += sample * sample;
            }
            total_energy += energy;
            if (energy > 1.0e-10) ++active_channels;
        }
        require(total_energy > 1.0e-6, "audible output");
        require(active_channels >= 2, "multichannel distribution");

        const auto centroid = engine.centroid();
        for (double value : centroid) require(std::isfinite(value), "finite centroid");
        std::cout << myseq::amoeba::moduleInfo(kind).title << " energy " << total_energy << '\n';
    }

    AmoebaEngine bypass(Kind::Orbit, 4);
    bypass.setSampleRate(48000.0);
    std::array<std::vector<double>, 4> bypass_output;
    std::array<double*, 4> bypass_pointers {};
    for (std::size_t channel = 0; channel < 4; ++channel) {
        bypass_output[channel].resize(frames);
        bypass_pointers[channel] = bypass_output[channel].data();
    }
    bypass.process(input.data(), bypass_pointers.data(), frames, 4, true, true, false);
    const double expected_ratio = 1.3 * myseq::amoeba::defaultParameters()[11] / 2.0;
    for (std::size_t channel = 0; channel < 4; ++channel)
        for (std::size_t i = 0; i < frames; ++i)
            require(std::abs(bypass_output[channel][i] - input[i] * expected_ratio) < 1.0e-12, "equal-power bypass");

    // Max may offer the input vector as an in-place output vector. The engine
    // must read each input sample before overwriting the aliased output frame.
    AmoebaEngine aliased(Kind::Orbit, 4);
    aliased.setSampleRate(48000.0);
    std::vector<double> aliased_input = input;
    std::array<std::vector<double>, 3> aliased_extra;
    std::array<double*, 4> aliased_outputs {{aliased_input.data(), nullptr, nullptr, nullptr}};
    for (std::size_t channel = 0; channel < aliased_extra.size(); ++channel) {
        aliased_extra[channel].resize(frames);
        aliased_outputs[channel + 1] = aliased_extra[channel].data();
    }
    aliased.process(aliased_input.data(), aliased_outputs.data(), frames, 4, true, false, false);
    double aliased_energy = 0.0;
    for (const auto& channel : aliased_extra)
        for (double sample : channel) aliased_energy += sample * sample;
    require(aliased_energy > 1.0e-6, "in-place input alias remains audible");

    return 0;
}
