"""Load tilesets and draw metatiles.

A tileset symbol (gTileset_General) is traced through
src/data/tilesets/headers.h, graphics.h and metatiles.h to its tiles.png,
palettes and metatile files, so renamed or shared tileset assets resolve the
same way the build does.
"""

from __future__ import annotations

import re
import struct
from functools import lru_cache
from pathlib import Path

from PIL import Image

from project import REPO, consts, read


@lru_cache(maxsize=None)
def _headers() -> dict[str, dict[str, str]]:
    out = {}
    for name, body in re.findall(r"const struct Tileset (gTileset_\w+)\s*=\s*\{(.*?)\};", read("src/data/tilesets/headers.h"), re.S):
        out[name] = dict(re.findall(r"\.(\w+)\s*=\s*([^,\n]+?),?\s*$", body, re.M))
    return out


@lru_cache(maxsize=None)
def _asset_paths() -> dict[str, list[str]]:
    """Symbol -> asset file paths (General and Building live in src/graphics.c)."""
    out: dict[str, list[str]] = {}
    for header in ("src/data/tilesets/graphics.h", "src/data/tilesets/metatiles.h", "src/graphics.c"):
        text = read(header)
        for m in re.finditer(r"const\s+u(?:16|32)\s+(\w+)\[\](?:\[\d+\])?\s*=\s*(\{.*?\};|[^;]*;)", text, re.S):
            # INCGFX_*("path", ".4bpp.lz", "-flags"): keep the paths only.
            out[m.group(1)] = re.findall(r'"([^"]+/[^"]+)"', m.group(2))
    return out


def _source_file(path: str) -> Path:
    """The checked-in file a build product comes from (x.4bpp.lz -> x.png, x.gbapal -> x.pal)."""
    p = REPO / path
    for suffix, repl in ((".4bpp.lz", ".png"), (".4bpp", ".png"), (".gbapal", ".pal")):
        if path.endswith(suffix):
            p = REPO / (path[: -len(suffix)] + repl)
    return p


def _read_pal(path: Path) -> list[tuple[int, int, int]]:
    lines = path.read_text().split()
    if lines[0] != "JASC-PAL":
        raise ValueError(f"{path}: not a JASC palette")
    count = int(lines[2])
    nums = list(map(int, lines[3 : 3 + count * 3]))
    # The GBA stores 5 bits per channel; round the same way the game shows them.
    return [tuple((v >> 3) << 3 for v in nums[i : i + 3]) for i in range(0, len(nums), 3)]


class UnknownTileset(ValueError):
    pass


class Tileset:
    def __init__(self, symbol: str) -> None:
        hdr = _headers().get(symbol)
        if hdr is None:
            raise UnknownTileset(f"unknown tileset {symbol}")
        assets = _asset_paths()
        self.symbol = symbol
        self.secondary = hdr.get("isSecondary", "FALSE").strip() == "TRUE"
        self.tiles_png = _source_file(assets[hdr["tiles"]][0])
        self.pal_paths = [_source_file(p) for p in assets.get(hdr["palettes"], [])]
        self.metatiles_path = REPO / assets[hdr["metatiles"]][0]
        self.attrs_path = REPO / assets[hdr["metatileAttributes"]][0]
        c = consts()
        raw = self.metatiles_path.read_bytes()
        per = c.tiles_per_metatile
        entries = struct.unpack(f"<{len(raw) // 2}H", raw)
        self.metatiles = [entries[i : i + per] for i in range(0, len(entries), per)]
        raw = self.attrs_path.read_bytes()
        self.attributes = list(struct.unpack(f"<{len(raw) // 2}H", raw))
        self._tiles = None
        self._pals = None

    def __len__(self) -> int:
        return len(self.metatiles)

    @property
    def tiles(self) -> list[bytes]:
        """8x8 tiles as 64 palette indices each."""
        if self._tiles is None:
            with Image.open(self.tiles_png) as img:
                if img.mode not in ("P", "L"):
                    raise ValueError(f"{self.tiles_png}: expected an indexed PNG, got {img.mode}")
                w, h = img.size
                px = img.tobytes()
            self._tiles = [
                bytes(px[(ty * 8 + j) * w + tx * 8 + i] & 0xF for j in range(8) for i in range(8))
                for ty in range(h // 8)
                for tx in range(w // 8)
            ]
        return self._tiles

    @property
    def palettes(self) -> list[list[tuple[int, int, int]]]:
        if self._pals is None:
            self._pals = [_read_pal(p) for p in self.pal_paths]
        return self._pals

    @property
    def name(self) -> str:
        return self.symbol[len("gTileset_"):]


@lru_cache(maxsize=None)
def load(symbol: str) -> Tileset:
    return Tileset(symbol)


class TilesetPair:
    """A layout's primary and secondary tilesets, as the game loads them together."""

    def __init__(self, primary: str, secondary: str) -> None:
        self.primary = load(primary)
        self.secondary = load(secondary)
        self._cache: dict[int, Image.Image] = {}
        self._tile_cache: dict[int, tuple[Image.Image, Image.Image] | None] = {}
        c = consts()
        pals = []
        for i in range(c.pals_total):
            src = self.primary if i < c.pals_in_primary else self.secondary
            pals.append(src.palettes[i] if i < len(src.palettes) else [(255, 0, 255)] * 16)
        self.palettes = pals
        self.backdrop = pals[0][0]

    @classmethod
    def for_layout(cls, layout: dict) -> "TilesetPair":
        return _pair(layout["primary_tileset"], layout["secondary_tileset"])

    def _split(self, metatile: int) -> tuple[Tileset, int]:
        n = consts().metatiles_in_primary
        return (self.primary, metatile) if metatile < n else (self.secondary, metatile - n)

    def exists(self, metatile: int) -> bool:
        ts, idx = self._split(metatile)
        return idx < len(ts.metatiles)

    def attributes(self, metatile: int) -> int | None:
        ts, idx = self._split(metatile)
        return ts.attributes[idx] if idx < len(ts.attributes) else None

    def behavior(self, metatile: int) -> int | None:
        attr = self.attributes(metatile)
        return None if attr is None else attr & consts().behavior_mask

    def count(self) -> tuple[int, int]:
        return len(self.primary), len(self.secondary)

    def ids(self) -> list[int]:
        n = consts().metatiles_in_primary
        return list(range(len(self.primary))) + [n + i for i in range(len(self.secondary))]

    def _tile(self, index: int) -> bytes | None:
        c = consts()
        ts, idx = (self.primary, index) if index < c.tiles_in_primary else (self.secondary, index - c.tiles_in_primary)
        tiles = ts.tiles
        return tiles[idx] if idx < len(tiles) else None

    def _tile_image(self, entry: int) -> tuple[Image.Image, Image.Image] | None:
        """An 8x8 tile entry (id, flips, palette) as an RGB image and its transparency mask."""
        key = entry
        if key in self._tile_cache:
            return self._tile_cache[key]
        tile = self._tile(entry & 0x3FF)
        if tile is None:
            self._tile_cache[key] = None
            return None
        pal = self.palettes[(entry >> 12) % len(self.palettes)]
        img = Image.frombytes("P", (8, 8), tile)
        img.putpalette([v for rgb in pal for v in rgb])
        img = img.convert("RGB")
        mask = Image.frombytes("L", (8, 8), bytes(255 if c else 0 for c in tile))
        if entry & 0x400:
            img, mask = img.transpose(Image.Transpose.FLIP_LEFT_RIGHT), mask.transpose(Image.Transpose.FLIP_LEFT_RIGHT)
        if entry & 0x800:
            img, mask = img.transpose(Image.Transpose.FLIP_TOP_BOTTOM), mask.transpose(Image.Transpose.FLIP_TOP_BOTTOM)
        self._tile_cache[key] = (img, mask)
        return img, mask

    def draw(self, metatile: int) -> Image.Image:
        """A metatile as a 16x16 RGB image (magenta if it doesn't exist). Don't modify the result."""
        if metatile in self._cache:
            return self._cache[metatile]
        ts, idx = self._split(metatile)
        if idx >= len(ts.metatiles):
            out = Image.new("RGB", (16, 16), (255, 0, 255))
        else:
            out = Image.new("RGB", (16, 16), self.backdrop)
            # Bottom, middle and top layers, four tiles each (top-left, top-right, bottom-left, bottom-right).
            for n, entry in enumerate(ts.metatiles[idx]):
                tile = self._tile_image(entry)
                if tile:
                    quad = n % 4
                    out.paste(tile[0], ((quad % 2) * 8, (quad // 2) * 8), tile[1])
        self._cache[metatile] = out
        return out

    def pixels(self, metatile: int) -> bytes:
        """The drawn metatile's raw RGB bytes, for comparing how metatiles look."""
        return self.draw(metatile).tobytes()


@lru_cache(maxsize=None)
def _pair(primary: str, secondary: str) -> TilesetPair:
    return TilesetPair(primary, secondary)


def pair(primary: str, secondary: str) -> TilesetPair:
    return _pair(primary, secondary)
