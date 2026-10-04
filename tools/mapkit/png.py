"""Just enough PNG for the map tools, using only the standard library.

Reads non-interlaced indexed PNGs (the tileset tiles.png files) as raw
palette indices, and writes RGB images.
"""

from __future__ import annotations

import struct
import zlib
from pathlib import Path


def _paeth(a: int, b: int, c: int) -> int:
    p = a + b - c
    pa, pb, pc = abs(p - a), abs(p - b), abs(p - c)
    if pa <= pb and pa <= pc:
        return a
    return b if pb <= pc else c


def read_indexed(path: Path) -> tuple[int, int, list[bytearray]]:
    """Return (width, height, rows of palette indices) for an indexed or grey PNG."""
    data = path.read_bytes()
    if data[:8] != b"\x89PNG\r\n\x1a\n":
        raise ValueError(f"{path}: not a PNG")
    pos, idat = 8, []
    width = height = depth = ctype = interlace = 0
    while pos < len(data):
        length, kind = struct.unpack(">I4s", data[pos : pos + 8])
        body = data[pos + 8 : pos + 8 + length]
        pos += 12 + length
        if kind == b"IHDR":
            width, height, depth, ctype, _, _, interlace = struct.unpack(">IIBBBBB", body)
        elif kind == b"IDAT":
            idat.append(body)
        elif kind == b"IEND":
            break
    if ctype not in (0, 3) or interlace:
        raise ValueError(f"{path}: only non-interlaced indexed/grey PNGs are supported")
    raw = zlib.decompress(b"".join(idat))
    stride = (width * depth + 7) // 8
    bpp = max(1, depth // 8)
    rows, prev = [], bytearray(stride)
    for y in range(height):
        start = y * (stride + 1)
        ftype = raw[start]
        line = bytearray(raw[start + 1 : start + 1 + stride])
        for i in range(stride):
            a = line[i - bpp] if i >= bpp else 0
            b = prev[i]
            c = prev[i - bpp] if i >= bpp else 0
            if ftype == 1:
                line[i] = (line[i] + a) & 0xFF
            elif ftype == 2:
                line[i] = (line[i] + b) & 0xFF
            elif ftype == 3:
                line[i] = (line[i] + ((a + b) >> 1)) & 0xFF
            elif ftype == 4:
                line[i] = (line[i] + _paeth(a, b, c)) & 0xFF
        prev = line
        if depth == 8:
            rows.append(line)
            continue
        per_byte = 8 // depth
        mask = (1 << depth) - 1
        px = bytearray(width)
        for x in range(width):
            byte = line[x // per_byte]
            shift = 8 - depth * (x % per_byte + 1)
            px[x] = (byte >> shift) & mask
        rows.append(px)
    return width, height, rows


class Image:
    """A mutable RGB image."""

    def __init__(self, width: int, height: int, fill: tuple[int, int, int] = (0, 0, 0)) -> None:
        self.width, self.height = width, height
        self.px = bytearray(bytes(fill) * (width * height))

    def put(self, x: int, y: int, rgb: tuple[int, int, int]) -> None:
        if 0 <= x < self.width and 0 <= y < self.height:
            i = (y * self.width + x) * 3
            self.px[i : i + 3] = bytes(rgb)

    def blend(self, x: int, y: int, rgb: tuple[int, int, int], alpha: float) -> None:
        if 0 <= x < self.width and 0 <= y < self.height:
            i = (y * self.width + x) * 3
            for k in range(3):
                self.px[i + k] = int(self.px[i + k] * (1 - alpha) + rgb[k] * alpha)

    def rect(self, x: int, y: int, w: int, h: int, rgb, alpha: float = 1.0) -> None:
        for j in range(max(0, y), min(self.height, y + h)):
            for i in range(max(0, x), min(self.width, x + w)):
                if alpha >= 1:
                    self.put(i, j, rgb)
                else:
                    self.blend(i, j, rgb, alpha)

    def outline(self, x: int, y: int, w: int, h: int, rgb) -> None:
        for i in range(x, x + w):
            self.put(i, y, rgb)
            self.put(i, y + h - 1, rgb)
        for j in range(y, y + h):
            self.put(x, j, rgb)
            self.put(x + w - 1, j, rgb)

    def paste_rgb(self, x: int, y: int, w: int, h: int, pixels: bytes | bytearray) -> None:
        """Copy a w*h block of packed RGB pixels to (x, y)."""
        for j in range(h):
            ty = y + j
            if not 0 <= ty < self.height:
                continue
            x0, x1 = max(0, x), min(self.width, x + w)
            if x0 >= x1:
                continue
            src = (j * w + (x0 - x)) * 3
            dst = (ty * self.width + x0) * 3
            self.px[dst : dst + (x1 - x0) * 3] = pixels[src : src + (x1 - x0) * 3]

    def scaled(self, factor: int) -> "Image":
        if factor == 1:
            return self
        out = Image(self.width * factor, self.height * factor)
        for y in range(self.height):
            row = bytearray()
            for x in range(self.width):
                i = (y * self.width + x) * 3
                row += self.px[i : i + 3] * factor
            for k in range(factor):
                start = ((y * factor + k) * out.width) * 3
                out.px[start : start + len(row)] = row
        return out

    def save(self, path: Path) -> None:
        stride = self.width * 3
        raw = b"".join(b"\x00" + bytes(self.px[y * stride : (y + 1) * stride]) for y in range(self.height))

        def chunk(kind: bytes, body: bytes) -> bytes:
            return struct.pack(">I", len(body)) + kind + body + struct.pack(">I", zlib.crc32(kind + body))

        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(
            b"\x89PNG\r\n\x1a\n"
            + chunk(b"IHDR", struct.pack(">IIBBBBB", self.width, self.height, 8, 2, 0, 0, 0))
            + chunk(b"IDAT", zlib.compress(raw, 6))
            + chunk(b"IEND", b"")
        )


# A 3x5 pixel font for labels on rendered images.
_FONT = {
    "0": "111101101101111", "1": "010110010010111", "2": "111001111100111", "3": "111001111001111",
    "4": "101101111001001", "5": "111100111001111", "6": "111100111101111", "7": "111001010010010",
    "8": "111101111101111", "9": "111101111001111", "A": "010101111101101", "B": "110101110101110",
    "C": "011100100100011", "D": "110101101101110", "E": "111100110100111", "F": "111100110100100",
    "G": "011100101101011", "H": "101101111101101", "I": "111010010010111", "J": "001001001101010",
    "K": "101101110101101", "L": "100100100100111", "M": "101111111101101", "N": "110101101101101",
    "O": "010101101101010", "P": "110101110100100", "Q": "010101101110011", "R": "110101110101101",
    "S": "011100010001110", "T": "111010010010010", "U": "101101101101111", "V": "101101101101010",
    "W": "101101111111101", "X": "101101010101101", "Y": "101101010010010", "Z": "111001010100111",
    "-": "000000111000000", ">": "100010001010100", "<": "001010100010001", "?": "111001010000010",
    ":": "000010000010000", ".": "000000000000010", "_": "000000000000111", "/": "001001010100100",
    " ": "000000000000000", "#": "101111101111101", "+": "000010111010000",
}


def text_width(text: str) -> int:
    return len(text) * 4 - 1 if text else 0


def draw_text(img: Image, x: int, y: int, text: str, fg=(255, 255, 255), bg=(0, 0, 0), alpha: float = 0.75) -> None:
    """Draw text with a translucent backing box. Unknown characters draw as '?'."""
    if bg is not None:
        img.rect(x - 1, y - 1, text_width(text) + 2, 7, bg, alpha)
    for n, ch in enumerate(text.upper()):
        glyph = _FONT.get(ch, _FONT["?"])
        for k, bit in enumerate(glyph):
            if bit == "1":
                img.put(x + n * 4 + k % 3, y + k // 3, fg)
