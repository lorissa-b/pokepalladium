"""Buildings: Emerald's own, taken from the original maps, placed where Platinum has its own.

A building can't be tiled one block at a time: a roof, its walls and its
door only look right as the whole piece an original map drew. So drafts
place buildings first, as whole pieces, and tile everything else around them.

- Pieces (Emerald). Every door warp in an original Emerald layout
  (original.py) marks a building: the blocks around the door that show a
  building, or are solid and aren't trees, water, rock, fences or objects,
  connected to it and no lower than it (plus anything sat on its roof).
  Each piece keeps its blocks, its doors (all on its bottom row; a terrace
  of two shops keeps both) and its kind, from the map its door leads to
  (pokecenter, mart, gym, lab, gate, house, other). Only town, city and
  route maps count, so interiors and dungeons add nothing.
- Targets (Platinum). platinum.Reference.buildings(): props standing on a
  block of solid tiles, with their doors and side entrances.
- Fitting. A piece can be used on a layout when every block it uses exists
  in the layout's tilesets: the same id with the same tileset, or a block
  that draws exactly the same (and behaves the same) in the layout's pair.
- Placing. Each Platinum building gets the piece and position that cost
  least: its door on Platinum's door, as little of the footprint left over
  and as little walkable ground covered as possible, the same kind
  (a Pokemon Center for a Pokemon Center). A building without a front door
  (a decorative block, or a gate entered from the side) gets a piece with
  its door bricked up, sat on the footprint's bottom row.

Besides the pieces as the originals draw them, plan() asks parts.py to make
new buildings from their parts, to each Platinum building's exact width
and depth (or one taller) with its doors where Platinum has them, and
lets those compete on the same costs (plus MADE). So a 5-wide Sinnoh house
gets a 5-wide house, and Jubilife's offices offices of their own size.

plan() returns the placements, with the blocks to fix in place; draft and
draft --finish stamp them, and autotile.fill() tiles around them.
"""

from __future__ import annotations

from collections import Counter
from dataclasses import dataclass, field
from functools import lru_cache

import materials
import original
import platinum
from compare import GROUP_OF, emerald_symbol
from project import behaviors, consts
from tileset import TilesetPair, UnknownTileset

# Solid blocks showing these are scenery, not part of a building.
SCENERY = {"grass", "tree", "water", "cliff", "rock", "fence", "ledge", "object", "flowers", "tallgrass", "sand", "cave", "dark"}
OUTDOOR = {"MAP_TYPE_TOWN", "MAP_TYPE_CITY", "MAP_TYPE_ROUTE", "MAP_TYPE_OCEAN_ROUTE"}
MAX_REACH = (10, 14)  # how far a piece may reach from its door: columns either side, rows up


@dataclass(frozen=True)
class Piece:
    """An Emerald building, as an original map drew it."""

    source: str  # layout id
    x: int  # top-left in the source layout
    y: int
    w: int
    h: int
    cells: tuple[tuple[int | None, ...], ...]  # packed blocks by row; None = not part of the building
    doors: tuple[tuple[int, int], ...]  # door cells, relative to the top-left; all on the bottom row
    kind: str
    dests: tuple[str, ...]
    primary: str
    secondary: str
    made: str = ""  # set for a building made from parts (parts.py): its name

    @property
    def name(self) -> str:
        return self.made or f"{self.source.removeprefix('LAYOUT_')}@{self.x},{self.y}"

    def blocks(self):
        """(dx, dy, block) for each cell that's part of the building."""
        for dy, row in enumerate(self.cells):
            for dx, b in enumerate(row):
                if b is not None:
                    yield dx, dy, b

    def wall(self) -> int:
        """A block to brick a door up with: the commonest wall block on the door row."""
        c = consts()
        doors = {self.cells[dy][dx] for dx, dy in self.doors}
        row = self.cells[self.h - 1]
        inner = [b for b in row[1:-1] if b is not None and b not in doors] or [b for b in row if b is not None and b not in doors]
        if not inner:
            return self.cells[self.h - 1][self.doors[0][0]]
        best = Counter(b & c.metatile_mask for b in inner).most_common(1)[0][0]
        return next(b for b in inner if b & c.metatile_mask == best)


def _is_door(tiles: TilesetPair, mid: int) -> bool:
    name = behaviors().get(tiles.behavior(mid), "")
    return "DOOR" in name and "WATER" not in name


def _part_of_building(tiles: TilesetPair, block: int) -> bool:
    mid, col, _ = consts().unpack(block)
    mats = materials.of(tiles, mid)
    if "building" in mats or _is_door(tiles, mid):
        return True
    # Roofs and walls are solid, and solid scenery is labelled as such, so this
    # also catches building blocks whose label missed it.
    return bool(col) and not mats & SCENERY


def _sandwiched(region: set[tuple[int, int]], blocks, tiles: TilesetPair) -> set[tuple[int, int]]:
    """Blocks with the building on both sides in their row, other than open ground: things on a roof."""
    c = consts()
    ground = {"grass", "path", "sand", "flowers", "tallgrass"}
    out = set()
    rows: dict[int, list[int]] = {}
    for x, y in region:
        rows.setdefault(y, []).append(x)
    for y, xs in rows.items():
        for x in range(min(xs) + 1, max(xs)):
            mid, col, _ = c.unpack(blocks.get(x, y))
            if (x, y) not in region and (col or not materials.of(tiles, mid) & ground):
                out.add((x, y))
    return out


@lru_cache(maxsize=None)
def pieces(primary: str) -> tuple[Piece, ...]:
    """Every building in the original layouts on this primary tileset (identical ones once)."""
    c = consts()
    found: dict[tuple, Piece] = {}
    maps = original.maps()
    for layout, blocks in original.layouts():
        if layout["primary_tileset"] != primary:
            continue
        try:
            tiles = TilesetPair.for_layout(layout)
        except UnknownTileset:
            continue
        doors: dict[tuple[int, int], str] = {}
        outdoor = [m for m in maps.get(layout["id"], []) if m.get("map_type") in OUTDOOR]
        for info in outdoor:
            for wp in info.get("warp_events") or []:
                x, y = int(wp["x"]), int(wp["y"])
                if blocks.inside(x, y) and _is_door(tiles, blocks.get(x, y) & c.metatile_mask):
                    doors[(x, y)] = wp.get("dest_map", "")
        for (x, y), dest in doors.items():
            region = {(x, y)}
            stack = [(x, y)]
            while stack:
                a, b = stack.pop()
                for n in ((a + 1, b), (a - 1, b), (a, b + 1), (a, b - 1)):
                    nx, ny = n
                    if (n in region or not blocks.inside(nx, ny) or ny > y or abs(nx - x) > MAX_REACH[0]
                            or y - ny > MAX_REACH[1]):
                        continue
                    if _part_of_building(tiles, blocks.get(nx, ny)):
                        region.add(n)
                        stack.append(n)
            # A terrace (two shops under one roof) stays whole, with all its doors: half a
            # building doesn't look like one. Doors placing doesn't need are bricked up.
            region |= _sandwiched(region, blocks, tiles)
            xs = [p[0] for p in region]
            ys = [p[1] for p in region]
            x0, y0, w, h = min(xs), min(ys), max(xs) - min(xs) + 1, max(ys) - min(ys) + 1
            if w < 2 or h < 2:
                continue  # a cave mouth or a door on its own
            cells = tuple(tuple(blocks.get(x0 + dx, y0 + dy) if (x0 + dx, y0 + dy) in region else None for dx in range(w))
                          for dy in range(h))
            own = tuple(sorted((dx - x0, dy - y0) for (dx, dy), to in doors.items() if (dx, dy) in region and dy == y))
            key = (cells, own, layout["secondary_tileset"] if any(
                b is not None and b & c.metatile_mask >= c.metatiles_in_primary for row in cells for b in row) else "")
            if key in found:
                continue
            found[key] = Piece(layout["id"], x0, y0, w, h, cells, own, platinum.building_kind(dest.removeprefix("MAP_")),
                               (dest,), layout["primary_tileset"], layout["secondary_tileset"])
    return tuple(sorted(found.values(), key=lambda p: (p.kind, p.w * p.h, p.name)))


@lru_cache(maxsize=None)
def _looks(primary: str, secondary: str) -> dict[tuple[bytes, int | None], int]:
    """How each metatile in a pair draws (and behaves) -> its id (the lowest, if several match)."""
    tiles = TilesetPair(primary, secondary)
    out: dict[tuple[bytes, int | None], int] = {}
    for mid in tiles.ids():
        out.setdefault((tiles.pixels(mid), tiles.behavior(mid)), mid)
    return out


def translate(piece: Piece, layout: dict) -> Piece | None:
    """The piece with its blocks as the layout's tilesets number them, or None if it can't be drawn there."""
    c = consts()
    if piece.primary != layout["primary_tileset"]:
        return None
    if piece.secondary == layout["secondary_tileset"]:
        return piece
    try:
        src = TilesetPair(piece.primary, piece.secondary)
        dst = TilesetPair.for_layout(layout)
    except UnknownTileset:
        return None
    looks = _looks(layout["primary_tileset"], layout["secondary_tileset"])
    mapping: dict[int, int] = {}
    for _, _, b in piece.blocks():
        mid = b & c.metatile_mask
        if mid in mapping:
            continue
        if mid < c.metatiles_in_primary:
            mapping[mid] = mid  # same primary tileset: same block
            continue
        hit = looks.get((src.pixels(mid), src.behavior(mid)))
        if hit is None or not dst.exists(hit):
            return None
        mapping[mid] = hit
    cells = tuple(tuple(None if b is None else (b & ~c.metatile_mask) | mapping[b & c.metatile_mask] for b in row)
                  for row in piece.cells)
    return Piece(piece.source, piece.x, piece.y, piece.w, piece.h, cells, piece.doors, piece.kind, piece.dests,
                 piece.primary, layout["secondary_tileset"], piece.made)


@lru_cache(maxsize=None)
def _library(primary: str, secondary: str, layout_id: str) -> tuple[Piece, ...]:
    from project import resolve_layout

    layout = resolve_layout(layout_id)
    return tuple(t for t in (translate(p, layout) for p in pieces(primary)) if t is not None)


def library(layout: dict) -> tuple[Piece, ...]:
    """Every piece that can be drawn with the layout's tilesets, numbered for them."""
    return _library(layout["primary_tileset"], layout["secondary_tileset"], layout["id"])


# ---------------------------------------------------------------- placing


@dataclass
class Placement:
    """A piece placed in a draft, standing in for a Platinum building (or None)."""

    piece: Piece
    x: int  # top-left in draft coordinates (may be partly off the map)
    y: int
    cells: dict[tuple[int, int], int]  # the blocks it fixes, on the map
    doors: list[tuple[int, int]]  # doors it keeps, in draft coordinates
    target: platinum.Building | None = None
    footprint: set[tuple[int, int]] = field(default_factory=set)  # Platinum's, in draft coordinates
    cost: float = 0.0
    notes: list[str] = field(default_factory=list)

    def describe(self) -> str:
        p = self.piece
        what = f"{self.target.prop.name} ({self.target.kind})" if self.target else "building"
        doors = " ".join(f"({x},{y})" for x, y in self.doors) or "none"
        return (f"{what}: {p.name} {p.w}x{p.h} ({p.kind}) at ({self.x},{self.y}), doors {doors}"
                + (f"; {'; '.join(self.notes)}" if self.notes else ""))


# Costs, roughly "tiles of shape lost".
COVER_WALKABLE = 2.5      # a building block on ground Platinum lets you walk on
COVER_RECESS = 0.8        # walkable ground inside the footprint's box: a porch or a yard between wings
COVER_OTHER = 4.0         # a block on another Platinum building's footprint
COVER_SOLID = 0.4         # a building block on solid ground outside Platinum's footprint
UNCOVERED = 1.0           # a footprint tile the piece leaves for the tiler
OFF_MAP = 1.5             # a building block cut off by the map's edge
DOOR_MISSING = 8.0        # a Platinum door with no door on the piece to match
DOOR_BRICKED = 0.5        # a piece's door bricked up
REUSE = 0.3               # each earlier use of the same piece, so a town gets some variety
KIND_COST = {
    # (Platinum kind, piece kind) -> cost; anything else costs OTHER_KIND.
    ("pokecenter", "pokecenter"): 0, ("mart", "mart"): 0, ("gym", "gym"): 0, ("lab", "lab"): 0,
    ("house", "house"): 0, ("gate", "gate"): 0, ("other", "other"): 0,
    ("lab", "house"): 2, ("lab", "other"): 1, ("other", "house"): 1, ("other", "gym"): 2, ("other", "lab"): 1,
    ("house", "other"): 1.5, ("house", "lab"): 2, ("gate", "other"): 1, ("gate", "house"): 2,
}
OTHER_KIND = 6.0
SPECIAL = {"pokecenter", "mart"}  # never stand in for anything else, and nothing stands in for them


def _kind_cost(want: str, have: str) -> float:
    if (want, have) in KIND_COST:
        return KIND_COST[(want, have)]
    if want in SPECIAL or have in SPECIAL:
        return 3 * OTHER_KIND
    return OTHER_KIND


def plan(ref: platinum.Reference, layout: dict, grid: list[str], region: tuple[int, int, int, int],
         make_new: bool = True) -> tuple[list[Placement], list[str]]:
    """Pieces for every Platinum building in the region. grid: the region's tile classes.

    With make_new, buildings made from parts (parts.py) to the exact size and
    doors of each Platinum building compete with the original pieces.

    Returns the placements and notes on buildings that got none.
    """
    x0, y0, w, h = region
    lib = library(layout)
    walk = [[GROUP_OF.get(ch) not in ("block", "none") for ch in row.ljust(w)] for row in grid]
    warps = {(e["x"] - x0, e["y"] - y0) for e in ref.warps}
    used: dict[tuple[int, int], Placement] = {}
    uses: Counter = Counter()
    placed, notes = [], []
    targets = []
    for b in ref.buildings():
        tiles = {(x - x0, y - y0) for x, y in b.tiles}
        on_map = {t for t in tiles if 0 <= t[0] < w and 0 <= t[1] < h}
        doors = [(x - x0, y - y0) for x, y in b.doors]
        if not on_map or (doors and not any(0 <= x < w and 0 <= y < h for x, y in doors)):
            continue
        if not doors and len(on_map) < len(tiles) / 2:
            continue
        targets.append((b, tiles, doors))
    # Buildings with doors first, the biggest first: they have the fewest good options.
    targets.sort(key=lambda t: (not t[2], -len(t[1])))
    owner = {t: n for n, (_, tiles, _) in enumerate(targets) for t in tiles}
    for n, (b, tiles, doors) in enumerate(targets):
        best = None
        bx0 = min(x for x, _ in tiles)
        bx1 = max(x for x, _ in tiles)
        bottom = max(y for _, y in tiles)
        top = min(y for _, y in tiles)
        candidates = []
        for piece in lib:
            if doors:
                anchors = {(dx - pdx, dy - pdy) for pdx, pdy in piece.doors for dx, dy in doors[:1]}
            else:
                anchors = {(ax, bottom - piece.h + 1) for ax in range(bx0 - piece.w + 2, bx1)}
            candidates.append((piece, anchors, 0.0))
        if make_new:
            for piece, extra in _made_for(b.kind, lib, layout, bx1 - bx0 + 1, bottom - top + 1,
                                          tuple(sorted(x - bx0 for x, _ in doors)) if doors else ()):
                candidates.append((piece, {(bx0, bottom - piece.h + 1)}, extra))
        for piece, anchors, extra in candidates:
            for ax, ay in anchors:
                cost = _kind_cost(b.kind, piece.kind) + REUSE * uses[piece.name] + extra
                if cost >= (best[0] if best else float("inf")):
                    continue
                kept, bad = [], False
                piece_doors = {(ax + dx, ay + dy) for dx, dy in piece.doors}
                for pd in piece_doors:
                    if pd in doors:
                        kept.append(pd)
                    else:
                        cost += DOOR_BRICKED
                cost += DOOR_MISSING * (len(doors) - len(kept))
                covered = set()
                for dx, dy, _ in piece.blocks():
                    x, y = ax + dx, ay + dy
                    covered.add((x, y))
                    if (x, y) in used or ((x, y) in warps and (x, y) not in kept):
                        bad = True
                        break
                    if not (0 <= x < w and 0 <= y < h):
                        if (x, y) in kept:
                            bad = True
                            break
                        cost += OFF_MAP
                    elif (x, y) in tiles:
                        pass
                    elif owner.get((x, y), n) != n:
                        cost += COVER_OTHER
                    elif walk[y][x]:
                        cost += COVER_RECESS if bx0 <= x <= bx1 and top <= y <= bottom else COVER_WALKABLE
                    else:
                        cost += COVER_SOLID
                if bad:
                    continue
                # Doors need open ground in front of them.
                if any(not (0 <= y + 1 < h) or (x, y + 1) in used or (x, y + 1) in covered for x, y in kept):
                    continue
                cost += UNCOVERED * len({t for t in tiles if 0 <= t[0] < w and 0 <= t[1] < h} - covered)
                if best is None or cost < best[0]:
                    best = (cost, piece, ax, ay, kept)
        what = f"{b.prop.name} ({b.kind}) at {b.box[0] - x0},{b.box[1] - y0} {b.box[2]}x{b.box[3]}"
        if best is None:
            notes.append(f"no Emerald building fits {what}")
            continue
        cost, piece, ax, ay, kept = best
        wall = piece.wall()
        cells = {}
        for dx, dy, block in piece.blocks():
            x, y = ax + dx, ay + dy
            if 0 <= x < w and 0 <= y < h:
                is_door = (dx, dy) in piece.doors
                cells[(x, y)] = wall if is_door and (x, y) not in kept else block
        p = Placement(piece, ax, ay, cells, kept, b, tiles, cost)
        if len(kept) < len(doors):
            p.notes.append(f"no door for Platinum's door(s) at {' '.join(f'({x},{y})' for x, y in doors if (x, y) not in kept)}")
        if b.side:
            p.notes.append("entered from the side in Platinum: " + " ".join(f"({x - x0},{y - y0})" for x, y in b.side))
        for t in cells:
            used[t] = p
        for x, y in kept:
            used[(x, y + 1)] = p  # keep the doorstep clear of later buildings
        uses[piece.name] += 1
        placed.append(p)
    placed.sort(key=lambda p: (p.y, p.x))
    return placed, notes


MADE = 1.0          # a building made from parts rather than taken whole
MADE_SCALE = 0.5    # times the made building's own cost (its joins and swapped parts)
SEEDS = 6           # pieces tried as seeds for each Platinum building


def _made_for(kind: str, lib, layout: dict, w: int, h: int, doors: tuple[int, ...]):
    """Buildings made from parts for a footprint: (piece, extra cost) for the best few seeds.

    Each is w wide, h or h + 1 tall (Emerald draws a roof taller than
    Platinum's footprint is deep), with doors at these columns.
    """
    import parts

    seeds = [p for p in lib if _kind_cost(kind, p.kind) <= 2 and (not doors or p.doors)]
    seeds.sort(key=lambda p: (_kind_cost(kind, p.kind), abs(p.w - w) + abs(p.h - h), p.name))
    out = []
    for seed in seeds[:SEEDS]:
        for height in (h, h + 1):
            made = parts.make(seed, layout, w, height, doors)
            if made is not None and made[0].cells != seed.cells:
                out.append((made[0], MADE + MADE_SCALE * made[1]))
    return out


def apply(placements: list[Placement], grid: list[str], layout: dict) -> list[str]:
    """The class grid with the placed buildings' classes in, and open ground in front of their doors.

    Where a piece is smaller than Platinum's building, the footprint it leaves
    becomes open ground, as the space beside an Emerald building would be,
    unless it backs onto other solid ground (a tree line behind the building).
    """
    tiles = TilesetPair.for_layout(layout)
    rows = [list(r) for r in grid]
    h, w = len(rows), len(rows[0]) if rows else 0
    covered = {t for p in placements for t in p.cells}
    footprints = {t for p in placements for t in p.footprint}

    def solid(x, y):
        return 0 <= x < w and 0 <= y < h and GROUP_OF.get(rows[y][x]) in ("block", "none")

    for p in placements:
        left = {(x, y) for x, y in p.footprint - covered if 0 <= x < w and 0 <= y < h}
        for x, y in sorted(left, key=lambda t: (-t[1], t[0])):
            backed = any(solid(nx, ny) and (nx, ny) not in footprints and (nx, ny) not in covered
                         for nx, ny in ((x + 1, y), (x - 1, y), (x, y - 1)))
            if not backed:
                rows[y][x] = "."
    for p in placements:
        for (x, y), block in p.cells.items():
            rows[y][x] = emerald_symbol(tiles, block)
        for x, y in p.doors:
            if 0 <= y + 1 < h and GROUP_OF.get(rows[y + 1][x]) in ("block", "none"):
                rows[y + 1][x] = "."
    return ["".join(r) for r in rows]


def fixed(placements: list[Placement]) -> dict[tuple[int, int], int]:
    out = {}
    for p in placements:
        out.update(p.cells)
    return out
