# Generation IV items

## What changed

The held items and berries introduced in Diamond, Pearl and Platinum are in the
game and work as they do in Platinum: their prices, descriptions, Fling power,
Natural Gift type and power, and battle behaviour follow
[pret/pokeplatinum](https://github.com/pret/pokeplatinum). The Generation IV
evolution items (Shiny, Dusk and Dawn Stone, Razor Claw, Razor Fang, Oval
Stone, Protector, Electirizer, Magmarizer, Dubious Disc and Reaper Cloth) now
have their Platinum icons.

The 21 new berries have berry tags, berry tree sprites and growth data, so they
can be planted, and they work in the Berry Blender and Berry Crush.

## How they work

**Power**

- **Choice Scarf** raises Speed by 1.5× and **Choice Specs** raise Sp. Atk by
  1.5×. Both lock their holder into one move, like Choice Band.
- **Muscle Band** powers up physical moves by 1.1× and **Wise Glasses** special
  moves by 1.1×.
- **Expert Belt** powers up super-effective moves by 1.2×.
- **Life Orb** powers up moves by 1.3×; the holder loses 1/10 of its HP once
  each turn it hits with a damaging move (not with Magic Guard).
- **Metronome** powers up a move by 10% each time it's used in a row, up to 2×.
- **Adamant Orb**, **Lustrous Orb** and **Griseous Orb** power up Dialga's,
  Palkia's and Giratina's Dragon moves and moves of their other type by 1.2×.
- The 16 **plates** power up moves of their type by 1.2×, as do the **Odd**
  (Psychic), **Rock**, **Wave** (Water) and **Rose** (Grass) **Incense**. Held
  by Arceus, a plate also sets its type through Multitype and the type of
  Judgment.
- **Quick Powder** doubles Ditto's Speed.

**Accuracy**

- **Wide Lens** raises accuracy by 1.1×.
- **Zoom Lens** raises accuracy by 1.2× if the target has already moved this
  turn.

**Getting hit**

- **Focus Sash** leaves its holder with 1 HP after a hit that would knock it
  out from full HP, then is used up.
- The **resist berries** (Occa to Babiri) halve the damage of a
  super-effective move of their type once, then are eaten. **Chilan Berry**
  halves the damage of any Normal move.
- **Jaboca Berry** hurts an attacker that hits with a physical move by 1/8 of
  its HP, then is eaten; **Rowap Berry** does the same for special moves.
- **Sticky Barb** moves to an attacker with no item that hits its holder with a
  contact move.

**At the end of each turn**

- **Black Sludge** restores 1/16 of a Poison type's HP and hurts any other
  holder by 1/8.
- **Sticky Barb** hurts its holder by 1/8 of its HP.
- **Flame Orb** burns its holder and **Toxic Orb** badly poisons it.
- **Micle Berry** boosts the accuracy of the holder's next move by 1.2× at 1/4
  HP (1/2 with Gluttony).

**Turn order**

- **Custap Berry** lets its holder move first in its priority bracket at 1/4
  HP (1/2 with Gluttony).
- **Lagging Tail** and **Full Incense** make their holder move last in its
  priority bracket.
- **Iron Ball** halves Speed, and grounds its holder against Ground moves and
  Spikes, through Levitate and Magnet Rise.

**Other battle effects**

- **Power Herb** lets a two-turn move (Solar Beam, Fly, Dig and so on) skip its
  charging turn, then is used up.
- **Light Clay** makes Reflect and Light Screen last 8 turns.
- **Damp Rock**, **Heat Rock**, **Smooth Rock** and **Icy Rock** make rain,
  sun, sandstorm and hail from moves last 8 turns.
- **Grip Claw** makes binding moves last 6 turns.
- **Big Root** raises the HP restored by draining moves, Leech Seed, Ingrain
  and Aqua Ring by 1.3×.
- **Destiny Knot** makes the Pokémon that infatuated its holder fall in love
  too.
- **Shed Shell** lets its holder switch out even when trapped.
- **Luck Incense** doubles prize money, like Amulet Coin.

**Outside battle**

- The **Power** items (Weight, Bracer, Belt, Lens, Band and Anklet) halve Speed
  and give 4 extra effort points in their stat for each Pokémon defeated.
- **Pure Incense** on the first Pokémon in the party cuts the wild encounter
  rate to 2/3, like Cleanse Tag.
- **Honey** can be used in the field to attract a wild Pokémon, like Sweet
  Scent. Pokémon with Honey Gather may find it after battle.
- Breeding a Pokémon with a baby form hatches the baby only if a parent holds
  the right incense: Full Incense for Munchlax, Odd for Mime Jr., Rock for
  Bonsly, Rose for Budew, Wave for Mantyke, Pure for Chingling and Luck for
  Happiny, as well as Lax and Sea Incense for Wynaut and Azurill.

## Not in the game

- Honey can't be slathered on trees, since there are no Honey Trees.
- Arceus, Dialga, Palkia and Giratina don't change form, since the game has
  no forms for them.

## Where it lives

| What | Where |
| --- | --- |
| Item IDs | `include/constants/items.h` |
| Hold effects, `HOLD_EFFECT_CHOICE_SCARF` (67) to `HOLD_EFFECT_ROWAP_BERRY` (107) | `include/constants/hold_effects.h` |
| Item data and descriptions | `src/data/items.h`, `src/data/text/item_descriptions.h` |
| Icons | `graphics/items/`, `src/data/graphics/items.h`, `src/data/item_icon_table.h` |
| Berry data and Berry Crush values | `src/berry.c` |
| Berry tags | `graphics/berries/`, `src/data/graphics/berries.h`, `src/item_menu_icons.c` |
| Berry trees | `graphics/object_events/pics/berry_trees/`, `src/data/object_events/berry_tree_graphics_tables.h` |
| End-of-turn, Life Orb, resist and on-hit items | `ItemBattleEffects` in `src/battle_util.c` |
| Speed and turn order | `GetBattlerSpeed` in `src/battle_util.c`, `CompareSpeeds` in `src/battle_main.c` |
| Damage, accuracy, Focus Sash, weather and screens | `src/battle_script_commands.c`, `CalculateBaseDamage` in `src/pokemon.c` |
| Power Herb, Destiny Knot and the item scripts | `data/battle_scripts_1.s` |
| Plates, Judgment and Multitype | `GetPlateType` in `src/pokemon.c`, `GetBattlerTypes` in `src/battle_util.c` |
| Power items' effort points | `MonGainEVs` in `src/pokemon.c` |
| Incense babies | `AlterEggSpeciesWithIncenseItem` in `src/daycare.c` |
| Honey | `ItemUseOutOfBattle_Honey` in `src/item_use.c`, `src/fldeff_sweetscent.c` |

The new berries sit together with the other berries, so every item after
Belue Berry has a new ID. `BAG_BERRIES_COUNT` in `include/constants/global.h`
is 67, one slot for each berry item. Both changes break existing saves.

## Upstream credit

Item data, icons, berry graphics and behaviour follow
[pret/pokeplatinum](https://github.com/pret/pokeplatinum). The Berry Crush
values for the new berries are Palladium's own, since Platinum has no Berry
Crush.
