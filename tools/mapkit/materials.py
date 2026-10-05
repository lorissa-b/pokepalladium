"""What each metatile depicts: grass, tree, cliff, water, building...

Labels live in materials/<Tileset>.txt (see General.txt for the format,
including the families that split trees and water by look),
written by looking at the tileset catalogs. Behaviours add what they make
certain, so water, tall grass, sand, ledges, doors and bridges are always
known. Tiles with no label are "unknown", and tilesets without a file are
unknown throughout.
"""

from __future__ import annotations

import re
from functools import lru_cache
from pathlib import Path

from project import behaviors, consts
from tileset import TilesetPair

DIR = Path(__file__).resolve().parent / "materials"

MATERIALS = {
    "grass", "path", "sand", "tallgrass", "flowers", "tree", "cliff", "rock", "water",
    "ledge", "bridge", "fence", "building", "object", "cave", "dark", "unknown",
    # Interiors and caves.
    "floor", "wall", "stairs", "ice", "lava",
}

# Families split a material by look; a style keeps one per material.
# Paths get theirs from their other labels (see of()): sandy where they're
# also sand, stone (gravel, paving, ash) otherwise.
FAMILIES = {
    "tree": {"dense", "round", "jungle", "pine"},
    "water": {"sea", "pond"},
    "path": {"sandy", "stone"},
}
TAGS = MATERIALS | set().union(*FAMILIES.values())

_FROM_BEHAVIOR = [
    (r"WATER|WATERFALL|PUDDLE|SEAWEED|NO_SURFACING|CURRENT", "water"),
    (r"^MB_TALL_GRASS$|LONG_GRASS", "tallgrass"),
    (r"SAND", "sand"),
    (r"JUMP_", "ledge"),
    # Non-animated doors are also cave mouths, so only these imply a building.
    (r"(?<!NON_)ANIMATED_DOOR|GYM_DOOR", "building"),
    (r"BRIDGE|PACIFIDLOG", "bridge"),
]


@lru_cache(maxsize=None)
def labels(tileset_name: str) -> dict[int, frozenset[str]]:
    """Metatile id -> materials, from materials/<tileset_name>.txt (empty if there's no file)."""
    path = DIR / f"{tileset_name}.txt"
    if not path.exists():
        return {}
    out: dict[int, frozenset[str]] = {}
    for n, line in enumerate(path.read_text().splitlines(), 1):
        line = line.split("#", 1)[0].strip()
        if not line:
            continue
        ids, *mats = line.split()
        add = all(t.startswith("+") for t in mats)
        mats = [t.lstrip("+") for t in mats]
        bad = set(mats) - TAGS
        if bad or not mats:
            raise ValueError(f"{path}:{n}: unknown material {', '.join(sorted(bad)) or '(none)'}")
        m = re.fullmatch(r"(0x[0-9A-Fa-f]+)(?:-(0x[0-9A-Fa-f]+))?", ids)
        if not m:
            raise ValueError(f"{path}:{n}: expected an id or id range, got {ids!r}")
        lo = int(m.group(1), 16)
        hi = int(m.group(2), 16) if m.group(2) else lo
        for i in range(lo, hi + 1):
            out[i] = out.get(i, frozenset()) | frozenset(mats) if add else frozenset(mats)
    return out


def of(tiles: TilesetPair, metatile: int) -> frozenset[str]:
    """The materials a metatile shows, with the layout's tilesets."""
    c = consts()
    ts = tiles.primary if metatile < c.metatiles_in_primary else tiles.secondary
    mats = set(labels(ts.name).get(metatile, frozenset({"unknown"})))
    name = behaviors().get(tiles.behavior(metatile), "")
    for pattern, mat in _FROM_BEHAVIOR:
        if re.search(pattern, name):
            mats.add(mat)
    if "lava" in mats:
        mats.discard("water")  # Magma Hideout's lava has a water behaviour
    if len(mats) > 1:
        mats.discard("unknown")
    if "path" in mats and not mats & FAMILIES["path"]:
        mats.add("sandy" if "sand" in mats else "stone")
    return frozenset(mats)


def family(mats: frozenset[str], material: str) -> str | None:
    """Which family of `material` a tile belongs to, if it's tagged with one."""
    tags = mats & FAMILIES.get(material, set())
    return next(iter(tags)) if tags else None
