"""Compare Emerald maps with Platinum references, and draft blueprints from them.

Both games' tiles are reduced to the same one-character classes (see
platinum.SYMBOLS / platinum.LEGEND), so a replica can be scored tile by tile
against the original and a reference can be turned into a starting layout.
"""

from __future__ import annotations

import re
from collections import Counter

import materials
import platinum
from project import Blockdata, behaviors, consts
from tileset import TilesetPair

# Classes that are the same for movement purposes.
GROUPS = {
    "walk": set('."Y:,DESm=sBip'),
    "block": set("#ot"),
    "water": set("~|"),
    "ledge": set("^v<>"),
    "climb": set("R"),
    "none": set(" "),
}
GROUP_OF = {ch: g for g, chars in GROUPS.items() for ch in chars}

_EMERALD_RULES = [
    (r"JUMP_(SOUTH|SOUTHEAST|SOUTHWEST)$", "v"),
    (r"JUMP_(NORTH|NORTHEAST|NORTHWEST)$", "^"),
    (r"JUMP_EAST$", ">"),
    (r"JUMP_WEST$", "<"),
    (r"^MB_TALL_GRASS$", '"'),
    (r"LONG_GRASS", "Y"),
    (r"WATERFALL", "|"),
    (r"PUDDLE|SHALLOW_WATER", ","),
    (r"WATER|SEAWEED|NO_SURFACING|CURRENT", "~"),
    (r"SAND", ":"),
    (r"ICE", "i"),
    (r"DOOR", "D"),
    (r"LADDER|ESCALATOR|STAIRS", "S"),
    (r"WARP|HOLE", "E"),
    (r"BRIDGE|PACIFIDLOG", "="),
    (r"(^|_)COUNTER", "t"),  # not ENCOUNTER
    (r"PC$|TELEVISION|REGION_MAP|BOOKSHELF|SHOP_SHELF|TRASH_CAN|VASE|PICTURE_BOOK", "o"),
    (r"BERRY_TREE_SOIL", "B"),
    (r"MUDDY_SLOPE", "m"),
]


def emerald_symbol(tiles: TilesetPair, block: int) -> str:
    c = consts()
    mid, col, _ = c.unpack(block)
    name = behaviors().get(tiles.behavior(mid), "")
    mats = materials.of(tiles, mid)
    if "snow" in mats and not col:
        return "s"  # snow leaves sand's footprints, but it's still snow
    for pattern, sym in _EMERALD_RULES:
        if re.search(pattern, name):
            # Furniture, counters and ledges are blocked by collision in Emerald too.
            return sym
    if col:
        return "#"
    # Walkable ground labelled as a path (materials/), so drafts know it apart from grass.
    return "p" if "path" in mats else "."


def emerald_grid(layout: dict) -> list[str]:
    blocks = Blockdata.for_layout(layout)
    tiles = TilesetPair.for_layout(layout)
    return ["".join(emerald_symbol(tiles, blocks.get(x, y)) for x in range(blocks.width)) for y in range(blocks.height)]


def mismatch_areas(mismatches: list[tuple[int, int, str, str]]) -> list[dict]:
    """Mismatched tiles grouped into touching areas (diagonals count), largest first.

    Each area has its tile `count`, bounding `box` (x, y, w, h) and the most
    common (here, Platinum) class pair as `most`.
    """
    left = {(x, y): (e, r) for x, y, e, r in mismatches}
    areas = []
    while left:
        start = next(iter(left))
        stack, cells = [start], {start: left.pop(start)}
        while stack:
            x, y = stack.pop()
            for dx in (-1, 0, 1):
                for dy in (-1, 0, 1):
                    n = (x + dx, y + dy)
                    if n in left:
                        cells[n] = left.pop(n)
                        stack.append(n)
        xs = [x for x, _ in cells]
        ys = [y for _, y in cells]
        areas.append({
            "count": len(cells),
            "box": (min(xs), min(ys), max(xs) - min(xs) + 1, max(ys) - min(ys) + 1),
            "most": Counter(cells.values()).most_common(1)[0][0],
        })
    return sorted(areas, key=lambda a: (-a["count"], a["box"][1], a["box"][0]))


def compare(layout: dict, ref: platinum.Reference, origin: tuple[int, int] = (0, 0)) -> dict:
    """Tile-by-tile comparison. origin = reference tile shown at the layout's (0, 0)."""
    egrid = emerald_grid(layout)
    ox, oy = origin
    rows, exact, same_group, total = [], 0, 0, 0
    mismatches = []
    for y in range(layout["height"]):
        row = []
        for x in range(layout["width"]):
            rx, ry = x + ox, y + oy
            e = egrid[y][x]
            r = ref.symbol(rx, ry) if 0 <= rx < ref.width and 0 <= ry < ref.height else " "
            if r == " ":
                row.append(" ")
                continue
            total += 1
            if e == r:
                exact += 1
                same_group += 1
                row.append(e)
            elif GROUP_OF.get(e) == GROUP_OF.get(r):
                same_group += 1
                row.append(e)
            else:
                row.append("X")
                mismatches.append((x, y, e, r))
        rows.append("".join(row))

    def near(points, x, y):
        return any(abs(px - x) + abs(py - y) <= 1 for px, py in points)

    ewarps = []
    from project import maps_using_layout

    for info in maps_using_layout(layout["id"]):
        ewarps += [(int(w["x"]), int(w["y"])) for w in info.get("warp_events") or []]
    rwarps = [(w["x"] - ox, w["y"] - oy, w.get("dest_header_id", "?")) for w in ref.warps]
    rwarps = [w for w in rwarps if 0 <= w[0] < layout["width"] and 0 <= w[1] < layout["height"]]
    missing_warps = [w for w in rwarps if not near(ewarps, w[0], w[1])]
    return {
        "grid": rows,
        "total": total,
        "exact": exact,
        "same_group": same_group,
        "mismatches": mismatches,
        "missing_warps": missing_warps,
    }


def best_origin(layout: dict, ref: platinum.Reference, margin: int = 8) -> tuple[int, int]:
    """The reference offset where the layout's movement matches best (ties go to the smallest shift)."""
    egrid = [[GROUP_OF.get(ch) for ch in row] for row in emerald_grid(layout)]
    rgrid = [[GROUP_OF.get(ch) for ch in row] for row in ref.grid()]
    w, h = layout["width"], layout["height"]
    best, best_score = (0, 0), -1
    for oy in range(-margin, ref.height - h + margin + 1):
        for ox in range(-margin, ref.width - w + margin + 1):
            score = 0
            for y in range(max(0, -oy), min(h, ref.height - oy)):
                erow, rrow = egrid[y], rgrid[y + oy]
                for x in range(max(0, -ox), min(w, ref.width - ox)):
                    if erow[x] == rrow[x + ox] != "none":
                        score += 1
            if score > best_score or (score == best_score and abs(ox) + abs(oy) < abs(best[0]) + abs(best[1])):
                best, best_score = (ox, oy), score
    return best


# Starting blocks for a draft, per primary tileset. Symbols without an entry
# become `keep` with a TODO so they're easy to find.
DRAFT_LEGENDS = {
    "gTileset_General": {
        "#": "[0x1D4 0x1D5; 0x1DC 0x1DD]  # dense trees",
        ".": "General_Grass",
        "p": "0x111  # dirt path",
        "s": "General_Snow",
        '"': "General_TallGrass",
        "Y": "General_LongGrass",
        "~": "General_CalmWater",
        ",": "0x13B  # shallow water",
        ":": "0x124  # sand",
        "v": "0x087  # ledge, jump south",
        "<": "0x085  # ledge, jump west",
        ">": "0x086  # ledge, jump east",
        "^": "[0x1D4 0x1D5; 0x1DC 0x1DD]  # Emerald has no north ledges",
        "D": "General_Door  # a door with no building placed: stamp the real building",
        "|": "0x04C  # waterfall",
        "E": "General_Grass  # TODO: warp tile (cave entrance, arrow warp...)",
    },
}


def draft(ref: platinum.Reference, layout: dict, region: tuple[int, int, int, int] | None = None,
          with_buildings: bool = True, make_new: bool = True, notes_to: list[str] | None = None) -> str:
    """A blockout blueprint of the Platinum map, with its events and props as comments.

    With `notes_to`, the blueprint is lean: the building placements and the
    Platinum notes go into that list instead, and legend lines carry no
    metatile descriptions.
    """
    x0, y0, w, h = region or (0, 0, ref.width, ref.height)
    grid = ref.grid()
    rows = [grid[y][x0 : x0 + w] if 0 <= y < ref.height else "" for y in range(y0, y0 + h)]
    placements, unplaced = _buildings(ref, layout, rows, (x0, y0, w, h), with_buildings, make_new)
    used = sorted({ch for r in rows for ch in r} - {" "})
    legends = DRAFT_LEGENDS.get(layout["primary_tileset"], {})
    try:
        rev = platinum.revision()
    except Exception:
        rev = "?"
    out = [
        f"# Draft of {ref.header} from pret/pokeplatinum@{rev}",
        f"# Reference region {x0},{y0} {w}x{h} of its {ref.width}x{ref.height} tiles. Regenerate with:",
        f"#   tools/mapkit/mapkit.py draft {ref.header} {layout['id']}" + (f" --region {x0},{y0},{w},{h}" if region else "")
        + (" --solid-unreachable" if ref.void else ""),
        "#",
        "# Tile classes: " + ", ".join(f"{k!r} {v}" for k, v in platinum.LEGEND if any(c in used for c in k)),
        "",
        f"layout {layout['id']}",
        f"size {w} {h}",
        "base none",
        "",
    ]
    fill = legends.get("#", "").split("#")[0].strip()
    if fill:
        out.append(f"fill {fill}")
    for ch in used:
        spec = legends.get(ch)
        out.append(f"legend {ch} = {spec}" if spec else f"legend {ch} = keep  # TODO: {dict(platinum.LEGEND).get(ch, ch)}")
    out.append("")
    out.append("grid 0 0")
    out += [r.rstrip() for r in rows]
    out.append("end")
    out.append("")
    if placements or unplaced:
        from blueprint import cells_text, shared_legend
        from render import describe_metatile

        tiles = TilesetPair.for_layout(layout)
        out.append("# ---- Buildings: Emerald pieces where Platinum has buildings (see buildings.py) ----")
        out += [f"# {n}" for n in unplaced]
        key_of = None
        if notes_to is not None:
            notes_to += ["# ---- Buildings placed (blueprint grid at each one's x, y) ----"] + [f"# {p.describe()}" for p in placements]
            if placements:
                key_of, legend = shared_legend([p.cells for p in placements], layout=layout)
                out.append(legend.rstrip())
        for p in placements:
            if notes_to is None:
                out.append(f"# {p.describe()}")
                out.append(cells_text(p.cells, lambda mid: describe_metatile(tiles, mid).split(" ", 1)[1]).rstrip())
            else:
                out.append(cells_text(p.cells, key_of=key_of).rstrip())
        out.append("")
    if notes_to is not None:
        notes_to += notes(ref, (x0, y0, w, h))
    else:
        out += notes(ref, (x0, y0, w, h))
    return "\n".join(out) + "\n"


def _buildings(ref, layout, rows, region, enabled: bool, make_new: bool = True):
    """Placements for the region's buildings (none when disabled)."""
    if not enabled:
        return [], []
    import buildings

    w = region[2]
    return buildings.plan(ref, layout, [r.ljust(w) for r in rows], region, make_new)


def path_look(ref: platinum.Reference, region: tuple[int, int, int, int]) -> str | None:
    """The path family most of the region's painted paths call for (ground.path_look), None if it has none."""
    import ground

    x0, y0, w, h = region
    looks = Counter(ground.path_look(ref.ground[y][x]) for y in range(max(0, y0), min(ref.height, y0 + h))
                    for x in range(max(0, x0), min(ref.width, x0 + w)) if ref.ground and ref.symbol(x, y) == "p")
    looks.pop(None, None)
    return looks.most_common(1)[0][0] if looks else None


def finished_draft(ref: platinum.Reference, layout: dict, region: tuple[int, int, int, int] | None = None,
                   style: str = "route", trees: str | None = "dense", water: str | None = "sea",
                   with_buildings: bool = True, make_new: bool = True, keep_shape: bool = False,
                   path: str | None = "auto", notes_to: list[str] | None = None):
    """A blueprint with every block chosen, tiled by example (see autotile.py).

    Returns (blueprint text, blocks, tally, seams). With `notes_to`, the
    blueprint is lean: the building placements, the tile classes it was drawn
    from and the Platinum notes go into that list instead, and legend lines
    carry no metatile descriptions.
    """
    import autotile
    import original
    from blueprint import blocks_text
    from render import describe_metatile

    x0, y0, w, h = region or (0, 0, ref.width, ref.height)
    grid = ref.grid()
    if style == "cave":
        grid = [row.replace("p", ".") for row in grid]  # caves draw any painted path as plain floor
    rows = [grid[y][x0 : x0 + w].ljust(w) if 0 <= y < ref.height else " " * w for y in range(y0, y0 + h)]
    import buildings

    placements, unplaced = _buildings(ref, layout, rows, (x0, y0, w, h), with_buildings, make_new)
    tiled = buildings.apply(placements, rows, layout, {(e["x"] - x0, e["y"] - y0) for e in ref.objects})
    fixed = buildings.fixed(placements)
    flexible, barriers = (set(), set()) if keep_shape else _flexible(ref, tiled, (x0, y0, w, h), placements)
    requested = path
    if path == "auto":
        path = path_look(ref, (x0, y0, w, h))
    blocks, tally, seams = autotile.fill(tiled, layout, style, trees, water, path, fixed, flexible, barriers)
    access = access_report(ref, blocks, layout, (x0, y0, w, h))
    tiles = TilesetPair.for_layout(layout)
    try:
        rev = platinum.revision()
    except Exception:
        rev = "?"
    families = ", ".join(f"{k} {v}" for k, v in (("trees", trees), ("water", water), ("paths", path)) if v) or "any families"
    head = [
        f"# Finished draft of {ref.header} from pret/pokeplatinum@{rev}, tiled by example from",
        f"# the original Emerald {layout['primary_tileset']} layouts (commit {original.ORIGINAL}).",
        f"# Style {style} ({families}); reference region {x0},{y0} {w}x{h}.",
        f"# {tally.get('flipped', 0)} blocks were made walkable or solid against Platinum to finish a sprite"
        + (" (--keep-shape turns this off)." if not keep_shape else "."),
        ("# Access is the same as Platinum's: the same areas to walk, surf and jump between, touching the same others."
         if not access else "# Access differs from Platinum's:"),
        *[f"#   {line}" for line in access],
        f"# {tally.get('off_style', 0)} blocks fell back off-style and {len(seams)} sit beside a block never seen",
        "# next to them in the originals: check those spots (draft --render ... --seams marks them).",
        f"#   tools/mapkit/mapkit.py draft {ref.header} {layout['id']} --finish"
        + (f" --region {x0},{y0},{w},{h}" if region else "")
        + (f" --style {style}" if style != "route" else "")
        + (" --solid-unreachable" if ref.void else "")
        + (f" --trees {trees or 'any'}" if trees != "dense" else "")
        + (f" --water {water or 'any'}" if water != "sea" else "")
        + (f" --path {requested or 'any'}" if requested != "auto" else "")
        + ("" if with_buildings else " --no-buildings")
        + ("" if make_new or not with_buildings else " --originals-only")
        + (" --keep-shape" if keep_shape else ""),
        "#",
    ]
    placed = (["# Buildings, whole from the original maps or made from their parts (name~WxH; buildings.py, parts.py):"]
              + [f"#   {p.describe()}" for p in placements] if placements else [])
    classes = ["# Tile classes it was drawn from (with the buildings in):", *[f"#   {r.rstrip()}" for r in tiled]]
    if notes_to is not None:
        notes_to += placed + classes + notes(ref, (x0, y0, w, h))
        head += [f"#   {n}" for n in unplaced]
        head.append(f"# {len(placements)} building(s) placed; the list, the tile classes and Platinum's events are in"
                    " the notes file next to this one.")
    else:
        head += placed + [f"#   {n}" for n in unplaced] + (["#"] if placements or unplaced else []) + classes
    head += [
        "",
        f"layout {layout['id']}",
        f"size {w} {h}",
        "base none",
        "",
    ]
    describe = None if notes_to is not None else lambda mid: describe_metatile(tiles, mid).split(" ", 1)[1]
    body = blocks_text(blocks, (0, 0), describe, layout if notes_to is not None else None)
    text = "\n".join(head) + "\n" + body + "\n"
    if notes_to is None:
        text += "\n".join(notes(ref, (x0, y0, w, h))) + "\n"
    return text, blocks, tally, seams


def access_report(ref: platinum.Reference, blocks, layout: dict, region) -> list[str]:
    """How where the player can go in these blocks differs from the Platinum region (empty: it doesn't).

    Compares the areas of each kind of movement (walking, surfing, each ledge
    direction...) and which touch which, with Platinum's objects as barriers;
    see autotile.access_differences.
    """
    import autotile

    x0, y0, w, h = region
    tiles = TilesetPair.for_layout(layout)
    before = [autotile._kind(ref.symbol(x0 + x, y0 + y) if 0 <= x0 + x < ref.width and 0 <= y0 + y < ref.height else " ")
              for y in range(h) for x in range(w)]
    after = [autotile._kind(emerald_symbol(tiles, blocks.get(x, y))) for y in range(h) for x in range(w)]
    walls = {(e["y"] - y0) * w + (e["x"] - x0) for e in ref.objects if 0 <= e["x"] - x0 < w and 0 <= e["y"] - y0 < h}
    return autotile.access_differences(before, after, w, h, walls)


def _flexible(ref: platinum.Reference, grid: list[str], region, placements):
    """Cells a finished draft may swap between walkable and solid, and Platinum's objects as barriers.

    Only cells on the border between walkable and solid, solid cells touching a
    building and a bridge's lanes beside water, and never the map's
    edge (where the exits are), events and the tiles around warps and signs,
    buildings or their doorsteps.
    """
    x0, y0, w, h = region
    keep: set[tuple[int, int]] = set()
    keep |= {(x, y) for x in range(w) for y in (0, h - 1)} | {(x, y) for y in range(h) for x in (0, w - 1)}
    for events, ring in ((ref.warps, True), (ref.signs, True), (ref.objects, False)):
        for e in events:
            x, y = e["x"] - x0, e["y"] - y0
            keep.add((x, y))
            if ring:
                keep |= {(x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)}
    for t in ref.triggers:
        for dy in range(t.get("length", 1)):
            for dx in range(t.get("width", 1)):
                keep.add((t["x"] - x0 + dx, t["y"] - y0 + dy))
    for p in placements:
        keep |= set(p.cells) | {(x, y + 1) for x, y in p.doors} | set(p.footprint)
    def at(x, y):
        return grid[y][x] if 0 <= y < len(grid) and 0 <= x < len(grid[y]) else " "

    # Only cells on the border between walkable and solid: finishing a sprite moves that
    # border by a block, and a forest or a field should never be eaten into.
    def n4(x, y):
        return ((x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1))

    flexible = {(x, y) for y in range(h) for x in range(w)
                if (x, y) not in keep and at(x, y) in "#."
                and any(at(nx, ny) in "#." and at(nx, ny) != at(x, y) for nx, ny in n4(x, y))}
    # Around a building: Emerald leaves a margin between a building and trees, so the
    # solid blocks touching one may open up (where that leaves access as it was).
    for p in placements:
        for x, y in p.cells:
            flexible |= {(nx, ny) for nx, ny in n4(x, y) if (nx, ny) not in keep and at(nx, ny) == "#"}
    # A bridge's outer lanes, beside water: Emerald's bridges are one block wide.
    flexible |= {(x, y) for y in range(h) for x in range(w)
                 if (x, y) not in keep and at(x, y) == "=" and any(at(nx, ny) == "~" for nx, ny in n4(x, y))}
    barriers = {(e["x"] - x0, e["y"] - y0) for e in ref.objects}
    return flexible, barriers


def notes(ref: platinum.Reference, region: tuple[int, int, int, int], relative: bool = True) -> list[str]:
    """Platinum's events and props inside the region, as comments.

    Coordinates are relative to the region's top-left (the draft's), or
    Platinum's own with `relative=False`.
    """
    rx, ry, w, h = region

    def inside(x, y):
        return 0 <= x - rx < w and 0 <= y - ry < h

    x0, y0 = (rx, ry) if relative else (0, 0)

    out = ["# ---- Platinum events (x, y" + (" in this layout" if relative else "") + ") ----"]
    for i, e in enumerate(ref.warps):
        if inside(e["x"], e["y"]):
            out.append(f"# warp {i}: ({e['x'] - x0},{e['y'] - y0}) -> {e.get('dest_header_id')} warp {e.get('dest_warp_id')}")
    for i, e in enumerate(ref.objects):
        if inside(e["x"], e["y"]):
            out.append(f"# object {i}: ({e['x'] - x0},{e['y'] - y0}) {e.get('graphics_id')} {e.get('movement_type')} {e.get('trainer_type')}")
    for i, e in enumerate(ref.signs):
        if inside(e["x"], e["y"]):
            out.append(f"# bg event {i}: ({e['x'] - x0},{e['y'] - y0}) type {e.get('type')} script {e.get('script')}")
    for i, e in enumerate(ref.triggers):
        if inside(e["x"], e["y"]):
            out.append(f"# trigger {i}: ({e['x'] - x0},{e['y'] - y0}) {e.get('width', 1)}x{e.get('length', 1)} {e.get('var')}={e.get('value')}")
    found = [b for b in ref.buildings() if any(inside(x, y) for x, y in b.tiles)]
    if found:
        out.append("# ---- Platinum buildings (footprint x, y, w x h; doors) ----")
        for b in found:
            bx, by, bw, bh = b.box
            doors = " ".join(f"({x - x0},{y - y0})" for x, y in b.doors) or "none"
            side = (" side " + " ".join(f"({x - x0},{y - y0})" for x, y in b.side)) if b.side else ""
            out.append(f"# {b.prop.name} {b.kind}: ({bx - x0},{by - y0}) {bw}x{bh}, doors {doors}{side}")
    props = [p for p in ref.props if inside(int(p.x), int(p.y))]
    if props:
        out.append("# ---- Platinum props (model, centre x, y) ----")
        for p in props:
            out.append(f"# {p.name} ({p.model}) at ({p.x - x0:g},{p.y - y0:g})")
    return out
