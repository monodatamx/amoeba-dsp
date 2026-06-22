{
  "patcher": {
    "fileversion": 1, "appversion": { "major": 9, "minor": 0, "revision": 0, "architecture": "x64", "modernui": 1 },
    "classnamespace": "box", "rect": [30.0, 30.0, 1540.0, 900.0], "default_fontsize": 12.0, "default_fontname": "Arial",
    "boxes": [
      { "box": { "id": "title", "maxclass": "comment", "text": "Amoeba Outputs Swarm — emergent flocking spatializer", "fontsize": 18.0, "patching_rect": [30.0, 15.0, 720.0, 26.0] } },
      { "box": { "id": "source", "maxclass": "newobj", "text": "noise~", "patching_rect": [30.0, 52.0, 48.0, 22.0] } },
      { "box": { "id": "event", "maxclass": "message", "text": "67 116 640 3", "patching_rect": [96.0, 52.0, 100.0, 22.0] } },
      { "box": { "id": "module", "maxclass": "newobj", "text": "amoeba.outputs.swarm~ @outputs 6", "patching_rect": [30.0, 90.0, 1040.0, 720.0] } },
      { "box": { "id": "dac", "maxclass": "newobj", "text": "dac~ 1 2 3 4 5 6", "patching_rect": [30.0, 840.0, 118.0, 22.0] } },
      { "box": { "id": "front", "maxclass": "message", "text": "layout front", "patching_rect": [1100.0, 105.0, 78.0, 22.0] } },
      { "box": { "id": "random", "maxclass": "message", "text": "layout random", "patching_rect": [1190.0, 105.0, 92.0, 22.0] } },
      { "box": { "id": "speaker", "maxclass": "message", "text": "speaker 4 130. 0.8 -15.", "patching_rect": [1100.0, 145.0, 154.0, 22.0] } },
      { "box": { "id": "character", "maxclass": "message", "text": "character murmur", "patching_rect": [1100.0, 185.0, 118.0, 22.0] } },
      { "box": { "id": "param", "maxclass": "message", "text": "set repulsion 0.62", "patching_rect": [1100.0, 225.0, 126.0, 22.0] } },
      { "box": { "id": "info", "maxclass": "comment", "text": "Ten virtual agents share the input while cohesion, separation, inertia, turbulence, and colony attraction produce flocking motion. Speaker distance and elevation remain part of the panning solution, even in irregular rooms.", "linecount": 7, "patching_rect": [1100.0, 285.0, 330.0, 120.0] } }
    ],
    "lines": [
      { "patchline": { "source": ["source", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["event", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["front", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["random", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["speaker", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["character", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["param", 0], "destination": ["module", 0] } },
      { "patchline": { "source": ["module", 0], "destination": ["dac", 0] } }, { "patchline": { "source": ["module", 1], "destination": ["dac", 1] } }, { "patchline": { "source": ["module", 2], "destination": ["dac", 2] } }, { "patchline": { "source": ["module", 3], "destination": ["dac", 3] } }, { "patchline": { "source": ["module", 4], "destination": ["dac", 4] } }, { "patchline": { "source": ["module", 5], "destination": ["dac", 5] } }
    ]
  }
}
