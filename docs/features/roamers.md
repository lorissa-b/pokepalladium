# Roamers

## What changed

Up to five roaming Pokémon can be active at once (vanilla allows one), enough
for Platinum's Mesprit, Cresselia and the three Kanto birds. Each roamer can be
set up to:

- **roam** between routes, like vanilla's Latios and Latias, or only between
  **land** routes, so it's never met while surfing;
- be a **stalker**, which follows the player from map to map instead of roaming;
- **flee** on its first turn, or stay and battle;
- **respawn** after being defeated: never, the next day, after a week, or at once;
- **scale** its level to the player's party (level 0), evolving as it goes.

Roamers keep their locations when the game is saved. Nothing adds a roamer yet
except Emerald's Latios/Latias event; Palladium's roamers need scripts that
call them in.

## Adding roamers

Write a function in `src/roamer.c` that adds them, and call it from a script
with `special` (add it to the end of `data/specials.inc` and declare it in
`include/roamer.h`):

```c
void InitLakeTrioRoamers(void)
{
    // Mark it as seen so its location shows in the Pokédex
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_MESPRIT), FLAG_SET_SEEN);
    TryAddTerrestrialRoamer(SPECIES_MESPRIT, 50, FLEES, NO_RESPAWN);
}
```

| Function | Roams |
| --- | --- |
| `TryAddRoamer(species, level, flees, respawn)` | land and sea routes |
| `TryAddTerrestrialRoamer(species, level, flees, respawn)` | land routes only |
| `TryAddStalker(species, level, flees, AMPHIBIOUS or TERRESTRIAL, respawn)` | follows the player |

`flees` is `FLEES` or `DOES_NOT_FLEE`; `respawn` is `NO_RESPAWN`,
`DAILY_RESPAWN`, `WEEKLY_RESPAWN` or `INSTANT_RESPAWN`. A level of 0 makes a
scaling roamer. Each returns `FALSE` if all `ROAMER_COUNT` slots are taken.

## Where it lives

| What | Where |
| --- | --- |
| `ROAMER_COUNT` | `include/constants/global.h` |
| The roamers in the save | `roamer[ROAMER_COUNT]` in `struct SaveBlock1`, `include/global.h` |
| Encounter odds, level scaling, roaming every route | the config block at the top of `src/roamer.c` |
| The routes they roam | `sRoamerLocations` and `sTerrestrialLocations` in `src/roamer.c` |
| `MULTIPLE_ROAMERS_EXAMPLE`, `SHOW_STALKERS_ON_POKEDEX` | `include/roamer.h` |

```{note}
The route tables are still Emerald's Hoenn routes, with the renamed ones
(Routes 201, 202, 204 and 219) under their new names. They need replacing with
Sinnoh's routes as the map comes together. Each set lists a route and the
routes a roamer can move to from it; the comment above the table explains the
rules that keep roamers from getting stuck.
```

## Upstream credit

DarkDown's [RoamersPlus](https://www.pokecommunity.com/threads/roamersplus-multiple-concurrent-roamers-and-other-roamer-related-features.472080/),
from tenaya15's updated `roamersplus` branch, via the pokeemerald wiki's
[RoamersPlus tutorial](https://github.com/pret/pokeemerald/wiki/RoamersPlus-%E2%80%90-multiple-Roamers-at-once/).
