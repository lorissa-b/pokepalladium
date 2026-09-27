# Moves

Move data: power, accuracy, PP, type, targeting and battle effects, plus any
moves Palladium adds, removes or rebalances.

```{note}
Scaffold page. Move tables still to be written.
```

## Where the data lives

| What | File |
| --- | --- |
| Move constants | `include/constants/moves.h` |
| Battle move data (power, type, PP, accuracy, flags) | `src/data/battle_moves.h` |
| Move effect constants | `include/constants/battle_move_effects.h` |
| Contest move data | `src/data/contest_moves.h` |
| Move descriptions | `src/data/text/move_descriptions.h` |
| Move animations | `data/battle_anim_scripts.s` |
| TM/HM learnsets | `src/data/pokemon/tmhm_learnsets.h` |
| Move tutor learnsets | `src/data/pokemon/tutor_learnsets.h` |

## Planned pages

- Full move table, sorted by type and by power
- Changed-from-vanilla list — the rebalance summary
- New moves, with effect and animation notes
- TM/HM and move tutor availability

## Conventions

- Give power/accuracy/PP as they appear in `battle_moves.h`, not as displayed
  in-game, and note where the two differ.
- When a move's `effect` changes, link the `EFFECT_*` constant so readers can
  find the implementing case in `src/battle_script_commands.c`.
