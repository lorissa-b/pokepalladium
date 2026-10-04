"""Consistency checks for layouts and maps.

Each finding is (level, map or layout, message). Levels:
  error    the game will misbehave (wrong size, missing target, wrong tiles)
  warning  probably a mistake (one-way warp, warp that can't be stepped on)
"""

from __future__ import annotations

import re

from project import Blockdata, behaviors, consts, layouts, maps, maps_using_layout
from tileset import TilesetPair, UnknownTileset

OPPOSITE = {"up": "down", "down": "up", "left": "right", "right": "left"}
SPECIAL_DESTS = {"MAP_DYNAMIC", "MAP_NONE", "MAP_UNDEFINED"}

# Behaviours a warp can be triggered from by walking (see IsWarpMetatileBehavior,
# IsArrowWarpMetatileBehavior and TryDoorWarp in src/field_control_avatar.c).
WARP_BEHAVIOR = re.compile(r"DOOR|WARP|LADDER|ESCALATOR|HOLE|STAIRS|SECRET_BASE_SPOT|SHOAL_CAVE_ENTRANCE")


def _in_bounds(e: dict, w: int, h: int) -> bool:
    return 0 <= int(e["x"]) < w and 0 <= int(e["y"]) < h


def check_layout(layout: dict) -> list[tuple[str, str, str]]:
    out = []
    lid = layout["id"]
    c = consts()
    try:
        blocks = Blockdata.for_layout(layout)
    except (OSError, ValueError) as e:
        return [("error", lid, str(e))]
    if blocks.extra:
        out.append(("warning", lid, f"{layout['blockdata_filepath']} has {blocks.extra} block(s) past {blocks.width}x{blocks.height}"))
    try:
        Blockdata.border_for_layout(layout)
    except (OSError, ValueError) as e:
        out.append(("error", lid, f"border: {e}"))
    if (blocks.width + 15) * (blocks.height + 14) > c.max_map_data_size:
        out.append(("error", lid, f"{blocks.width}x{blocks.height} is too big for the map buffer (MAX_MAP_DATA_SIZE {c.max_map_data_size})"))
    try:
        tiles = TilesetPair.for_layout(layout)
    except UnknownTileset as e:
        return out + [("error", lid, str(e))]
    missing: dict[int, list[tuple[int, int]]] = {}
    for y in range(blocks.height):
        for x in range(blocks.width):
            mid = blocks.get(x, y) & c.metatile_mask
            if mid != c.undefined and not tiles.exists(mid):
                missing.setdefault(mid, []).append((x, y))
    for mid, where in sorted(missing.items()):
        pts = ", ".join(f"({x},{y})" for x, y in where[:4]) + (" ..." if len(where) > 4 else "")
        out.append(("error", lid, f"metatile {mid:#05x} doesn't exist in {layout['primary_tileset']}/{layout['secondary_tileset']}: {pts}"))
    return out


# The screen is 15x10 blocks with the player in column 7 and row 4 (see
# include/fieldmap.h's MAP_OFFSET and the camera in src/fieldmap.c).
VIEW_LEFT = VIEW_RIGHT = 7
VIEW_UP, VIEW_DOWN = 5, 5


def _visible_neighbour_blocks(direction: str, offset: int, a: dict, ablocks: Blockdata, b: dict):
    """Blocks of B that can be on screen while the player stands on a passable block of A.

    A connection `direction` on A with `offset` puts B's x (or y) 0 at A's
    x (or y) = offset (FillNorthConnection etc. in src/fieldmap.c).
    """
    c = consts()
    aw, ah, bw, bh = a["width"], a["height"], b["width"], b["height"]

    def to_a(bx, by):
        if direction == "up":
            return bx + offset, by - bh
        if direction == "down":
            return bx + offset, ah + by
        if direction == "left":
            return bx - bw, by + offset
        return aw + bx, by + offset

    # Passable blocks of A near the shared edge.
    if direction in ("up", "down"):
        rows = range(0, VIEW_UP) if direction == "up" else range(ah - VIEW_DOWN, ah)
        stand = [(x, y) for y in rows for x in range(aw) if 0 <= y < ah and not c.unpack(ablocks.get(x, y))[1]]
    else:
        cols = range(0, VIEW_LEFT) if direction == "left" else range(aw - VIEW_RIGHT, aw)
        stand = [(x, y) for x in cols for y in range(ah) if 0 <= x < aw and not c.unpack(ablocks.get(x, y))[1]]
    stand = set(stand)
    for by in range(bh):
        for bx in range(bw):
            ax, ay = to_a(bx, by)
            if any((px, py) in stand for px in range(ax - VIEW_RIGHT, ax + VIEW_LEFT + 1) for py in range(ay - VIEW_DOWN + 1, ay + VIEW_UP + 1)):
                yield bx, by


def check_map(info: dict) -> list[tuple[str, str, str]]:
    out = []
    mid_ = info["id"]
    all_maps, all_layouts = maps(), layouts()
    layout = all_layouts.get(info.get("layout"))
    if layout is None:
        return [("error", mid_, f"layout {info.get('layout')} doesn't exist")]
    w, h = layout["width"], layout["height"]
    c = consts()
    try:
        blocks = Blockdata.for_layout(layout)
    except (OSError, ValueError):
        blocks = None
    try:
        tiles = TilesetPair.for_layout(layout)
    except UnknownTileset:
        return out  # reported by check_layout
    mb = behaviors()

    for kind, label in (("object_events", "object"), ("warp_events", "warp"), ("bg_events", "bg event"), ("coord_events", "coord event")):
        for i, e in enumerate(info.get(kind) or []):
            if not _in_bounds(e, w, h):
                out.append(("error", mid_, f"{label} {i} at ({e['x']},{e['y']}) is outside the {w}x{h} map"))

    for i, warp in enumerate(info.get("warp_events") or []):
        dest = warp.get("dest_map")
        if dest not in SPECIAL_DESTS:
            target = all_maps.get(dest)
            if target is None:
                out.append(("error", mid_, f"warp {i} goes to {dest}, which doesn't exist"))
            else:
                did = str(warp.get("dest_warp_id"))
                twarps = target.get("warp_events") or []
                if did.isdigit():
                    if int(did) >= len(twarps):
                        out.append(("error", mid_, f"warp {i} goes to {dest} warp {did}, but it only has {len(twarps)}"))
                    elif twarps[int(did)].get("dest_map") not in (mid_, "MAP_DYNAMIC"):
                        out.append(("warning", mid_, f"warp {i} -> {dest} warp {did}, which leads to {twarps[int(did)].get('dest_map')} instead of back here"))
        if blocks is not None and _in_bounds(warp, w, h):
            beh = tiles.behavior(blocks.get(int(warp["x"]), int(warp["y"])) & c.metatile_mask)
            name = mb.get(beh, "") if beh is not None else ""
            if not WARP_BEHAVIOR.search(name):
                out.append(("warning", mid_, f"warp {i} at ({warp['x']},{warp['y']}) is on a {name or 'missing'} tile, so walking onto it won't use it"))

    for conn in info.get("connections") or []:
        d, other_id, offset = conn["direction"], conn["map"], int(conn["offset"])
        other = all_maps.get(other_id)
        if other is None:
            out.append(("error", mid_, f"connection {d} to {other_id}, which doesn't exist"))
            continue
        if d not in OPPOSITE:
            continue  # dive/emerge
        back = [k for k in other.get("connections") or [] if k["map"] == mid_ and k["direction"] == OPPOSITE[d]]
        if not back:
            out.append(("error", mid_, f"connects {d} to {other_id}, but {other_id} doesn't connect {OPPOSITE[d]} back"))
        elif all(int(k["offset"]) != -offset for k in back):
            out.append(("error", mid_, f"connects {d} to {other_id} at offset {offset}, but the way back uses {back[0]['offset']} (expected {-offset})"))
        olayout = all_layouts.get(other.get("layout"))
        if olayout is None:
            continue
        span_a = w if d in ("up", "down") else h
        span_b = olayout["width"] if d in ("up", "down") else olayout["height"]
        if offset >= span_a or offset + span_b <= 0:
            out.append(("error", mid_, f"connection {d} to {other_id} at offset {offset} doesn't touch this map"))
        # The neighbour's edge is drawn with this map's tilesets.
        if (olayout["primary_tileset"], olayout["secondary_tileset"]) == (layout["primary_tileset"], layout["secondary_tileset"]):
            continue
        try:
            oblocks = Blockdata.for_layout(olayout)
        except (OSError, ValueError):
            continue
        if blocks is None:
            continue
        try:
            otiles = TilesetPair.for_layout(olayout)
        except UnknownTileset:
            continue
        bad = []
        for x, y in _visible_neighbour_blocks(d, offset, layout, blocks, olayout):
            m = oblocks.get(x, y) & c.metatile_mask
            if m == c.undefined:
                continue
            # Fine if it happens to look the same with this map's tilesets.
            if tiles.pixels(m) != otiles.pixels(m):
                bad.append((x, y))
        if bad:
            pts = ", ".join(f"({x},{y})" for x, y in bad[:4]) + (f" and {len(bad) - 4} more" if len(bad) > 4 else "")
            out.append(("error", mid_, f"from here, {other_id}'s {OPPOSITE[d]} edge is drawn with {layout['primary_tileset']}/{layout['secondary_tileset']} and looks wrong at {pts}"))
    return out


def run(names: list[str] | None = None) -> list[tuple[str, str, str]]:
    from project import resolve_map, resolve_layout

    findings = []
    if names:
        targets_maps, targets_layouts = [], {}
        for n in names:
            info = resolve_map(n)
            if info:
                targets_maps.append(info)
                targets_layouts[info["layout"]] = layouts().get(info["layout"])
            else:
                lay = resolve_layout(n)
                targets_layouts[lay["id"]] = lay
                targets_maps += maps_using_layout(lay["id"])
    else:
        targets_maps = list(maps().values())
        targets_layouts = dict(layouts())
    for lay in targets_layouts.values():
        if lay:
            findings += check_layout(lay)
    for info in targets_maps:
        findings += check_map(info)
    return findings
