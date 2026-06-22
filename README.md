# Amoeba DSP

Amoeba DSP is a collection of ten speaker-aware spatial processors for Max/MSP. Each object accepts one audio signal and creates between 1 and 18 discrete MSP signal outlets. A radial interface combines editable physical speaker placement with automated motion inspired by amoebas, flocking, growth, fluids, membranes, vortices, and granular clouds.

This is an organic speaker-aware spatialization system rather than a strict spherical-harmonic Ambisonics encoder/decoder. It uses normalized equal-power distance panning against the speaker coordinates drawn by the user, so irregular installations remain part of the automation model.

## Externals

- `amoeba.outputs.orbit~`: eccentric orbits, precession, comet paths, and gravity wells.
- `amoeba.outputs.swarm~`: flocking agents with cohesion, separation, panic, and murmuration.
- `amoeba.outputs.flow~`: curl-field advection, tides, rapids, and turbulent streams.
- `amoeba.outputs.growth~`: branching tips, phyllotaxis, fern, coral, and bloom structures.
- `amoeba.outputs.physarum~`: nutrient-seeking slime agents and connected vein behavior.
- `amoeba.outputs.membrane~`: elastic-body waves, surface tension, stretch, and rupture.
- `amoeba.outputs.vortex~`: paired counter-rotating vortices and spiral attractors.
- `amoeba.outputs.brownian~`: inertial random walks from dust through plasma motion.
- `amoeba.outputs.granular~`: four-second multihead time grains distributed as a moving cloud.
- `amoeba.outputs.constellation~`: speaker-aware Lissajous nodes, webs, and spatial galaxies.

Prebuilt universal macOS bundles are included under `externals/`. They contain both Apple Silicon and Intel architectures.

## Creating an object

Use the `@outputs` creation attribute to choose the number of physical signal outlets:

```text
amoeba.outputs.orbit~ @outputs 8
amoeba.outputs.swarm~ @outputs 12
amoeba.outputs.granular~ @outputs 18
```

The outlet count is structural. Recreate the object to change it. Connect the outlets directly to the corresponding channels of `dac~`, an audio interface routing layer, or a recorder. The leftmost outlet is speaker 1.

## Radial speaker editor

Numbered nodes represent physical outputs. Drag a node to edit its azimuth and distance. Select it to expose precise azimuth, distance, and elevation controls in the right panel. Drag the bright central core to offset the complete organic trajectory.

Four layout generators provide starting points:

```text
layout circle
layout front
layout dome
layout random
```

Speaker coordinates can also be entered explicitly:

```text
speaker 1 -45. 1.0 0.
speaker 2 45. 0.85 25.
```

The arguments are `one_based_output azimuth_degrees distance elevation_degrees`.

## Performance and automation

```text
center 0.2 -0.15
auto 1
freeze 1
bypass 1
character random
param 1 0.5
set turbulence 0.72
randomize
reset
dump
tooltips 1
```

All objects accept a standard sequencer event list:

```text
pitch velocity duration_ms channel
```

Events inject pitch- and velocity-dependent energy into the active motion system. They can be connected directly to event outputs from `megaseq` or another MIDI event generator.

## Presets

Each external and output count owns an independent 16-slot bank. Presets retain all twelve behavior parameters, the core position, automation state, and coordinates for all eighteen possible speakers.

```text
preset 1 store MyRoom
preset 1 recall
preset 1 clear
preset list
preset save
preset load
```

Default banks are stored in `~/.myseq/presets/`.

## Installation

Place this repository in the Max Packages directory:

```text
~/Documents/Max 9/Packages/Amoeba DSP
```

Alternatively, add the repository root to Max's File Preferences search paths. Restart Max or refresh the file database after installation. Open any object help file from Max to load a six-speaker working example.

## Building

A local checkout of Cycling '74 `max-sdk-base` is required. CMake automatically checks common Max SDK locations and can fetch `max-sdk-base` when it is not available locally.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

On macOS, the default build creates universal `arm64` and `x86_64` bundles in `externals/`.

To run only the portable DSP test without the Max SDK:

```sh
cmake -S . -B build -DAMOEBA_BUILD_EXTERNALS=OFF
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Repository layout

- `source/include/`: shared spatial DSP engine, Max wrapper, presets, and tooltip helpers.
- `source/projects/`: ten small external targets using the shared implementation.
- `externals/`: prebuilt universal macOS `.mxo` bundles.
- `help/`: interactive Max help patches.
- `tests/`: portable DSP validation for every organic motion system and all 18 outputs.
