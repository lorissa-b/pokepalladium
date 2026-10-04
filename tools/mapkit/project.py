"""Read the decomp's map data: constants, layouts, maps, metatile names.

Everything is read from the repo on demand, so the tools follow changes to
the headers (the map grid and metatile formats here are not vanilla
pokeemerald's: see c11d4adb "Expand metatile count to 4096 and add
triple-layer metatiles").
"""

from __future__ import annotations

import json
import re
import struct
from functools import lru_cache
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
MAPS_DIR = REPO / "data" / "maps"
LAYOUTS_JSON = REPO / "data" / "layouts" / "layouts.json"


def read(path: Path | str) -> str:
    return (REPO / path).read_text(encoding="utf-8")


def parse_defines(*paths: str) -> dict[str, int]:
    """Integer #defines from headers, resolving simple references and shifts."""
    values: dict[str, int] = {}
    for path in paths:
        for name, expr in re.findall(r"^#define[ \t]+(\w+)[ \t]+(.+?)[ \t]*(?://.*)?$", read(path), re.M):
            try:
                values[name] = eval_expr(expr, values)
            except (ValueError, SyntaxError, NameError, TypeError):
                pass
    return values


def eval_expr(expr: str, names: dict[str, int]) -> int:
    expr = re.sub(r"\b(0x[0-9A-Fa-f]+|\d+)[uUlL]*\b", r"\1", expr)
    if not re.fullmatch(r"[\w\s()+\-*/<>|&~^]+", expr):
        raise ValueError(expr)
    return int(eval(expr, {"__builtins__": {}}, dict(names)))


def parse_enum(path: str, prefix: str) -> dict[str, int]:
    """Names and values of the first enum whose members start with prefix."""
    text = read(path)
    for body in re.findall(r"enum\s*\w*\s*\{(.*?)\}", text, re.S):
        body = re.sub(r"//.*", "", body)
        body = re.sub(r"/\*.*?\*/", "", body, flags=re.S)
        entries = [e.strip() for e in body.split(",") if e.strip()]
        if not entries or not entries[0].startswith(prefix):
            continue
        values: dict[str, int] = {}
        nxt = 0
        for entry in entries:
            name, _, expr = (p.strip() for p in entry.partition("="))
            if expr:
                nxt = eval_expr(expr, values)
            values[name] = nxt
            nxt += 1
        return values
    raise ValueError(f"no {prefix}* enum in {path}")


class Consts:
    """The map grid and tileset constants in effect, read from the headers."""

    def __init__(self) -> None:
        d = parse_defines("include/fieldmap.h", "include/global.fieldmap.h")
        self.metatile_mask = d["MAPGRID_METATILE_ID_MASK"]
        self.collision_mask = d["MAPGRID_COLLISION_MASK"]
        self.collision_shift = d["MAPGRID_COLLISION_SHIFT"]
        self.elevation_mask = d["MAPGRID_ELEVATION_MASK"]
        self.elevation_shift = d["MAPGRID_ELEVATION_SHIFT"]
        self.metatiles_in_primary = d["NUM_METATILES_IN_PRIMARY"]
        self.metatiles_total = d["NUM_METATILES_TOTAL"]
        self.tiles_in_primary = d["NUM_TILES_IN_PRIMARY"]
        self.pals_in_primary = d["NUM_PALS_IN_PRIMARY"]
        self.pals_total = d["NUM_PALS_TOTAL"]
        self.tiles_per_metatile = d["NUM_TILES_PER_METATILE"]
        self.max_map_data_size = d["MAX_MAP_DATA_SIZE"]
        self.map_offset = d["MAP_OFFSET"]
        self.behavior_mask = d["METATILE_ATTR_BEHAVIOR_MASK"]
        self.layer_mask = d["METATILE_ATTR_LAYER_MASK"]
        self.layer_shift = d["METATILE_ATTR_LAYER_SHIFT"]
        self.undefined = d["MAPGRID_METATILE_ID_MASK"]
        self.max_collision = self.collision_mask >> self.collision_shift
        self.max_elevation = self.elevation_mask >> self.elevation_shift

    def unpack(self, block: int) -> tuple[int, int, int]:
        return (
            block & self.metatile_mask,
            (block & self.collision_mask) >> self.collision_shift,
            (block & self.elevation_mask) >> self.elevation_shift,
        )

    def pack(self, metatile: int, collision: int = 0, elevation: int = 0) -> int:
        if not 0 <= metatile <= self.metatile_mask:
            raise ValueError(f"metatile {metatile:#x} out of range")
        if not 0 <= collision <= self.max_collision:
            raise ValueError(f"collision {collision} out of range (0-{self.max_collision})")
        if not 0 <= elevation <= self.max_elevation:
            raise ValueError(f"elevation {elevation} out of range (0-{self.max_elevation})")
        return metatile | (collision << self.collision_shift) | (elevation << self.elevation_shift)


@lru_cache(maxsize=None)
def consts() -> Consts:
    return Consts()


@lru_cache(maxsize=None)
def behaviors() -> dict[int, str]:
    """MB_* value -> name."""
    return {v: k for k, v in parse_enum("include/constants/metatile_behaviors.h", "MB_").items()}


@lru_cache(maxsize=None)
def behavior_values() -> dict[str, int]:
    return parse_enum("include/constants/metatile_behaviors.h", "MB_")


@lru_cache(maxsize=None)
def metatile_labels() -> dict[str, dict[int, str]]:
    """Tileset symbol -> metatile id -> label (from metatile_labels.h)."""
    labels: dict[str, dict[int, str]] = {}
    current = None
    for line in read("include/constants/metatile_labels.h").splitlines():
        m = re.match(r"//\s*(gTileset_\w+)", line)
        if m:
            current = labels.setdefault(m.group(1), {})
            continue
        m = re.match(r"#define\s+METATILE_(\w+)\s+(0x[0-9A-Fa-f]+|\d+)", line)
        if m and current is not None:
            current[int(m.group(2), 0)] = m.group(1)
    return labels


@lru_cache(maxsize=None)
def label_values() -> dict[str, int]:
    """Label (without METATILE_) -> metatile id, for every tileset."""
    return {name: mid for per in metatile_labels().values() for mid, name in per.items()}


# ---------------------------------------------------------------- layouts


@lru_cache(maxsize=None)
def layouts() -> dict[str, dict]:
    """LAYOUT_* id -> layouts.json entry."""
    data = json.loads(LAYOUTS_JSON.read_text())
    return {entry["id"]: entry for entry in data["layouts"] if entry.get("id")}


@lru_cache(maxsize=None)
def maps() -> dict[str, dict]:
    """MAP_* id -> map.json contents (with "dir" added)."""
    out = {}
    for path in sorted(MAPS_DIR.glob("*/map.json")):
        info = json.loads(path.read_text())
        info["dir"] = path.parent.name
        out[info["id"]] = info
    return out


def resolve_map(name: str) -> dict | None:
    """Accept MAP_FOO, a directory name (FooBar) or a loose spelling."""
    all_maps = maps()
    if name in all_maps:
        return all_maps[name]
    key = re.sub(r"[^a-z0-9]", "", name.lower())
    for info in all_maps.values():
        if key in (re.sub(r"[^a-z0-9]", "", info["dir"].lower()), re.sub(r"[^a-z0-9]", "", info["id"][4:].lower())):
            return info
    return None


def resolve_layout(name: str) -> dict:
    """A LAYOUT_* id, a layout name, or anything resolve_map accepts."""
    all_layouts = layouts()
    if name in all_layouts:
        return all_layouts[name]
    for entry in all_layouts.values():
        if entry["name"] == name or entry["name"] == name + "_Layout":
            return entry
    info = resolve_map(name)
    if info:
        return all_layouts[info["layout"]]
    raise SystemExit(f"error: no map or layout called {name!r}")


def maps_using_layout(layout_id: str) -> list[dict]:
    return [m for m in maps().values() if m.get("layout") == layout_id]


# ---------------------------------------------------------------- blockdata


class Blockdata:
    """A grid of map blocks (metatile id + collision + elevation), row-major."""

    def __init__(self, width: int, height: int, blocks: list[int] | None = None) -> None:
        self.width, self.height = width, height
        self.extra = 0
        self.blocks = list(blocks) if blocks is not None else [0] * (width * height)
        if len(self.blocks) != width * height:
            raise ValueError(f"{len(self.blocks)} blocks for a {width}x{height} grid")

    @classmethod
    def load(cls, path: Path | str, width: int, height: int) -> "Blockdata":
        raw = (REPO / path).read_bytes()
        count = len(raw) // 2
        if count < width * height:
            raise ValueError(f"{path}: {count} blocks, expected {width}x{height}={width * height}")
        # The game ignores trailing data (a few vanilla layouts have a spare block).
        blocks = cls(width, height, list(struct.unpack(f"<{width * height}H", raw[: width * height * 2])))
        blocks.extra = count - width * height
        return blocks

    @classmethod
    def for_layout(cls, layout: dict) -> "Blockdata":
        return cls.load(layout["blockdata_filepath"], layout["width"], layout["height"])

    @classmethod
    def border_for_layout(cls, layout: dict) -> "Blockdata":
        return cls.load(layout["border_filepath"], layout.get("border_width", 2), layout.get("border_height", 2))

    def save(self, path: Path | str) -> None:
        (REPO / path).write_bytes(struct.pack(f"<{len(self.blocks)}H", *self.blocks))

    def inside(self, x: int, y: int) -> bool:
        return 0 <= x < self.width and 0 <= y < self.height

    def get(self, x: int, y: int) -> int:
        return self.blocks[y * self.width + x]

    def set(self, x: int, y: int, block: int) -> None:
        if self.inside(x, y):
            self.blocks[y * self.width + x] = block

    def crop(self, x: int, y: int, w: int, h: int) -> "Blockdata":
        if x < 0 or y < 0 or x + w > self.width or y + h > self.height:
            raise ValueError(f"region {x},{y} {w}x{h} is outside the {self.width}x{self.height} grid")
        return Blockdata(w, h, [self.get(x + i, y + j) for j in range(h) for i in range(w)])

    def paste(self, other: "Blockdata", x: int, y: int) -> None:
        for j in range(other.height):
            for i in range(other.width):
                self.set(x + i, y + j, other.get(i, j))
