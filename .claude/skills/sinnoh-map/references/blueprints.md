# Editing blueprints

- Prefer adding lines to the end of the blueprint over editing its grid.
  Commands run in order and later ones draw over earlier ones, so a change
  needs no reading at all:
  - `rect X Y W H BLOCK` fills an area (e.g. clear a stray tree patch);
  - `set X Y BLOCK` sets one block;
  - `stamp LAYOUT X Y W H at DX DY` copies part of another map;
  - `buildings <LAYOUT> --piece <NAME> --at X,Y` prints a building's legend
    and grid; append the output as it is.
  Blocks are written like `0x1D4`, `0x1D4/c1/e0`, a label such as
  `General_Grass`, or a 2x2 group `[0x1D4 0x1D5; 0x1DC 0x1DD]`.
- When the grid itself has to change, Read only those rows (`offset` and
  `limit`) and Edit them.
- Either way, check with `build --dry-run --render` and the PNG rather than
  by re-reading the file.
- The grid format isn't vanilla: 12-bit metatile ids, secondary ids start at
  0x800, collision is 1 bit and elevation 3 bits (0-7). Normal ground is
  elevation 3, water 1, trees and walls collision 1 at elevation 0. Doors
  carry collision 1. Leave out `/c` and `/e` in legend lines and these
  defaults are applied from existing usage.
- Layout size is limited by MAX_MAP_DATA_SIZE: (width+15) x (height+14) must
  stay at or under 10240 blocks. `build` resizes `layouts.json` when the
  blueprint's `size` changes.

The directives (`layout`, `size`, `base`, `fill`, `rect`, `stamp`, `set`,
`legend`, `grid`) are described in the "Blueprints" section of
`docs/map/tooling.md`.
