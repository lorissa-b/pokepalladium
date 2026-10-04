"""Draw layouts, maps (with their events), Platinum references and metatile catalogs."""

from __future__ import annotations

from functools import lru_cache

from PIL import Image, ImageDraw, ImageFont

from project import Blockdata, behaviors, consts, metatile_labels
from tileset import TilesetPair

WARP = (255, 64, 255)
OBJECT = (64, 160, 255)
SIGN = (255, 220, 0)
COORD = (64, 255, 96)
GRID = (0, 0, 0, 90)
BLOCKED = (255, 0, 0, 100)
MISMATCH = (255, 0, 0, 115)
PROP = (255, 255, 255)
SEAM = (255, 60, 200)

# One colour per elevation for the elevation overlay.
ELEVATION_COLOURS = [
    (40, 40, 40), (0, 90, 255), (0, 200, 255), (0, 220, 120),
    (180, 230, 0), (255, 170, 0), (255, 60, 0), (255, 0, 200),
]

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


@lru_cache(maxsize=None)
def font(size: int) -> ImageFont.FreeTypeFont:
    return ImageFont.load_default(size=size)


def label_size(ts: int) -> int:
    """Font size for labels on tiles ts pixels wide."""
    return max(8, min(14, ts // 2 + 2))


def drawer(img: Image.Image) -> ImageDraw.ImageDraw:
    """A drawing context that blends RGBA colours onto the image."""
    return ImageDraw.Draw(img, "RGBA")


def text(d: ImageDraw.ImageDraw, xy, s: str, fill=(255, 255, 255), size: int = 10, anchor: str = "la") -> None:
    d.text(xy, s, fill=fill, font=font(size), anchor=anchor, stroke_width=max(1, size // 8), stroke_fill=(0, 0, 0))


def draw_blocks(blocks: Blockdata, tiles: TilesetPair, scale: int = 1, region=None) -> Image.Image:
    x0, y0, w, h = region or (0, 0, blocks.width, blocks.height)
    img = Image.new("RGB", (w * 16, h * 16), tiles.backdrop)
    c = consts()
    for y in range(h):
        for x in range(w):
            if blocks.inside(x0 + x, y0 + y):
                img.paste(tiles.draw(blocks.get(x0 + x, y0 + y) & c.metatile_mask), (x * 16, y * 16))
    return scaled(img, scale)


def scaled(img: Image.Image, scale: int) -> Image.Image:
    return img if scale == 1 else img.resize((img.width * scale, img.height * scale), Image.Resampling.NEAREST)


def overlay_grid(img: Image.Image, w: int, h: int, ts: int, origin=(0, 0), labels: bool = True) -> None:
    """Tile grid, with coordinate labels every 4 tiles along the top and left."""
    d = drawer(img)
    ox, oy = origin
    for x in range(w + 1):
        d.line([(x * ts, 0), (x * ts, h * ts)], fill=GRID)
    for y in range(h + 1):
        d.line([(0, y * ts), (w * ts, y * ts)], fill=GRID)
    if not labels:
        return
    size = label_size(ts)
    for x in range(w):
        if (ox + x) % 4 == 0:
            text(d, (x * ts + 2, 1), str(ox + x), size=size)
    for y in range(1, h):
        if (oy + y) % 4 == 0:
            text(d, (2, y * ts + 1), str(oy + y), size=size)


def tint(img: Image.Image, cells, ts: int, rgba) -> None:
    d = drawer(img)
    for x, y in cells:
        d.rectangle([x * ts, y * ts, (x + 1) * ts - 1, (y + 1) * ts - 1], fill=rgba)


def outline_cells(img: Image.Image, cells, ts: int, rgb) -> None:
    d = drawer(img)
    for x, y in cells:
        d.rectangle([x * ts, y * ts, (x + 1) * ts - 1, (y + 1) * ts - 1], outline=rgb + (220,), width=max(1, ts // 10))


def overlay_collision(img: Image.Image, blocks: Blockdata, ts: int, region=None) -> None:
    c = consts()
    x0, y0, w, h = region or (0, 0, blocks.width, blocks.height)
    tint(img, [(x, y) for y in range(h) for x in range(w)
               if blocks.inside(x0 + x, y0 + y) and c.unpack(blocks.get(x0 + x, y0 + y))[1]], ts, BLOCKED)


def overlay_elevation(img: Image.Image, blocks: Blockdata, ts: int, region=None) -> None:
    c = consts()
    d = drawer(img)
    x0, y0, w, h = region or (0, 0, blocks.width, blocks.height)
    for y in range(h):
        for x in range(w):
            if not blocks.inside(x0 + x, y0 + y):
                continue
            _, col, elev = c.unpack(blocks.get(x0 + x, y0 + y))
            d.rectangle([x * ts, y * ts, (x + 1) * ts - 1, (y + 1) * ts - 1], fill=ELEVATION_COLOURS[elev % 8] + (115,))
            if ts >= 16:
                text(d, (x * ts + ts // 2, y * ts + ts // 2), ("X" if col else "") + str(elev), size=label_size(ts), anchor="mm")


def _mark(d, ts: int, x, y, rgb, label: str, w: int = 1, h: int = 1, fill: int = 0, label_at: str = "bottom") -> None:
    box = [x * ts, y * ts, (x + w) * ts - 1, (y + h) * ts - 1]
    d.rectangle(box, outline=rgb, width=max(1, ts // 8), fill=rgb + (fill,) if fill else None)
    if ts >= 8:
        size = label_size(ts)
        if label_at == "bottom":
            text(d, (x * ts + 2, (y + h) * ts - 1), label, fill=rgb, size=size, anchor="ld")
        else:
            text(d, (x * ts + 2, y * ts + 1), label, fill=rgb, size=size)


def overlay_events(img: Image.Image, info: dict, ts: int, region=None) -> None:
    """Mark warps (W), objects (O), signs/hidden items (S) and triggers (T), numbered as in map.json."""
    d = drawer(img)
    x0, y0 = (region or (0, 0))[:2]
    for i, e in enumerate(info.get("coord_events") or []):
        _mark(d, ts, e["x"] - x0, e["y"] - y0, COORD, f"T{i}", fill=60)
    for i, e in enumerate(info.get("bg_events") or []):
        _mark(d, ts, e["x"] - x0, e["y"] - y0, SIGN, f"S{i}")
    for i, e in enumerate(info.get("object_events") or []):
        _mark(d, ts, e["x"] - x0, e["y"] - y0, OBJECT, f"O{i}")
    for i, e in enumerate(info.get("warp_events") or []):
        _mark(d, ts, e["x"] - x0, e["y"] - y0, WARP, f"W{i}")


def catalog(tiles: TilesetPair, columns: int = 16, scale: int = 2, only: list[int] | None = None,
            notes: dict[int, str] | None = None) -> Image.Image:
    """Every metatile in a tileset pair, labelled with its hex id (and a note under each, if given)."""
    ids = only if only is not None else tiles.ids()
    cell = 16 * scale
    if notes:
        cell = max(cell, 56)
    pad = 12
    foot = 24 if notes else 0
    rows = max(1, (len(ids) + columns - 1) // columns)
    img = Image.new("RGB", (columns * cell, rows * (cell + pad + foot)), (32, 32, 32))
    d = drawer(img)
    for n, mid in enumerate(ids):
        x, y = (n % columns) * cell, (n // columns) * (cell + pad + foot)
        tile = scaled(tiles.draw(mid), scale)
        img.paste(tile, (x + (cell - tile.width) // 2, y + pad))
        d.text((x + cell // 2, y + pad // 2), f"{mid:03X}", fill=(255, 255, 255), font=font(10), anchor="mm")
        if notes and notes.get(mid):
            note = notes[mid]
            text_, colour = (note, (255, 230, 120)) if isinstance(note, str) else note
            for k, line in enumerate(text_.split()[:2]):
                d.text((x + cell // 2, y + pad + 16 * scale + 6 + k * 10), line, fill=colour, font=font(9), anchor="mm")
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


def draw_classes(grid: list[str], scale: int = 8) -> Image.Image:
    """A grid of tile-class characters as coloured squares (ledges show their direction)."""
    h, w = len(grid), max((len(r) for r in grid), default=0)
    img = Image.new("RGB", (w * scale, h * scale))
    d = drawer(img)
    for y, row in enumerate(grid):
        for x, ch in enumerate(row):
            d.rectangle([x * scale, y * scale, (x + 1) * scale - 1, (y + 1) * scale - 1], fill=CLASS_COLOURS.get(ch, (255, 0, 255)))
            if ch in "^v<>" and scale >= 6:
                cx, cy, r = x * scale + scale / 2, y * scale + scale / 2, scale / 3
                tri = {
                    "^": [(cx, cy - r), (cx - r, cy + r), (cx + r, cy + r)],
                    "v": [(cx, cy + r), (cx - r, cy - r), (cx + r, cy - r)],
                    "<": [(cx - r, cy), (cx + r, cy - r), (cx + r, cy + r)],
                    ">": [(cx + r, cy), (cx - r, cy - r), (cx - r, cy + r)],
                }[ch]
                d.polygon(tri, fill=(255, 255, 255))
    return img


def overlay_reference(img: Image.Image, ref, ts: int, origin=(0, 0)) -> None:
    """Platinum events and props on a class image (or an Emerald render aligned to it)."""
    d = drawer(img)
    ox, oy = origin
    for p in ref.props:
        px, py = (p.x - ox) * ts, (p.y - oy) * ts
        r = max(2, ts // 6)
        d.ellipse([px - r, py - r, px + r, py + r], fill=PROP, outline=(0, 0, 0))
        if ts >= 8:
            text(d, (px + r + 1, py), f"P{p.model}", size=label_size(ts), anchor="lm")
    for i, e in enumerate(ref.triggers):
        _mark(d, ts, e["x"] - ox, e["y"] - oy, COORD, f"T{i}", e.get("width", 1), e.get("length", 1), label_at="top")
    for i, e in enumerate(ref.signs):
        _mark(d, ts, e["x"] - ox, e["y"] - oy, SIGN, f"S{i}", label_at="top")
    for i, e in enumerate(ref.objects):
        _mark(d, ts, e["x"] - ox, e["y"] - oy, OBJECT, f"O{i}", label_at="top")
    for i, e in enumerate(ref.warps):
        _mark(d, ts, e["x"] - ox, e["y"] - oy, WARP, f"W{i}", label_at="top")


def pieces_sheet(pieces, tiles: TilesetPair, scale: int = 2, columns: int = 6) -> Image.Image:
    """Buildings (buildings.Piece) drawn one per cell with their names; doors outlined."""
    if not pieces:
        return Image.new("RGB", (64, 32), (32, 32, 32))
    ts = 16 * scale
    cw = max(p.w for p in pieces) * ts + 12
    ch = max(p.h for p in pieces) * ts + 30
    rows = (len(pieces) + columns - 1) // columns
    img = Image.new("RGB", (min(columns, len(pieces)) * cw, rows * ch), (32, 32, 32))
    d = drawer(img)
    for n, p in enumerate(pieces):
        ox, oy = (n % columns) * cw + 6, (n // columns) * ch + 18
        for dx, dy, block in p.blocks():
            img.paste(scaled(tiles.draw(block & consts().metatile_mask), scale), (ox + dx * ts, oy + dy * ts))
        for dx, dy in p.doors:
            d.rectangle([ox + dx * ts, oy + dy * ts, ox + (dx + 1) * ts - 1, oy + (dy + 1) * ts - 1], outline=WARP, width=2)
        text(d, (ox, oy - 16), f"{p.name} {p.w}x{p.h} {p.kind}", size=10)
    return img


def side_by_side(*images: Image.Image, gap: int = 8, titles: list[str] | None = None) -> Image.Image:
    """Images next to each other, top-aligned, with optional titles above."""
    head = 18 if titles else 0
    w = sum(i.width for i in images) + gap * (len(images) - 1)
    h = max(i.height for i in images) + head
    out = Image.new("RGB", (w, h), (24, 24, 24))
    d = drawer(out)
    x = 0
    for n, im in enumerate(images):
        out.paste(im, (x, head))
        if titles:
            text(d, (x + 2, 2), titles[n], size=12)
        x += im.width + gap
    return out
