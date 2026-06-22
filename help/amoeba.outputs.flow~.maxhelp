{
  "patcher": {
    "fileversion": 1, "appversion": { "major": 9, "minor": 0, "revision": 0, "architecture": "x64", "modernui": 1 },
    "classnamespace": "box", "rect": [30.0, 30.0, 1540.0, 900.0], "default_fontsize": 12.0, "default_fontname": "Arial",
    "boxes": [
      { "box": { "id": "title", "maxclass": "comment", "text": "Amoeba Outputs Flow — curl-field and turbulent stream spatializer", "fontsize": 18.0, "patching_rect": [30.0, 15.0, 760.0, 26.0] } },
      { "box": { "id": "source", "maxclass": "newobj", "text": "saw~ 82.41", "patching_rect": [30.0, 52.0, 76.0, 22.0] } },
      { "box": { "id": "event", "maxclass": "message", "text": "48 108 1200 4", "patching_rect": [125.0, 52.0, 106.0, 22.0] } },
      { "box": { "id": "module", "maxclass": "newobj", "text": "amoeba.outputs.flow~ @outputs 6", "patching_rect": [30.0, 90.0, 1040.0, 720.0] } },
      { "box": { "id": "dac", "maxclass": "newobj", "text": "dac~ 1 2 3 4 5 6", "patching_rect": [30.0, 840.0, 118.0, 22.0] } },
      { "box": { "id": "dome", "maxclass": "message", "text": "layout dome", "patching_rect": [1100.0, 105.0, 82.0, 22.0] } },
      { "box": { "id": "center", "maxclass": "message", "text": "center 0.25 -0.2", "patching_rect": [1195.0, 105.0, 102.0, 22.0] } },
      { "box": { "id": "speaker", "maxclass": "message", "text": "speaker 2 35. 1.1 30.", "patching_rect": [1100.0, 145.0, 142.0, 22.0] } },
      { "box": { "id": "character", "maxclass": "message", "text": "character rapids", "patching_rect": [1100.0, 185.0, 112.0, 22.0] } },
      { "box": { "id": "param", "maxclass": "message", "text": "set turbulence 0.78", "patching_rect": [1100.0, 225.0, 132.0, 22.0] } },
      { "box": { "id": "info", "maxclass": "comment", "text": "Six virtual fluid parcels follow a bounded curl field. RATE changes advection time, TURBULENCE disrupts streamlines, ROTATION changes handedness, and ELEVATION makes the field useful with dome or balcony speakers.", "linecount": 7, "patching_rect": [1100.0, 285.0, 330.0, 120.0] } }
    ],
    "lines": [
      { "patchline": { "source": ["source", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["event", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["dome", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["center", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["speaker", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["character", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["param", 0], "destination": ["module", 0] } },
      { "patchline": { "source": ["module", 0], "destination": ["dac", 0] } }, { "patchline": { "source": ["module", 1], "destination": ["dac", 1] } }, { "patchline": { "source": ["module", 2], "destination": ["dac", 2] } }, { "patchline": { "source": ["module", 3], "destination": ["dac", 3] } }, { "patchline": { "source": ["module", 4], "destination": ["dac", 4] } }, { "patchline": { "source": ["module", 5], "destination": ["dac", 5] } }
    ]
  }
}
