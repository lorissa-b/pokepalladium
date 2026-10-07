---
name: sinnoh-map
description: Build or redraw a Sinnoh map (town, city, route, interior) as a replica on top of a Hoenn map, or fix an existing replica's layout, warps or connections. Use for any request to turn a Hoenn map into a Sinnoh one, draw or adjust a map.bin, place buildings, or check a map against Diamond/Pearl/Platinum.
---

# Building a Sinnoh map

Use `tools/mapkit/mapkit.py` rather than one-off scripts. It reads Platinum's
real map data, so tile positions, exits, warps and NPC spots can be copied
exactly instead of guessed from screenshots. It needs Pillow
(`pip install -r tools/mapkit/requirements.txt`).

This file covers everything the workflow needs. Don't read
`docs/map/tooling.md` (it's long and explains how the tools work inside);
for one command's options run `mapkit.py <command> -h`.

## Keep output small

Everything printed or opened stays in context for the rest of the session,
so:

- Print a grid once. `platinum show` prints the whole map as a grid each
  time; after the first look, add `--region X,Y,W,H` to see only the part
  being worked on. `--events` adds the event list to the same output, so one
  `show --events` is enough. Use `--ground` only when paths matter, and with
  `--region`.
- `tileset --list` needs `--behavior NAME` (e.g. `DOOR`, `TALL_GRASS`).
  `--all` prints every metatile, hundreds of lines: don't.
- `compare` with `-q` prints only the score and missing warps; look at its
  `--render` PNG for where the differences are, not at the text grid.
- Prefer PNGs for looking at a whole map; read text grids only for exact
  coordinates, and crop them with `--region`.
- Don't re-read a blueprint after building it; `build` reports problems.
- One map per session where possible: earlier maps' output is dead weight.

## Workflow

1. **Reference.** `platinum list <words>` finds the header name. Then
   `platinum show <HEADER> --events` (grid, warps, NPCs, signs, triggers and
   buildings in local coordinates) and `platinum show <HEADER> --render
   build/mapkit/ref.png` (open with Read). `--ground` marks where Platinum's
   ground is painted as a path (`p`); drafts draw those as Emerald paths.
   Two maps side by side in one matrix can be joined wherever a header is
   taken: `ROUTE_201+VERITY_LAKEFRONT`.
2. **Target.** `mapkit.py info <Map>` (layout, size, tilesets, connections,
   events) and `render <Map> --grid --events`. Note which neighbours it
   connects to and which tilesets they use.
3. **Draft.** `mapkit.py draft <HEADER> <LAYOUT> -o build/mapkit/<map>.bp`
   gives a blockout with exits, roads, grass, water, ledges and buildings in
   the right places and the Platinum events/props listed as comments. Keep
   the Platinum proportions; crop with `--region X,Y,W,H` only when the map
   must be smaller. `--finish` picks a real metatile for every tile, learned
   from the original Emerald maps (`--style route|town|cave`,
   `--trees dense|round|jungle|pine|any`, `--water sea|pond|any`,
   `--path sandy|stone|any`, `--keep-shape` to stop it moving tree-line
   borders by a block); `--render PNG --seams` outlines the spots to touch up.
   For a cave, add `--solid-unreachable --finish --style cave --water any`,
   and crop the void around it with `--region`. A cave exit is the warp on a
   south-arrow floor tile (`0x807` in the cave set) with the mouth's light
   edge (`0x858 0x859 0x85A`) below it.
4. **Detail.** The draft already places a whole Emerald building on every
   Platinum building (town or route), door on Platinum's door. Its notes list
   each placement and anything unmatched (`no Emerald building fits`, `no
   door for Platinum's door`). Buildings whose name has `~WxH` were made
   from parts to Platinum's exact size; `--originals-only` keeps to the
   originals' own buildings. Swap a building using
   `buildings <LAYOUT> --render build/mapkit/pieces.png` and
   `buildings <LAYOUT> --piece <NAME> --at X,Y` (add `--size WxH --doors
   1,5` to make it another size), or `stamp`/`extract` from
   another map. Find metatiles with `tileset <Map>` (PNG) and
   `tileset <Map> --list --behavior <NAME>`. Keep each building's door on the
   tile where Platinum has its door warp.
5. **Preview, then build.** Run `build <file> --dry-run --render build/mapkit/preview.png`
   and look at the PNG. Then run `build <file>`, which writes map.bin,
   resizes layouts.json when the size changes, and runs `check`.
6. **Score.** Run `compare <Map> <HEADER> --origin auto -q --render build/mapkit/cmp.png`.
   The PNG shows the map with mismatches tinted red, and Platinum's map
   below it (beside it for tall maps) at the same scale. Fix red areas that
   aren't deliberate, and add any Platinum warps it lists as missing.
   Matching is by movement group (walk, blocked, water, ledge, climb), so
   sand against dirt still matches.
7. **Events.** Move warps, NPCs, signs and triggers in map.json to the
   Platinum positions (shifted by the compare origin). Interior maps' exit
   warps must point back at the right warp ids.
8. **Check.** `mapkit.py check <Map> <each neighbour> -w` must show no
   errors for the maps you touched. Pay attention to "edge looks wrong":
   the strip of a neighbour that's on screen is drawn with the current map's
   tilesets, so near shared edges use primary-tileset metatiles (ids below
   0x800) unless both maps share a secondary tileset.

## Blueprints

A blueprint is a text file of commands applied in order; `#` starts a
comment.

```text
layout LAYOUT_JUBILIFE_CITY      # the layout to build
size 64 48                       # optional; build resizes layouts.json to match
base none                        # self (current map.bin, the default), none, or another layout
define tree = [0x1D4 0x1D5; 0x1DC 0x1DD]   # pattern, picked by map coordinates
fill tree
rect 0 40 64 8 General_Grass     # x y w h block
stamp LAYOUT_PETALBURG_CITY 14 3 6 5 at 20 30
set 12 7 General_Door
legend T = tree
grid 0 0                         # rows of legend keys until 'end'
TTTT....TTTT
end
```

Blocks are a metatile id (`0x1CE`) or label (`General_Grass`). A space in a
grid row keeps the block already there. `grid X Y w2` reads two characters
per key. Stamps need the same primary tileset.

## Rules that keep biting

- Where the player can go must stay as in Platinum: what's blocked by Surf,
  Cut, Rock Climb or a ledge stays blocked. A finished draft's header says
  "Access is the same as Platinum's" or lists what differs; fix anything it
  lists before building, and keep it that way when editing by hand.
- Connection offsets: a connection on map A with offset `o` puts the
  neighbour's x (or y) 0 at A's x (or y) = `o`. The way back uses `-o`.
  `check` verifies both sides and that the maps actually touch.
- The grid format isn't vanilla: 12-bit metatile ids, secondary ids start
  at 0x800, collision is 1 bit and elevation 3 bits (0-7). Normal ground is
  elevation 3, water 1, trees and walls collision 1 at elevation 0. Doors
  carry collision 1. Leave out `/c` and `/e` in blueprints and these
  defaults are applied from existing usage.
- Layout size is limited by MAX_MAP_DATA_SIZE: (width+15) x (height+14)
  must stay at or under 10240 blocks.
- `build/` is gitignored: blueprints and PNGs there are scratch, and the
  committed result is the map.bin.
- Render and look at every layout change before committing; describe what
  was matched to Platinum in the commit message.
