"""What Platinum's ground looks like, tile by tile, from a chunk's 3D model.

A land data chunk carries, after its tile attributes and props, an NSBMD
model of the terrain. Its polygons are painted with named materials:
`nsand` for a sand path, `hage` for bare earth, `nhana` for flowers,
`ngrass` for grass and so on. The tile attributes only say where the player
can go, so this is the one place paths and flower beds are recorded.

ground_materials() rasterises the model: at the centre of each tile it takes
the highest polygon that is ground (not a tree, a canopy or a shadow) and
returns that polygon's material. kind() sorts material names into the few
kinds a draft cares about.

The NSBMD reading covers what Platinum's terrain models use: one model, the
render commands that bind a material to each shape (MAT and SHP), and the
GX display lists of the shapes (vertex, texture and normal commands). Node
transforms are not applied; the terrain models are exported unrotated.
"""

from __future__ import annotations

import re
import struct
from collections import Counter

CHUNK = 32
TILE_UNITS = 16  # world units per tile
HALF = CHUNK * TILE_UNITS / 2

# Material name -> kind, first match wins. Names lose their lightmap and
# palette suffixes (_lm2, _pl, .1) before matching. Anything unmatched is
# "unknown", and `mapkit.py platinum show --ground` lists those names, so the
# table grows as maps turn them up.
KINDS = [
    (r"shadow|kage", "shadow"),
    (r"tree|wood|^ki_|conttree|forest|mori", "tree"),
    (r"bridge", "bridge"),
    # Twinleaf, Sandgem and the early routes paint paths with nsand (and its
    # edges, nsandp) and worn ground with hage; cities have road pieces:
    # c1_r1* in Jubilife, c3_road* in Oreburgh, c4_road* in Hearthome.
    (r"^n?sand|hage|road|^c\d_r\d|michi|dirt|tsuchi|^path", "path"),
    (r"hana|flower|_fl_", "flowers"),
    (r"grass|kusa|shiba|lawn|^lgreen", "grass"),
    (r"lake|sea|water|mizu|umi|puddle|river|kawa|pond", "water"),
    (r"beach|hamabe", "sand"),
    (r"snow|sonw|yuki", "snow"),
    (r"rock|iwa|cliff|criff|gake|stone|peak", "rock"),
]
# Which look of Emerald path a Platinum path material calls for: roads and
# city paving are stone, everything else (sand, bare earth) is sandy.
STONE_PATHS = r"road|^c\d_r\d|paving|stone"


def path_look(material: str | None) -> str | None:
    if kind(material) != "path":
        return None
    return "stone" if re.search(STONE_PATHS, base_name(material).lower()) else "sandy"


# Kinds that sit over the ground rather than being it.
OVERHEAD = {"tree", "shadow"}


def base_name(material: str) -> str:
    """`nsandp_lm2` -> `nsandp`, `lakep.1_pl` -> `lakep`."""
    name = re.sub(r"(\.\d+)?(_pl)?$", "", material)
    return re.sub(r"_lm\d*$", "", name)


def kind(material: str | None) -> str | None:
    if material is None:
        return None
    name = base_name(material).lower()
    for pattern, k in KINDS:
        if re.search(pattern, name):
            return k
    return "unknown"


# ---------------------------------------------------------------- NSBMD


class _Reader:
    def __init__(self, raw: bytes) -> None:
        self.raw = raw

    def u8(self, o: int) -> int:
        return self.raw[o]

    def u16(self, o: int) -> int:
        return struct.unpack_from("<H", self.raw, o)[0]

    def u32(self, o: int) -> int:
        return struct.unpack_from("<I", self.raw, o)[0]

    def dictionary(self, off: int) -> tuple[list[str], list[bytes]]:
        """A Nitro name dictionary: its names and raw entries, in order."""
        n = self.u8(off + 1)
        info = off + self.u16(off + 6)
        size = self.u16(info)
        names_at = info + self.u16(info + 2)
        names = [self.raw[names_at + 16 * i: names_at + 16 * i + 16].split(b"\0")[0].decode("latin-1") for i in range(n)]
        entries = [self.raw[info + 4 + size * i: info + 4 + size * (i + 1)] for i in range(n)]
        return names, entries


# Parameter words taken by each GX command id.
_GX_PARAMS = {
    0x00: 0, 0x10: 1, 0x11: 0, 0x12: 1, 0x13: 1, 0x14: 1, 0x15: 0, 0x16: 16, 0x17: 12, 0x18: 16,
    0x19: 12, 0x1A: 9, 0x1B: 3, 0x1C: 3, 0x20: 1, 0x21: 1, 0x22: 1, 0x23: 2, 0x24: 1, 0x25: 1,
    0x26: 1, 0x27: 1, 0x28: 1, 0x29: 1, 0x2A: 1, 0x2B: 1, 0x30: 1, 0x31: 1, 0x32: 1, 0x33: 1,
    0x34: 32, 0x40: 1, 0x41: 0, 0x50: 1, 0x60: 1, 0x70: 3, 0x71: 2, 0x72: 1,
}


def _s16(v: int) -> int:
    return v - 0x10000 if v & 0x8000 else v


def _s10(v: int) -> int:
    return v - 0x400 if v & 0x200 else v


def _triangles(r: _Reader, start: int, size: int, scale: float) -> list[tuple]:
    """The triangles a GX display list draws, as ((x, y, z), ...) in world units."""
    tris = []
    o, end = start, start + size
    vtx = [0.0, 0.0, 0.0]
    prim, strip = 0, []
    while o + 4 <= end:
        cmds = r.raw[o: o + 4]
        o += 4
        for c in cmds:
            n = _GX_PARAMS.get(c, 0)
            p = [r.u32(o + 4 * k) for k in range(n)] if o + 4 * n <= len(r.raw) else [0] * n
            o += 4 * n
            if c == 0x40:  # BEGIN_VTXS
                prim, strip = p[0] & 3, []
                continue
            if c == 0x23:  # VTX_16
                vtx = [_s16(p[0] & 0xFFFF) / 4096, _s16(p[0] >> 16) / 4096, _s16(p[1] & 0xFFFF) / 4096]
            elif c == 0x24:  # VTX_10
                vtx = [_s10(p[0] & 0x3FF) / 64, _s10((p[0] >> 10) & 0x3FF) / 64, _s10((p[0] >> 20) & 0x3FF) / 64]
            elif c == 0x25:  # VTX_XY
                vtx = [_s16(p[0] & 0xFFFF) / 4096, _s16(p[0] >> 16) / 4096, vtx[2]]
            elif c == 0x26:  # VTX_XZ
                vtx = [_s16(p[0] & 0xFFFF) / 4096, vtx[1], _s16(p[0] >> 16) / 4096]
            elif c == 0x27:  # VTX_YZ
                vtx = [vtx[0], _s16(p[0] & 0xFFFF) / 4096, _s16(p[0] >> 16) / 4096]
            elif c == 0x28:  # VTX_DIFF
                # 10-bit .9 fractions, divided by 8: steps of 1/4096 like VTX_16.
                vtx = [vtx[0] + _s10(p[0] & 0x3FF) / 4096, vtx[1] + _s10((p[0] >> 10) & 0x3FF) / 4096,
                       vtx[2] + _s10((p[0] >> 20) & 0x3FF) / 4096]
            else:
                continue
            strip.append(tuple(v * scale for v in vtx))
            if prim == 0 and len(strip) == 3:  # triangles
                tris.append(tuple(strip))
                strip = []
            elif prim == 1 and len(strip) == 4:  # quads
                a, b, cc, d = strip
                tris += [(a, b, cc), (a, cc, d)]
                strip = []
            elif prim == 2 and len(strip) >= 3:  # triangle strip
                tris.append(tuple(strip[-3:]))
            elif prim == 3 and len(strip) >= 4 and len(strip) % 2 == 0:  # quad strip
                a, b, cc, d = strip[-4:]
                tris += [(a, b, d), (a, d, cc)]
    return tris


def shapes(model: bytes) -> list[tuple[str, list[tuple]]]:
    """Each shape of a terrain NSBMD as (material name, triangles)."""
    r = _Reader(model)
    if model[:4] != b"BMD0":
        return []
    mdl0 = None
    for i in range(r.u16(0x0E)):
        off = r.u32(0x10 + 4 * i)
        if model[off: off + 4] == b"MDL0":
            mdl0 = off
    if mdl0 is None:
        return []
    _, entries = r.dictionary(mdl0 + 8)
    base = mdl0 + struct.unpack_from("<I", entries[0])[0]
    _, sbc, mat, shp, _ = struct.unpack_from("<5I", model, base)
    scale = struct.unpack_from("<i", model, base + 20 + 8)[0] / 4096
    materials, _ = r.dictionary(base + mat + 4)
    _, shape_entries = r.dictionary(base + shp)

    # Render commands: which material is bound when each shape is drawn.
    shape_material: dict[int, int] = {}
    pc, current = base + sbc, None
    while pc < base + mat:
        op = model[pc]
        cmd = op & 0x1F
        if cmd == 0x00:
            pc += 1
        elif cmd == 0x01:
            break
        elif cmd == 0x02:
            pc += 3
        elif cmd == 0x03:
            pc += 2
        elif cmd == 0x04:
            current = model[pc + 1]
            pc += 2
        elif cmd == 0x05:
            shape_material[model[pc + 1]] = current
            pc += 2
        elif cmd == 0x06:
            pc += 4 + bool(op & 0x20) + bool(op & 0x40)
        elif cmd in (0x07, 0x08):
            pc += 2 + bool(op & 0x20) + bool(op & 0x40)
        elif cmd == 0x09:
            pc += 3 + 3 * model[pc + 2]
        elif cmd == 0x0B:
            pc += 1
        elif cmd in (0x0C, 0x0D):
            pc += 3
        else:
            break

    out = []
    for i, entry in enumerate(shape_entries):
        so = base + shp + struct.unpack_from("<I", entry)[0]
        dl_off, dl_size = struct.unpack_from("<II", model, so + 8)
        m = shape_material.get(i)
        name = materials[m] if m is not None and m < len(materials) else "?"
        out.append((name, _triangles(r, so + dl_off, dl_size, scale)))
    return out


def _height(px: float, pz: float, tri: tuple) -> float | None:
    """The triangle's height at (px, pz) seen from above, or None if it doesn't cover the point."""
    (x1, y1, z1), (x2, y2, z2), (x3, y3, z3) = tri
    d = (z2 - z3) * (x1 - x3) + (x3 - x2) * (z1 - z3)
    if abs(d) < 1e-9:
        return None
    a = ((z2 - z3) * (px - x3) + (x3 - x2) * (pz - z3)) / d
    b = ((z3 - z1) * (px - x3) + (x1 - x3) * (pz - z3)) / d
    c = 1 - a - b
    if min(a, b, c) < -1e-6:
        return None
    return a * y1 + b * y2 + c * y3


def ground_materials(model: bytes) -> list[list[str | None]]:
    """For each tile of a 32x32 chunk, the material of the ground at its centre (None if no ground)."""
    grid: list[list[str | None]] = [[None] * CHUNK for _ in range(CHUNK)]
    best: list[list[float]] = [[float("-inf")] * CHUNK for _ in range(CHUNK)]
    for name, tris in shapes(model):
        if kind(name) in OVERHEAD:
            continue
        for tri in tris:
            xs = [v[0] for v in tri]
            zs = [v[2] for v in tri]
            tx0 = max(0, int((min(xs) + HALF) // TILE_UNITS))
            tx1 = min(CHUNK - 1, int((max(xs) + HALF) // TILE_UNITS))
            tz0 = max(0, int((min(zs) + HALF) // TILE_UNITS))
            tz1 = min(CHUNK - 1, int((max(zs) + HALF) // TILE_UNITS))
            for tz in range(tz0, tz1 + 1):
                pz = -HALF + (tz + 0.5) * TILE_UNITS
                for tx in range(tx0, tx1 + 1):
                    px = -HALF + (tx + 0.5) * TILE_UNITS
                    h = _height(px, pz, tri)
                    if h is not None and h >= best[tz][tx] - 1e-6:
                        best[tz][tx] = h
                        grid[tz][tx] = name
    return grid


def summary(grid: list[list[str | None]]) -> Counter:
    """How many tiles each material covers."""
    return Counter(m for row in grid for m in row if m is not None)
