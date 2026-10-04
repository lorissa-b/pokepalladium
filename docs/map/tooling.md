# Map tooling

`tools/mapkit/mapkit.py` is a command-line kit for building Sinnoh replicas
on top of the Hoenn maps. It reads and draws the game's layouts, pulls the
original maps out of [pret/pokeplatinum](https://github.com/pret/pokeplatinum)
as a reference, drafts and builds layouts from a plain-text blueprint, scores
a replica against the original, and checks maps for the mistakes that are easy
to make by hand.

It needs Python 3 and [Pillow](https://python-pillow.org/), and runs from the
repo root:

```sh
pip install -r tools/mapkit/requirements.txt
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

   Add `--finish` to choose every block instead (see [Finished drafts](#finished-drafts)), and
   `--render out.png --events` to preview it with Platinum's events marked.

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
   differences red on the Emerald map, outlines Platinum's events and props,
   and puts the Platinum map, cropped to the same area, alongside.

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
| `tileset MAP` or `tileset PRIMARY SECONDARY` | Metatile catalog PNG, or `--list` as text. `--only primary\|secondary`, `--behavior NAME`, `--materials`. |
| `materials status` | How many metatiles of each tileset are labelled. |
| `materials suggest TILESET...` | Draft labels and review sheets for tilesets. `--guesses-only`. |
| `extract MAP` | A region as a blueprint that rebuilds it exactly. `--standalone` adds the header so it builds on its own. |
| `build FILE` | Build a blueprint into its layout's `map.bin`, then check it. `--dry-run`, `--render PNG`. |
| `check [MAP...]` | Check layouts and maps (all of them by default). `-w` adds warnings. Exits non-zero on errors. |
| `platinum list [QUERY]` | Search Platinum's map headers. |
| `platinum show HEADER` | The Platinum map as text, with events and props. `--events`, `--region`, `--render [PNG]`, `--json FILE`. |
| `platinum update` | Move the cached pokeplatinum checkout to its latest commit. |
| `draft HEADER LAYOUT` | A blueprint for LAYOUT drafted from a Platinum map. `--region X,Y,W,H`, `--finish [--style --trees --water]`, `--render PNG [--events --grid --seams]`. |
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

## Finished drafts

`draft --finish` picks a real Emerald metatile for every tile, by example
(`autotile.py`):

- It learns from the **original Emerald layouts**, read from this repo's
  history at commit `73761a50` (before any map was redrawn), and only from
  blocks the player can see: within the 15x10 screen of some passable block.
- **Pairings:** how often each metatile sits beside each other one, side by
  side and diagonally. Pairings never seen in an original map cost a lot.
- **Materials and families:** each metatile is labelled with what it shows
  (see below). The `route` style builds blocked tiles from trees and walkable
  tiles from grass, with tall grass, water, ledges and bridges where Platinum
  has them, and falls back on cliff, rock or sand only where nothing else
  fits. One tree family and one water family are kept for the whole map:
  `--trees dense|round|jungle|pine|any` (default `dense`, the Sinnoh-like
  canopy) and `--water sea|pond|any` (default `sea`). `--style town` also
  allows paths, fences, buildings and objects.
- **Surroundings:** among what fits, metatiles the originals use in the same
  class surroundings cost less.

It then minimises the total cost: a greedy first version, sweeps that swap
each tile for its cheapest option given all its neighbours, and moves that
replace whole 2x2 patches (taken from the original maps) around every
remaining problem, since a tree or a shoreline can't move one tile at a time.
Where Platinum's shape can't be built from Emerald's pieces, an area settles
on the least-bad combination.

The blueprint header counts the off-style tiles and the **seams** (tiles next
to one they never sit beside in the originals); `--render out.png --seams`
outlines them. Those are the spots to touch up. A finished draft is a strong
starting point, not a final map.

### Materials

`tools/mapkit/materials/<Tileset>.txt` lists what each metatile depicts, for
all 74 tilesets (every metatile is labelled):

- outdoors: grass, path, sand, tallgrass, flowers, tree, cliff, rock, water,
  ledge, bridge, fence, building, object, cave, dark;
- caves and interiors: floor, wall, stairs, ice, lava (and object for
  furniture);
- families, which split trees (dense, round, jungle, pine) and water (sea,
  pond) by look, so a map can keep to one of each.

Each file is one id or id range per line with its materials; a `+` line adds
to what's already there, and a later line replaces an earlier one, so
corrections can go at the end. Behaviours add water, tall grass, sand,
ledges, doors and bridges on top. `tileset MAP --materials` draws a catalog
with each tile's labels under it.

**Labelling a new or changed tileset.** `materials suggest TILESET` writes a
draft file and review sheets to `build/mapkit/materials/suggest/`. Each tile
is labelled from, in order of trust: an exact copy of a labelled tile, a
recoloured copy (the same pixel pattern in other colours), its behaviour, or a
guess from the closest-looking labelled tile. Sheets colour each label by its
source (white, cyan, green, yellow); `--guesses-only` shows only the guesses,
which are what need checking. Copy the draft to `tools/mapkit/materials/`,
fix what's wrong, and `materials status` shows coverage.

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
| `render.py` | Map, overlay, catalog and side-by-side drawing (Pillow). |
| `blueprint.py` | The blueprint format: parse, build, extract. |
| `check.py` | The checks. |
| `platinum.py` | The pokeplatinum reader. |
| `compare.py` | Tile classes for Emerald, `compare`, `draft`. |
| `autotile.py` | `draft --finish`: learning from the original maps, and the cost-minimising fill. |
| `original.py` | The original Emerald layouts from history, and which blocks are visible. |
| `materials.py`, `materials/` | What each metatile depicts, for every tileset. |
| `label_assist.py` | `materials suggest`: labels from copies, behaviours and look-alikes. |
| `test_mapkit.py` | Tests: `python3 -m unittest discover tools/mapkit`. |

The map grid and metatile formats are read from `include/fieldmap.h` and
`include/global.fieldmap.h`, not hard-coded: this repo uses 12-bit metatile
ids, 1-bit collision, 3-bit elevation and triple-layer metatiles.
