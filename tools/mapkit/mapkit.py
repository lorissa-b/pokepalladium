#!/usr/bin/env python3
"""mapkit: inspect, render, build and check maps, and pull Sinnoh references.

Run `tools/mapkit/mapkit.py <command> -h` for each command's options. See
docs/map/tooling.md for the workflow these support.
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))

import blueprint  # noqa: E402
import check as checks  # noqa: E402
import compare as cmp  # noqa: E402
import platinum  # noqa: E402
import render  # noqa: E402
from project import REPO, Blockdata, behaviors, consts, maps_using_layout, resolve_layout, resolve_map  # noqa: E402
from tileset import TilesetPair, UnknownTileset, pair  # noqa: E402

DEFAULT_OUT = REPO / "build" / "mapkit"


def region_arg(text: str) -> tuple[int, ...]:
    parts = tuple(int(p, 0) for p in text.replace(" ", "").split(","))
    if len(parts) not in (2, 4):
        raise argparse.ArgumentTypeError("expected X,Y or X,Y,W,H")
    return parts


def full_region(layout: dict, region) -> tuple[int, int, int, int]:
    if not region:
        return (0, 0, layout["width"], layout["height"])
    if len(region) == 2:
        return (region[0], region[1], layout["width"] - region[0], layout["height"] - region[1])
    return region


def out_path(arg: str | None, default_name: str) -> Path:
    return Path(arg) if arg else DEFAULT_OUT / default_name


def map_for(name: str) -> tuple[dict | None, dict]:
    info = resolve_map(name)
    layout = resolve_layout(name)
    return info, layout


# ---------------------------------------------------------------- commands


def cmd_info(a) -> None:
    info, layout = map_for(a.map)
    c = consts()
    tiles = TilesetPair.for_layout(layout)
    print(f"layout    {layout['id']}  {layout['width']}x{layout['height']}  {layout['blockdata_filepath']}")
    print(f"tilesets  {layout['primary_tileset']} ({len(tiles.primary)} metatiles), {layout['secondary_tileset']} ({len(tiles.secondary)} metatiles)")
    users = [m["id"] for m in maps_using_layout(layout["id"])]
    print(f"used by   {', '.join(users) or '(no maps)'}")
    if not info:
        return
    print(f"map       {info['id']}  data/maps/{info['dir']}/  {info.get('map_type')}  {info.get('region_map_section')}")
    for conn in info.get("connections") or []:
        print(f"connects  {conn['direction']:<5} {conn['map']} offset {conn['offset']}")
    blocks = Blockdata.for_layout(layout)
    for i, w in enumerate(info.get("warp_events") or []):
        mid = blocks.get(w["x"], w["y"]) & c.metatile_mask if blocks.inside(w["x"], w["y"]) else None
        beh = behaviors().get(tiles.behavior(mid), "?") if mid is not None else "outside map"
        print(f"warp {i:<3}  ({w['x']},{w['y']}) -> {w['dest_map']} warp {w['dest_warp_id']}  [{beh}]")
    for i, o in enumerate(info.get("object_events") or []):
        print(f"object {i:<2} ({o['x']},{o['y']}) e{o.get('elevation')} {o.get('graphics_id')} {o.get('script')}")
    for i, b in enumerate(info.get("bg_events") or []):
        print(f"bg {i:<5}  ({b['x']},{b['y']}) {b.get('type')} {b.get('script') or b.get('item') or ''}")
    for i, t in enumerate(info.get("coord_events") or []):
        print(f"coord {i:<3}  ({t['x']},{t['y']}) {t.get('var')}={t.get('var_value')} {t.get('script')}")


def cmd_dump(a) -> None:
    _, layout = map_for(a.map)
    blocks = Blockdata.for_layout(layout)
    x0, y0, w, h = full_region(layout, a.region)
    c = consts()
    tiles = TilesetPair.for_layout(layout)
    if a.classes:
        grid = cmp.emerald_grid(layout)
        rows = [grid[y][x0 : x0 + w] for y in range(y0, y0 + h)]
        print_ruled(rows, x0, y0, 1)
        return
    cell = {"metatile": 3, "collision": 1, "elevation": 1, "block": 8, "behavior": 2}[a.layer]
    rows = []
    for y in range(y0, y0 + h):
        parts = []
        for x in range(x0, x0 + w):
            mid, col, elev = c.unpack(blocks.get(x, y))
            if a.layer == "metatile":
                parts.append(f"{mid:03X}")
            elif a.layer == "collision":
                parts.append("X" if col else ".")
            elif a.layer == "elevation":
                parts.append(f"{elev:X}")
            elif a.layer == "behavior":
                parts.append(f"{(tiles.behavior(mid) or 0):02X}")
            else:
                parts.append(f"{mid:03X}/{col}/{elev:X}".ljust(cell))
        rows.append((" " if cell > 1 else "").join(parts))
    print_ruled(rows, x0, y0, cell + (1 if cell > 1 else 0))


def print_ruled(rows: list[str], x0: int, y0: int, step: int) -> None:
    """Rows with column numbers above and row numbers on the left."""
    width = (len(rows[0]) + (1 if step > 1 else 0)) // step if rows else 0
    if step == 1:
        tens = "".join(str((x0 + i) // 10 % 10) if (x0 + i) % 10 == 0 else " " for i in range(width))
        ones = "".join(str((x0 + i) % 10) for i in range(width))
        print("     " + tens)
        print("     " + ones)
    else:
        print("     " + "".join(str(x0 + i).ljust(step) for i in range(width)))
    for j, row in enumerate(rows):
        print(f"{y0 + j:>3}  {row}")


def cmd_render(a) -> None:
    info, layout = map_for(a.map)
    blocks = Blockdata.for_layout(layout)
    region = full_region(layout, a.region)
    tiles = TilesetPair.for_layout(layout)
    img = render.draw_blocks(blocks, tiles, a.scale, region)
    ts = 16 * a.scale
    if a.collision:
        render.overlay_collision(img, blocks, ts, region)
    if a.elevation:
        render.overlay_elevation(img, blocks, ts, region)
    if a.grid:
        render.overlay_grid(img, region[2], region[3], ts, region[:2])
    if a.events and info:
        render.overlay_events(img, info, ts, region)
    path = out_path(a.output, f"{layout['name'].removesuffix('_Layout')}.png")
    img.save(path)
    print(path)


def cmd_tileset(a) -> None:
    if a.secondary:
        tiles = pair(a.target, a.secondary)
        name = f"{tiles.primary.name}+{tiles.secondary.name}"
    else:
        layout = resolve_layout(a.target)
        tiles = TilesetPair.for_layout(layout)
        name = f"{tiles.primary.name}+{tiles.secondary.name}"
    ids = tiles.ids()
    c = consts()
    if a.only == "primary":
        ids = [i for i in ids if i < c.metatiles_in_primary]
    elif a.only == "secondary":
        ids = [i for i in ids if i >= c.metatiles_in_primary]
    if a.behavior:
        want = a.behavior.upper()
        ids = [i for i in ids if want in behaviors().get(tiles.behavior(i), "")]
    if a.list:
        defaults = blueprint.usage_defaults()
        for i in ids:
            ts = tiles.primary if i < c.metatiles_in_primary else tiles.secondary
            col_elev = defaults.get((ts.symbol, i))
            usual = f"usually c{col_elev[0]}/e{col_elev[1]}" if col_elev else "unused"
            print(f"{render.describe_metatile(tiles, i)}  ({usual})")
        return
    img = render.catalog(tiles, a.columns, a.scale, ids)
    path = out_path(a.output, f"tileset_{name}.png")
    img.save(path)
    print(path)


def cmd_extract(a) -> None:
    _, layout = map_for(a.map)
    region = full_region(layout, a.region)
    tiles = TilesetPair.for_layout(layout)
    text = blueprint.extract(layout, region, standalone=a.standalone, describe=lambda m: render.describe_metatile(tiles, m).split(" ", 1)[1])
    if a.output:
        Path(a.output).write_text(text)
        print(a.output)
    else:
        sys.stdout.write(text)


def cmd_build(a) -> None:
    bp = blueprint.load(Path(a.blueprint), a.layout)
    try:
        blocks = bp.build()
    except blueprint.BlueprintError as e:
        raise SystemExit(f"error: {e}")
    for w in bp.warnings:
        print(f"warning: {w}")
    layout = bp.layout
    if a.dry_run:
        print(f"{layout['id']}: {blocks.width}x{blocks.height} (dry run, nothing written)")
    else:
        for note in blueprint.write(layout, blocks):
            print(note)
        print(f"wrote {layout['blockdata_filepath']} ({blocks.width}x{blocks.height})")
    if a.render:
        img = render.draw_blocks(blocks, TilesetPair.for_layout(layout), a.scale)
        render.overlay_grid(img, blocks.width, blocks.height, 16 * a.scale)
        Path(a.render).parent.mkdir(parents=True, exist_ok=True)
        img.save(Path(a.render))
        print(a.render)
    if not a.dry_run:
        findings = checks.run([layout["id"]])
        report(findings)


def report(findings) -> int:
    errors = 0
    for level, where, msg in findings:
        errors += level == "error"
        print(f"{level}: {where}: {msg}")
    return errors


def cmd_check(a) -> None:
    findings = checks.run(a.maps or None)
    if not a.warnings:
        findings = [f for f in findings if f[0] == "error"]
    errors = report(findings)
    count = len(a.maps) if a.maps else "all"
    print(f"checked {count} map(s): {errors} error(s), {len(findings) - errors} warning(s)")
    sys.exit(1 if errors else 0)


def cmd_platinum(a) -> None:
    if a.action == "update":
        print(f"pokeplatinum is at {platinum.update()}")
        return
    if a.action == "list":
        for h in platinum.search(" ".join(a.names)):
            print(h)
        return
    ref = platinum.load(" ".join(a.names))
    region = a.region
    if a.json:
        Path(a.json).parent.mkdir(parents=True, exist_ok=True)
        Path(a.json).write_text(json.dumps(ref.to_json(), indent=1))
        print(a.json)
    if a.render is not None:
        img = render.draw_classes(ref.grid(), a.scale)
        render.overlay_reference(img, ref, a.scale)
        render.overlay_grid(img, ref.width, ref.height, a.scale, labels=a.scale >= 8)
        path = out_path(a.render or None, f"platinum_{ref.header[len('MAP_HEADER_'):].lower()}.png")
        img.save(path)
        print(path)
    if a.json or a.render is not None:
        return
    print(f"{ref.header}: {ref.width}x{ref.height} tiles from {ref.matrix}, origin {ref.origin} in matrix tiles")
    for d, names in ref.neighbours.items():
        print(f"  {d}: {', '.join(names)}")
    print()
    grid = ref.grid(events=a.events)
    x0, y0, w, h = region if region and len(region) == 4 else (0, 0, ref.width, ref.height)
    print_ruled([r[x0 : x0 + w] for r in grid[y0 : y0 + h]], x0, y0, 1)
    print()
    print("  " + "   ".join(f"{k!r} {v}" for k, v in platinum.LEGEND))
    if a.events:
        print("  'W' warp   '@' object   '?' sign/hidden item   'T' trigger")
    print()
    for line in cmp.notes(ref, (0, 0, ref.width, ref.height)):
        print(line[2:] if line.startswith("# ") else line)


def cmd_draft(a) -> None:
    ref = platinum.load(a.header)
    layout = resolve_layout(a.layout)
    region = tuple(a.region) if a.region else None
    if region and len(region) == 2:
        region = (region[0], region[1], ref.width - region[0], ref.height - region[1])
    text = cmp.draft(ref, layout, region)
    if a.output:
        Path(a.output).parent.mkdir(parents=True, exist_ok=True)
        Path(a.output).write_text(text)
        print(a.output)
    else:
        sys.stdout.write(text)


def cmd_compare(a) -> None:
    _, layout = map_for(a.map)
    ref = platinum.load(a.header)
    if a.origin == "auto":
        origin = cmp.best_origin(layout, ref)
        print(f"best origin: {origin[0]},{origin[1]}")
    else:
        origin = region_arg(a.origin) if a.origin else (0, 0)
    result = cmp.compare(layout, ref, origin)
    if a.render:
        info = resolve_map(a.map)
        img = render.draw_blocks(Blockdata.for_layout(layout), TilesetPair.for_layout(layout), a.scale)
        ts = 16 * a.scale
        for x, y, *_ in result["mismatches"]:
            img.rect(x * ts, y * ts, ts, ts, (255, 0, 0), 0.45)
        render.overlay_reference(img, ref, ts, origin)
        render.overlay_grid(img, layout["width"], layout["height"], ts)
        if info and a.events:
            render.overlay_events(img, info, ts)
        Path(a.render).parent.mkdir(parents=True, exist_ok=True)
        img.save(Path(a.render))
        print(a.render)
    if not a.quiet:
        print_ruled(result["grid"], 0, 0, 1)
        print("\n  'X' = movement differs from Platinum; blank = outside the reference\n")
        for x, y, e, r in result["mismatches"][: a.limit]:
            print(f"  ({x},{y}) is {e!r} here, {r!r} in Platinum")
        if len(result["mismatches"]) > a.limit:
            print(f"  ... {len(result['mismatches']) - a.limit} more")
    for x, y, dest in result["missing_warps"]:
        print(f"  no warp near ({x},{y}); Platinum has one to {dest}")
    t = result["total"] or 1
    print(f"{layout['id']} vs {ref.header} at origin {origin}: movement matches on {result['same_group']}/{result['total']} tiles "
          f"({100 * result['same_group'] / t:.1f}%), exact class on {result['exact']} ({100 * result['exact'] / t:.1f}%)")


def main(argv=None) -> None:
    p = argparse.ArgumentParser(prog="mapkit", description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = p.add_subparsers(dest="cmd", required=True)
    region_help = "X,Y or X,Y,W,H to limit to a region"

    s = sub.add_parser("info", help="summarise a map: layout, tilesets, connections, events")
    s.add_argument("map", help="MAP_* id, map directory name, LAYOUT_* id or layout name")
    s.set_defaults(func=cmd_info)

    s = sub.add_parser("dump", help="print a layout as text")
    s.add_argument("map")
    s.add_argument("--layer", choices=["metatile", "collision", "elevation", "behavior", "block"], default="metatile")
    s.add_argument("--classes", action="store_true", help="tile classes (the same symbols the Platinum tools use)")
    s.add_argument("--region", type=region_arg, help=region_help)
    s.set_defaults(func=cmd_dump)

    s = sub.add_parser("render", help="draw a map to PNG")
    s.add_argument("map")
    s.add_argument("-o", "--output", help=f"PNG path (default {DEFAULT_OUT.relative_to(REPO)}/<layout>.png)")
    s.add_argument("--region", type=region_arg, help=region_help)
    s.add_argument("--scale", type=int, default=1)
    s.add_argument("--grid", action="store_true", help="tile grid with coordinates")
    s.add_argument("--events", action="store_true", help="mark warps, objects, signs and triggers")
    s.add_argument("--collision", action="store_true", help="tint impassable blocks red")
    s.add_argument("--elevation", action="store_true", help="tint blocks by elevation")
    s.set_defaults(func=cmd_render)

    s = sub.add_parser("tileset", help="metatile catalog for a layout or tileset pair")
    s.add_argument("target", help="a map/layout, or a primary tileset symbol (with SECONDARY)")
    s.add_argument("secondary", nargs="?", help="secondary tileset symbol, e.g. gTileset_Rustboro")
    s.add_argument("-o", "--output")
    s.add_argument("--list", action="store_true", help="print ids, behaviours, labels and usual collision/elevation")
    s.add_argument("--only", choices=["primary", "secondary"])
    s.add_argument("--behavior", help="only metatiles whose MB_* name contains this")
    s.add_argument("--columns", type=int, default=16)
    s.add_argument("--scale", type=int, default=2)
    s.set_defaults(func=cmd_tileset)

    s = sub.add_parser("extract", help="write a layout region as a blueprint (for stamping or editing)")
    s.add_argument("map")
    s.add_argument("--region", type=region_arg, help=region_help)
    s.add_argument("--standalone", action="store_true", help="include the layout/size header so it builds on its own")
    s.add_argument("-o", "--output")
    s.set_defaults(func=cmd_extract)

    s = sub.add_parser("build", help="build a blueprint into the layout's map.bin")
    s.add_argument("blueprint")
    s.add_argument("--layout", help="override the blueprint's layout line")
    s.add_argument("--dry-run", action="store_true", help="parse and build without writing")
    s.add_argument("--render", help="also draw the result to this PNG")
    s.add_argument("--scale", type=int, default=1)
    s.set_defaults(func=cmd_build)

    s = sub.add_parser("check", help="check layouts, events, warps and connections")
    s.add_argument("maps", nargs="*", help="maps or layouts (default: everything)")
    s.add_argument("-w", "--warnings", action="store_true", help="show warnings too")
    s.set_defaults(func=cmd_check)

    s = sub.add_parser("platinum", help="read a Sinnoh map from pret/pokeplatinum")
    s.add_argument("action", choices=["show", "list", "update"])
    s.add_argument("names", nargs="*", help="map header, e.g. JUBILIFE_CITY or 'route 204 south'")
    s.add_argument("--events", action="store_true", help="overlay events on the grid")
    s.add_argument("--region", type=region_arg, help="X,Y,W,H of the grid to print")
    s.add_argument("--json", help="write the full reference (behaviours, collision, events, props) as JSON")
    s.add_argument("--render", nargs="?", const="", help="draw the reference to PNG")
    s.add_argument("--scale", type=int, default=8, help="pixels per tile for --render")
    s.set_defaults(func=cmd_platinum)

    s = sub.add_parser("draft", help="draft a blueprint from a Platinum map")
    s.add_argument("header", help="Platinum map header")
    s.add_argument("layout", help="layout the blueprint builds (sets tilesets and output path)")
    s.add_argument("--region", type=region_arg, help="X,Y,W,H of the Platinum map to use")
    s.add_argument("-o", "--output")
    s.set_defaults(func=cmd_draft)

    s = sub.add_parser("compare", help="score a map against its Platinum original")
    s.add_argument("map")
    s.add_argument("header")
    s.add_argument("--origin", help="Platinum tile X,Y at the map's (0,0), or 'auto' to find the best fit")
    s.add_argument("--render", help="draw the map with mismatches tinted red and Platinum events outlined")
    s.add_argument("--events", action="store_true", help="with --render, also mark this map's events")
    s.add_argument("--scale", type=int, default=2)
    s.add_argument("--limit", type=int, default=20, help="mismatches to list")
    s.add_argument("-q", "--quiet", action="store_true", help="only print the summary")
    s.set_defaults(func=cmd_compare)

    a = p.parse_args(argv)
    try:
        a.func(a)
    except BrokenPipeError:
        pass
    except UnknownTileset as e:
        raise SystemExit(f"error: {e}")


if __name__ == "__main__":
    main()
