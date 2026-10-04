"""The original Emerald layouts, read from this repo's history.

Commit ORIGINAL holds every Hoenn layout already converted to this repo's
map grid format (12-bit metatile ids, 1-bit collision, 3-bit elevation) and
before any map was redrawn. Later tileset changes only append metatiles, so
the current tilesets still describe these layouts correctly.
"""

from __future__ import annotations

import json
import subprocess
from functools import lru_cache

from project import REPO, Blockdata, consts

ORIGINAL = "73761a50"  # "Add custom border dimensions", the last commit before the Sinnoh maps

# The screen is 15x10 blocks; the player stands in column 7 and row 4-5.
VIEW_X, VIEW_Y = 7, 5


def _git(*args: str, data: bytes | None = None) -> bytes:
    try:
        return subprocess.run(["git", *args], cwd=REPO, input=data, capture_output=True, check=True).stdout
    except subprocess.CalledProcessError as e:
        raise SystemExit(
            f"error: can't read the original layouts at {ORIGINAL} ({e.stderr.decode().strip()}); "
            "if this is a shallow clone, run `git fetch --unshallow`"
        ) from None


def _batch(paths: list[str]) -> list[bytes | None]:
    """The contents of these files at ORIGINAL (None for any that's missing)."""
    out = _git("cat-file", "--batch", data="".join(f"{ORIGINAL}:{p}\n" for p in paths).encode())
    result, pos = [], 0
    for _ in paths:
        end = out.index(b"\n", pos)
        header = out[pos:end].split()
        if header[-1] == b"missing":
            result.append(None)
            pos = end + 1
            continue
        size = int(header[2])
        result.append(out[end + 1 : end + 1 + size])
        pos = end + 1 + size + 1
    return result


@lru_cache(maxsize=None)
def maps() -> dict[str, list[dict]]:
    """Layout id -> the original map.json of every map using it."""
    files = [f for f in _git("ls-tree", "-r", "--name-only", ORIGINAL, "data/maps").decode().split() if f.endswith("/map.json")]
    out: dict[str, list[dict]] = {}
    for blob in _batch(files):
        if blob:
            info = json.loads(blob)
            out.setdefault(info.get("layout"), []).append(info)
    return out


@lru_cache(maxsize=None)
def layouts() -> list[tuple[dict, Blockdata]]:
    """(layouts.json entry, blocks) for every original layout."""
    entries = [e for e in json.loads(_git("show", f"{ORIGINAL}:data/layouts/layouts.json"))["layouts"] if e.get("id")]
    request = "".join(f"{ORIGINAL}:{e['blockdata_filepath']}\n" for e in entries).encode()
    out = _git("cat-file", "--batch", data=request)
    result, pos = [], 0
    for e in entries:
        end = out.index(b"\n", pos)
        header = out[pos:end].split()
        if header[-1] == b"missing":
            pos = end + 1
            continue
        size = int(header[2])
        blob = out[end + 1 : end + 1 + size]
        pos = end + 1 + size + 1
        count = e["width"] * e["height"]
        if len(blob) >= count * 2:
            import struct

            result.append((e, Blockdata(e["width"], e["height"], list(struct.unpack(f"<{count}H", blob[: count * 2])))))
    return result


def visible(blocks: Blockdata) -> list[list[bool]]:
    """Blocks that can be on screen while the player stands on a passable block."""
    c = consts()
    w, h = blocks.width, blocks.height
    # 2D prefix sums of passable blocks, to count them in each view window.
    acc = [[0] * (w + 1) for _ in range(h + 1)]
    for y in range(h):
        run = 0
        for x in range(w):
            run += 0 if c.unpack(blocks.get(x, y))[1] else 1
            acc[y + 1][x + 1] = acc[y][x + 1] + run

    def passable_in(x0, y0, x1, y1):
        x0, y0, x1, y1 = max(0, x0), max(0, y0), min(w, x1), min(h, y1)
        if x0 >= x1 or y0 >= y1:
            return 0
        return acc[y1][x1] - acc[y0][x1] - acc[y1][x0] + acc[y0][x0]

    return [[passable_in(x - VIEW_X, y - VIEW_Y, x + VIEW_X + 1, y + VIEW_Y + 1) > 0 for x in range(w)] for y in range(h)]
