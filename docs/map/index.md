# Map

The world of Palladium: routes, towns, dungeons, connections and wild
encounters.

```{note}
Scaffold page. Region and route write-ups still to be written.
```

```{toctree}
:maxdepth: 1

towns/index
routes/index
tooling
encounters
```

## Where the data lives

The repo currently carries the vanilla Hoenn set of **523 maps** under
`data/maps/`. Each map is a directory holding its own `map.json`
(connections, warps, object events) and `scripts.pory`/`scripts.inc`.

| What | File |
| --- | --- |
| Map group registry | `data/maps/map_groups.json` |
| Map group constants | `include/constants/map_groups.h` |
| Per-map definitions | `data/maps/<MapName>/map.json` |
| Per-map scripts | `data/maps/<MapName>/scripts.pory` |
| Layouts (dimensions, border, blockdata) | `data/layouts/` |
| Tilesets | `data/tilesets/` |
| Wild encounter tables | `src/data/wild_encounters.json` |
| Trainers and their teams | `src/data/trainers.h`, `src/data/trainer_parties.h` |
| Item ball contents | `data/scripts/item_ball_scripts.inc` |
| Region map section constants | `include/constants/region_map_sections.h` |
| Region map graphics | `src/data/region_map/` |

## Editing maps

Maps are edited with [Porymap](https://github.com/huderlem/porymap) rather than
by hand — open the repo root as a Porymap project. Note that Porymap's own
config files (`porymap.*.cfg`) are gitignored, so each contributor keeps their
own.

Prefer editing `map.json` through Porymap over editing it directly; the tool
keeps the layout, border and blockdata files consistent with each other.

For Sinnoh replicas, [the map tooling](tooling.md) pulls the original map out of
pokeplatinum, drafts a layout from it, builds layouts from text blueprints,
scores a replica against the original and checks warps, connections and
tilesets. Run `tools/mapkit/mapkit.py check <map>` after changing a map.

## Planned pages

- Region overview, with a route-by-route walkthrough order
- Connections between maps on the town and route pages
- New or substantially redesigned maps, and what changed from vanilla Hoenn

## Conventions

- Name maps by their `MAP_*` constant when precision matters, and by their
  in-game name otherwise.
- Give encounter rates as the percentages produced by the slot weights in
  `wild_encounters.json`, not as raw slot counts. [The encounter tooling](encounters.md)
  prints them.
