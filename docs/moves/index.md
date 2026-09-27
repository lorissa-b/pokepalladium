# Moves

Move data: power, accuracy, PP, type, targeting and battle effects, plus any
moves Palladium adds, removes or rebalances.

Palladium implements the per-move {doc}`physical/special split
<../features/physical-special-split>`, so a move's category is its own property
rather than a consequence of its type.

```{toctree}
:maxdepth: 1

types/index
```

## Where the data lives

| What | File |
| --- | --- |
| Move constants | `include/constants/moves.h` |
| Battle move data (power, type, PP, accuracy, category, flags) | `src/data/battle_moves.h` |
| Move effect constants | `include/constants/battle_move_effects.h` |
| `MOVE_CATEGORY_` constants | `include/pokemon.h` |
| Contest move data | `src/data/contest_moves.h` |
| Move descriptions | `src/data/text/move_descriptions.h` |
| Move animations | `data/battle_anim_scripts.s` |
| TM/HM learnsets | `src/data/pokemon/tmhm_learnsets.h` |
| Move tutor learnsets | `src/data/pokemon/tutor_learnsets.h` |

## Generated pages

{doc}`types/index` is built by `docs/_ext/gen_moves.py` on every Sphinx
run: one page per type, listing each move's category, power, accuracy, PP,
contest category and effect, with the `EFFECT_` constant that implements it. Because it is generated, **do not edit those pages** —
they are gitignored and overwritten each build.

## Still to write

- Changed-from-vanilla list — the rebalance summary
- New moves, with effect and animation notes
- TM/HM and move tutor availability

## Conventions

- Give power/accuracy/PP as they appear in `battle_moves.h`, not as displayed
  in-game, and note where the two differ.
- When a move's `effect` changes, link the `EFFECT_*` constant so readers can
  find the implementing case in `src/battle_script_commands.c`.
