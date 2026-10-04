"""Suggest material labels for a tileset, to review by eye.

Labels come from, in order of trust:

1. dup:   the metatile draws exactly like one already labelled (any
          labelled tileset), so it gets the same labels;
2. beh:   its behaviour says what it is (water, a counter, stairs...);
3. guess: the closest-looking labelled metatile, by a coarse colour layout.

The result is a materials file (same format as materials/*.txt) with each
line's source in a comment, and a catalog sheet with the suggestions written
under each tile (dup in white, behaviour in green, guesses in yellow), so a
reviewer only has to correct what's wrong.
"""

from __future__ import annotations

import math
from collections import Counter
from functools import lru_cache

import materials
import project
from tileset import TilesetPair, _headers, load


def primary_for(secondary: str) -> str:
    """The primary tileset most layouts pair this secondary with."""
    if not load(secondary).secondary:
        return secondary
    uses = Counter(l["primary_tileset"] for l in project.layouts().values() if l["secondary_tileset"] == secondary)
    return uses.most_common(1)[0][0] if uses else "gTileset_General"


def pair_for(symbol: str) -> TilesetPair:
    ts = load(symbol)
    if ts.secondary:
        return TilesetPair(primary_for(symbol), symbol)
    # A primary on its own: pair it with any secondary it's used with (only its own ids are looked at).
    uses = Counter(l["secondary_tileset"] for l in project.layouts().values() if l["primary_tileset"] == symbol)
    return TilesetPair(symbol, uses.most_common(1)[0][0])


def own_ids(symbol: str) -> list[int]:
    c = project.consts()
    ts = load(symbol)
    base = c.metatiles_in_primary if ts.secondary else 0
    return [base + i for i in range(len(ts))]


def _features(img) -> tuple[float, ...]:
    """A coarse colour layout: the average colour of each 4x4 pixel square."""
    small = img.resize((4, 4), resample=4)  # BOX
    return tuple(v for px in small.getdata() for v in px)


@lru_cache(maxsize=None)
def reference() -> tuple[dict[bytes, tuple[frozenset, str]], list[tuple[tuple[float, ...], frozenset, str]]]:
    """Every labelled metatile: (pixels -> labels, [(features, labels, name)])."""
    exact: dict[bytes, tuple[frozenset, str]] = {}
    feats = []
    for symbol in sorted(_headers()):
        name = symbol[len("gTileset_"):]
        labels = materials.labels(name)
        if not labels:
            continue
        try:
            pair = pair_for(symbol)
        except Exception:
            continue
        for mid in own_ids(symbol):
            if mid not in labels:
                continue
            mats = materials.of(pair, mid)
            if "unknown" in mats:
                continue
            img = pair.draw(mid)
            src = f"{name} {mid:#05x}"
            exact.setdefault(img.tobytes(), (mats, src))
            feats.append((_features(img), mats, src))
    return exact, feats


# Behaviours that say what a tile is (beyond materials.py's, which always apply).
_BEHAVIOR_HINTS = [
    ("COUNTER", "object"), ("_PC", "object"), ("MB_PC", "object"), ("TELEVISION", "object"),
    ("BOOKSHELF", "object"), ("SHOP_SHELF", "object"), ("VASE", "object"), ("TRASH_CAN", "object"),
    ("REGION_MAP", "object"), ("CABLE_BOX", "object"), ("BLUEPRINT", "object"), ("SLOT_MACHINE", "object"),
    ("ROULETTE", "object"), ("QUESTIONNAIRE", "object"), ("POKEBLOCK_FEEDER", "object"),
    ("LADDER", "stairs"), ("ESCALATOR", "stairs"), ("STAIRS", "stairs"),
    ("ICE", "ice"), ("MB_CAVE", "floor"), ("INDOOR_ENCOUNTER", "floor"), ("HOT_SPRINGS", "water"),
    ("ASHGRASS", "tallgrass"), ("FOOTPRINTS", "sand"), ("BERRY_TREE_SOIL", "path"),
    ("ARROW_WARP", "floor"), ("SECRET_BASE", "object"),
]


def suggest(symbol: str) -> list[tuple[int, frozenset, str, str]]:
    """(metatile, labels, source kind, note) for every metatile of a tileset."""
    exact, feats = reference()
    pair = pair_for(symbol)
    names = project.behaviors()
    c = project.consts()
    out = []
    for mid in own_ids(symbol):
        img = pair.draw(mid)
        key = img.tobytes()
        beh = names.get(pair.behavior(mid), "")
        base = set(materials.of(pair, mid)) - {"unknown"}
        hints = {mat for pat, mat in _BEHAVIOR_HINTS if pat in beh}
        if key in exact:
            mats, src = exact[key]
            out.append((mid, frozenset(set(mats) | base | hints), "dup", src))
            continue
        if hints or base:
            out.append((mid, frozenset(base | hints), "beh", beh))
            continue
        f = _features(img)
        best = min(feats, key=lambda r: sum((a - b) ** 2 for a, b in zip(f, r[0])))
        dist = math.sqrt(sum((a - b) ** 2 for a, b in zip(f, best[0])))
        # Collision matters for guesses: walkable art shouldn't come out as trees, nor walls as floor.
        out.append((mid, best[1], "guess", f"{best[2]} d={dist:.0f}"))
    return out


def write(symbol: str, rows, path) -> None:
    """A materials file for the suggestions, runs of equal labels merged."""
    name = symbol[len("gTileset_"):]
    lines = [
        f"# What each {symbol} metatile depicts. Suggested by label_assist.py",
        "# (dup = draws exactly like a labelled tile, beh = from its behaviour,",
        "# guess = closest-looking labelled tile), then reviewed by eye.",
        "",
    ]
    i = 0
    while i < len(rows):
        mid, mats, kind, note = rows[i]
        j = i
        while j + 1 < len(rows) and rows[j + 1][1] == mats and rows[j + 1][2] == kind and rows[j + 1][0] == rows[j][0] + 1:
            j += 1
        ids = f"{mid:#05x}" if i == j else f"{mid:#05x}-{rows[j][0]:#05x}"
        label = " ".join(sorted(m for m in mats if m not in set().union(*materials.FAMILIES.values()))) or "unknown"
        fams = sorted(m for m in mats if m in set().union(*materials.FAMILIES.values()))
        lines.append(f"{ids:<12}{label:<24}# {kind}: {note}" if i == j else f"{ids:<12}{label:<24}# {kind}")
        if fams:
            lines.append(f"{ids:<12}" + " ".join("+" + f for f in fams))
        i = j + 1
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("\n".join(lines) + "\n")
