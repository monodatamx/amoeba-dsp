{
  "patcher": {
    "fileversion": 1, "appversion": { "major": 9, "minor": 0, "revision": 0, "architecture": "x64", "modernui": 1 },
    "classnamespace": "box", "rect": [30.0, 30.0, 1540.0, 900.0], "default_fontsize": 12.0, "default_fontname": "Arial",
    "boxes": [
      { "box": { "id": "title", "maxclass": "comment", "text": "Amoeba Outputs Vortex — paired spiral attractor spatializer", "fontsize": 18.0, "patching_rect": [30.0, 15.0, 740.0, 26.0] } },
      { "box": { "id": "source", "maxclass": "newobj", "text": "saw~ 55", "patching_rect": [30.0, 52.0, 60.0, 22.0] } },
      { "box": { "id": "event", "maxclass": "message", "text": "36 118 840 4", "patching_rect": [108.0, 52.0, 100.0, 22.0] } },
      { "box": { "id": "module", "maxclass": "newobj", "text": "amoeba.outputs.vortex~ @outputs 6", "patching_rect": [30.0, 90.0, 1040.0, 720.0] } },
      { "box": { "id": "dac", "maxclass": "newobj", "text": "dac~ 1 2 3 4 5 6", "patching_rect": [30.0, 840.0, 118.0, 22.0] } },
      { "box": { "id": "circle", "maxclass": "message", "text": "layout circle", "patching_rect": [1100.0, 105.0, 84.0, 22.0] } },
      { "box": { "id": "center", "maxclass": "message", "text": "center 0. 0.", "patching_rect": [1195.0, 105.0, 74.0, 22.0] } },
      { "box": { "id": "speaker", "maxclass": "message", "text": "speaker 1 -60. 0.9 0.", "patching_rect": [1100.0, 145.0, 140.0, 22.0] } },
      { "box": { "id": "character", "maxclass": "message", "text": "character storm", "patching_rect": [1100.0, 185.0, 110.0, 22.0] } },
      { "box": { "id": "param", "maxclass": "message", "text": "set rotation 0.92", "patching_rect": [1100.0, 225.0, 122.0, 22.0] } },
      { "box": { "id": "info", "maxclass": "comment", "text": "Counter-rotating wells pull six audio foci into paired spirals. Attraction sets well gravity, repulsion separates the pair, rotation changes angular force, and turbulence destabilizes otherwise repeatable trajectories.", "linecount": 7, "patching_rect": [1100.0, 285.0, 330.0, 120.0] } }
    ],
    "lines": [
      { "patchline": { "source": ["source", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["event", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["circle", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["center", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["speaker", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["character", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["param", 0], "destination": ["module", 0] } },
      { "patchline": { "source": ["module", 0], "destination": ["dac", 0] } }, { "patchline": { "source": ["module", 1], "destination": ["dac", 1] } }, { "patchline": { "source": ["module", 2], "destination": ["dac", 2] } }, { "patchline": { "source": ["module", 3], "destination": ["dac", 3] } }, { "patchline": { "source": ["module", 4], "destination": ["dac", 4] } }, { "patchline": { "source": ["module", 5], "destination": ["dac", 5] } }
    ]
  }
}
