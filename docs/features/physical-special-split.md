# Physical/special split

## What changed

Whether a move draws on Attack or Special Attack is now a property of **the
move**, not of its type. Fire Punch is physical, Hyper Beam is special, and
every type can carry moves of either kind.

In vanilla Emerald the split follows the type: types before `TYPE_MYSTERY` are
physical and types after it are special, so every Fire move is special and every
Normal move is physical regardless of what the move actually does. This is the
Generation IV behaviour, ported by following the
[pret wiki tutorial](https://github.com/pret/pokeemerald/wiki/add-physical-special-split).

## Where it lives

| What | Where |
| --- | --- |
| `category` byte on the move struct | `struct BattleMove` in `include/pokemon.h` |
| `MOVE_CATEGORY_PHYSICAL` / `_SPECIAL` / `_STATUS` | `include/pokemon.h` |
| `IS_MOVE_PHYSICAL` / `IS_MOVE_SPECIAL` / `IS_MOVE_STATUS` | `include/battle.h` |
| Per-move categories | `src/data/battle_moves.h` |
| Damage calculation | `CalculateBaseDamage` in `src/pokemon.c` |

The macros replace vanilla's `IS_TYPE_PHYSICAL` / `IS_TYPE_SPECIAL` and take a
move rather than a type:

```c
#define IS_MOVE_PHYSICAL(move) (gBattleMoves[move].category == MOVE_CATEGORY_PHYSICAL)
```

Every move in `battle_moves.h` carries the field, so a new move needs a
`.category` line alongside its `.flags`:

```c
[MOVE_FIRE_PUNCH] =
{
    ...
    .flags = FLAG_MAKES_CONTACT | FLAG_PROTECT_AFFECTED | FLAG_MIRROR_MOVE_AFFECTED,
    .category = MOVE_CATEGORY_PHYSICAL,
},
```

## Knock-on mechanics changes

Three pieces of vanilla damage code were correct only because of the old
type-based rule, and needed rewriting rather than just re-pointing at the new
macro. Each one is a real behaviour change, not a refactor:

- **Type-boosting hold items** (Charcoal, Magnet, Miracle Seed, …) boosted
  whichever of Attack or Special Attack matched the item's type. They now boost
  both, and the move's category decides which one is read. A Charcoal now boosts
  a physical Fire move.
- **Weather and Flash Fire** only ran inside the special branch, since every
  type they affect was special in Gen III. They are now applied outside it, so
  rain weakens a physical Fire move and sun boosts one.
- **Thick Fat** halved the attacker's Special Attack, which would do nothing
  against a physical Fire or Ice move. It now halves `gBattleMovePower`, so it
  applies to both categories.

Status moves are also newly distinct from "not physical". The Mirror Coat branch
in `Cmd_datahpupdate` tests `IS_MOVE_SPECIAL` explicitly, because
`!IS_MOVE_PHYSICAL` would now also match status moves.

## Known limitation: the battle AI

The AI does not use these macros. `data/battle_ai_scripts.s` keeps its own
tables of which *types* are physical and special, and those tables are unchanged.
They affect three decisions:

1. Attack-lowering versus Special-Attack-lowering moves
2. Reflect versus Light Screen
3. Counter versus Mirror Coat

The AI reads the *types of the opposing Pokémon* rather than the moves it knows,
so this check was already a poor proxy before the split; it is now also
inconsistent with the damage formula. Fixing it belongs with a broader AI pass.
The tutorial leaves it alone for the same reason.

## Upstream credit

The tutorial and the per-move category table are from the
[pret pokeemerald wiki](https://github.com/pret/pokeemerald/wiki/add-physical-special-split).
