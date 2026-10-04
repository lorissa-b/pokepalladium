---
name: sinnoh-map
description: Build or redraw a Sinnoh map (town, city, route, interior) as a replica on top of a Hoenn map, or fix an existing replica's layout, warps or connections. Use for any request to turn a Hoenn map into a Sinnoh one, draw or adjust a map.bin, place buildings, or check a map against Diamond/Pearl/Platinum.
---

# Building a Sinnoh map

Use `tools/mapkit/mapkit.py` (see `docs/map/tooling.md` for every option)
rather than one-off scripts. It reads Platinum's real map data, so tile
positions, exits, warps and NPC spots can be copied exactly instead of
guessed from screenshots. It needs Pillow
(`pip install -r tools/mapkit/requirements.txt`).

## Workflow

1. **Reference.** Read the original map:
   `tools/mapkit/mapkit.py platinum show <HEADER> --events` and
   `platinum show <HEADER> --render build/mapkit/ref.png` (open the PNG with
   Read). Use `platinum list <words>` to find the header name.
2. **Target.** Read the Hoenn map being converted:
   `mapkit.py info <Map>` and `render <Map> --grid --events`. Note which
   neighbours it connects to and which tilesets they use.
3. **Draft.** `mapkit.py draft <HEADER> <LAYOUT> -o build/mapkit/<map>.bp`
   gives a blockout with exits, roads, grass, water and ledges in the right
   places and the Platinum events/props listed as comments. Keep the Platinum
   proportions; crop with `--region` only when the map must be smaller.
4. **Detail.** Replace placeholders with real Emerald buildings:
   `stamp` from a map on the same tilesets, or paste `extract --region`
   output. Find metatiles with `tileset <Map>` (PNG) and
   `tileset <Map> --list --behavior <NAME>`. Keep each building's door on the
   tile where Platinum has its door warp.
5. **Preview, then build.** Run `build <file> --dry-run --render build/mapkit/preview.png`
   and look at the PNG. Then run `build <file>`, which writes map.bin and
   resizes layouts.json when the size changes.
6. **Score.** Run `compare <Map> <HEADER> --origin auto --render build/mapkit/cmp.png`.
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

## Rules that keep biting

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
