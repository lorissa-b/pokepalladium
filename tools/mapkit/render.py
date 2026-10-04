"""Draw layouts, maps (with their events) and metatile catalogs to PNG."""

from __future__ import annotations

from png import Image, draw_text, text_width
from project import Blockdata, behaviors, consts, metatile_labels
from tileset import TilesetPair

WARP = (255, 64, 255)
OBJECT = (64, 160, 255)
SIGN = (255, 220, 0)
COORD = (64, 255, 96)
GRID = (0, 0, 0)
BLOCKED = (255, 0, 0)

# One colour per elevation for the elevation overlay.
ELEVATION_COLOURS = [
    (40, 40, 40), (0, 90, 255), (0, 200, 255), (0, 220, 120),
    (180, 230, 0), (255, 170, 0), (255, 60, 0), (255, 0, 200),
]


def draw_blocks(blocks: Blockdata, tiles: TilesetPair, scale: int = 1, region=None) -> Image:
    x0, y0, w, h = region or (0, 0, blocks.width, blocks.height)
    img = Image(w * 16, h * 16, tiles.backdrop)
    c = consts()
    for y in range(h):
        for x in range(w):
            if blocks.inside(x0 + x, y0 + y):
                mid = blocks.get(x0 + x, y0 + y) & c.metatile_mask
                img.paste_rgb(x * 16, y * 16, 16, 16, tiles.draw(mid))
    return img.scaled(scale)


def overlay_grid(img: Image, w: int, h: int, ts: int, origin=(0, 0), every: int = 1, labels: bool = True) -> None:
    """Tile grid, with coordinate labels every 4 tiles along the top and left."""
    ox, oy = origin
    for y in range(h):
        for x in range(w):
            if (ox + x) % every == 0:
                for j in range(ts):
                    img.blend(x * ts, y * ts + j, GRID, 0.35)
            if (oy + y) % every == 0:
                for i in range(ts):
                    img.blend(x * ts + i, y * ts, GRID, 0.35)
    if not labels:
        return
    for x in range(w):
        if (ox + x) % 4 == 0:
            draw_text(img, x * ts + 1, 1, str(ox + x))
    for y in range(h):
        if (oy + y) % 4 == 0 and y:
            draw_text(img, 1, y * ts + 1, str(oy + y))


def overlay_collision(img: Image, blocks: Blockdata, ts: int, region=None) -> None:
    c = consts()
    x0, y0, w, h = region or (0, 0, blocks.width, blocks.height)
    for y in range(h):
        for x in range(w):
            if blocks.inside(x0 + x, y0 + y) and c.unpack(blocks.get(x0 + x, y0 + y))[1]:
                img.rect(x * ts, y * ts, ts, ts, BLOCKED, 0.4)


def overlay_elevation(img: Image, blocks: Blockdata, ts: int, region=None) -> None:
    c = consts()
    x0, y0, w, h = region or (0, 0, blocks.width, blocks.height)
    for y in range(h):
        for x in range(w):
            if blocks.inside(x0 + x, y0 + y):
                _, col, elev = c.unpack(blocks.get(x0 + x, y0 + y))
                img.rect(x * ts, y * ts, ts, ts, ELEVATION_COLOURS[elev % 8], 0.45)
                if ts >= 16:
                    draw_text(img, x * ts + 2, y * ts + 2, ("X" if col else "") + str(elev), bg=None)


def overlay_events(img: Image, info: dict, ts: int, region=None) -> None:
    """Mark warps (W), objects (O), signs/hidden items (S) and triggers (T), numbered as in map.json."""
    x0, y0 = (region or (0, 0))[:2]

    def mark(x, y, rgb, label, w=1, h=1, fill=0.0):
        px, py = (x - x0) * ts, (y - y0) * ts
        if fill:
            img.rect(px, py, w * ts, h * ts, rgb, fill)
        img.outline(px, py, w * ts, h * ts, rgb)
        img.outline(px + 1, py + 1, w * ts - 2, h * ts - 2, rgb)
        if ts >= 16:
            draw_text(img, px + 2, py + ts - 7, label, fg=rgb)

    for i, e in enumerate(info.get("coord_events") or []):
        mark(e["x"], e["y"], COORD, f"T{i}", fill=0.25)
    for i, e in enumerate(info.get("bg_events") or []):
        mark(e["x"], e["y"], SIGN, f"S{i}")
    for i, e in enumerate(info.get("object_events") or []):
        mark(e["x"], e["y"], OBJECT, f"O{i}")
    for i, e in enumerate(info.get("warp_events") or []):
        mark(e["x"], e["y"], WARP, f"W{i}")


def catalog(tiles: TilesetPair, columns: int = 16, scale: int = 2, only: list[int] | None = None) -> Image:
    """Every metatile in a tileset pair, labelled with its hex id."""
    ids = only if only is not None else tiles.ids()
    cell = 16 * scale
    pad = 9
    rows = (len(ids) + columns - 1) // columns
    img = Image(columns * cell, rows * (cell + pad), (32, 32, 32))
    for n, mid in enumerate(ids):
        x, y = (n % columns) * cell, (n // columns) * (cell + pad)
        tile = Image(16, 16)
        tile.px = bytearray(tiles.draw(mid))
        big = tile.scaled(scale)
        img.paste_rgb(x, y + pad, cell, cell, big.px)
        label = f"{mid:03X}"
        draw_text(img, x + (cell - text_width(label)) // 2, y + 1, label, bg=None)
    return img


def describe_metatile(tiles: TilesetPair, mid: int) -> str:
    """'0x00D MB_TALL_GRASS layer=1 General_TallGrass' style summary."""
    c = consts()
    attr = tiles.attributes(mid)
    if attr is None:
        return f"{mid:#05x} (missing)"
    beh = behaviors().get(attr & c.behavior_mask, f"MB_{attr & c.behavior_mask:#x}")
    layer = (attr & c.layer_mask) >> c.layer_shift
    ts = tiles.primary if mid < c.metatiles_in_primary else tiles.secondary
    label = metatile_labels().get(ts.symbol, {}).get(mid, "")
    return f"{mid:#05x} {beh} layer={layer} {label}".rstrip()


# Colours for the tile classes in platinum.LEGEND.
CLASS_COLOURS = {
    "#": (40, 72, 40), ".": (196, 220, 150), '"': (80, 170, 60), "Y": (40, 130, 40),
    "~": (70, 120, 230), "|": (150, 190, 255), ",": (140, 180, 230), ":": (230, 210, 140),
    "i": (200, 240, 255), "m": (130, 100, 60), "s": (245, 245, 250), "^": (170, 120, 60),
    "v": (170, 120, 60), "<": (170, 120, 60), ">": (170, 120, 60), "D": (200, 60, 60),
    "E": (230, 120, 40), "S": (180, 80, 180), "R": (120, 110, 100), "=": (170, 140, 100),
    "B": (120, 70, 40), "t": (150, 120, 90), "o": (110, 90, 140), " ": (0, 0, 0),
    "X": (255, 0, 0),
}


def draw_classes(grid: list[str], scale: int = 8) -> Image:
    """A grid of tile-class characters as coloured squares (ledges show their direction)."""
    h, w = len(grid), max((len(r) for r in grid), default=0)
    img = Image(w * scale, h * scale)
    for y, row in enumerate(grid):
        for x, ch in enumerate(row):
            img.rect(x * scale, y * scale, scale, scale, CLASS_COLOURS.get(ch, (255, 0, 255)))
            if ch in "^v<>" and scale >= 6:
                cx, cy = x * scale + scale // 2, y * scale + scale // 2
                dx, dy = {"^": (0, -1), "v": (0, 1), "<": (-1, 0), ">": (1, 0)}[ch]
                for k in range(scale // 3):
                    img.put(cx + dx * k, cy + dy * k, (255, 255, 255))
    return img


def overlay_reference(img: Image, ref, ts: int, origin=(0, 0)) -> None:
    """Platinum events and props on a class image (or an Emerald render aligned to it)."""
    ox, oy = origin

    def mark(x, y, rgb, label, w=1, h=1):
        px, py = (x - ox) * ts, (y - oy) * ts
        img.outline(px, py, w * ts, h * ts, rgb)
        if ts >= 8:
            draw_text(img, px + 1, py + 1, label, fg=rgb)

    for p in ref.props:
        px, py = int((p.x - ox) * ts), int((p.y - oy) * ts)
        img.rect(px - 1, py - 1, 3, 3, (255, 255, 255))
        if ts >= 8:
            draw_text(img, px + 3, py - 2, f"P{p.model}", fg=(255, 255, 255))
    for i, e in enumerate(ref.triggers):
        mark(e["x"], e["y"], COORD, f"T{i}", e.get("width", 1), e.get("length", 1))
    for i, e in enumerate(ref.signs):
        mark(e["x"], e["y"], SIGN, f"S{i}")
    for i, e in enumerate(ref.objects):
        mark(e["x"], e["y"], OBJECT, f"O{i}")
    for i, e in enumerate(ref.warps):
        mark(e["x"], e["y"], WARP, f"W{i}")
