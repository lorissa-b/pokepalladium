"""Turn a grid of tile classes into finished blocks, by example.

Most metatiles only fit next to particular others: a cliff corner needs the
right edge pieces beside it, a shoreline tile the right water and land
pieces. Rather than writing those rules down, this learns them from the
original Emerald layouts (original.py):

- Adjacency: every pair of blocks seen next to each other, in each of the 8
  directions, in parts of a map the player can see. Generated maps only use
  pairs that occur in a real map, so shapes come out whole.
- Preference: for each class and the classes around it, which blocks are
  used there and how often (a forest edge prefers tree tiles, a shore next
  to grass prefers grassy shore pieces).

Filling is constraint solving in the manner of Wave Function Collapse: every
cell starts with all blocks of its target class, each choice removes the
blocks its neighbours can no longer have, the most constrained cell is
decided next, and a dead end clears the area around it to be solved
again. Where the target shape can't
be built from Emerald's pieces at all (a one-tile tree line, say) the cells
may take any block that moves the same way (preferring its own class), so
the walkable shape is kept exactly while the art has room to fit; choices
also favour blocks often seen beside the neighbours already placed, so one
style of tree or cliff carries on. Cells that still can't be built widen to
any block.
"""

from __future__ import annotations

import math
from collections import Counter, defaultdict
from functools import lru_cache

import materials
import original
from compare import GROUP_OF, emerald_symbol
from project import Blockdata, consts
from tileset import TilesetPair, UnknownTileset

OUTSIDE = "#"
DIRS = [(-1, -1), (0, -1), (1, -1), (-1, 0), (1, 0), (-1, 1), (0, 1), (1, 1)]
N4 = [(0, -1), (1, 0), (0, 1), (-1, 0)]
# Pairs side by side are hard rules; diagonal pairs only count towards the score.
HARD = {i for i, d in enumerate(DIRS) if 0 in d}
N8 = [(-1, -1), (0, -1), (1, -1), (1, 0), (1, 1), (0, 1), (-1, 1), (-1, 0)]

# A context is trusted once it has been seen this often.
MIN_EXAMPLES = 3
# What a map may be built from. `allow` is every material a block may show;
# `need` names the material a cell of each class must show, and `extra`
# what else that class may use (so shores can have rocky edges and ledges
# their cliff faces). Classes not listed only keep their movement.
STYLES = {
    "route": {
        "allow": {"grass", "tallgrass", "tree", "water", "ledge", "bridge", "path"},
        "need": {"#": "tree", ".": "grass", "p": "path", '"': "tallgrass", "Y": "tallgrass", "~": "water", "|": "water",
                 ",": "water", ":": "sand", "^": "ledge", "v": "ledge", "<": "ledge", ">": "ledge", "=": "bridge"},
        "extra": {"~": {"cliff", "rock", "sand"}, "|": {"cliff"}, ":": {"sand"}, "^": {"cliff"}, "v": {"cliff"},
                  "<": {"cliff"}, ">": {"cliff"}, "=": {"water", "path"},
                  # Path pieces show the sand or grass at their edges (Littleroot's sand pit).
                  "p": {"sand", "grass"}},
        # What a cell may fall back on, at a cost, where its own materials can't fit.
        "fallback": {"cliff", "rock", "sand"},
    },
    "town": {
        # No "building": buildings are placed whole beforehand (buildings.py), never pieced together.
        "allow": {"grass", "tallgrass", "flowers", "tree", "water", "ledge", "bridge", "path", "fence", "object"},
        "need": {"#": None, ".": None, "p": "path", '"': "tallgrass", "~": "water", "=": "bridge",
                 "^": "ledge", "v": "ledge", "<": "ledge", ">": "ledge"},
        "extra": {"~": {"cliff", "rock", "sand"}, "^": {"cliff"}, "v": {"cliff"}, "<": {"cliff"}, ">": {"cliff"},
                  "p": {"sand", "grass"}},
        "fallback": {"cliff", "rock", "sand"},
    },
    "cave": {
        # Emerald's caves fill solid rock with the cave set's raised floor and edge it with wall
        # faces and ridges, so a wall cell may be floor, cliff or rock. Only water comes from the
        # primary tileset: its cliffs and mountain tops are outdoor blocks.
        "allow": {"floor", "cliff", "rock", "water", "ledge", "stairs", "cave", "dark"},
        "need": {"#": None, ".": "floor", "~": "water", ",": "water", "^": "ledge", "v": "ledge",
                 "<": "ledge", ">": "ledge"},
        "extra": {"~": {"cliff", "rock"}, ",": {"floor"}, "^": {"cliff"}, "v": {"cliff"},
                  "<": {"cliff"}, ">": {"cliff"}},
        "fallback": {"rock", "floor", "dark"},
        "primary": {"water"},
        # Solid cells with nothing but solid within this many blocks take the block the
        # originals most often put inside solid rock, so wall faces only line the edges.
        "deep": 2,
    },
}



def _context(get, x: int, y: int, offsets) -> str:
    return "".join(get(x + dx, y + dy) for dx, dy in offsets)


def _keys(get, x: int, y: int):
    centre = get(x, y)
    return [("n8", centre, _context(get, x, y, N8)), ("n4", centre, _context(get, x, y, N4)), ("c", centre)]


class Model:
    """Blocks, their classes, which may sit next to which, and context preferences."""

    def __init__(self, primary: str, secondary: str) -> None:
        c = consts()
        self.index: dict[int, int] = {}
        self.blocks: list[int] = []
        self.classes: list[str] = []
        self.materials: list[frozenset[str]] = []
        self.freq: Counter = Counter()
        allow: list[dict[int, int]] = [defaultdict(int) for _ in DIRS]
        pairs: list[Counter] = [Counter() for _ in DIRS]
        contexts: dict = defaultdict(Counter)
        collision: dict[int, Counter] = defaultdict(Counter)
        patches: Counter = Counter()
        self.examples = 0
        self.maps = 0
        target = TilesetPair(primary, secondary)

        for layout, blocks in original.layouts():
            if layout["primary_tileset"] != primary:
                continue
            try:
                tiles = TilesetPair.for_layout(layout)
            except UnknownTileset:
                continue
            self.maps += 1
            same_secondary = layout["secondary_tileset"] == secondary
            w, h = blocks.width, blocks.height
            seen = original.visible(blocks)
            classes = [emerald_symbol(tiles, b) for b in blocks.blocks]

            def get(x, y):
                return classes[y * w + x] if 0 <= x < w and 0 <= y < h else OUTSIDE

            def idx(x, y):
                """The block's index if it's visible and the target can use it, else None."""
                if not (0 <= x < w and 0 <= y < h and seen[y][x]):
                    return None
                # Collision and elevation are filled in afterwards, so learn on metatiles alone.
                b = blocks.get(x, y) & c.metatile_mask
                if not same_secondary and b >= c.metatiles_in_primary:
                    return None
                if b not in self.index:
                    if not target.exists(b):
                        return None
                    self.index[b] = len(self.blocks)
                    self.blocks.append(b)
                    self.materials.append(materials.of(target, b))
                i = self.index[b]
                collision[i][c.unpack(blocks.get(x, y))[1:]] += 1
                return i

            for y in range(h):
                for x in range(w):
                    i = idx(x, y)
                    if i is None:
                        continue
                    self.examples += 1
                    self.freq[i] += 1
                    for d, (dx, dy) in enumerate(DIRS):
                        j = idx(x + dx, y + dy)
                        if j is not None:
                            if d in HARD:
                                allow[d][i] |= 1 << j
                            pairs[d][(i, j)] += 1
                    for key in _keys(get, x, y):
                        contexts[key][i] += 1
                    # 2x2 patches (this block at the top-left), for moves that keep shapes whole.
                    r, d, rd = idx(x + 1, y), idx(x, y + 1), idx(x + 1, y + 1)
                    if r is not None and d is not None and rd is not None:
                        patches[(i, r, d, rd)] += 1

        # A metatile's class, with the collision it usually has.
        # Each block's usual collision and elevation in the originals: what its class is
        # learned with, so also what a draft writes it with.
        self.attrs = [collision[i].most_common(1)[0][0] if collision[i] else None for i in range(len(self.blocks))]
        self.classes = [emerald_symbol(target, c.pack(b, a[0] if a else 0)) for b, a in zip(self.blocks, self.attrs)]
        self.allow = [dict(a) for a in allow]
        self.pairs = pairs
        self.patches = patches
        self.contexts = dict(contexts)
        self.class_mask: dict[str, int] = defaultdict(int)
        self.group_mask: dict[str, int] = defaultdict(int)
        for i, ch in enumerate(self.classes):
            self.class_mask[ch] |= 1 << i
            self.group_mask[GROUP_OF.get(ch, "walk")] |= 1 << i
        self.all_mask = (1 << len(self.blocks)) - 1
        self._target = target

    def index_of(self, metatile: int) -> int:
        """The index of a metatile, adding it (with no examples) if the originals never showed it."""
        if metatile not in self.index:
            c = consts()
            self.index[metatile] = len(self.blocks)
            self.blocks.append(metatile)
            self.materials.append(materials.of(self._target, metatile))
            self.attrs.append(None)
            self.classes.append(emerald_symbol(self._target, c.pack(metatile, 0, 0)))
        return self.index[metatile]


def _kind(ch: str) -> str | None:
    """What a class means for getting around: its movement group (ledges by direction), None if solid."""
    group = GROUP_OF.get(ch, "walk")
    if group in ("block", "none"):
        return None
    return "ledge" + ch if group == "ledge" else group


def _access(kinds: list[str | None], w: int, h: int, barriers: set[int]):
    """Connected areas of each kind, and which areas touch (4-neighbours).

    Barrier cells (Platinum's objects: cut trees, boulders, people) are each an
    area of their own, so a way around one counts as a new connection.
    """
    label = [-1] * (w * h)
    n = 0
    for s0 in range(w * h):
        if kinds[s0] is None or label[s0] >= 0:
            continue
        label[s0] = n
        if s0 not in barriers:
            stack = [s0]
            while stack:
                i = stack.pop()
                x, y = i % w, i // w
                for dx, dy in N4:
                    if 0 <= x + dx < w and 0 <= y + dy < h:
                        j = i + dy * w + dx
                        if label[j] < 0 and j not in barriers and kinds[j] == kinds[s0]:
                            label[j] = n
                            stack.append(j)
        n += 1
    touches = set()
    for i in range(w * h):
        if label[i] < 0:
            continue
        x, y = i % w, i // w
        for j in ((i + 1) if x + 1 < w else -1, (i + w) if y + 1 < h else -1):
            if j >= 0 and label[j] >= 0 and label[j] != label[i]:
                touches.add((min(label[i], label[j]), max(label[i], label[j])))
    return label, touches


def _sealed(label: list[int], touches: set[tuple[int, int]]) -> set[int]:
    """Areas that touch no other area: nothing can reach them (the walkable top row of a roof)."""
    touching = {a for pair in touches for a in pair}
    return {a for a in set(label) if a >= 0 and a not in touching}


def _same_access(before: list[str | None], after: list[str | None], w: int, h: int, barriers: set[int],
                 reference=None) -> bool:
    """Whether two grids give the same areas with the same connections.

    Nothing joined, split, lost or new, and no area gaining or losing a border
    with water, a ledge or anything else. reference: _access(before) if known.
    """
    old, old_touch = reference or _access(before, w, h, barriers)
    new, new_touch = _access(after, w, h, barriers)
    sealed = _sealed(new, new_touch)
    to_old: dict[int, set[int]] = defaultdict(set)
    to_new: dict[int, set[int]] = defaultdict(set)
    for i in range(w * h):
        if new[i] >= 0 and old[i] >= 0 and before[i] == after[i]:
            to_old[new[i]].add(old[i])
            to_new[old[i]].add(new[i])
    if any(new[i] >= 0 and new[i] not in sealed and len(to_old.get(new[i], ())) != 1 for i in range(w * h)):
        return False  # two areas joined, or a new one made of swapped cells alone
    old_sealed = _sealed(old, old_touch)
    if any(len(ns) != 1 for ns in to_new.values()) or any(o not in to_new and o not in old_sealed for o in set(old) if o >= 0):
        return False  # an area split, or one gone (one nothing could reach doesn't matter)
    as_old = {a: next(iter(o)) for a, o in to_old.items()}
    mapped = {(min(as_old[a], as_old[b]), max(as_old[a], as_old[b])) for a, b in new_touch}
    return mapped == old_touch


def access_differences(before: list[str | None], after: list[str | None], w: int, h: int,
                       barriers: set[int]) -> list[str]:
    """How access differs between two grids of movement kinds (see _kind), one line per difference.

    Empty when they're the same: each area of each kind is still one area,
    nothing is joined, split, lost or new, and every area touches the same
    others. A new area touching nothing (the walkable top row of a roof) is
    out of reach, so it doesn't count.
    """
    old, old_touch = _access(before, w, h, barriers)
    new, new_touch = _access(after, w, h, barriers)
    cells_old: dict[int, list[int]] = defaultdict(list)
    cells_new: dict[int, list[int]] = defaultdict(list)
    for i in range(w * h):
        if old[i] >= 0:
            cells_old[old[i]].append(i)
        if new[i] >= 0:
            cells_new[new[i]].append(i)
    to_old: dict[int, set[int]] = defaultdict(set)
    to_new: dict[int, set[int]] = defaultdict(set)
    for i in range(w * h):
        if new[i] >= 0 and old[i] >= 0 and before[i] == after[i]:
            to_old[new[i]].add(old[i])
            to_new[old[i]].add(new[i])

    def where(cells) -> str:
        i = min(cells)
        return f"({i % w},{i // w})"

    def name(kinds, cells) -> str:
        return f"{kinds[cells[0]]} area at {where(cells)} ({len(cells)} tiles)"

    out = []
    sealed = _sealed(new, new_touch)
    for a, cells in cells_new.items():
        olds = to_old.get(a, set())
        if not olds and a in sealed:
            continue  # nothing can reach it
        if not olds:
            out.append(f"new {name(after, cells)}")
        elif len(olds) > 1:
            out.append(f"joined: {', '.join(name(before, cells_old[o]) for o in sorted(olds))}")
    old_sealed = _sealed(old, old_touch)
    for o, cells in cells_old.items():
        news = to_new.get(o, set())
        if not news and o in old_sealed:
            continue  # nothing could reach it
        if not news:
            out.append(f"gone: {name(before, cells)}")
        elif len(news) > 1:
            out.append(f"split: {name(before, cells)} into {len(news)}")
    if out:
        return out
    as_old = {a: next(iter(o)) for a, o in to_old.items()}
    mapped = {(min(as_old[a], as_old[b]), max(as_old[a], as_old[b])) for a, b in new_touch}
    for a, b in sorted(mapped - old_touch):
        out.append(f"now touch: {name(before, cells_old[a])} and {name(before, cells_old[b])}")
    for a, b in sorted(old_touch - mapped):
        out.append(f"no longer touch: {name(before, cells_old[a])} and {name(before, cells_old[b])}")
    return out


def _accept_swaps(before: list[str | None], after: list[str | None], w: int, h: int, barriers: set[int],
                  swapped: set[int]) -> set[int]:
    """The swaps to undo: each group of touching swaps is kept only if access stays the same with it."""
    reference = _access(before, w, h, barriers)
    left, groups = set(swapped), []
    while left:
        start = left.pop()
        group, stack = {start}, [start]
        while stack:
            i = stack.pop()
            x, y = i % w, i // w
            for dx, dy in DIRS:
                j = i + dy * w + dx
                if 0 <= x + dx < w and 0 <= y + dy < h and j in left:
                    left.remove(j)
                    group.add(j)
                    stack.append(j)
        groups.append(group)
    base = list(before)
    undo: set[int] = set()
    for group in sorted(groups, key=lambda g: (len(g), min(g))):
        trial = list(base)
        for i in group:
            trial[i] = after[i]
        if _same_access(before, trial, w, h, barriers, reference):
            base = trial
            continue
        # The group as a whole changes access: keep what can be kept of it, one cell at a time.
        for i in sorted(group):
            trial = list(base)
            trial[i] = after[i]
            if _same_access(before, trial, w, h, barriers, reference):
                base = trial
            else:
                undo.add(i)
    return undo


@lru_cache(maxsize=None)
def model(primary: str, secondary: str) -> Model:
    return Model(primary, secondary)


def _bits(mask: int) -> list[int]:
    out = []
    while mask:
        low = mask & -mask
        out.append(low.bit_length() - 1)
        mask ^= low
    return out


# Costs. Lower is better; a unit is roughly "e times less likely".
DIAGONAL_WEIGHT = 0.5    # diagonal neighbours count half as much as side-by-side ones
OFF_STYLE_COST = 8.0     # a block that isn't the style's material or family for its cell
SWEEPS = 12              # improvement passes over the whole map (stops early when settled)
MAX_CANDIDATES = 400     # blocks considered per cell, preferred ones first
PATCH_CANDIDATES = 300   # 2x2 patches tried per problem spot, most common first
PATCH_ROUNDS = 6         # rounds of patch moves over the problem spots
FLIP_COST = 4.0          # a flexible cell made walkable or blocked against Platinum, to finish a sprite
FLIPPABLE = "#.="        # classes a flexible cell may swap (# and . with each other, = to water)
ACCESS_ROUNDS = 4        # rounds of undoing swaps that change access before undoing them all


def fill(grid: list[str], layout: dict, style: str = "route", trees: str | None = "dense",
         water: str | None = "sea", path: str | None = None, fixed: dict[tuple[int, int], int] | None = None,
         flexible: set[tuple[int, int]] | None = None, barriers: set[tuple[int, int]] | None = None,
         ) -> tuple[Blockdata, Counter, list[tuple[int, int]]]:
    """Blocks for a class grid (rows of class characters, ' ' = outside the map).

    Picks, for every cell, a block that moves the same way as its class,
    minimising the total cost of blocks unusual for their surroundings and
    of neighbours that rarely or never sit together in the original maps.
    A greedy pass lays out a first version; then each block is repeatedly
    swapped for whichever option lowers the cost given all its neighbours,
    until nothing changes. Then, around every cell still off-style or beside
    a block it's never seen next to, whole 2x2 patches from the original maps
    are tried, since a tree or a shoreline can't move one block at a time.
    Where Platinum's shape can't be built exactly, an area settles on the
    least-bad combination.

    trees/water/path pick the family the style keeps to (None: any). fixed: blocks
    already decided (buildings, see buildings.py), by (x, y); they're kept as
    they are, collision and elevation included, and the rest fits around them.

    flexible: cells that may be made walkable or blocked against their class
    (for FLIP_COST each) when that finishes a sprite Platinum's shape cuts in
    half, such as a tree line one block thick. Only blocked and walkable
    cells swap, and access must come out exactly as it went in: the same
    areas of each kind (walking, water, each ledge direction...) touching
    the same others, with each of `barriers` (Platinum's objects: cut trees,
    boulders, people) an area of its own so nothing gains a way around one.
    Swaps near any change are undone, and all of them if that doesn't do it.

    Returns the blocks, a tally (context levels used, sweeps, seams,
    off-style cells) and the seams: cells beside a block they never sit
    next to in the original maps, which are the places to check by eye.
    """
    m = model(layout["primary_tileset"], layout["secondary_tileset"])
    h, w = len(grid), max(len(r) for r in grid)
    rows = [r.ljust(w) for r in grid]
    target = [rows[y][x] if rows[y][x] != " " else OUTSIDE for y in range(h) for x in range(w)]
    chosen = {"tree": trees, "water": water, "path": path}
    spec = STYLES[style]
    family_tags = set().union(*materials.FAMILIES.values())
    tally: Counter = Counter()

    def get(x, y):
        return target[y * w + x] if 0 <= x < w and 0 <= y < h else OUTSIDE

    def from_primary_ok(i: int) -> bool:
        """A style with a "primary" set only takes primary-tileset blocks showing one of those materials."""
        if "primary" not in spec or m.blocks[i] >= consts().metatiles_in_primary:
            return True
        return bool(m.materials[i] & spec["primary"])

    # Candidates: the cell's movement group. Preferred: also the style's materials and families.
    preferred: dict[str, set[int]] = {}
    candidates: dict[str, list[int]] = {}
    for ch in set(target):
        need = spec["need"].get(ch)
        ok = spec["allow"] | spec["extra"].get(ch, set())
        group = _bits(m.group_mask.get(GROUP_OF.get(ch, "walk"), 0)) or list(range(len(m.blocks)))
        good = set()
        for i in group:
            mats = m.materials[i]
            core = mats - family_tags
            if not core <= ok or (need is not None and need not in core) or not from_primary_ok(i):
                continue
            if any(fam and materials.family(mats, mat) not in (None, fam) for mat, fam in chosen.items()):
                continue
            good.add(i)
        preferred[ch] = good
        # Off-style blocks stay possible (at a cost) so impossible shapes still get the nearest
        # fit, but only natural ones: never buildings, signs or paving on a route.
        fallback = spec["allow"] | spec.get("fallback", set())
        rest = [i for i in group if i not in good and (m.materials[i] - family_tags) <= fallback
                and from_primary_ok(i)]
        by_use = sorted(good, key=lambda i: -m.freq[i]) + sorted(rest, key=lambda i: -m.freq[i])
        candidates[ch] = by_use[:MAX_CANDIDATES] or group[:MAX_CANDIDATES]

    # Deep inside solid ground (a style with "deep"), use the originals' usual filling.
    deep = spec.get("deep")
    if deep and preferred.get("#"):
        inside = m.contexts.get(("n8", "#", "#" * 8), Counter())
        block = max(preferred["#"], key=lambda b: (inside.get(b, 0), m.freq[b]))
        packed = consts().pack(m.blocks[block], *(m.attrs[block] or (1, 0)))
        fixed = dict(fixed or {})
        for y in range(h):
            for x in range(w):
                if (x, y) in fixed or (flexible and (x, y) in flexible):
                    continue
                if all(get(x + dx, y + dy) in ("#", OUTSIDE)
                       for dy in range(-deep, deep + 1) for dx in range(-deep, deep + 1)):
                    fixed[(x, y)] = packed
        tally["deep"] = sum(1 for v in fixed.values() if v == packed)

    def context_costs(i: int) -> dict[int, float]:
        x, y = i % w, i // w
        for key in _keys(get, x, y):
            counts = m.contexts.get(key)
            if counts and (sum(counts.values()) >= MIN_EXAMPLES or key[0] == "c"):
                tally[key[0]] += 1
                total = sum(counts.values())
                return {b: -math.log(n / total) for b, n in counts.items()}
        tally["none"] += 1
        return {}

    unseen_context = -math.log(1e-4)
    ctx = [context_costs(i) for i in range(w * h)]
    pref = [preferred[target[i]] for i in range(w * h)]
    cands = [candidates[target[i]] for i in range(w * h)]
    want_group = [GROUP_OF.get(ch, "walk") for ch in target]

    # Flexible cells may take the other class too, at a cost (its context as if it were that class).
    other = {"#": ".", ".": "#", "p": "#", "=": "~"}  # a bridge lane may go back to the water it crosses
    alt_ctx: dict[int, dict[int, float]] = {}

    def make_flexible(i: int) -> None:
        ch = target[i]
        alt = other[ch]
        if alt not in candidates:
            _add_class(alt)
        target[i] = alt
        alt_ctx[i] = context_costs(i)
        target[i] = ch
        cands[i] = list(dict.fromkeys(candidates[ch] + candidates[alt]))
        pref[i] = preferred[ch] | preferred[alt]

    def _add_class(ch: str) -> None:
        need = spec["need"].get(ch)
        ok = spec["allow"] | spec["extra"].get(ch, set())
        group = _bits(m.group_mask.get(GROUP_OF.get(ch, "walk"), 0))
        good = {i for i in group if (m.materials[i] - family_tags) <= ok
                and (need is None or need in m.materials[i] - family_tags)
                and not any(fam and materials.family(m.materials[i], mat) not in (None, fam) for mat, fam in chosen.items())}
        preferred[ch] = good
        candidates[ch] = sorted(good, key=lambda i: -m.freq[i])[:MAX_CANDIDATES]

    for x, y in flexible or ():
        if 0 <= x < w and 0 <= y < h and target[y * w + x] in FLIPPABLE and target[y * w + x] in other:
            make_flexible(y * w + x)

    neighbours = []
    for i in range(w * h):
        x, y = i % w, i // w
        neighbours.append([(d, (y + dy) * w + x + dx) for d, (dx, dy) in enumerate(DIRS)
                           if 0 <= x + dx < w and 0 <= y + dy < h])
    weight = [1.0 if d in HARD else DIAGONAL_WEIGHT for d in range(len(DIRS))]
    pair_cache: dict = {}

    def pair_cost(d: int, a: int, b: int) -> float:
        """Cost of block b sitting in direction d of block a."""
        key = (d, a, b)
        hit = pair_cache.get(key)
        if hit is None:
            n = m.pairs[d].get((a, b), 0)
            hit = -math.log((n + 0.05) / (min(m.freq[a], m.freq[b]) + 1))
            pair_cache[key] = hit
        return hit

    def cost(i: int, b: int, assign: list) -> float:
        if GROUP_OF.get(m.classes[b], "walk") != want_group[i]:
            c = FLIP_COST + alt_ctx.get(i, {}).get(b, unseen_context)
        else:
            c = ctx[i].get(b, unseen_context)
        if b not in pref[i]:
            c += OFF_STYLE_COST
        for d, n in neighbours[i]:
            nb = assign[n]
            if nb is not None:
                c += weight[d] * pair_cost(d, b, nb)
        return c

    # A first version in reading order: each cell sees its decided west and north neighbours
    # (and every fixed block).
    cc = consts()
    assign: list = [None] * (w * h)
    pinned = set()
    for (x, y), block in (fixed or {}).items():
        if 0 <= x < w and 0 <= y < h:
            i = y * w + x
            assign[i] = m.index_of(block & cc.metatile_mask)
            pinned.add(i)
    free = [i for i in range(w * h) if i not in pinned]
    for i in free:
        assign[i] = min(cands[i], key=lambda b: cost(i, b, assign))

    # Improve: swap each block for its best option given all its neighbours, until settled.
    for sweep in range(SWEEPS):
        changed = 0
        for i in free:
            current = cost(i, assign[i], assign)
            best = min(cands[i], key=lambda b: cost(i, b, assign))
            if cost(i, best, assign) < current - 1e-9:
                assign[i] = best
                changed += 1
        tally["sweeps"] = sweep + 1
        if not changed:
            break

    def seam(i: int) -> bool:
        """Beside a block it's never seen next to (two fixed blocks are left alone)."""
        return any(d in HARD and not m.pairs[d].get((assign[i], assign[n])) and not (i in pinned and n in pinned)
                   for d, n in neighbours[i])

    def is_problem(i: int) -> bool:
        return i not in pinned and (assign[i] not in pref[i] or seam(i))

    # Patch moves: a tree or a shoreline is several blocks, so single swaps can't move it.
    # Around each problem cell, try whole 2x2 patches from the original maps that fit
    # the four cells' classes and the style, and keep the cheapest.
    patches_for: dict[tuple[str, ...], list] = {}

    def fitting(sig, cells):
        if sig not in patches_for:
            allowed = [pref[c] for c in cells]
            found = [p for p, n in m.patches.most_common() if all(p[k] in allowed[k] for k in range(4))]
            patches_for[sig] = found[:PATCH_CANDIDATES]
        return patches_for[sig]

    def window_cost(cells, values) -> float:
        saved = [assign[c] for c in cells]
        for c, v in zip(cells, values):
            assign[c] = v
        total = sum(cost(c, assign[c], assign) for c in cells)
        for c, v in zip(cells, saved):
            assign[c] = v
        return total

    for _ in range(PATCH_ROUNDS):
        improved = 0
        problems = [i for i in range(w * h) if is_problem(i)]
        windows = {(x, y) for i in problems for x in (i % w - 1, i % w) for y in (i // w - 1, i // w)
                   if 0 <= x < w - 1 and 0 <= y < h - 1}
        for x, y in sorted(windows):
            cells = (y * w + x, y * w + x + 1, (y + 1) * w + x, (y + 1) * w + x + 1)
            if any(cell in pinned for cell in cells):
                continue
            sig = tuple(target[c] for c in cells)
            current = window_cost(cells, [assign[c] for c in cells])
            best, best_cost = None, current - 1e-9
            for p in fitting(sig, cells):
                pc = window_cost(cells, p)
                if pc < best_cost:
                    best, best_cost = p, pc
            if best is not None:
                for c, v in zip(cells, best):
                    assign[c] = v
                improved += 1
        tally["patch_moves"] += improved
        if not improved:
            break

    # Swaps must leave every area reachable exactly as in Platinum: undo those near any
    # change in the areas or what they touch, settle those spots again, and if that
    # doesn't do it, undo every swap.
    before_kinds = [_kind(ch) for ch in target]
    walls = {y * w + x for x, y in barriers or () if 0 <= x < w and 0 <= y < h}

    def settle() -> None:
        for _ in range(SWEEPS):
            changed = 0
            for i in free:
                best = min(cands[i], key=lambda b: cost(i, b, assign))
                if cost(i, best, assign) < cost(i, assign[i], assign) - 1e-9:
                    assign[i] = best
                    changed += 1
            if not changed:
                break

    def unflex(cells) -> None:
        for i in cells:
            ch = target[i]
            cands[i], pref[i] = candidates[ch], preferred[ch]
            alt_ctx.pop(i, None)
            assign[i] = min(cands[i], key=lambda b: cost(i, b, assign))

    for attempt in range(ACCESS_ROUNDS + 1):
        after_kinds = [_kind(m.classes[b]) if i in alt_ctx else before_kinds[i] for i, b in enumerate(assign)]
        swapped = {i for i in alt_ctx if after_kinds[i] != before_kinds[i]}
        if not swapped:
            break
        bad = _accept_swaps(before_kinds, after_kinds, w, h, walls, swapped)
        if not bad:
            break
        unflex(set(alt_ctx) if attempt == ACCESS_ROUNDS else bad)
        settle()
    tally["flipped"] = sum(1 for i in free if GROUP_OF.get(m.classes[assign[i]], "walk") != want_group[i])

    seams = [(i % w, i // w) for i in range(w * h) if seam(i)]
    tally["seams"] = len(seams)
    tally["off_style"] = sum(1 for i in free if assign[i] not in pref[i])

    from blueprint import default_attrs

    out = Blockdata(w, h)
    for i, b in enumerate(assign):
        mid = m.blocks[b]
        # Written as the originals have it, so it moves the way it was chosen to.
        out.blocks[i] = cc.pack(mid, *(m.attrs[b] or default_attrs(mid, layout)))
    for (x, y), block in (fixed or {}).items():
        if 0 <= x < w and 0 <= y < h:
            out.blocks[y * w + x] = block
    return out, tally, seams
