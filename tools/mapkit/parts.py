"""New buildings from the parts of Emerald's: any size, doors anywhere.

Emerald's tilesets hold more than the buildings its maps draw: roof, wall,
window and corner parts that join in more ways than the originals ever use.
make() builds a building of a given size, with its doors where they're
wanted, from the parts of a seed building (buildings.Piece) and the parts
that fit with it.

Which parts join. Two blocks may sit side by side (or one above the other)
when some original map has them so, or when their shared edge looks like it
does in a pair that does: the right edge of A is drawn like the right edge
of a block seen to the left of B, or the left edge of B like the left edge
of a block seen to the right of A; or the art runs on across the join, the
pixels either side of it differing no more than neighbouring pixels inside
each block do. Shingles, wall boards and window frames continue across such
joins; a sign or an emblem cut in half doesn't. The cost of a join is 0 for
a pair seen in the originals and otherwise the smallest of those
differences (mean per-channel difference along the 16-pixel edge) over
EDGE_SCALE; joins costing over JOIN_MAX are bad.

Building one. The seed's rows are laid out to the new height and then its
columns to the new width, each by dynamic programming over sequences of
rows/columns (the first and last stay first and last, repeats and skips
allowed) so that joins cost least. Rows come from the seed, in order, none
running more than twice (buildings grow by storeys; a roof made taller than
its walls looks wrong). Columns come from the seed and from its relatives
drawn the same height (buildings sharing three or more parts with it), since
a wider house is often the same parts in another order; door columns go
exactly where doors are asked for and nowhere else. Blocks that appear only once in any original building (signs,
emblems) are never used twice. Then each block may be swapped for any part
of the seed's family (the seed, buildings sharing parts with it, and
building blocks whose edges fit them), which fixes joins the sequence alone
couldn't and bricks up doors that came along with a repeated column. A
result with any bad join left is refused.
"""

from __future__ import annotations

import math
from collections import Counter, defaultdict
from functools import lru_cache

import buildings
import materials
import original
from buildings import Piece
from project import consts
from tileset import TilesetPair, UnknownTileset

EDGE_SCALE = 8.0     # edge difference that costs as much as one unit
JOIN_MAX = 2.0       # joins costing more than this look broken
REORDER = 0.4        # a repeated or skipped row/column instead of the next one
FEATURE_LOST = 3.0   # a one-off block (sign, emblem) left out
SWAP = 0.5           # a block swapped for another of the seed's own parts
SWAP_FOREIGN = 1.2   # ... or for a part the seed doesn't have
FOREIGN_COLUMN = 0.3  # a column taken from a relative of the seed
STRETCH = 0.2        # each row or column added or removed
LOWER = 0.8          # each row a building is made shorter than its seed (roofs get squashed)
SWEEPS = 8
BAD = 50.0


def _edges(tiles: TilesetPair, mid: int):
    """Pixel columns 0, 15, 1, 14 and rows 0, 15, 1, 14 of a drawn metatile, flattened."""
    px = tiles.draw(mid).load()
    col = lambda x: tuple(v for y in range(16) for v in px[x, y])  # noqa: E731
    row = lambda y: tuple(v for x in range(16) for v in px[x, y])  # noqa: E731
    return col(0), col(15), row(0), row(15), col(1), col(14), row(1), row(14)


def _seam(near_a, edge_a, edge_b, near_b) -> float:
    """How much more the art changes across a join than just inside the two blocks."""
    across = _diff(edge_a, edge_b)
    inside = (_diff(near_a, edge_a) + _diff(edge_b, near_b)) / 2
    return max(0.0, across - inside)


def _diff(a, b) -> float:
    return sum(abs(i - j) for i, j in zip(a, b)) / len(a)


class Joins:
    """Which blocks of a layout's tilesets join, learned from the originals and their edges."""

    def __init__(self, primary: str, secondary: str) -> None:
        c = consts()
        self.tiles = TilesetPair(primary, secondary)
        looks = buildings._looks(primary, secondary)
        # right[a]: blocks seen right of a; below[a]: blocks seen below a (and the reverses).
        self.right: dict[int, set[int]] = defaultdict(set)
        self.left: dict[int, set[int]] = defaultdict(set)
        self.below: dict[int, set[int]] = defaultdict(set)
        self.above: dict[int, set[int]] = defaultdict(set)
        mapping: dict[tuple[str, str], dict[int, int | None]] = {}
        for layout, blocks in original.layouts():
            if layout["primary_tileset"] != primary:
                continue
            key = (layout["primary_tileset"], layout["secondary_tileset"])
            if key not in mapping:
                try:
                    src = TilesetPair(*key)
                except UnknownTileset:
                    mapping[key] = {}
                    continue
                same = key[1] == secondary
                mapping[key] = {m: (m if same else looks.get((src.pixels(m), src.behavior(m)))) for m in src.ids()}
            to = mapping[key]
            if not to:
                continue
            w, h = blocks.width, blocks.height
            ids = [to.get(b & c.metatile_mask) for b in blocks.blocks]
            for y in range(h):
                for x in range(w):
                    a = ids[y * w + x]
                    if a is None:
                        continue
                    if x + 1 < w and (b := ids[y * w + x + 1]) is not None:
                        self.right[a].add(b)
                        self.left[b].add(a)
                    if y + 1 < h and (b := ids[(y + 1) * w + x]) is not None:
                        self.below[a].add(b)
                        self.above[b].add(a)
        self._edges: dict[int, tuple] = {}
        self._h: dict[tuple[int, int], float] = {}
        self._v: dict[tuple[int, int], float] = {}

    def edges(self, mid: int):
        if mid not in self._edges:
            self._edges[mid] = _edges(self.tiles, mid)
        return self._edges[mid]

    def h(self, a: int, b: int) -> float:
        """Cost of b right of a (metatile ids)."""
        key = (a, b)
        if key not in self._h:
            if b in self.right.get(a, ()):
                cost = 0.0
            else:
                ea, eb = self.edges(a), self.edges(b)
                d1 = min((_diff(eb[0], self.edges(x)[0]) for x in self.right.get(a, ())), default=math.inf)
                d2 = min((_diff(ea[1], self.edges(y)[1]) for y in self.left.get(b, ())), default=math.inf)
                d3 = _seam(ea[5], ea[1], eb[0], eb[4])
                cost = min(d1, d2, d3, BAD * EDGE_SCALE) / EDGE_SCALE
            self._h[key] = cost
        return self._h[key]

    def v(self, a: int, b: int) -> float:
        """Cost of b below a."""
        key = (a, b)
        if key not in self._v:
            if b in self.below.get(a, ()):
                cost = 0.0
            else:
                ea, eb = self.edges(a), self.edges(b)
                d1 = min((_diff(eb[2], self.edges(x)[2]) for x in self.below.get(a, ())), default=math.inf)
                d2 = min((_diff(ea[3], self.edges(y)[3]) for y in self.above.get(b, ())), default=math.inf)
                d3 = _seam(ea[7], ea[3], eb[2], eb[6])
                cost = min(d1, d2, d3, BAD * EDGE_SCALE) / EDGE_SCALE
            self._v[key] = cost
        return self._v[key]


@lru_cache(maxsize=None)
def joins(primary: str, secondary: str) -> Joins:
    return Joins(primary, secondary)


# ---------------------------------------------------------------- what a family has


def _mid(b: int | None) -> int | None:
    return None if b is None else b & consts().metatile_mask


@lru_cache(maxsize=None)
def _catalogue(layout_id: str):
    """For a layout: one-off blocks, door blocks, the usual packed block per metatile, and border roles."""
    from project import resolve_layout

    layout = resolve_layout(layout_id)
    tiles = TilesetPair.for_layout(layout)
    lib = buildings.library(layout)
    most: Counter = Counter()
    packed: dict[int, Counter] = defaultdict(Counter)
    roles: dict[str, set[int]] = {"left": set(), "right": set(), "top": set(), "bottom": set()}
    for p in lib:
        counts = Counter(_mid(b) for _, _, b in p.blocks())
        for m, n in counts.items():
            most[m] = max(most[m], n)
        for _, _, b in p.blocks():
            packed[_mid(b)][b] += 1
        for row in p.cells:
            filled = [b for b in row if b is not None]
            if filled:
                roles["left"].add(_mid(filled[0]))
                roles["right"].add(_mid(filled[-1]))
        for x in range(p.w):
            filled = [p.cells[y][x] for y in range(p.h) if p.cells[y][x] is not None]
            if filled:
                roles["top"].add(_mid(filled[0]))
        roles["bottom"] |= {_mid(b) for b in p.cells[-1] if b is not None}
    doors = {m for m in packed if buildings._is_door(tiles, m)}
    # Signs and emblems: blocks of centres, marts and gyms that no building of theirs repeats.
    # (A house's roof blocks are one-offs too, but repeating them is how a house grows.)
    special = {_mid(b) for p in lib if p.kind in ("pokecenter", "mart", "gym") for _, _, b in p.blocks()}
    once = {m for m, n in most.items() if n == 1 and m not in doors and m in special}
    usual = {m: cnt.most_common(1)[0][0] for m, cnt in packed.items()}
    return once, doors, usual, {k: frozenset(v) for k, v in roles.items()}, lib


@lru_cache(maxsize=None)
def _relatives(seed: Piece, layout_id: str) -> tuple[Piece, ...]:
    """Buildings sharing at least three parts with the seed: the same style."""
    *_, lib = _catalogue(layout_id)
    own = {_mid(b) for _, _, b in seed.blocks()}
    return tuple(p for p in lib if p != seed and len({_mid(b) for _, _, b in p.blocks()} & own) >= 3)


def family(seed: Piece, layout: dict) -> frozenset[int]:
    """Parts a building from this seed may use: its own, its relatives', and building blocks that fit them."""
    return _family(seed, layout["id"])


@lru_cache(maxsize=None)
def _family(seed: Piece, layout_id: str) -> frozenset[int]:
    from project import resolve_layout

    layout = resolve_layout(layout_id)
    once, doors, usual, roles, lib = _catalogue(layout_id)
    own = {_mid(b) for _, _, b in seed.blocks()}
    fam = set(own)
    for p in _relatives(seed, layout_id):
        fam |= {_mid(b) for _, _, b in p.blocks()}
    j = joins(layout["primary_tileset"], layout["secondary_tileset"])
    tiles = j.tiles
    extra = [m for m in tiles.ids() if m not in fam and m not in doors and "building" in materials.of(tiles, m)]
    for m in extra:
        if any(min(j.h(f, m), j.h(m, f), j.v(f, m), j.v(m, f)) <= 0.5 for f in own):
            fam.add(m)
    return frozenset(fam)


# ---------------------------------------------------------------- making one


def _sequence(n_out: int, n_in: int, step_cost, first_last: bool = True, fixed: dict[int, set[int]] | None = None,
              avoid: dict[int, float] | None = None, once: set[int] | None = None, max_run: int = 0,
              forward: bool = False):
    """The cheapest sequence of n_out indexes into 0..n_in-1 (dynamic programming).

    step_cost(a, b): cost of b following a. The first and last indexes are the
    first and last inputs; fixed[i] limits what position i may take; avoid[j]
    is a cost for using j outside a fixed position; inputs in `once` may be
    used at most once, and cost FEATURE_LOST each if left out. max_run caps
    how many times in a row one input repeats (0: no cap); forward forbids
    going back to an earlier input. Returns (cost, sequence) or (inf, None).
    """
    fixed = fixed or {}
    avoid = avoid or {}
    once = sorted(once or ())
    bit = {j: 1 << k for k, j in enumerate(once)}
    if n_out == 1:
        return (0.0, [0]) if n_in == 1 else (math.inf, None)
    if n_in == 1:
        return math.inf, None

    def allowed(i: int) -> list[int]:
        if i in fixed:
            opts = sorted(fixed[i])
        elif first_last and i == 0:
            opts = [0]
        elif first_last and i == n_out - 1:
            opts = [n_in - 1]
        else:
            inner = range(1, n_in - 1) if first_last and n_in > 2 else range(n_in)
            opts = list(inner)
        if first_last and i == 0:
            opts = [o for o in opts if o == 0]
        if first_last and i == n_out - 1:
            opts = [o for o in opts if o == n_in - 1]
        return opts

    def extra(i: int, j: int) -> float:
        return 0.0 if i in fixed else avoid.get(j, 0.0)

    states: dict[tuple[int, int, int], tuple[float, list[int]]] = {}
    for j in allowed(0):
        states[(j, bit.get(j, 0), 1)] = (extra(0, j), [j])
    for i in range(1, n_out):
        nxt: dict[tuple[int, int, int], tuple[float, list[int]]] = {}
        opts = allowed(i)
        for (a, mask, run), (cost, seq) in states.items():
            for b in opts:
                if b in bit and mask & bit[b]:
                    continue
                if forward and b < a:
                    continue
                r = run + 1 if b == a else 1
                if max_run and r > max_run:
                    continue
                c = cost + step_cost(a, b) + extra(i, b)
                key = (b, mask | bit.get(b, 0), r)
                if key not in nxt or c < nxt[key][0]:
                    nxt[key] = (c, seq + [b])
        states = nxt
        if not states:
            return math.inf, None
    best = (math.inf, None)
    for (_, mask, _), (cost, seq) in states.items():
        lost = sum(1 for j in once if not mask & bit[j])
        total = cost + FEATURE_LOST * lost
        if total < best[0]:
            best = (total, seq)
    return best


def make(seed: Piece, layout: dict, w: int, h: int, doors: tuple[int, ...] = ()) -> tuple[Piece, float] | None:
    """A w x h building from the seed's parts, with doors at these columns of its bottom row.

    Returns the new piece and its cost (0 = only joins seen in the originals),
    or None when it can't be built without a bad join.
    """
    return _make(seed, layout["id"], w, h, tuple(sorted(doors)))


@lru_cache(maxsize=4096)
def _make(seed: Piece, layout_id: str, w: int, h: int, doors: tuple[int, ...]):
    from project import resolve_layout

    layout = resolve_layout(layout_id)
    if w < 2 or h < 2 or any(not 0 <= d < w for d in doors):
        return None
    once, door_ids, usual, roles, _ = _catalogue(layout_id)
    j = joins(layout["primary_tileset"], layout["secondary_tileset"])
    cells = [[_mid(b) for b in row] for row in seed.cells]
    seed_doors = {dx for dx, _ in seed.doors}
    if doors and not seed_doors:
        return None

    def pair_cost(fn, a, b) -> float:
        if a is None or b is None:
            return 0.0 if a is None and b is None else 0.3
        return min(fn(a, b), BAD)

    def order(a: int, b: int) -> float:
        return 0.0 if b == a + 1 else REORDER

    # Rows: the bottom (door) row stays last, the top stays first.
    unique_rows = {r for r in range(seed.h - 1) if any(m in once for m in cells[r] if m is not None)}
    # A row repeats at most once running: buildings grow by storeys, and a roof made
    # taller than its walls looks wrong.
    rcost, rows = _sequence(h, seed.h, lambda a, b: order(a, b) + sum(pair_cost(j.v, cells[a][x], cells[b][x]) for x in range(seed.w)),
                            once=unique_rows, max_run=2, forward=True)
    if rows is None:
        return None
    grid = [list(cells[r]) for r in rows]
    # Columns, on the rows as laid out; doors where asked. Besides the seed's own, columns of
    # relatives drawn the same height (other houses of its style) may go in its middle: a wider
    # house is often the same parts in another order.
    pool = [tuple(grid[y][x] for y in range(h)) for x in range(seed.w)]
    door_cols = set(seed_doors)
    foreign: set[int] = set()
    own_ids = {m for row in cells for m in row if m is not None}
    seen_cols = set(pool)
    for rel in _relatives(seed, layout_id):
        if rel.h != seed.h:
            continue
        rel_doors = {dx for dx, _ in rel.doors}
        for x in range(1, rel.w - 1):
            col = tuple(_mid(rel.cells[r][x]) for r in rows)
            if col in seen_cols or None in col:
                continue
            seen_cols.add(col)
            if x in rel_doors:
                door_cols.add(len(pool))
            foreign.add(len(pool))
            pool.append(col)
    # Keep the seed's last column last: move it to the end of the pool.
    last = seed.w - 1
    order_ix = [i for i in range(len(pool)) if i != last] + [last]
    pool = [pool[i] for i in order_ix]
    remap = {old: new for new, old in enumerate(order_ix)}
    door_cols = {remap[i] for i in door_cols}
    foreign = {remap[i] for i in foreign}
    nxt = {remap[x]: remap[x + 1] for x in range(seed.w - 1)}

    def col_step(a: int, b: int) -> float:
        cost = 0.0 if nxt.get(a) == b else REORDER
        if b in foreign:
            cost += FOREIGN_COLUMN
        return cost + sum(pair_cost(j.h, pool[a][y], pool[b][y]) for y in range(h))

    unique_cols = {i for i, col in enumerate(pool) if i not in door_cols and any(m in once for m in col if m is not None)}
    fixed = {d: door_cols for d in doors}
    avoid = {i: math.inf for i in door_cols}  # a door column only where a door goes
    ccost, cols = _sequence(w, len(pool), col_step, fixed=fixed, avoid=avoid, once=unique_cols & {remap[x] for x in range(seed.w)})
    if cols is None:
        return None
    plan = [[pool[x][y] for x in cols] for y in range(h)]
    door_cells = {(d, h - 1) for d in doors}

    # Swap blocks for other parts where that lowers the cost.
    fam = family(seed, layout)
    interior = [m for m in fam if m not in door_ids]
    by_role = {k: [m for m in interior if m in roles[k]] for k in roles}
    cur = [row[:] for row in plan]

    def count(m):
        return sum(row.count(m) for row in cur)

    own = own_ids | {m for col in pool for m in col if m is not None}

    def swap_cost(x: int, y: int, m: int) -> float:
        return 0.0 if m == plan[y][x] else SWAP if m in own else SWAP_FOREIGN

    def cell_cost(x: int, y: int, m: int) -> float:
        cost = swap_cost(x, y, m)
        if (x, y) not in door_cells and m in door_ids:
            cost += BAD
        if m in once and count(m) - (cur[y][x] == m) >= 1:
            cost += BAD
        if x > 0:
            cost += pair_cost(j.h, cur[y][x - 1], m)
        if x + 1 < w:
            cost += pair_cost(j.h, m, cur[y][x + 1])
        if y > 0:
            cost += pair_cost(j.v, cur[y - 1][x], m)
        if y + 1 < h:
            cost += pair_cost(j.v, m, cur[y + 1][x])
        return cost

    def options(x: int, y: int) -> list[int]:
        opts = set(interior)
        for k, at_edge in (("left", x == 0 or cur[y][x - 1] is None), ("right", x == w - 1 or cur[y][x + 1] is None),
                           ("top", y == 0 or cur[y - 1][x] is None), ("bottom", y == h - 1)):
            if at_edge:
                opts &= set(by_role[k])
        return list(opts | {plan[y][x]} - set(door_ids))

    # Two-block moves: a column copied for its roof brings its door along, and the door and
    # the frame above it only change together. Pairs come from the originals, within the family.
    famset = set(interior)
    vpairs = [(a, b) for a in interior for b in j.below.get(a, ()) if b in famset]
    hpairs = [(a, b) for a in interior for b in j.right.get(a, ()) if b in famset]

    def try_pair(cells2, pairs) -> bool:
        (x1, y1), (x2, y2) = cells2
        if any(cur[y][x] is None or (x, y) in door_cells for x, y in cells2):
            return False
        o1, o2 = options(x1, y1), options(x2, y2)
        s1, s2 = set(o1), set(o2)

        def both(a, b) -> float:
            old = cur[y1][x1], cur[y2][x2]
            cur[y1][x1], cur[y2][x2] = a, b
            cost = cell_cost(x1, y1, a) + cell_cost(x2, y2, b)
            cur[y1][x1], cur[y2][x2] = old
            return cost

        now = both(cur[y1][x1], cur[y2][x2])
        best, best_cost = None, now - 1e-9
        for a, b in pairs:
            if a in s1 and b in s2:
                c = both(a, b)
                if c < best_cost:
                    best, best_cost = (a, b), c
        if best:
            cur[y1][x1], cur[y2][x2] = best
            return True
        return False

    for _ in range(SWEEPS):
        changed = False
        for y in range(h):
            for x in range(w):
                if cur[y][x] is None or (x, y) in door_cells:
                    continue
                now = cell_cost(x, y, cur[y][x])
                best = min(options(x, y), key=lambda m: cell_cost(x, y, m))
                if cell_cost(x, y, best) < now - 1e-9:
                    cur[y][x] = best
                    changed = True
        for y in range(h):
            for x in range(w):
                if y + 1 < h and try_pair(((x, y), (x, y + 1)), vpairs):
                    changed = True
                if x + 1 < w and try_pair(((x, y), (x + 1, y)), hpairs):
                    changed = True
        if not changed:
            break

    total, worst = 0.0, 0.0
    for y in range(h):
        for x in range(w):
            m = cur[y][x]
            if m is None:
                continue
            if (x, y) not in door_cells and m in door_ids:
                return None
            total += swap_cost(x, y, m)
            for c in ((pair_cost(j.h, m, cur[y][x + 1]) if x + 1 < w else 0.0),
                      (pair_cost(j.v, m, cur[y + 1][x]) if y + 1 < h else 0.0)):
                total += c
                worst = max(worst, c)
    if worst > JOIN_MAX:
        return None
    for d in doors:
        if cur[h - 1][d] not in door_ids:
            return None
    seed_packed = {}
    for _, _, b in seed.blocks():
        seed_packed.setdefault(_mid(b), b)
    c = consts()
    out = tuple(tuple(None if m is None else seed_packed.get(m, usual.get(m, c.pack(m, 1, 0))) for m in row) for row in cur)
    name = f"{seed.name}~{w}x{h}" + (f"d{'-'.join(map(str, doors))}" if doors else "")
    piece = Piece(seed.source, seed.x, seed.y, w, h, out, tuple((d, h - 1) for d in doors), seed.kind, seed.dests,
                  seed.primary, layout["secondary_tileset"], made=name)
    return piece, total + LOWER * max(0, seed.h - h) + STRETCH * (abs(seed.w - w) + abs(seed.h - h))
