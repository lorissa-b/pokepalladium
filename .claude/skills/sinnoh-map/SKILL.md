---
name: sinnoh-map
description: Build or redraw a Sinnoh map (town, city, route, interior) as a replica on top of a Hoenn map, or fix an existing replica's layout, warps or connections. Use for any request to turn a Hoenn map into a Sinnoh one, draw or adjust a map.bin, place buildings, or check a map against Diamond/Pearl/Platinum.
---

# Building a Sinnoh map

Use `tools/mapkit/mapkit.py` rather than one-off scripts. It reads
Platinum's real map data, so tile positions, exits, warps and NPC spots can
be copied exactly instead of guessed from screenshots. It needs Pillow
(`pip install -r tools/mapkit/requirements.txt`). For a command's options,
run `mapkit.py <command> --help`.

Read a reference file below only when you reach the step that needs it:

| When | Read |
| --- | --- |
| Swapping or resizing a building, finding metatiles | `references/buildings.md` |
| Editing a blueprint by hand, or a layout's size | `references/blueprints.md` |
| Moving events, connecting maps, fixing `check` findings | `references/events-and-connections.md` |
| Anything else a command does | the matching section of `docs/map/tooling.md` (`grep -n '^#'` lists them) |

## Workflow

1. **Reference.** `mapkit.py platinum list <words>` finds the header name.
   Look at the map as a picture:
   `platinum show <HEADER> --render build/mapkit/ref.png`, then Read the PNG.
   Print it as text (`--events`, or `--ground` for painted paths) only with
   `--region X,Y,W,H` around the part you're working on.
2. **Target.** `mapkit.py info <Map>` and
   `render <Map> --grid --events -o build/mapkit/target.png` for the Hoenn
   map being converted. Note its neighbours and their tilesets.
3. **Draft.** `mapkit.py draft <HEADER> <LAYOUT> --lean -o build/mapkit/<map>.bp`
   gives a blockout with exits, roads, grass, water, ledges and a whole
   Emerald building on every Platinum building. Platinum's events, props and
   the building list go to `build/mapkit/<map>.notes`; `grep` it rather than
   reading it. Keep Platinum's proportions; crop with `--region` only when
   the map must be smaller. For a cave, add
   `--solid-unreachable --finish --style cave --water any` and crop the void
   with `--region`.
4. **Detail.** Fix anything the draft's header lists as unmatched, and swap
   buildings you'd rather have another way (see `references/buildings.md`).
5. **Preview, then build.** `build <file> --dry-run --render build/mapkit/preview.png`,
   look at the PNG, then `build <file>`. Both say how many blocks change and
   where; if that's not what you meant, look before writing.
6. **Score.** `compare <Map> <HEADER> --origin auto -q --render build/mapkit/cmp.png`
   and look at the PNG. Fix red areas that aren't deliberate and add any
   Platinum warps it lists as missing. Drop `-q` only to get the mismatch
   list, and pass the origin it found as `--origin X,Y` on later runs.
7. **Events.** Move warps, NPCs, signs and triggers to Platinum's positions
   (see `references/events-and-connections.md`).
8. **Check.** `mapkit.py check <Map> <each neighbour> -w` must show no errors
   for the maps you touched.

## Rules

- Where the player can go must stay as in Platinum: what's blocked by Surf,
  Cut, Rock Climb or a ledge stays blocked. A finished draft's header says
  "Access is the same as Platinum's" or lists what differs; fix anything it
  lists before building, and keep it that way when editing by hand.
- Keep output small. mapkit's text grids cost two punctuation characters per
  tile, and every copy stays in the conversation:
  - look at whole maps as PNGs, and print text grids (`dump`, `extract`,
    `platinum show`) only with `--region`;
  - always filter `tileset --list` (`--behavior` or `--material`);
  - never Read a whole blueprint; read the rows you're changing;
  - don't re-run a command whose output you already have unless something
    changed;
  - for a large map, commit once the layout is done (steps 1-6) and do the
    events in a fresh session.
- `build/` is gitignored: blueprints, notes and PNGs there are scratch, and
  the committed result is the map.bin.
- Render and look at every layout change before committing; describe what
  was matched to Platinum in the commit message.
