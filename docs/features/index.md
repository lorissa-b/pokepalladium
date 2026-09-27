# Features

Engine and gameplay changes Palladium makes on top of vanilla Emerald —
mechanics, QoL, UI, and anything that isn't species, move or map data.

```{note}
Scaffold page. The repo currently tracks upstream pokeemerald closely, so this
section is where divergences get recorded as they land.
```

```{toctree}
:maxdepth: 1

physical-special-split
```

## What belongs here

Anything that changes how the game *behaves* rather than what data it contains:

- Battle mechanics — the {doc}`physical/special split
  <physical-special-split>`, crit and damage formula changes, ability or
  held-item reworks
- Progression — level caps, EXP curve changes, trainer rebalancing
- Quality of life — running indoors, reusable TMs, faster text, bag sorting
- UI and presentation — new menus, summary screen changes, party screen tweaks
- Build-time options and feature flags

Species, move and map content belongs in {doc}`../pokedex/index`,
{doc}`../moves/index` and {doc}`../map/index` instead.

## Planned pages

- Changes from vanilla Emerald — the headline list, kept current
- Battle mechanics reference
- Quality-of-life toggles and how to turn them off
- Credits for upstream features pulled in from pret or the wider decomp community

## Conventions

Each feature page is most useful with four things:

1. **What changed**, in one sentence, from the player's point of view.
2. **Why**, if it isn't obvious.
3. **Where it lives** — the files and functions touched, so it can be found,
   reviewed or reverted.
4. **Upstream credit**, where the feature came from someone else's work.

When a feature is gated behind a build flag or constant, name the flag so
readers can find every site that checks it.
