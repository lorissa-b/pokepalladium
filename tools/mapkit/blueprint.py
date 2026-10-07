"""Blueprints: a small text format that builds a layout's map.bin.

    # Comments start with '#' on directive lines.
    layout LAYOUT_JUBILIFE_CITY     # target: tilesets and map.bin path from layouts.json
    size 64 48                      # optional; build updates layouts.json to match
    base self                       # start from: self (current map.bin), none, or another layout
    fill tree                       # fill everything with a block
    rect 0 40 64 8 0x001            # fill a rectangle
    stamp LAYOUT_PETALBURG_CITY 10 4 5 5 at 20 30   # copy blocks from another layout
    set 12 7 General_Door           # one block
    legend T = [0x1D4 0x1D5; 0x1DC 0x1DD]          # grid key -> block
    legend . = 0x001
    grid 0 0                        # rows of keys follow, until 'end'
    TTTT....TTTT
    TT........TT
    end

A block is a metatile with optional collision and elevation:

    0x1CE  462  General_Grass  METATILE_General_Grass     metatile by id or label
    0x00E/c1/e3                                           explicit collision / elevation
    [0x1D4 0x1D5; 0x1DC 0x1DD]                            pattern, picked by map x/y
    keep                                                  leave what is there

Without /c and /e, a metatile gets the collision and elevation it most often
has in the existing layouts that share its tileset, so trees come out
impassable and water at elevation 1 without spelling it out.

In a grid, a space (or a key with no legend entry, which is an error)
keeps the block underneath, and short rows leave the rest untouched.
`grid X Y w2` reads two characters per cell, for regions with more distinct
blocks than there are single characters.
"""

from __future__ import annotations

import json
import re
from collections import Counter, defaultdict
from dataclasses import dataclass
from functools import lru_cache
from pathlib import Path

from project import LAYOUTS_JSON, REPO, Blockdata, consts, label_values, layouts, resolve_layout

KEEP = None


class BlueprintError(Exception):
    pass


@dataclass
class Block:
    """A metatile with collision and elevation, or a pattern of them."""

    cells: list[list[int]]  # packed blocks, [row][col]; a plain block is 1x1

    def at(self, x: int, y: int) -> int:
        row = self.cells[y % len(self.cells)]
        return row[x % len(row)]


@lru_cache(maxsize=None)
def usage_defaults() -> dict[tuple[str, int], tuple[int, int]]:
    """(tileset symbol, metatile) -> its most common (collision, elevation) across all layouts."""
    c = consts()
    counts: dict[tuple[str, int], Counter] = defaultdict(Counter)
    for layout in layouts().values():
        try:
            blocks = Blockdata.for_layout(layout)
        except (OSError, ValueError):
            continue
        for v in blocks.blocks:
            mid, col, elev = c.unpack(v)
            ts = layout["primary_tileset"] if mid < c.metatiles_in_primary else layout["secondary_tileset"]
            counts[(ts, mid)][(col, elev)] += 1
    return {key: cnt.most_common(1)[0][0] for key, cnt in counts.items()}


def default_attrs(metatile: int, layout: dict) -> tuple[int, int]:
    c = consts()
    ts = layout["primary_tileset"] if metatile < c.metatiles_in_primary else layout["secondary_tileset"]
    return usage_defaults().get((ts, metatile), (0, 3))


def parse_metatile(token: str) -> int:
    if re.fullmatch(r"0x[0-9A-Fa-f]+|\d+", token):
        return int(token, 0)
    name = token[len("METATILE_"):] if token.startswith("METATILE_") else token
    labels = label_values()
    if name in labels:
        return labels[name]
    raise BlueprintError(f"unknown metatile {token!r}")


def parse_single(token: str, layout: dict) -> int:
    parts = token.split("/")
    mid = parse_metatile(parts[0])
    col, elev = default_attrs(mid, layout)
    for mod in parts[1:]:
        if re.fullmatch(r"c\d+", mod):
            col = int(mod[1:])
        elif re.fullmatch(r"e\d+", mod):
            elev = int(mod[1:])
        else:
            raise BlueprintError(f"bad modifier {mod!r} in {token!r} (use /cN or /eN)")
    try:
        return consts().pack(mid, col, elev)
    except ValueError as e:
        raise BlueprintError(f"{token}: {e}") from None


def parse_block(text: str, layout: dict, names: dict[str, Block | None]) -> Block | None:
    text = text.strip()
    if text == "keep":
        return KEEP
    if text in names:
        return names[text]
    if text.startswith("["):
        if not text.endswith("]"):
            raise BlueprintError(f"unclosed pattern {text!r}")
        rows = [r.split() for r in text[1:-1].split(";")]
        if not rows or any(not r for r in rows):
            raise BlueprintError(f"empty row in pattern {text!r}")
        return Block([[parse_single(t, layout) for t in r] for r in rows])
    if " " in text:
        raise BlueprintError(f"one block expected, got {text!r} (patterns go in [...])")
    return Block([[parse_single(text, layout)]])


def _strip_comment(line: str) -> str:
    return re.sub(r"(^|\s)#.*$", "", line).strip()


class Blueprint:
    def __init__(self, text: str, source: str = "<blueprint>", layout_override: str | None = None) -> None:
        self.source = source
        self.lines = text.splitlines()
        self.layout: dict | None = resolve_layout(layout_override) if layout_override else None
        self.size: tuple[int, int] | None = None
        self.warnings: list[str] = []

    def error(self, lineno: int, msg: str) -> BlueprintError:
        return BlueprintError(f"{self.source}:{lineno}: {msg}")

    def build(self) -> Blockdata:
        layout = self.layout
        blocks: Blockdata | None = None
        legend: dict[str, Block | None] = {}
        names: dict[str, Block | None] = {}
        i = 0

        def need_grid(lineno: int) -> Blockdata:
            nonlocal blocks
            if layout is None:
                raise self.error(lineno, "no `layout` line before the first drawing command")
            if blocks is None:
                w, h = self.size or (layout["width"], layout["height"])
                blocks = Blockdata(w, h)
                self._base_self(blocks, layout)
            return blocks

        def ints(lineno: int, values: list[str], n: int) -> list[int]:
            if len(values) < n:
                raise self.error(lineno, f"expected {n} numbers")
            try:
                return [int(v, 0) for v in values[:n]]
            except ValueError:
                raise self.error(lineno, f"expected numbers, got {' '.join(values[:n])}") from None

        while i < len(self.lines):
            lineno = i + 1
            raw = self.lines[i]
            i += 1
            # Legend keys can be any character, '#' included, so split them off before comments.
            m = re.match(r"\s*legend (\S{1,2}) = (.*)$", raw)
            if m:
                try:
                    legend[m.group(1)] = parse_block(_strip_comment(m.group(2)), layout, names)
                except BlueprintError as e:
                    raise self.error(lineno, str(e)) from None
                continue
            line = _strip_comment(raw)
            if not line:
                continue
            word, _, rest = line.partition(" ")
            args = rest.split()
            try:
                if word == "layout":
                    if self.layout is None:
                        layout = self.layout = resolve_layout(args[0])
                elif word == "size":
                    if blocks is not None:
                        raise self.error(lineno, "`size` must come before drawing")
                    self.size = tuple(ints(lineno, args, 2))
                elif word == "base":
                    if layout is None:
                        raise self.error(lineno, "`base` needs a `layout` line first")
                    w, h = self.size or (layout["width"], layout["height"])
                    blocks = Blockdata(w, h)
                    src = args[0] if args else "self"
                    if src == "self":
                        self._base_self(blocks, layout)
                    elif src != "none":
                        blocks.paste(Blockdata.for_layout(resolve_layout(src)), 0, 0)
                elif word == "define":
                    name, _, spec = rest.partition("=")
                    names[name.strip()] = parse_block(spec, layout, names)
                elif word == "legend":
                    raise self.error(lineno, "expected `legend K = BLOCK` (a 1 or 2 character key)")
                elif word == "fill":
                    g = need_grid(lineno)
                    self._fill(g, 0, 0, g.width, g.height, parse_block(rest, layout, names))
                elif word == "rect":
                    g = need_grid(lineno)
                    x, y, w, h = ints(lineno, args, 4)
                    self._fill(g, x, y, w, h, parse_block(" ".join(args[4:]), layout, names))
                elif word == "set":
                    g = need_grid(lineno)
                    x, y = ints(lineno, args, 2)
                    self._fill(g, x, y, 1, 1, parse_block(" ".join(args[2:]), layout, names))
                elif word == "stamp":
                    g = need_grid(lineno)
                    src = resolve_layout(args[0])
                    x, y, w, h = ints(lineno, args[1:], 4)
                    dx, dy = x, y
                    if len(args) > 5:
                        if args[5] != "at" or len(args) < 8:
                            raise self.error(lineno, "expected `stamp SOURCE X Y W H [at DX DY]`")
                        dx, dy = ints(lineno, args[6:], 2)
                    piece = Blockdata.for_layout(src).crop(x, y, w, h)
                    self._check_stamp_tilesets(lineno, src, layout, piece)
                    g.paste(piece, dx, dy)
                elif word == "grid":
                    g = need_grid(lineno)
                    x0, y0 = ints(lineno, args, 2)
                    width = int(args[2][1:]) if len(args) > 2 and re.fullmatch(r"w\d", args[2]) else 1
                    row = 0
                    while True:
                        if i >= len(self.lines):
                            raise self.error(lineno, "grid has no `end`")
                        text = self.lines[i].rstrip("\n")
                        i += 1
                        if text.strip() == "end":
                            break
                        for col in range(0, len(text.rstrip()), width):
                            key = text[col : col + width].ljust(width)
                            if key.strip() == "":
                                continue
                            if key not in legend:
                                raise self.error(i, f"no legend entry for {key!r}")
                            block = legend[key]
                            if block is not KEEP:
                                gx, gy = x0 + col // width, y0 + row
                                g.set(gx, gy, block.at(gx, gy))
                        row += 1
                else:
                    raise self.error(lineno, f"unknown command {word!r}")
            except BlueprintError as e:
                if str(e).startswith(self.source):
                    raise
                raise self.error(lineno, str(e)) from None
            except (IndexError, ValueError) as e:
                raise self.error(lineno, f"{line!r}: {e}") from None
        if blocks is None:
            blocks = need_grid(len(self.lines))
        return blocks

    @staticmethod
    def _fill(g: Blockdata, x: int, y: int, w: int, h: int, block: Block | None) -> None:
        if block is KEEP:
            return
        for yy in range(y, y + h):
            for xx in range(x, x + w):
                g.set(xx, yy, block.at(xx, yy))

    @staticmethod
    def _base_self(g: Blockdata, layout: dict) -> None:
        """Start from the target's current blocks (cropped or padded to the new size)."""
        path = REPO / layout["blockdata_filepath"]
        if not path.exists():
            return
        raw = path.read_bytes()
        if len(raw) == layout["width"] * layout["height"] * 2:
            g.paste(Blockdata.for_layout(layout), 0, 0)

    def _check_stamp_tilesets(self, lineno: int, src: dict, layout: dict, piece: Blockdata) -> None:
        if src["primary_tileset"] != layout["primary_tileset"]:
            raise self.error(lineno, f"{src['id']} uses {src['primary_tileset']}, not {layout['primary_tileset']}")
        if src["secondary_tileset"] == layout["secondary_tileset"]:
            return
        # Secondary metatile ids only mean the same thing with the same secondary tileset.
        c = consts()
        if any((b & c.metatile_mask) >= c.metatiles_in_primary for b in piece.blocks):
            self.warnings.append(
                f"{self.source}:{lineno}: stamped {src['secondary_tileset']} metatiles from {src['id']} "
                f"into a {layout['secondary_tileset']} layout; they will draw as different tiles"
            )


def write(layout: dict, blocks: Blockdata) -> list[str]:
    """Save blocks as the layout's map.bin, resizing the layouts.json entry if needed."""
    notes = []
    if (blocks.width, blocks.height) != (layout["width"], layout["height"]):
        text = LAYOUTS_JSON.read_text()
        data = json.loads(text)
        for entry in data["layouts"]:
            if entry.get("id") == layout["id"]:
                notes.append(f"resized {layout['id']} from {entry['width']}x{entry['height']} to {blocks.width}x{blocks.height} in layouts.json")
                entry["width"], entry["height"] = blocks.width, blocks.height
        indent = 2 if '\n  "' in text else 4
        LAYOUTS_JSON.write_text(json.dumps(data, indent=indent, ensure_ascii=False) + "\n")
        layout["width"], layout["height"] = blocks.width, blocks.height
        layouts.cache_clear()
    limit = consts().max_map_data_size
    if (blocks.width + 15) * (blocks.height + 14) > limit:
        notes.append(
            f"warning: ({blocks.width}+15)*({blocks.height}+14) is over MAX_MAP_DATA_SIZE ({limit}); "
            "the map will not fit in the game's map buffer"
        )
    blocks.save(layout["blockdata_filepath"])
    return notes


KEY_CHARS = (
    ".,:;'\"`~-_=+*%&$@!?/\\|^<>()[]{}"
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
)


def extract(layout: dict, region: tuple[int, int, int, int], standalone: bool = False, describe=None) -> str:
    """Write a region of a layout as a blueprint (legend + grid) that rebuilds it exactly."""
    x0, y0, w, h = region
    blocks = Blockdata.for_layout(layout).crop(x0, y0, w, h)
    head = []
    if standalone:
        whole = region == (0, 0, layout["width"], layout["height"])
        head = [f"layout {layout['id']}", f"size {layout['width']} {layout['height']}" if whole else "base self", ""]
    return "\n".join(head) + ("\n" if head else "") + blocks_text(blocks, (x0, y0), describe)


def blocks_text(blocks: Blockdata, at: tuple[int, int] = (0, 0), describe=None, layout: dict | None = None) -> str:
    """Legend and grid lines that draw these blocks with their top-left at `at`.

    With `layout`, legend lines leave out collision and elevation where they
    are the defaults a bare metatile id gets on that layout.
    """
    order = [b for b, _ in Counter(blocks.blocks).most_common()]
    width = 1 if len(order) <= len(KEY_CHARS) else 2
    keys = list(KEY_CHARS) if width == 1 else [a + b for a in KEY_CHARS for b in KEY_CHARS]
    key_of = dict(zip(order, keys))
    out = legend_lines(order, key_of, describe, layout)
    out.append(f"grid {at[0]} {at[1]}" + (" w2" if width == 2 else ""))
    for y in range(blocks.height):
        out.append("".join(key_of[blocks.get(x, y)] for x in range(blocks.width)))
    out.append("end")
    return "\n".join(out) + "\n"


def cells_text(cells: dict[tuple[int, int], int], describe=None, prefix: str = "B",
               key_of: dict[int, str] | None = None) -> str:
    """Legend and grid lines that set just these cells, keeping every other block.

    Keys are two characters starting with `prefix`, so they never clash with
    a draft's one-character legend. With `key_of` (from shared_legend), the
    grid uses those keys and no legend lines are written.
    """
    if not cells:
        return ""
    xs = [x for x, _ in cells]
    ys = [y for _, y in cells]
    out = []
    if key_of is None:
        order = [b for b, _ in Counter(cells.values()).most_common()]
        if len(order) > len(KEY_CHARS):
            raise ValueError("too many distinct blocks for one cells grid")
        key_of = {b: prefix + k for b, k in zip(order, KEY_CHARS)}
        out = legend_lines(order, key_of, describe)
    out.append(f"grid {min(xs)} {min(ys)} w2")
    for y in range(min(ys), max(ys) + 1):
        out.append("".join(key_of[cells[(x, y)]] if (x, y) in cells else "  " for x in range(min(xs), max(xs) + 1)).rstrip())
    out.append("end")
    return "\n".join(out) + "\n"


def legend_lines(order: list[int], key_of: dict[int, str], describe=None, layout: dict | None = None) -> list[str]:
    """One legend line per block; with `layout`, default collision and elevation are left out."""
    c = consts()
    out = []
    for b in order:
        mid, col, elev = c.unpack(b)
        note = f"  # {describe(mid)}" if describe else ""
        attrs = "" if layout is not None and default_attrs(mid, layout) == (col, elev) else f"/c{col}/e{elev}"
        out.append(f"legend {key_of[b]} = {mid:#05x}{attrs}{note}")
    return out


def shared_legend(cell_sets: list[dict[tuple[int, int], int]], describe=None,
                  prefixes: str = "BCFGHJ", layout: dict | None = None) -> tuple[dict[int, str], str]:
    """One legend for several cells grids, so each block is defined once.

    Returns the keys to pass to cells_text and the legend lines defining them
    (with `layout`, leaving out default collision and elevation).
    """
    order = [b for b, _ in Counter(b for cells in cell_sets for b in cells.values()).most_common()]
    keys = [p + k for p in prefixes for k in KEY_CHARS]
    if len(order) > len(keys):
        raise ValueError("too many distinct blocks for one shared legend")
    key_of = dict(zip(order, keys))
    return key_of, "\n".join(legend_lines(order, key_of, describe, layout)) + "\n"


def load(path: Path, layout_override: str | None = None) -> Blueprint:
    return Blueprint(path.read_text(), str(path), layout_override)
