# Editing blueprints

- Read only the rows you're changing (Read with `offset` and `limit`), Edit
  them, then build with `--dry-run --render` and look at the PNG instead of
  re-reading the file.
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
