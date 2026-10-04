"""Read Sinnoh maps from pret/pokeplatinum as a reference for replicas.

Platinum's overworld is a matrix of 32x32-tile chunks ("land data"). Each
chunk stores a u16 per tile: bit 15 is collision and the low byte is the tile
behaviour. The map header says which matrix a map lives in and which events
file it uses; events (warps, NPCs, signs, triggers) use matrix-wide tile
coordinates. Buildings and other 3D props are listed per chunk with their
model id and position.

This module turns one map header into a local tile grid with its events and
props, in the same coordinates an Emerald layout uses (x right, y down,
origin at the map's top-left corner).

The pokeplatinum checkout is a blobless partial clone kept in a cache
directory; only the files a command needs are downloaded. Set
POKEPLATINUM_DIR to use an existing full checkout instead.
"""

from __future__ import annotations

import json
import os
import re
import struct
import subprocess
from dataclasses import dataclass, field
from functools import lru_cache
from pathlib import Path

REMOTE = "https://github.com/pret/pokeplatinum"
CHUNK = 32  # tiles per land data side
TILE_UNITS = 16  # world units per tile
FX32_ONE = 1 << 12

COLLISION_BIT = 0x8000
BEHAVIOR_MASK = 0x00FF

# Tile behaviour name -> one-character class used in text grids and drafts.
# Anything not listed is "." when passable and "#" when blocked.
SYMBOLS = {
    "TALL_GRASS": '"',
    "VERY_TALL_GRASS": "Y",
    "WATER_RIVER": "~",
    "WATER_SEA": "~",
    "WATERFALL": "|",
    "PUDDLE": ",",
    "PUDDLE_NO_SPLASHING": ",",
    "SHALLOW_WATER": ",",
    "ICE": "i",
    "SAND": ":",
    "JUMP_EAST": ">",
    "JUMP_WEST": "<",
    "JUMP_NORTH": "^",
    "JUMP_SOUTH": "v",
    "JUMP_EAST_TWICE": ">",
    "JUMP_WEST_TWICE": "<",
    "JUMP_NORTH_TWICE": "^",
    "JUMP_SOUTH_TWICE": "v",
    "DOOR": "D",
    "WARP_ENTRANCE_EAST": "E",
    "WARP_ENTRANCE_WEST": "E",
    "WARP_ENTRANCE_NORTH": "E",
    "WARP_ENTRANCE_SOUTH": "E",
    "WARP_EAST": "E",
    "WARP_WEST": "E",
    "WARP_NORTH": "E",
    "WARP_SOUTH": "E",
    "WARP_STAIRS_EAST": "S",
    "WARP_STAIRS_WEST": "S",
    "WARP_PANEL": "E",
    "ESCALATOR": "S",
    "ESCALATOR_FLIP_FACE": "S",
    "ROCK_CLIMB_N_S": "R",
    "ROCK_CLIMB_E_W": "R",
    "MUD": "m",
    "MUD_DEEP": "m",
    "MUD_WITH_GRASS": "m",
    "MUD_DEEP_WITH_GRASS": "m",
    "SNOW_DEEP": "s",
    "SNOW_DEEPER": "s",
    "SNOW_DEEPEST": "s",
    "SNOW_SHALLOW": "s",
    "SNOW_WITH_SHADOWS": "s",
    "BERRY_PATCH": "B",
    "TABLE": "t",
    "PC": "o",
    "TOWN_MAP": "o",
    "TV": "o",
    "SMALL_BOOKSHELF_1": "o",
    "SMALL_BOOKSHELF_2": "o",
    "BOOKSHELF_1": "o",
    "BOOKSHELF_2": "o",
    "TRASH_CAN": "o",
    "MART_SHELF_1": "o",
    "MART_SHELF_2": "o",
    "MART_SHELF_3": "o",
    "BIKE_PARKING": "o",
}
for _name in ("BRIDGE", "BRIDGE_START", "BRIDGE_OVER_CAVE", "BRIDGE_OVER_WATER", "BRIDGE_OVER_SAND", "BRIDGE_OVER_SNOW"):
    SYMBOLS[_name] = "="
for _dir in ("N_S", "E_W"):
    for _suffix in ("", "_OVER_ENCS", "_OVER_WATER", "_OVER_SAND"):
        SYMBOLS[f"BIKE_BRIDGE_{_dir}{_suffix}"] = "="

LEGEND = [
    ("#", "blocked"), (".", "walkable"), ('"', "tall grass"), ("Y", "very tall grass"),
    ("~", "surfable water"), ("|", "waterfall"), (",", "puddle/shallow water"), (":", "sand"),
    ("i", "ice"), ("m", "mud"), ("s", "snow"), ("^v<>", "ledge (jump direction)"),
    ("D", "door"), ("E", "warp/entrance"), ("S", "stairs/escalator"), ("R", "rock climb"),
    ("=", "bridge"), ("B", "berry patch"), ("t", "table/counter"), ("o", "furniture (PC, TV, shelf...)"),
    (" ", "outside this map"),
]


def cache_dir() -> Path:
    env = os.environ.get("POKEPLATINUM_DIR")
    if env:
        return Path(env).expanduser()
    base = Path(os.environ.get("XDG_CACHE_HOME", Path.home() / ".cache"))
    return base / "pokepalladium" / "pokeplatinum"


def _git(*args: str, cwd: Path | None = None) -> str:
    return subprocess.run(["git", *args], cwd=cwd, check=True, capture_output=True, text=True).stdout


def ensure(*paths: str) -> Path:
    """Make sure these repo-relative files exist locally, downloading them if needed."""
    root = cache_dir()
    if os.environ.get("POKEPLATINUM_DIR"):
        missing = [p for p in paths if not (root / p).exists()]
        if missing:
            raise SystemExit(f"error: {root} is missing {', '.join(missing)}")
        return root
    if not (root / ".git").exists():
        root.parent.mkdir(parents=True, exist_ok=True)
        print(f"Cloning {REMOTE} (blobless) into {root}...", flush=True)
        _git("clone", "--quiet", "--depth", "1", "--filter=blob:none", "--no-checkout", REMOTE, str(root))
    missing = [p for p in paths if not (root / p).exists()]
    if missing:
        _git("checkout", "HEAD", "--", *missing, cwd=root)
    return root


def update() -> str:
    """Move the cached checkout to pokeplatinum's latest commit."""
    root = cache_dir()
    if os.environ.get("POKEPLATINUM_DIR"):
        raise SystemExit("error: POKEPLATINUM_DIR is set; update that checkout yourself")
    ensure()
    _git("fetch", "--quiet", "--depth", "1", "origin", "HEAD", cwd=root)
    present = [p for p in _git("ls-files", cwd=root).split("\n") if p and (root / p).exists()]
    _git("reset", "--quiet", "--soft", "FETCH_HEAD", cwd=root)
    if present:
        _git("checkout", "HEAD", "--", *present, cwd=root)
    return _git("rev-parse", "--short", "HEAD", cwd=root).strip()


def revision() -> str:
    return _git("rev-parse", "--short", "HEAD", cwd=ensure()).strip()


# ---------------------------------------------------------------- tables


@lru_cache(maxsize=None)
def tile_behaviors() -> dict[int, str]:
    root = ensure("include/constants/field/map_tile_behaviors.h")
    text = (root / "include/constants/field/map_tile_behaviors.h").read_text()
    body = re.search(r"enum TileBehavior\s*\{(.*?)\}", text, re.S).group(1)
    out, value = {}, 0
    for entry in re.sub(r"//.*", "", body).split(","):
        name, _, expr = (p.strip() for p in entry.partition("="))
        if not name:
            continue
        if expr:
            value = int(expr, 0)
        out[value] = name[len("TILE_BEHAVIOR_"):]
        value += 1
    return out


def symbol_for(attr: int) -> str:
    name = tile_behaviors().get(attr & BEHAVIOR_MASK, "")
    sym = SYMBOLS.get(name)
    blocked = bool(attr & COLLISION_BIT)
    if sym is None or (blocked and sym in ".:,"):
        return "#" if blocked else "."
    return sym


@lru_cache(maxsize=None)
def headers() -> dict[str, dict[str, str]]:
    """MAP_HEADER_* -> its fields in include/data/map_headers.h."""
    root = ensure("include/data/map_headers.h")
    text = (root / "include/data/map_headers.h").read_text()
    out = {}
    for name, body in re.findall(r"\[(MAP_HEADER_\w+)\]\s*=\s*\{(.*?)\}", text, re.S):
        out[name] = dict(re.findall(r"\.(\w+)\s*=\s*([^,\n]+)", body))
    return out


def find_header(query: str) -> str:
    all_headers = headers()
    q = query.upper().replace(" ", "_")
    for cand in (q, "MAP_HEADER_" + q):
        if cand in all_headers:
            return cand
    loose = re.sub(r"[^A-Z0-9]", "", q)
    hits = [h for h in all_headers if re.sub(r"[^A-Z0-9]", "", h[len("MAP_HEADER_"):]) == loose]
    if len(hits) == 1:
        return hits[0]
    raise SystemExit(f"error: no unique Platinum map header for {query!r}; try `platinum list {query}`")


@lru_cache(maxsize=None)
def land_data_files() -> list[str]:
    root = ensure("res/field/maps/data/map_data.order")
    return (root / "res/field/maps/data/map_data.order").read_text().split()


@lru_cache(maxsize=None)
def prop_model_names() -> list[str]:
    root = ensure("res/field/props/models/map_prop_models.order")
    return [Path(n).stem for n in (root / "res/field/props/models/map_prop_models.order").read_text().split()]


def _land_path(name: str) -> str:
    """MAP_018 -> res/field/maps/data/map_data_018.bin."""
    return f"res/field/maps/data/{land_data_files()[int(name.split('_')[1])]}"


# ---------------------------------------------------------------- reference


@dataclass
class Prop:
    model: int
    name: str
    x: float  # tile coordinates (centre of the model), local to the map
    y: float
    height: float


@dataclass
class Reference:
    header: str
    matrix: str
    width: int
    height: int
    origin: tuple[int, int]  # matrix-wide tile coordinates of local (0, 0)
    attrs: list[list[int | None]]  # None = tile belongs to another map
    warps: list[dict] = field(default_factory=list)
    objects: list[dict] = field(default_factory=list)
    signs: list[dict] = field(default_factory=list)
    triggers: list[dict] = field(default_factory=list)
    props: list[Prop] = field(default_factory=list)
    neighbours: dict[str, list[str]] = field(default_factory=dict)

    def symbol(self, x: int, y: int) -> str:
        a = self.attrs[y][x]
        return " " if a is None else symbol_for(a)

    def grid(self, events: bool = False) -> list[str]:
        rows = [[self.symbol(x, y) for x in range(self.width)] for y in range(self.height)]
        if events:
            for marks, ch in ((self.triggers, "T"), (self.signs, "?"), (self.objects, "@"), (self.warps, "W")):
                for e in marks:
                    for dy in range(e.get("length", 1)):
                        for dx in range(e.get("width", 1)):
                            x, y = e["x"] + dx, e["y"] + dy
                            if 0 <= x < self.width and 0 <= y < self.height:
                                rows[y][x] = ch
        return ["".join(r) for r in rows]

    def to_json(self) -> dict:
        return {
            "header": self.header,
            "matrix": self.matrix,
            "width": self.width,
            "height": self.height,
            "origin": list(self.origin),
            "grid": self.grid(),
            "behaviors": [[None if a is None else tile_behaviors().get(a & BEHAVIOR_MASK, hex(a & BEHAVIOR_MASK)) for a in row] for row in self.attrs],
            "collision": [[None if a is None else int(bool(a & COLLISION_BIT)) for a in row] for row in self.attrs],
            "warps": self.warps,
            "objects": self.objects,
            "signs": self.signs,
            "triggers": self.triggers,
            "props": [p.__dict__ for p in self.props],
            "neighbours": self.neighbours,
        }


def _strip(name: str) -> str:
    return name.strip().rstrip(",")


def load(query: str) -> Reference:
    header = find_header(query)
    info = headers()[header]
    matrix_id = _strip(info["mapMatrixID"])
    events_id = _strip(info.get("eventsArchiveID", "events_empty"))
    matrix_path = f"res/field/matrices/{matrix_id}.json"
    events_path = f"res/field/events/{events_id}.json"
    root = ensure(matrix_path, events_path)
    matrix = json.loads((root / matrix_path).read_text())
    grid_maps = matrix["maps"]
    grid_headers = matrix.get("headers") or []
    rows, cols = len(grid_maps), len(grid_maps[0])

    def owns(r, c):
        if grid_maps[r][c] == "MAP_NONE":
            return False
        return grid_headers[r][c] == header if grid_headers else True

    chunks = [(r, c) for r in range(rows) for c in range(cols) if owns(r, c)]
    if not chunks:
        raise SystemExit(f"error: {header} has no chunks in {matrix_id}")
    r0, r1 = min(r for r, _ in chunks), max(r for r, _ in chunks)
    c0, c1 = min(c for _, c in chunks), max(c for _, c in chunks)
    width, height = (c1 - c0 + 1) * CHUNK, (r1 - r0 + 1) * CHUNK
    origin = (c0 * CHUNK, r0 * CHUNK)

    ensure(*{_land_path(grid_maps[r][c]) for r, c in chunks})
    attrs: list[list[int | None]] = [[None] * width for _ in range(height)]
    props: list[Prop] = []
    names = prop_model_names()
    for r, c in chunks:
        raw = (root / _land_path(grid_maps[r][c])).read_bytes()
        attr_size, prop_size, _, _ = struct.unpack("<4I", raw[:16])
        tiles = struct.unpack(f"<{attr_size // 2}H", raw[16 : 16 + attr_size])
        bx, by = (c - c0) * CHUNK, (r - r0) * CHUNK
        for i, a in enumerate(tiles):
            attrs[by + i // CHUNK][bx + i % CHUNK] = a
        base = 16 + attr_size
        for off in range(base, base + prop_size, 48):
            model, px, py, pz = struct.unpack("<i3i", raw[off : off + 16])
            # Prop positions are relative to the chunk's centre, in fx32 world units.
            tx = bx + CHUNK / 2 + px / FX32_ONE / TILE_UNITS
            ty = by + CHUNK / 2 + pz / FX32_ONE / TILE_UNITS
            name = names[model] if 0 <= model < len(names) else "?"
            props.append(Prop(model, name, round(tx, 2), round(ty, 2), round(py / FX32_ONE / TILE_UNITS, 2)))

    events = json.loads((root / events_path).read_text())

    def local(e: dict) -> dict:
        out = dict(e)
        out["x"] = e["x"] - origin[0]
        out["y"] = e["z"] - origin[1]
        out.pop("z", None)
        if "length" in e:
            out["length"] = e["length"]
        return out

    ref = Reference(header, matrix_id, width, height, origin, attrs)
    ref.warps = [local(e) for e in events.get("warp_events", [])]
    ref.objects = [local(e) for e in events.get("object_events", [])]
    ref.signs = [local(e) for e in events.get("bg_events", [])]
    ref.triggers = [local(e) for e in events.get("coord_events", [])]
    ref.props = props

    if grid_headers:
        seen: dict[str, set[str]] = {"up": set(), "down": set(), "left": set(), "right": set()}
        for r, c in chunks:
            for d, (dr, dc) in {"up": (-1, 0), "down": (1, 0), "left": (0, -1), "right": (0, 1)}.items():
                rr, cc = r + dr, c + dc
                if 0 <= rr < rows and 0 <= cc < cols and grid_maps[rr][cc] != "MAP_NONE":
                    other = grid_headers[rr][cc]
                    if other != header and other.startswith("MAP_HEADER_") and other != "MAP_HEADER_EVERYWHERE":
                        seen[d].add(other)
        ref.neighbours = {d: sorted(v) for d, v in seen.items() if v}
    return ref


def search(query: str = "") -> list[str]:
    q = re.sub(r"[^A-Z0-9]", "", query.upper())
    return [h for h in headers() if q in re.sub(r"[^A-Z0-9]", "", h)]
