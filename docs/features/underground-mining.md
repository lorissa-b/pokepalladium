# Underground mining

## What changed

Palladium has no Underground, so the items Platinum hides in its walls can be
dug up another way. The **Explorer Kit** is a key item that works on any
outdoor map, from the bag or registered to SELECT. Using it pulls a rock wall
up out of the ground and opens a mining screen that follows the rules of
Platinum's wall digging. There's no limit on how often it can be used.

The Explorer Kit is in the bag from the start of a new game. In Platinum the
Underground Man in Eterna City gives it out, and it will move there once
Eterna City is built.

## How it works

**The wall**

- The wall is 13 × 10 cells of dirt, each 2, 4 or 6 layers deep, laid out with
  Platinum's random patches.
- It hides 2 to 8 items. The first wall always has 3 items and no rocks. Later
  walls also have rocks, which can't be dug through, up to 8 buried objects in
  all.
- A cell that has been dug down to the wall behind shows gold where an item is
  and grey stone where a rock is. Once every cell of an item is clear, its bag
  icon appears.

**Digging**

- Move the cursor with the +Control Pad and strike with A. The cursor marks
  every cell the current tool hits.
- The **pickaxe** takes 2 layers off its cell and 1 off the four cells beside
  it, and costs the wall 4 of its 196 strength.
- The **hammer** takes 2 layers off its cell and the four beside it, and 1 off
  the four diagonal cells, but costs the wall 8.
- R, L or SELECT switches between the pickaxe and the hammer.
- A blow that hits a bare rock doesn't spread to the cells around it.
- The meter on the right shows how much strength the wall has left. It turns
  orange at half strength and red at a quarter.

**Ending a dig**

- The dig ends when every item is uncovered, when the wall collapses, or when
  B is pressed and the player chooses to stop.
- Every item uncovered by then goes into the bag. If the bag has no room for an
  item, it's left behind.

**What can be found**

The weights are Platinum's. Which fossil turns up early depends on whether the
Trainer ID is odd (Skull Fossil) or even (Armor Fossil). The other fossils, and
much better odds for the stones and rocks, come once the National Pokédex is
obtained. Each plate can be dug up only once. Out of a total weight of about
170 (360 after the National Pokédex):

| Item | Before National Pokédex (odd / even ID) | After (odd / even ID) |
| --- | --- | --- |
| Heart Scale | 33 / 33 | 30 / 30 |
| Skull Fossil | 25 / 0 | 12 / 0 |
| Armor Fossil | 0 / 25 | 0 / 12 |
| Red, Blue, Yellow and Green Shard | 13 / 13 each | 17 / 17 each |
| Revive | 8 / 8 | 10 / 10 |
| Max Revive | 1 / 1 | 2 / 2 |
| Hard Stone | 4 / 4 | 20 / 20 |
| Everstone | 4 / 4 | 20 / 20 |
| Fire Stone | 4 / 1 | 30 / 5 |
| Thunder Stone | 4 / 1 | 30 / 5 |
| Sun Stone | 4 / 1 | 15 / 3 |
| Water Stone | 1 / 4 | 5 / 30 |
| Leaf Stone | 2 / 4 | 5 / 30 |
| Moon Stone | 2 / 4 | 3 / 15 |
| Star Piece | 2 / 2 | 10 / 10 |
| Rare Bone | 2 / 2 | 10 / 10 |
| Icy Rock | 2 / 1 | 11 / 5 |
| Heat Rock | 2 / 1 | 11 / 5 |
| Smooth Rock | 1 / 2 | 5 / 11 |
| Damp Rock | 1 / 2 | 5 / 11 |
| Light Clay | 1 / 1 | 5 / 2 |
| Iron Ball | 1 / 1 | 2 / 5 |
| Helix Fossil | — | 12 / 4 |
| Claw Fossil | — | 12 / 4 |
| Root Fossil | — | 4 / 12 |
| Dome Fossil | — | 1 / 13 |
| Old Amber | — | 5 / 5 |
| Each of the 16 plates | 1 / 1 | 1 / 1 |

**New items**

- **Explorer Kit**, a key item.
- **Skull Fossil** and **Armor Fossil**, which sell for 500.
- **Rare Bone**, which sells for 5,000.

All seven fossils are now ordinary items in the Items pocket, so they stack and
can be sold, as in Platinum. Before, the Root, Claw, Helix and Dome Fossil and
Old Amber were key items.

## Not in the game

- **Spheres** aren't in the walls, since there's nothing to trade them for.
  Without them, the other items come up far more often than in Platinum.
- **Odd Keystone** isn't in the game yet, and **Oval Stone** never appears in
  Platinum's walls either.
- **Fossil revival:** nothing revives fossils yet. Oreburgh's Mining Museum has
  no interior.
- **Radars and link digging:** there's no Explorer Kit radar, and walls can't
  be dug with other players.

## Where it lives

| What | Where |
| --- | --- |
| The minigame: wall generation, digging, the item table and the screen | `src/mining.c` |
| Using the Explorer Kit, and where it works | `ItemUseOutOfBattle_ExplorerKit` in `src/item_use.c` |
| The "used the Explorer Kit" script and the `StartMiningGame` special | `data/scripts/explorer_kit.inc`, `data/specials.inc` |
| The dirt, wall, treasure and rock tiles | `graphics/mining/tiles.png` |
| `FLAG_HAS_MINED_WALL` (first wall) and `FLAG_MINED_FLAME_PLATE` to `FLAG_MINED_IRON_PLATE` | `include/constants/flags.h` |
| New item IDs, data, descriptions and icons | `include/constants/items.h`, `src/data/items.h`, `src/data/text/item_descriptions.h`, `src/data/graphics/items.h`, `src/data/item_icon_table.h` |
| The Explorer Kit in a new game's bag | `NewGameInitData` in `src/new_game.c` |

The new items take unused item slots (`ITEM_069`, `ITEM_10B`, `ITEM_15B` and
`ITEM_15C`), so no other item IDs change. Saves from before this change will
have these flags clear and no Explorer Kit, and a fossil already in the Key
Items pocket stays there.

## Upstream credit

The digging rules, dirt patterns, object shapes and weights, the dirt and rock
art, and the Explorer Kit, Skull Fossil, Armor Fossil and Rare Bone icons
follow [pret/pokeplatinum](https://github.com/pret/pokeplatinum)
(`src/underground/mining.c`). The cursor is the region map's, and the message
box, frames and Yes/No menu are Emerald's own.
