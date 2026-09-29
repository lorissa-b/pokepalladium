# Pokédex

Species available in Palladium: their base stats, typings, evolutions, learnsets
and dex entries.

```{toctree}
:maxdepth: 1

families/index
```

## Where the data lives

The dex is generated from the decomp's data headers rather than written by hand,
so edit these and the game and docs stay in agreement.

| What | File |
| --- | --- |
| Species constants | `include/constants/species.h` |
| Base stats, types, abilities, growth | `src/data/pokemon/species_info.h` |
| Dex entries (text, height, weight) | `src/data/pokemon/pokedex_entries.h` |
| Dex ordering (national/regional) | `src/data/pokemon/pokedex_orders.h` |
| Evolution methods | `src/data/pokemon/evolution.h` |
| Level-up learnsets | `src/data/pokemon/level_up_learnsets.h` |
| Egg moves | `src/data/pokemon/egg_moves.h` |
| Front/back sprites, palettes | `graphics/pokemon/` |

## Generated pages

{doc}`families/index` is built from the data headers by
`docs/_ext/gen_pokedex.py` on every Sphinx run, one page per evolution
family. Each species gets a heading with its front sprite, an overview table
(types, abilities, evolutions, base stats) and one moves table with side-by-side
level-up, TM/HM and egg move columns.

Because it is generated, **do not edit those pages** — they are gitignored
and overwritten each build. Change `src/data/pokemon/` instead.

## Still to write

- Regional dex listing, in dex order
- Base stat tables
- Type chart, if Palladium alters it

## Conventions

- Refer to species by their `SPECIES_*` constant on first mention, then by name.
- Note explicitly where Palladium diverges from vanilla Emerald — the diff from
  vanilla is the interesting part for most readers.
