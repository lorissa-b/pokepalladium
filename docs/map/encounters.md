# Encounter tooling

`tools/encounters/encounters.py` analyses which Pokémon each map offers: its
wild encounters for every method and time of day, and the trainers a player
can actually battle there. It is for checking a route's composition while
designing it, so tables stay readable and trainers use Pokémon the player can
find.

It needs only Python 3 and runs from the repo root. Its output is Markdown
(headings, tables and lists), so it reads as rendered tables in chat or a docs
page:

```sh
tools/encounters/encounters.py <command> --help
```

Maps are named as [mapkit](tooling.md) names them (`MAP_ROUTE201`, `Route201`,
`route 201`), and a name can be shortened or span words (`twinleaf`,
`ravaged path`). With no maps given, every command looks at the converted
Sinnoh maps: those in a Sinnoh region map section and named after it. Route 105
and Rusturf Tunnel share a Sinnoh section but are left out. `--all` looks at
every map with encounters instead.

Rates are the percentages the slot weights in `wild_encounters.json` produce.
Fishing is split into Old, Good and Super Rod. Times of day follow the game: four
tables are morning, day, evening and night; two are morning and day, then
evening and night; any other number means the first table is used all day.

## Commands

**`summary`** gives one row per map, then every species with the maps and
methods it appears on:

| Map | Tables | Grass/table | Grass total | All species | Top 3 | Grass Lv. | Trainers |
|---|---|---|---|---|---|---|---|
| Route 201 | 4 | 5 | 12 | 12 | 77% | 2-4 | 2 |
| Ravaged Path | 4 | 5 | 5 | 12 | 72% | 5-7 | 0 |

- *Grass/table*: species in each grass table.
- *Grass total*: grass species across all times of day.
- *Top 3*: the three most common species' share of a grass table.
- *Trainers*: distinct trainers. A rival with a team for each starter counts once.

These columns show how varied a route feels.

**`overview`** compares the maps with each other, in four parts:

- **Shared and unique species:** for each map, how many of its wild species
  are also wild on another map here, how much of its grass encounter rate
  goes to those shared species, and which species only it has.
- **Overlap between maps:** a grid giving, for each pair of maps, the share of
  all the species on either map that are on both.
- **Type variety:** how many types each map has and its type mix, both
  counting each species once and weighted by grass encounter rate.
- **Generations:** the share of each map's species from each generation (by
  National Dex number), both counting each species once and weighted by grass
  encounter rate.

Surfing and fishing count towards the species totals as well as grass.

**`show MAP...`** prints a table for each encounter method on the given maps,
with species down the side and times of day across (times that share a table
are merged), then the trainers with their teams. `--types` adds each map's
grass type mix, with a dual type counting half to each type.

```sh
tools/encounters/encounters.py show route204 ravaged path --types
```

**`species NAME...`** lists where each species can be caught (map, method,
times, rate and levels) and which trainers use it.

```sh
tools/encounters/encounters.py species shinx pachirisu
```

**`check`** flags:

- grass tables with more species than `--max-species` (default 5; 0 skips this)
- an encounter whose level is under half or over twice its table's typical
  level, such as a Lv. 5 slot in a Lv. 20-45 Super Rod table
- trainer Pokémon whose evolution family can't be caught on any of the maps
  being checked (the starters count as obtainable)

It also notes maps whose four tables are identical. It exits 1 when it prints
a warning, so it can gate a script.

Trainers are found by following the map's scripts from its object, coord and
background events and its map scripts. Battles left over in `scripts.inc` that
nothing reaches, such as a Hoenn battle left behind when a map is converted,
are left out.

## Tests

```sh
python3 -m unittest discover tools/encounters
```
