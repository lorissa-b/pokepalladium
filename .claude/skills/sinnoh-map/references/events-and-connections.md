# Events and connections

## Events

- Move warps, NPCs, signs and triggers in `map.json` to Platinum's positions
  from the draft's `.notes` file, shifted by the origin `compare` found.
  `grep warp`, `grep object` and so on rather than reading the whole file.
- Interior maps' exit warps must point back at the right warp ids.
- `info <Map> --all` lists the map's current events. The Hoenn ones can be
  deleted or reused freely.

## Connections

- A connection on map A with offset `o` puts the neighbour's x (or y) 0 at
  A's x (or y) = `o`. The way back uses `-o`. `check` verifies both sides and
  that the maps actually touch.
- "edge looks wrong" from `check`: the strip of a neighbour that's on screen
  is drawn with the current map's tilesets, so near shared edges use
  primary-tileset metatiles (ids below 0x800) unless both maps share a
  secondary tileset.

More: the "Checks" section of `docs/map/tooling.md`.
