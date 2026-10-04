"""Compare Emerald maps with Platinum references, and draft blueprints from them.

Both games' tiles are reduced to the same one-character classes (see
platinum.SYMBOLS / platinum.LEGEND), so a replica can be scored tile by tile
against the original and a reference can be turned into a starting layout.
"""

from __future__ import annotations

import re

import platinum
from project import Blockdata, behaviors, consts
from tileset import TilesetPair

# Classes that are the same for movement purposes.
GROUPS = {
    "walk": set('."Y:,DESm=sBi'),
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
    (r"COUNTER", "t"),
    (r"PC$|TELEVISION|REGION_MAP|BOOKSHELF|SHOP_SHELF|TRASH_CAN|VASE|PICTURE_BOOK", "o"),
    (r"BERRY_TREE_SOIL", "B"),
    (r"MUDDY_SLOPE", "m"),
]


def emerald_symbol(tiles: TilesetPair, block: int) -> str:
    c = consts()
    mid, col, _ = c.unpack(block)
    name = behaviors().get(tiles.behavior(mid), "")
    for pattern, sym in _EMERALD_RULES:
        if re.search(pattern, name):
            # Furniture, counters and ledges are blocked by collision in Emerald too.
            return sym
    return "#" if col else "."


def emerald_grid(layout: dict) -> list[str]:
    blocks = Blockdata.for_layout(layout)
    tiles = TilesetPair.for_layout(layout)
    return ["".join(emerald_symbol(tiles, blocks.get(x, y)) for x in range(blocks.width)) for y in range(blocks.height)]


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
        '"': "General_TallGrass",
        "Y": "General_LongGrass",
        "~": "General_CalmWater",
        ",": "0x13B  # shallow water",
        ":": "0x124  # sand",
        "v": "0x087  # ledge, jump south",
        "<": "0x085  # ledge, jump west",
        ">": "0x086  # ledge, jump east",
        "^": "[0x1D4 0x1D5; 0x1DC 0x1DD]  # Emerald has no north ledges",
        "D": "General_Door  # placeholder: stamp the real building",
        "|": "0x04C  # waterfall",
        "E": "General_Grass  # TODO: warp tile (cave entrance, arrow warp...)",
    },
}


def draft(ref: platinum.Reference, layout: dict, region: tuple[int, int, int, int] | None = None) -> str:
    x0, y0, w, h = region or (0, 0, ref.width, ref.height)
    grid = ref.grid()
    rows = [grid[y][x0 : x0 + w] if 0 <= y < ref.height else "" for y in range(y0, y0 + h)]
    used = sorted({ch for r in rows for ch in r} - {" "})
    legends = DRAFT_LEGENDS.get(layout["primary_tileset"], {})
    try:
        rev = platinum.revision()
    except Exception:
        rev = "?"
    out = [
        f"# Draft of {ref.header} from pret/pokeplatinum@{rev}",
        f"# Reference region {x0},{y0} {w}x{h} of its {ref.width}x{ref.height} tiles. Regenerate with:",
        f"#   tools/mapkit/mapkit.py draft {ref.header} {layout['id']}" + (f" --region {x0},{y0},{w},{h}" if region else ""),
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
    out += notes(ref, (x0, y0, w, h))
    return "\n".join(out) + "\n"


def finished_draft(ref: platinum.Reference, layout: dict, region: tuple[int, int, int, int] | None = None,
                   style: str = "route", trees: str | None = "dense", water: str | None = "sea"):
    """A blueprint with every block chosen, tiled by example (see autotile.py).

    Returns (blueprint text, blocks, tally, seams).
    """
    import autotile
    import original
    from blueprint import blocks_text
    from render import describe_metatile

    x0, y0, w, h = region or (0, 0, ref.width, ref.height)
    grid = ref.grid()
    rows = [grid[y][x0 : x0 + w].ljust(w) if 0 <= y < ref.height else " " * w for y in range(y0, y0 + h)]
    blocks, tally, seams = autotile.fill(rows, layout, style, trees, water)
    tiles = TilesetPair.for_layout(layout)
    try:
        rev = platinum.revision()
    except Exception:
        rev = "?"
    families = ", ".join(f"{k} {v}" for k, v in (("trees", trees), ("water", water)) if v) or "any families"
    head = [
        f"# Finished draft of {ref.header} from pret/pokeplatinum@{rev}, tiled by example from",
        f"# the original Emerald {layout['primary_tileset']} layouts (commit {original.ORIGINAL}).",
        f"# Style {style} ({families}); reference region {x0},{y0} {w}x{h}.",
        f"# {tally.get('off_style', 0)} blocks fell back off-style and {len(seams)} sit beside a block never seen",
        "# next to them in the originals: check those spots (draft --render ... --seams marks them).",
        f"#   tools/mapkit/mapkit.py draft {ref.header} {layout['id']} --finish"
        + (f" --region {x0},{y0},{w},{h}" if region else "")
        + (f" --style {style}" if style != "route" else "")
        + (f" --trees {trees or 'any'}" if trees != "dense" else "")
        + (f" --water {water or 'any'}" if water != "sea" else ""),
        "#",
        "# Tile classes it was drawn from:",
        *[f"#   {r.rstrip()}" for r in rows],
        "",
        f"layout {layout['id']}",
        f"size {w} {h}",
        "base none",
        "",
    ]
    body = blocks_text(blocks, (0, 0), lambda mid: describe_metatile(tiles, mid).split(" ", 1)[1])
    text = "\n".join(head) + "\n" + body + "\n" + "\n".join(notes(ref, (x0, y0, w, h))) + "\n"
    return text, blocks, tally, seams


def notes(ref: platinum.Reference, region: tuple[int, int, int, int]) -> list[str]:
    """Platinum's events and props in the draft's coordinates, as comments."""
    x0, y0, w, h = region

    def inside(x, y):
        return 0 <= x - x0 < w and 0 <= y - y0 < h

    out = ["# ---- Platinum events (x, y in this layout) ----"]
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
    props = [p for p in ref.props if inside(int(p.x), int(p.y))]
    if props:
        out.append("# ---- Platinum props (model, centre x, y) ----")
        for p in props:
            out.append(f"# {p.name} ({p.model}) at ({p.x - x0:g},{p.y - y0:g})")
    return out
