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
    },
    "town": {
        "allow": {"grass", "tallgrass", "flowers", "tree", "water", "ledge", "bridge", "path", "fence", "building", "object"},
        "need": {"#": None, ".": None, '"': "tallgrass", "~": "water", "=": "bridge",
                 "^": "ledge", "v": "ledge", "<": "ledge", ">": "ledge"},
        "extra": {"~": {"cliff", "rock", "sand"}, "^": {"cliff"}, "v": {"cliff"}, "<": {"cliff"}, ">": {"cliff"}},
    },
}

# How much less a block of another class (but the same movement) is wanted.
CLASS_PENALTY = 0.01
# How much each earlier use of a block (up to 20) raises its score.
REUSE_BONUS = 0.5
# Blocks tried per cell before treating it as a dead end.
MAX_OPTIONS = 40
# Times one cell's surroundings are cleared and re-solved before it's forced.
MAX_REPAIRS = 0


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

        # A metatile's class, with the collision it usually has.
        self.classes = [
            emerald_symbol(target, c.pack(b, collision[i].most_common(1)[0][0] if collision[i] else 0))
            for i, b in enumerate(self.blocks)
        ]
        self.allow = [dict(a) for a in allow]
        self.pairs = pairs
        self.contexts = dict(contexts)
        self.class_mask: dict[str, int] = defaultdict(int)
        self.group_mask: dict[str, int] = defaultdict(int)
        for i, ch in enumerate(self.classes):
            self.class_mask[ch] |= 1 << i
            self.group_mask[GROUP_OF.get(ch, "walk")] |= 1 << i
        self.all_mask = (1 << len(self.blocks)) - 1
        self._support: dict = {}

    def support(self, d: int, domain: int) -> int:
        """Every block allowed in direction d of some block in domain."""
        key = (d, domain)
        hit = self._support.get(key)
        if hit is None:
            allow, hit, rest = self.allow[d], 0, domain
            while rest:
                low = rest & -rest
                hit |= allow.get(low.bit_length() - 1, 0)
                rest ^= low
            if len(self._support) > 200_000:
                self._support.clear()
            self._support[key] = hit
        return hit


@lru_cache(maxsize=None)
def model(primary: str, secondary: str) -> Model:
    return Model(primary, secondary)


class Contradiction(Exception):
    pass


def _bits(mask: int) -> list[int]:
    out = []
    while mask:
        low = mask & -mask
        out.append(low.bit_length() - 1)
        mask ^= low
    return out


def fill(grid: list[str], layout: dict, style: str = "route", passes: int = 2) -> tuple[Blockdata, Counter]:
    """Blocks for a class grid (rows of class characters, ' ' = outside the map).

    With passes=2 the grid is filled twice: the second pass starts out
    preferring the blocks the first pass used most, so the whole map settles
    on one style of tree and water instead of switching between areas.
    """
    prefer: Counter = Counter()
    for n in range(passes):
        blocks, tally, used = _fill(grid, layout, style, prefer)
        prefer = Counter({b: k for b, k in used.most_common(40)})
    return blocks, tally


def _fill(grid: list[str], layout: dict, style: str, prefer: Counter) -> tuple[Blockdata, Counter, Counter]:
    """One filling pass; see fill.

    Returns the blocks and a tally: which context level decided each cell,
    how many cells had to be relaxed to a looser class, and how much
    backtracking it took.
    """
    m = model(layout["primary_tileset"], layout["secondary_tileset"])
    h, w = len(grid), max(len(r) for r in grid)
    rows = [r.ljust(w) for r in grid]
    target = [rows[y][x] if rows[y][x] != " " else OUTSIDE for y in range(h) for x in range(w)]

    def get(x, y):
        return target[y * w + x] if 0 <= x < w and 0 <= y < h else OUTSIDE

    # Level 0: blocks that move the same way as the cell's class and are made
    # of what the style allows for it (a forest cell must show trees, and may
    # only show allowed materials). Level 1: the same movement, any material.
    # Level 2: anything. Cells the examples can't build at level 0 widen.
    level = [0] * (w * h)
    spec = STYLES[style]
    style_masks: dict[str, int] = {}

    def style_mask(ch: str) -> int:
        if ch not in style_masks:
            need = spec["need"].get(ch)
            ok = spec["allow"] | spec["extra"].get(ch, set())
            group = m.group_mask.get(GROUP_OF.get(ch, "walk"), 0)
            mask = 0
            for i in _bits(group):
                mats = m.materials[i]
                if mats <= ok and (need is None or need in mats):
                    mask |= 1 << i
            style_masks[ch] = mask
        return style_masks[ch]

    def base(i: int) -> int:
        ch = target[i]
        masks = [style_mask(ch), m.group_mask.get(GROUP_OF.get(ch, "walk"), 0), m.all_mask]
        for lv in range(level[i], 3):
            if masks[lv]:
                return masks[lv]
        return m.all_mask

    neighbours = []
    for i in range(w * h):
        x, y = i % w, i // w
        neighbours.append([(d, (y + dy) * w + x + dx) for d, (dx, dy) in enumerate(DIRS) if 0 <= x + dx < w and 0 <= y + dy < h])
    hard_neighbours = [[(d, n) for d, n in ns if d in HARD] for ns in neighbours]

    def propagate(domains: list[int], queue: list[int]) -> None:
        pending = set(queue)
        while queue:
            i = queue.pop()
            pending.discard(i)
            for d, n in hard_neighbours[i]:
                nd = domains[n] & m.support(d, domains[i])
                if nd != domains[n]:
                    if not nd:
                        raise Contradiction(n)
                    domains[n] = nd
                    if n not in pending:
                        pending.add(n)
                        queue.append(n)

    def relax_around(i: int) -> None:
        """Widen one cell; if it's already wide open, its neighbours."""
        if level[i] < 2:
            level[i] += 1
            return
        for _, n in hard_neighbours[i]:
            level[n] = min(2, level[n] + 1)

    # Make the starting domains consistent, widening where the shape can't be built.
    domains = [base(i) for i in range(w * h)]
    for _ in range(w * h * 3):
        domains = [base(i) for i in range(w * h)]
        try:
            propagate(domains, list(range(w * h)))
            break
        except Contradiction as e:
            relax_around(e.args[0])

    tally: Counter = Counter()
    tally["relaxed"] = sum(1 for lv in level if lv)

    def ranked(i: int, dom: int) -> tuple[str, list[int]]:
        """The blocks in a domain, best first, and the context level that ranked them.

        A block scores by how often it's used in this cell's class context,
        times how often it sits next to each neighbour already decided, with
        a heavy penalty for not being the cell's own class.
        """
        x, y = i % w, i // w
        name, wts = "none", {}
        for key in _keys(get, x, y):
            counts = m.contexts.get(key)
            if counts and (sum(counts.values()) >= MIN_EXAMPLES or key[0] == "c"):
                total = sum(counts.values())
                name, wts = key[0], {b: n / total for b, n in counts.items()}
                break
        decided = [(d, domains[n].bit_length() - 1) for d, n in neighbours[i] if domains[n].bit_count() == 1]
        want = target[i]

        def score(b: int) -> float:
            s = wts.get(b, 0.0) + 1e-6 * m.freq[b] / m.examples
            if m.classes[b] != want:
                s *= CLASS_PENALTY
            for d, nb in decided:
                s *= (m.pairs[d].get((b, nb), 0) + 0.01) / (m.freq[b] + 1)
            # Keep to the blocks the map already uses, so one tree and water style carries through.
            return s * (1 + REUSE_BONUS * min(used[b], 20))

        return name, sorted(_bits(dom), key=score, reverse=True)

    used: Counter = Counter(prefer)
    undecided = set(range(w * h))
    repairs: Counter = Counter()
    backtracks = 0

    def reset(centre: int, radius: int) -> None:
        """Undo every decision within radius of centre and make the area consistent again."""
        nonlocal domains
        cx, cy = centre % w, centre // w
        area = [y * w + x for y in range(max(0, cy - radius), min(h, cy + radius + 1))
                for x in range(max(0, cx - radius), min(w, cx + radius + 1))]
        # Undecided cells just outside were narrowed by the old decisions too.
        margin = [y * w + x for y in range(max(0, cy - radius - 2), min(h, cy + radius + 3))
                  for x in range(max(0, cx - radius - 2), min(w, cx + radius + 3))]
        area = set(area)
        loose = {i for i in margin if i in undecided}
        for _ in range(500):
            trial = domains[:]
            for i in area | loose:
                trial[i] = base(i)
            region = area | loose
            queue = list(region) + [n for i in region for _, n in hard_neighbours[i] if n not in region]
            try:
                propagate(trial, queue)
            except Contradiction as e:
                bad = e.args[0]
                if bad in region and level[bad] < 2:
                    relax_around(bad)
                elif bad in region:
                    # Even any block clashes: its fixed neighbours disagree, so re-solve them.
                    grown = {n for _, n in hard_neighbours[bad] if n not in region}
                    if not grown:
                        break
                    area |= {n for n in grown if n not in undecided}
                    loose |= {n for n in grown if n in undecided}
                elif bad in undecided:
                    loose.add(bad)  # narrowed by older decisions: start it afresh
                else:
                    area.add(bad)  # a decision outside clashes: re-solve it too
                continue
            for i in area:
                if i not in undecided:
                    used[domains[i].bit_length() - 1] -= 1
            domains = trial
            undecided.update(area)
            return
        # Couldn't make the area consistent: leave it as it was and let the cell be forced.
        repairs[centre] = MAX_REPAIRS

    while undecided:
        cell = min(undecided, key=lambda i: (domains[i].bit_count(), i))
        name, opts = ranked(cell, domains[cell])
        tally[name] += 1
        for pick in opts[:MAX_OPTIONS]:
            trial = domains[:]
            trial[cell] = 1 << pick
            try:
                propagate(trial, [cell])
            except Contradiction:
                backtracks += 1
                continue
            domains = trial
            undecided.discard(cell)
            used[pick] += 1
            break
        else:
            # A dead end: clear the area around it and solve it again, wider each time.
            repairs[cell] += 1
            if repairs[cell] <= MAX_REPAIRS:
                tally["repairs"] += 1
                reset(cell, min(1 + repairs[cell], 5))
                continue
            # Still stuck: take the block that agrees with the most neighbours and carry on.
            tally["forced"] += 1
            _, opts = ranked(cell, base(cell))
            decided = [(d, domains[n].bit_length() - 1) for d, n in hard_neighbours[cell] if domains[n].bit_count() == 1]

            def agreement(b):
                return sum(1 for d, nb in decided if (m.allow[d].get(b, 0) >> nb) & 1)

            best = max(opts[:200], key=agreement)
            domains = domains[:]
            domains[cell] = 1 << best
            undecided.discard(cell)
            used[best] += 1
            for d, n in hard_neighbours[cell]:
                if n in undecided:
                    nd = domains[n] & m.support(d, domains[cell])
                    if nd:
                        domains[n] = nd
    tally["backtracks"] = backtracks

    from blueprint import default_attrs

    c = consts()
    out = Blockdata(w, h)
    for i, dom in enumerate(domains):
        mid = m.blocks[dom.bit_length() - 1]
        out.blocks[i] = c.pack(mid, *default_attrs(mid, layout))
    used = Counter(dom.bit_length() - 1 for dom in domains)
    return out, tally, used
