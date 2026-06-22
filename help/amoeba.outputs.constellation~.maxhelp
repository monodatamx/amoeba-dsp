{
  "patcher": {
    "fileversion": 1, "appversion": { "major": 9, "minor": 0, "revision": 0, "architecture": "x64", "modernui": 1 },
    "classnamespace": "box", "rect": [30.0, 30.0, 1540.0, 900.0], "default_fontsize": 12.0, "default_fontname": "Arial",
    "boxes": [
      { "box": { "id": "title", "maxclass": "comment", "text": "Amoeba Outputs Constellation — Lissajous node spatializer", "fontsize": 18.0, "patching_rect": [30.0, 15.0, 740.0, 26.0] } },
      { "box": { "id": "source", "maxclass": "newobj", "text": "cycle~ 261.63", "patching_rect": [30.0, 52.0, 88.0, 22.0] } },
      { "box": { "id": "event", "maxclass": "message", "text": "84 96 1800 3", "patching_rect": [136.0, 52.0, 104.0, 22.0] } },
      { "box": { "id": "module", "maxclass": "newobj", "text": "amoeba.outputs.constellation~ @outputs 6", "patching_rect": [30.0, 90.0, 1040.0, 720.0] } },
      { "box": { "id": "dac", "maxclass": "newobj", "text": "dac~ 1 2 3 4 5 6", "patching_rect": [30.0, 840.0, 118.0, 22.0] } },
      { "box": { "id": "front", "maxclass": "message", "text": "layout front", "patching_rect": [1100.0, 105.0, 78.0, 22.0] } },
      { "box": { "id": "circle", "maxclass": "message", "text": "layout circle", "patching_rect": [1190.0, 105.0, 84.0, 22.0] } },
      { "box": { "id": "speaker", "maxclass": "message", "text": "speaker 6 150. 1.25 0.", "patching_rect": [1100.0, 145.0, 148.0, 22.0] } },
      { "box": { "id": "character", "maxclass": "message", "text": "character galaxy", "patching_rect": [1100.0, 185.0, 112.0, 22.0] } },
      { "box": { "id": "preset", "maxclass": "message", "text": "preset 4 store SpeakerWeb", "patching_rect": [1100.0, 225.0, 168.0, 22.0] } },
      { "box": { "id": "info", "maxclass": "comment", "text": "Twelve Lissajous nodes form repeatable webs that remain aware of edited speaker distances and elevations. Use low turbulence for installations and increase diffusion or rotation for continuously evolving spatial galaxies.", "linecount": 7, "patching_rect": [1100.0, 285.0, 330.0, 120.0] } }
    ],
    "lines": [
      { "patchline": { "source": ["source", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["event", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["front", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["circle", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["speaker", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["character", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["preset", 0], "destination": ["module", 0] } },
      { "patchline": { "source": ["module", 0], "destination": ["dac", 0] } }, { "patchline": { "source": ["module", 1], "destination": ["dac", 1] } }, { "patchline": { "source": ["module", 2], "destination": ["dac", 2] } }, { "patchline": { "source": ["module", 3], "destination": ["dac", 3] } }, { "patchline": { "source": ["module", 4], "destination": ["dac", 4] } }, { "patchline": { "source": ["module", 5], "destination": ["dac", 5] } }
    ]
  }
}
