# Map tooling

`tools/mapkit/mapkit.py` is a command-line kit for building Sinnoh replicas
on top of the Hoenn maps. It reads and draws the game's layouts, pulls the
original maps out of [pret/pokeplatinum](https://github.com/pret/pokeplatinum)
as a reference, drafts and builds layouts from a plain-text blueprint, scores
a replica against the original, and checks maps for the mistakes that are easy
to make by hand.

It needs only Python 3 (no packages to install) and runs from the repo root:

```sh
tools/mapkit/mapkit.py <command> --help
```

Maps can be named by `MAP_*` id, by directory (`JubilifeCity`) or loosely
(`jubilife city`); layouts by `LAYOUT_*` id. PNGs go to `build/mapkit/` unless
`-o` says otherwise.

## Building a replica

1. **Look at the original.** `platinum show` prints the Platinum map as a
   grid of tile classes, with its warps, NPCs, signs, triggers and buildings
   in local coordinates; `--render` draws it.

   ```sh
   tools/mapkit/mapkit.py platinum list route 20
   tools/mapkit/mapkit.py platinum show JUBILIFE_CITY --events
   tools/mapkit/mapkit.py platinum show JUBILIFE_CITY --render
   ```

2. **Draft a blueprint** from it. Walkable tiles become grass, blocked tiles
   become trees, tall grass, water and ledges become their Emerald
   equivalents, and doors get a placeholder. Platinum's events and props
   (buildings) are listed at the bottom as comments, in the draft's
   coordinates. `--region X,Y,W,H` takes part of the Platinum map, for a
   replica smaller than the original.

   ```sh
   tools/mapkit/mapkit.py draft JUBILIFE_CITY LAYOUT_JUBILIFE_CITY -o jubilife.bp
   ```

3. **Find the pieces.** `tileset` draws every metatile the layout can use,
   labelled with its id; `--list` prints ids with their behaviour, label and
   usual collision/elevation. `extract` copies a region of any existing map as
   a blueprint, ready to paste in, and `render --grid` shows where things are.

   ```sh
   tools/mapkit/mapkit.py tileset JubilifeCity
   tools/mapkit/mapkit.py tileset JubilifeCity --list --behavior DOOR
   tools/mapkit/mapkit.py render PetalburgCity --grid --events
   tools/mapkit/mapkit.py extract PetalburgCity --region 14,3,6,5
   ```

4. **Edit and build.** Replace placeholders with real buildings (`stamp`
   them from another map, or paste an `extract`), then build. `build` writes
   the layout's `map.bin`, resizes it in `layouts.json` if the blueprint says
   so, and runs `check` on the result. `--dry-run --render out.png` previews
   without writing.

   ```sh
   tools/mapkit/mapkit.py build jubilife.bp --dry-run --render build/mapkit/preview.png
   tools/mapkit/mapkit.py build jubilife.bp
   ```

5. **Score it.** `compare` lines the replica up with the original
   (`--origin auto` finds the best offset), marks every tile where movement
   differs, and lists Platinum warps with no warp nearby. `--render` tints the
   differences red on the Emerald map and outlines Platinum's events and
   props.

   ```sh
   tools/mapkit/mapkit.py compare JubilifeCity JUBILIFE_CITY --origin auto --render build/mapkit/jubilife_vs.png
   ```

6. **Move the events** in `map.json` (Porymap is still the easiest way), then
   `check` the map and its neighbours.

## Commands

| Command | What it does |
| --- | --- |
| `info MAP` | Layout, size, tilesets, connections, and every event with the behaviour of the tile under each warp. |
| `dump MAP` | The layout as text. `--layer metatile\|collision\|elevation\|behavior\|block`, or `--classes` for the same tile classes the Platinum commands use. `--region X,Y,W,H`. |
| `render MAP` | PNG of the map. `--grid` (coordinates every 4 tiles), `--events`, `--collision`, `--elevation`, `--region`, `--scale`. |
| `tileset MAP` or `tileset PRIMARY SECONDARY` | Metatile catalog PNG, or `--list` as text. `--only primary\|secondary`, `--behavior NAME`. |
| `extract MAP` | A region as a blueprint that rebuilds it exactly. `--standalone` adds the header so it builds on its own. |
| `build FILE` | Build a blueprint into its layout's `map.bin`, then check it. `--dry-run`, `--render PNG`. |
| `check [MAP...]` | Check layouts and maps (all of them by default). `-w` adds warnings. Exits non-zero on errors. |
| `platinum list [QUERY]` | Search Platinum's map headers. |
| `platinum show HEADER` | The Platinum map as text, with events and props. `--events`, `--region`, `--render [PNG]`, `--json FILE`. |
| `platinum update` | Move the cached pokeplatinum checkout to its latest commit. |
| `draft HEADER LAYOUT` | A blueprint for LAYOUT drafted from a Platinum map. `--region X,Y,W,H`. |
| `compare MAP HEADER` | Score a map against the Platinum original. `--origin X,Y\|auto`, `--render PNG`, `-q`. |

### Tile classes

The Platinum commands, `dump --classes`, `draft` and `compare` reduce both
games' tiles to one character each:

| | | | | | |
| --- | --- | --- | --- | --- | --- |
| `#` blocked | `.` walkable | `"` tall grass | `Y` very tall grass | `~` surfable water | `\|` waterfall |
| `,` puddle / shallow water | `:` sand | `i` ice | `m` mud | `s` snow | `^ v < >` ledge, by jump direction |
| `D` door | `E` warp / entrance | `S` stairs / escalator | `R` rock climb | `=` bridge | `B` berry patch |
| `t` table / counter | `o` furniture | ` ` outside the map | | | |

`compare` counts two tiles as matching when they're in the same movement group
(walk, blocked, water, ledge, climb), so a Platinum sand path against an
Emerald dirt path still matches.

## Blueprints

A blueprint is a text file of commands, applied in order:

```text
# Comments start with '#' on command lines.
layout LAYOUT_JUBILIFE_CITY      # the layout to build: tilesets and map.bin path
size 64 48                       # optional; build resizes layouts.json to match
base none                        # start from: self (the current map.bin), none, or another layout
define tree = [0x1D4 0x1D5; 0x1DC 0x1DD]
fill tree                        # fill everything
rect 0 40 64 8 General_Grass     # fill a rectangle
stamp LAYOUT_PETALBURG_CITY 14 3 6 5 at 20 30   # copy blocks from another layout
set 12 7 General_Door            # one block
legend T = tree                  # a grid key
legend . = 0x001
grid 0 0                         # rows of keys until 'end'
TTTT....TTTT
TT........TT
end
```

Without a `base` line, drawing starts from the layout's current `map.bin`, so
a short blueprint can patch an existing map.

**Blocks.** A metatile by id (`0x1CE`, `462`) or by its label from
`include/constants/metatile_labels.h` (`General_Grass`, with or without the
`METATILE_` prefix). Add `/cN` and `/eN` to set collision and elevation:
`0x00E/c1/e3`. Without them, a metatile gets the collision and elevation it
most often has in the existing layouts, so trees come out impassable, doors
get their collision bit and water sits at elevation 1. `tileset --list` shows
what that will be for each metatile.

**Patterns.** `[A B; C D]` repeats a block of metatiles, picked by the map
coordinates, so 2x2 trees line up however a rectangle or grid is placed.

**Grids.** Each character is a legend key; a space keeps the block that's
already there, and so does the rest of a short row. `grid X Y w2` reads two
characters per key, which `extract` uses for regions with more distinct blocks
than there are single characters. `#` works as a key.

**Stamps** need the same primary tileset; stamping secondary metatiles from a
layout with a different secondary tileset prints a warning, because those ids
are different tiles.

## Checks

`check` reports:

- `map.bin` and `border.bin` sizes that don't match `layouts.json`, layouts
  too big for the game's map buffer (`MAX_MAP_DATA_SIZE`), metatile ids the
  tilesets don't have, and tilesets that don't exist;
- events outside the map;
- warps to maps or warp ids that don't exist, and (as warnings) warps whose
  destination doesn't lead back, or that sit on a tile the player can't warp
  from by walking onto it;
- connections to missing maps, connections without a matching connection
  back (opposite direction, negated offset) and connections that don't touch
  the map;
- neighbours whose edge looks wrong from this map. The game draws the strip of
  a connected map that's on screen with the *current* map's tilesets, so a
  neighbour's metatiles near the edge must look the same with both tileset
  pairs. The check compares the drawn pixels of every neighbour block that's
  on screen (15x10 blocks around the player) from a passable block near the
  edge.

## The Platinum reference

Platinum's overworld is a matrix of 32x32-tile chunks. Each chunk's land data
stores a `u16` per tile (bit 15 collision, low byte the tile behaviour) and
lists its 3D props (buildings, doors, signs) by model id and position. A map
header names its matrix and its events file, whose coordinates are
matrix-wide. `platinum.py` collects the chunks that belong to a header into
one local grid and moves the events and props into the same coordinates.

The first Platinum command makes a blobless clone of pokeplatinum in
`~/.cache/pokepalladium/pokeplatinum` and then downloads only the files each
map needs. Set `POKEPLATINUM_DIR` to use an existing checkout instead.

Most prop models are only numbered (`prop_model_011`), but their positions
show where buildings stand: the doors' props sit on the door warps, and a
building's footprint shows as a block of `#` behind them.

## Code

| File | Contents |
| --- | --- |
| `project.py` | Grid and tileset constants read from the headers, layouts, maps, metatile labels, `Blockdata` (read/write `map.bin`). |
| `tileset.py` | Tilesets traced from their symbols to their files, metatile drawing (three layers). |
| `png.py` | Standard-library PNG reading and writing, and a 3x5 label font. |
| `render.py` | Map, overlay and catalog drawing. |
| `blueprint.py` | The blueprint format: parse, build, extract. |
| `check.py` | The checks. |
| `platinum.py` | The pokeplatinum reader. |
| `compare.py` | Tile classes for Emerald, `compare`, `draft`. |
| `test_mapkit.py` | Tests: `python3 -m unittest discover tools/mapkit`. |

The map grid and metatile formats are read from `include/fieldmap.h` and
`include/global.fieldmap.h`, not hard-coded: this repo uses 12-bit metatile
ids, 1-bit collision, 3-bit elevation and triple-layer metatiles.
