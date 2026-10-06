---
name: sinnoh-map
description: Build or redraw a Sinnoh map (town, city, route, interior) as a replica on top of a Hoenn map, or fix an existing replica's layout, warps or connections. Use for any request to turn a Hoenn map into a Sinnoh one, draw or adjust a map.bin, place buildings, or check a map against Diamond/Pearl/Platinum.
---

# Building a Sinnoh map

Use `tools/mapkit/mapkit.py` rather than one-off scripts. For options, run
`mapkit.py <command> --help`, or read only the section of
`docs/map/tooling.md` you need (`grep -n '^#' docs/map/tooling.md` lists
them); don't read the whole file. It reads Platinum's real map data, so tile
positions, exits, warps and NPC spots can be copied exactly instead of
guessed from screenshots. It needs Pillow
(`pip install -r tools/mapkit/requirements.txt`).

## Workflow

1. **Reference.** Look at the original map first as a picture:
   `tools/mapkit/mapkit.py platinum show <HEADER> --render build/mapkit/ref.png`
   (open the PNG with Read). Print the text grid with `--events` only for
   the part you're working on (`--region X,Y,W,H`). Use
   `platinum list <words>` to find the header name.
   `platinum show <HEADER> --ground` shows where Platinum's ground is painted
   as paths, flowers and so on; drafts draw its paths (`p`) as Emerald paths.
2. **Target.** Read the Hoenn map being converted:
   `mapkit.py info <Map>` and `render <Map> --grid --events` (the PNG, not
   `dump`). Note which neighbours it connects to and which tilesets they use.
3. **Draft.** `mapkit.py draft <HEADER> <LAYOUT> -o build/mapkit/<map>.bp`
   gives a blockout with exits, roads, grass, water, ledges and buildings in
   the right places and the Platinum events/props listed as comments. Keep the Platinum
   proportions; crop with `--region` only when the map must be smaller.
   The draft file is large (around 40 KB for a 64x64 map), mostly comments.
   Don't Read it whole. Read its header (`head -40`), `grep` the notes you
   need, and view the grid a block of rows at a time with `sed -n`.
   For a cave, add `--solid-unreachable --finish --style cave --water any`,
   and crop the void around it with `--region`.
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
   `tileset <Map> --list --behavior <NAME>` (always filter `--list`:
   unfiltered, it prints every metatile, about 15k tokens). Keep each
   building's door on the tile where Platinum has its door warp.
5. **Preview, then build.** Run `build <file> --dry-run --render build/mapkit/preview.png`
   and look at the PNG. Then run `build <file>`, which writes map.bin and
   resizes layouts.json when the size changes.
6. **Score.** Run `compare <Map> <HEADER> --origin auto -q --render build/mapkit/cmp.png`
   and look at the PNG. Drop `-q` only when you need the mismatch list, and
   pass a fixed `--origin X,Y` after the first run.
   Fix red areas that aren't deliberate, and add any Platinum warps it lists
   as missing.
7. **Events.** Move warps, NPCs, signs and triggers in map.json to the
   Platinum positions (shifted by the compare origin). Interior maps' exit
   warps must point back at the right warp ids.
8. **Check.** `mapkit.py check <Map> <each neighbour> -w` must show no
   errors for the maps you touched. Pay attention to "edge looks wrong":
   the strip of a neighbour that's on screen is drawn with the current map's
   tilesets, so near shared edges use primary-tileset metatiles (ids below
   0x800) unless both maps share a secondary tileset.

## Keeping output small

mapkit's text grids cost a lot of tokens: two punctuation characters per
tile, and every copy stays in the conversation for the rest of the session.

- Prefer PNGs to text grids for looking at a whole map. Use text grids
  (`dump`, `extract`, `platinum show`) with `--region` around the area being
  edited.
- To change a blueprint, Read only the rows being changed (`offset` and
  `limit`), Edit them, then rebuild. Check the edit with a render rather
  than by re-reading the file.
- Don't re-run `info`, `platinum show` or `buildings --render` for something
  already read in this session unless it changed.
- Pipe long listings through `head` or `grep` rather than printing them
  whole.
- For a large map, finish the layout (steps 1-6) and commit before moving
  events, so the events work can start in a fresh session.

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
