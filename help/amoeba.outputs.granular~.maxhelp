{
  "patcher": {
    "fileversion": 1, "appversion": { "major": 9, "minor": 0, "revision": 0, "architecture": "x64", "modernui": 1 },
    "classnamespace": "box", "rect": [30.0, 30.0, 1540.0, 900.0], "default_fontsize": 12.0, "default_fontname": "Arial",
    "boxes": [
      { "box": { "id": "title", "maxclass": "comment", "text": "Amoeba Outputs Granular — multihead temporal spatial cloud", "fontsize": 18.0, "patching_rect": [30.0, 15.0, 760.0, 26.0] } },
      { "box": { "id": "source", "maxclass": "newobj", "text": "cycle~ 130.81", "patching_rect": [30.0, 52.0, 88.0, 22.0] } },
      { "box": { "id": "event", "maxclass": "message", "text": "65 120 1100 3", "patching_rect": [136.0, 52.0, 108.0, 22.0] } },
      { "box": { "id": "module", "maxclass": "newobj", "text": "amoeba.outputs.granular~ @outputs 6", "patching_rect": [30.0, 90.0, 1040.0, 720.0] } },
      { "box": { "id": "dac", "maxclass": "newobj", "text": "dac~ 1 2 3 4 5 6", "patching_rect": [30.0, 840.0, 118.0, 22.0] } },
      { "box": { "id": "dome", "maxclass": "message", "text": "layout dome", "patching_rect": [1100.0, 105.0, 82.0, 22.0] } },
      { "box": { "id": "freeze", "maxclass": "message", "text": "freeze 1", "patching_rect": [1195.0, 105.0, 62.0, 22.0] } },
      { "box": { "id": "speaker", "maxclass": "message", "text": "speaker 2 30. 1. 55.", "patching_rect": [1100.0, 145.0, 138.0, 22.0] } },
      { "box": { "id": "character", "maxclass": "message", "text": "character shards", "patching_rect": [1100.0, 185.0, 112.0, 22.0] } },
      { "box": { "id": "param", "maxclass": "message", "text": "set color 0.86", "patching_rect": [1100.0, 225.0, 104.0, 22.0] } },
      { "box": { "id": "info", "maxclass": "comment", "text": "Ten independent playback heads read a four-second internal history and occupy separate moving positions. COLOR controls history depth, TURBULENCE jitters read locations, and FREEZE preserves both the buffer and cloud geometry.", "linecount": 7, "patching_rect": [1100.0, 285.0, 330.0, 120.0] } }
    ],
    "lines": [
      { "patchline": { "source": ["source", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["event", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["dome", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["freeze", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["speaker", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["character", 0], "destination": ["module", 0] } }, { "patchline": { "source": ["param", 0], "destination": ["module", 0] } },
      { "patchline": { "source": ["module", 0], "destination": ["dac", 0] } }, { "patchline": { "source": ["module", 1], "destination": ["dac", 1] } }, { "patchline": { "source": ["module", 2], "destination": ["dac", 2] } }, { "patchline": { "source": ["module", 3], "destination": ["dac", 3] } }, { "patchline": { "source": ["module", 4], "destination": ["dac", 4] } }, { "patchline": { "source": ["module", 5], "destination": ["dac", 5] } }
    ]
  }
}
