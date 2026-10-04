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
        "allow": {"grass", "tallgrass", "tree", "water", "ledge", "bridge"},
        "need": {"#": "tree", ".": "grass", '"': "tallgrass", "Y": "tallgrass", "~": "water", "|": "water",
                 ",": "water", ":": "sand", "^": "ledge", "v": "ledge", "<": "ledge", ">": "ledge", "=": "bridge"},
        "extra": {"~": {"cliff", "rock", "sand"}, "|": {"cliff"}, ":": {"sand"}, "^": {"cliff"}, "v": {"cliff"},
                  "<": {"cliff"}, ">": {"cliff"}, "=": {"water", "path"}},
        # What a cell may fall back on, at a cost, where its own materials can't fit.
        "fallback": {"cliff", "rock", "sand"},
    },
    "town": {
        # No "building": buildings are placed whole beforehand (buildings.py), never pieced together.
        "allow": {"grass", "tallgrass", "flowers", "tree", "water", "ledge", "bridge", "path", "fence", "object"},
        "need": {"#": None, ".": None, '"': "tallgrass", "~": "water", "=": "bridge",
                 "^": "ledge", "v": "ledge", "<": "ledge", ">": "ledge"},
        "extra": {"~": {"cliff", "rock", "sand"}, "^": {"cliff"}, "v": {"cliff"}, "<": {"cliff"}, ">": {"cliff"}},
        "fallback": {"cliff", "rock", "sand"},
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
                collision[i][c.unpack(blocks.get(x, y))[1]] += 1
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
        self.classes = [
            emerald_symbol(target, c.pack(b, collision[i].most_common(1)[0][0] if collision[i] else 0))
            for i, b in enumerate(self.blocks)
        ]
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
            self.classes.append(emerald_symbol(self._target, c.pack(metatile, 0, 0)))
        return self.index[metatile]


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


def fill(grid: list[str], layout: dict, style: str = "route", trees: str | None = "dense",
         water: str | None = "sea", fixed: dict[tuple[int, int], int] | None = None
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

    trees/water pick the family the style keeps to (None: any). fixed: blocks
    already decided (buildings, see buildings.py), by (x, y); they're kept as
    they are, collision and elevation included, and the rest fits around them.

    Returns the blocks, a tally (context levels used, sweeps, seams,
    off-style cells) and the seams: cells beside a block they never sit
    next to in the original maps, which are the places to check by eye.
    """
    m = model(layout["primary_tileset"], layout["secondary_tileset"])
    h, w = len(grid), max(len(r) for r in grid)
    rows = [r.ljust(w) for r in grid]
    target = [rows[y][x] if rows[y][x] != " " else OUTSIDE for y in range(h) for x in range(w)]
    chosen = {"tree": trees, "water": water}
    spec = STYLES[style]
    family_tags = set().union(*materials.FAMILIES.values())
    tally: Counter = Counter()

    def get(x, y):
        return target[y * w + x] if 0 <= x < w and 0 <= y < h else OUTSIDE

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
            if not core <= ok or (need is not None and need not in core):
                continue
            if any(fam and materials.family(mats, mat) not in (None, fam) for mat, fam in chosen.items()):
                continue
            good.add(i)
        preferred[ch] = good
        # Off-style blocks stay possible (at a cost) so impossible shapes still get the nearest
        # fit, but only natural ones: never buildings, signs or paving on a route.
        fallback = spec["allow"] | spec.get("fallback", set())
        rest = [i for i in group if i not in good and (m.materials[i] - family_tags) <= fallback]
        by_use = sorted(good, key=lambda i: -m.freq[i]) + sorted(rest, key=lambda i: -m.freq[i])
        candidates[ch] = by_use[:MAX_CANDIDATES] or group[:MAX_CANDIDATES]

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

    seams = [(i % w, i // w) for i in range(w * h) if seam(i)]
    tally["seams"] = len(seams)
    tally["off_style"] = sum(1 for i in free if assign[i] not in pref[i])

    from blueprint import default_attrs

    out = Blockdata(w, h)
    for i, b in enumerate(assign):
        mid = m.blocks[b]
        out.blocks[i] = cc.pack(mid, *default_attrs(mid, layout))
    for (x, y), block in (fixed or {}).items():
        if 0 <= x < w and 0 <= y < h:
            out.blocks[y * w + x] = block
    return out, tally, seams
