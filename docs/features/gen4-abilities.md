# Generation IV abilities

## What changed

All 47 abilities introduced in Diamond, Pearl and Platinum are in the game, from
Tangled Feet to Bad Dreams, and every species has its Platinum abilities. They
work as they do in Platinum: the behaviour, numbers and battle messages follow
[pret/pokeplatinum](https://github.com/pret/pokeplatinum).

Species that gained an ability in Generation IV have it as their second ability
(Eevee gets Adaptability, Machop No Guard, Porygon Download, and so on), and the
Generation IV species that had none now have theirs. Shinx, Luxio, Luxray,
Riolu, Lucario and Ambipom also list their abilities in Platinum's order, so a
Pokémon in the first slot gets Platinum's first ability. Hidden abilities that
are Generation IV abilities (Solar Power, Technician, Sniper and so on) are
filled in too; see {doc}`hidden-abilities`.

## How they work

**On entering battle**

- **Snow Warning** summons hail that lasts until the weather changes.
- **Download** raises Sp. Atk if the foes' total Defense is at least their
  Sp. Def, otherwise Attack. Foes behind a substitute don't count.
- **Anticipation** shudders if a foe knows a super-effective move or a one-hit
  KO move (fixed-damage moves, Counter, Mirror Coat and Metal Burst don't count).
- **Forewarn** reveals the foe's strongest move. One-hit KO moves count as 150
  power, Counter, Mirror Coat and Metal Burst as 120, and other variable-power
  moves as 80.
- **Frisk** reveals one of the foes' held items.
- **Slow Start** halves Attack and Speed for five turns after switching in.
- **Mold Breaker** announces itself.

**Damage and accuracy**

- **Adaptability** makes same-type moves 2× instead of 1.5×.
- **Technician** powers up moves of 60 power or less by 1.5×.
- **Iron Fist** powers up punching moves by 1.2×; **Reckless** does the same for
  recoil and crash moves.
- **Rivalry** does 1.25× damage to a foe of the same gender and 0.75× to the
  other gender.
- **Normalize** makes every move Normal type.
- **Sniper** makes critical hits do 3× damage; **Super Luck** raises the
  critical-hit ratio.
- **Tinted Lens** doubles not-very-effective damage; **Filter** and **Solid
  Rock** cut super-effective damage to 3/4.
- **Heatproof** halves Fire damage and burn damage; **Dry Skin** takes 1.25× Fire
  damage.
- **Scrappy** hits Ghost types with Normal and Fighting moves.
- **Simple** doubles its stat stages; **Unaware** ignores the other Pokémon's.
- **Solar Power** raises Sp. Atk by 1.5× in sun; **Flower Gift** raises the
  Attack and Sp. Def of its holder and allies by 1.5× in sun.
- **No Guard** makes moves by or against its holder always hit, even during Fly
  or Dig.
- **Snow Cloak** raises evasion in hail; **Tangled Feet** raises it when confused.
- **Skill Link** makes 2–5-hit moves hit five times.
- **Mold Breaker** ignores the target's abilities that would get in the way of
  its moves (the same list as Platinum, such as Levitate, Wonder Guard, Sturdy,
  Filter and Clear Body), and its moves aren't drawn in by Lightning Rod or Storm
  Drain.

**Getting hit**

- **Motor Drive** absorbs Electric moves and raises Speed.
- **Dry Skin** absorbs Water moves to restore 1/4 of its HP.
- **Anger Point** maxes Attack after taking a critical hit.
- **Aftermath** hurts a Pokémon that knocks it out with a contact move by 1/4 of
  its HP, unless a Pokémon with Damp is out.
- **Storm Drain** draws single-target Water moves to itself in double battles,
  like Lightning Rod.
- **Steadfast** raises Speed when its holder flinches.

**At the end of each turn**

- **Poison Heal** restores 1/8 of its HP instead of taking poison damage.
- **Hydration** cures its status in rain.
- **Ice Body** restores 1/16 of its HP in hail; **Dry Skin** restores 1/8 in
  rain. Both, and Snow Cloak, aren't hurt by hail.
- **Dry Skin** and **Solar Power** lose 1/8 of their HP in sun.
- **Bad Dreams** hurts sleeping foes by 1/8 of their HP.

**Other effects**

- **Magic Guard** takes damage only from attacks: no recoil, crash damage,
  weather, poison, burn, Leech Seed, Nightmare, Curse, binding moves, Spikes,
  Stealth Rock, Rough Skin, Aftermath or Bad Dreams.
- **Leaf Guard** prevents status problems in sun, including Yawn.
- **Klutz** can't use its held item, though Macho Brace still slows it.
- **Gluttony** eats the berries that work at 1/4 HP (the stat-raising berries,
  Lansat and Starf) at 1/2 HP instead.
- **Unburden** doubles Speed once its held item is used up or lost.
- **Quick Feet** raises Speed by 1.5× while it has a status problem, and
  paralysis doesn't slow it.
- **Stall** always moves last in its priority bracket.
- **Multitype** makes Arceus the type of the plate it holds, in battle and on
  the summary screen. It can't be suppressed, swapped, copied or replaced:
  Gastro Acid, Worry Seed, Skill Swap, Role Play and Trace fail on it. A
  Pokémon with Multitype can't use Conversion, Conversion 2 or Camouflage, and
  Thief and Covet can't take items from or give items to it.
- **Honey Gather** may find Honey after a battle if its holder has no item: a 5%
  chance at levels 1–10, rising by 5% every 10 levels to 50% at levels 91–100.

**Out of battle**, with the ability on the first Pokémon in the party, Quick Feet
halves the wild encounter rate, Snow Cloak halves it in snow, and No Guard
doubles it.

## Not yet in the game

- **Flower Gift** powers up the team, but Cherrim doesn't change to its
  Sunshine Form, since the game has no forms for it.
- **Multitype** changes Arceus's type but not its sprite, since the game has
  no forms for it.
- Trace copies Download, Frisk and the other switch-in abilities, but they
  don't activate when copied.
- The AI doesn't know about the new immunities (Motor Drive, Dry Skin) and may
  still use those moves into them.

## Where it lives

| What | Where |
| --- | --- |
| Ability IDs, `ABILITY_TANGLED_FEET` (78) to `ABILITY_BAD_DREAMS` (124) | `include/constants/abilities.h` |
| Names and summary-screen descriptions | `src/data/text/abilities.h` |
| Species abilities | `src/data/pokemon/species_info.h` |
| Switch-in, end-of-turn, absorbing and contact abilities | `AbilityBattleEffects` in `src/battle_util.c` |
| Poison Heal, Magic Guard and Bad Dreams at the end of the turn | `DoBattlerEndTurnEffects` in `src/battle_util.c` |
| Speed (Quick Feet, Slow Start, Unburden, Simple) | `GetBattlerSpeed` in `src/battle_util.c` |
| Stall | `CompareSpeeds` in `src/battle_main.c` |
| Mold Breaker | `GetDefenderAbility` in `src/battle_util.c`; scripts use `jumpifignorableability` |
| Leaf Guard | `IsLeafGuardProtected` in `src/battle_script_commands.c`; scripts use `jumpifleafguardprotected` |
| Klutz | `IsBattlerItemSuppressed` in `src/battle_util.c` |
| Accuracy, critical hits, type effectiveness, multi-hit moves | `src/battle_script_commands.c` |
| Damage formula | `CalculateBaseDamage` in `src/pokemon.c` |
| Battle scripts and messages | `data/battle_scripts_1.s`, `src/battle_message.c` |
| Wild encounter rate | `WildEncounterCheck` in `src/wild_encounter.c` |
| Multitype's type | `GetBattlerTypes` in `src/battle_util.c`, `GetPlateType` in `src/pokemon.c` |
| Honey Gather | `Cmd_pickup` in `src/battle_script_commands.c` |

The IDs are one higher than Platinum's, because Emerald keeps the unused
Cacophony at 76.

Hail from Snow Warning lasts until the weather changes, like the other weather
abilities, using a new `B_WEATHER_HAIL_PERMANENT` flag.

## Upstream credit

Ability behaviour, species abilities and battle messages follow
[pret/pokeplatinum](https://github.com/pret/pokeplatinum).
