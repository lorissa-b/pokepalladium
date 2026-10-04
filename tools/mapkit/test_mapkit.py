"""Tests for mapkit. Run with: python3 -m unittest discover tools/mapkit

They read the repo's data but never write to it. The Platinum tests only run
when a pokeplatinum checkout is already cached (see platinum.cache_dir).
"""

from __future__ import annotations

import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

import blueprint  # noqa: E402
import check  # noqa: E402
import platinum  # noqa: E402
from project import Blockdata, behaviors, consts, layouts, resolve_layout  # noqa: E402


class GridFormat(unittest.TestCase):
    def test_pack_round_trip(self):
        c = consts()
        for mid, col, elev in [(0, 0, 0), (0x1D4, 1, 0), (0x8BB, 0, 3), (c.metatile_mask, c.max_collision, c.max_elevation)]:
            self.assertEqual(c.unpack(c.pack(mid, col, elev)), (mid, col, elev))

    def test_pack_rejects_out_of_range(self):
        c = consts()
        with self.assertRaises(ValueError):
            c.pack(0, c.max_collision + 1, 0)
        with self.assertRaises(ValueError):
            c.pack(c.metatile_mask + 1)


class Blueprints(unittest.TestCase):
    def build(self, text: str) -> Blockdata:
        return blueprint.Blueprint(text).build()

    def test_extract_rebuilds_every_layout_exactly(self):
        for layout in layouts().values():
            try:
                original = Blockdata.for_layout(layout)
            except (OSError, ValueError):
                continue
            text = blueprint.extract(layout, (0, 0, layout["width"], layout["height"]))
            built = self.build(f"layout {layout['id']}\nbase none\n" + text)
            self.assertEqual(built.blocks, original.blocks, layout["id"])

    def test_extract_region_into_another_position(self):
        layout = resolve_layout("LAYOUT_JUBILIFE_CITY")
        original = Blockdata.for_layout(layout)
        text = blueprint.extract(layout, (10, 5, 6, 4)).replace("grid 10 5", "grid 0 0")
        built = self.build(f"layout {layout['id']}\nsize 6 4\nbase none\n" + text)
        self.assertEqual(built.blocks, original.crop(10, 5, 6, 4).blocks)

    def test_patterns_follow_map_coordinates(self):
        g = self.build(
            "layout LAYOUT_JUBILIFE_CITY\nsize 4 4\nbase none\n"
            "rect 1 1 3 3 [0x1D4/c1/e0 0x1D5/c1/e0; 0x1DC/c1/e0 0x1DD/c1/e0]\n"
        )
        mids = [[g.get(x, y) & consts().metatile_mask for x in range(4)] for y in range(4)]
        self.assertEqual(mids[1][1:], [0x1DD, 0x1DC, 0x1DD])
        self.assertEqual(mids[2][1:], [0x1D5, 0x1D4, 0x1D5])
        self.assertEqual(mids[0], [0, 0, 0, 0])

    def test_grid_spaces_and_short_rows_keep_blocks(self):
        g = self.build(
            "layout LAYOUT_JUBILIFE_CITY\nsize 3 2\nbase none\nfill 0x001/c0/e3\n"
            "legend # = 0x1D4/c1/e0\ngrid 0 0\n #\n#\nend\n"
        )
        c = consts()
        self.assertEqual([c.unpack(b)[0] for b in g.blocks], [0x001, 0x1D4, 0x001, 0x1D4, 0x001, 0x001])

    def test_hash_is_a_legend_key_not_a_comment(self):
        g = self.build("layout LAYOUT_JUBILIFE_CITY\nsize 1 1\nbase none\nlegend # = 0x00D  # tall grass\ngrid 0 0\n#\nend\n")
        self.assertEqual(g.get(0, 0) & consts().metatile_mask, 0x00D)

    def test_defaults_come_from_usage(self):
        layout = resolve_layout("LAYOUT_JUBILIFE_CITY")
        self.assertEqual(blueprint.default_attrs(0x1D4, layout), (1, 0))  # dense trees: impassable
        self.assertEqual(blueprint.default_attrs(0x001, layout), (0, 3))  # grass: walkable ground

    def test_errors_name_the_line(self):
        with self.assertRaisesRegex(blueprint.BlueprintError, r":3: no legend entry for 'z'"):
            self.build("layout LAYOUT_JUBILIFE_CITY\ngrid 0 0\nz\nend\n")
        with self.assertRaisesRegex(blueprint.BlueprintError, r":2: unknown metatile"):
            self.build("layout LAYOUT_JUBILIFE_CITY\nfill NotAMetatile\n")


class FinishedDrafts(unittest.TestCase):
    def test_route_fill_keeps_movement_and_style(self):
        import autotile
        import materials
        from compare import GROUP_OF, emerald_symbol
        from tileset import TilesetPair

        layout = resolve_layout("LAYOUT_ROUTE218")
        grid = [
            "##########",
            "##......##",
            "##..\"\"..##",
            "##......##",
            "##~~~~~~##",
            "##~~~~~~##",
        ]
        blocks, tally, _ = autotile.fill(grid, layout)
        tiles = TilesetPair.for_layout(layout)
        for y, row in enumerate(grid):
            for x, ch in enumerate(row):
                b = blocks.get(x, y)
                got = emerald_symbol(tiles, b)
                self.assertEqual(GROUP_OF[got], GROUP_OF[ch], (x, y, hex(b)))
                mats = materials.of(tiles, b & consts().metatile_mask)
                self.assertNotIn("building", mats, (x, y))
                self.assertIn(materials.family(mats, "tree"), (None, "dense"), (x, y))


    def test_fixed_blocks_are_kept_and_tiled_around(self):
        import autotile
        import buildings

        layout = resolve_layout("LAYOUT_SANDGEM_TOWN")
        house = next(p for p in buildings.library(layout) if p.name == "OLDALE_TOWN@4,4")
        fixed = {(2 + dx, 1 + dy): b for dx, dy, b in house.blocks()}
        grid = ["########", "#......#", "#......#", "#......#", "#......#", "#......#", "########"]
        grid = buildings.apply([buildings.Placement(house, 2, 1, fixed, [(3, 4)])], grid, layout)
        blocks, _, _ = autotile.fill(grid, layout, "town", fixed=fixed)
        for (x, y), b in fixed.items():
            self.assertEqual(blocks.get(x, y), b, (x, y))
        self.assertFalse(consts().unpack(blocks.get(3, 5))[1], "the doorstep stays walkable")


class Buildings(unittest.TestCase):
    def test_pieces_come_whole_from_the_originals(self):
        import buildings

        by_name = {p.name: p for p in buildings.pieces("gTileset_General")}
        house = by_name["OLDALE_TOWN@4,4"]
        self.assertEqual((house.w, house.h, house.doors, house.kind), (4, 4, ((1, 3),), "house"))
        self.assertTrue(all(b is not None for row in house.cells for b in row), "the roof is part of it")
        self.assertEqual(by_name["OLDALE_TOWN@5,13"].kind, "pokecenter")
        self.assertEqual(by_name["OLDALE_TOWN@13,3"].kind, "mart")
        # Two shops under one roof stay one piece, with both doors.
        self.assertEqual(by_name["LAVARIDGE_TOWN@11,12"].doors, ((1, 3), (5, 3)))

    def test_pieces_only_on_tilesets_that_draw_them(self):
        import buildings

        for layout_id in ("LAYOUT_SANDGEM_TOWN", "LAYOUT_JUBILIFE_CITY"):
            layout = resolve_layout(layout_id)
            tiles_ok = buildings.library(layout)
            self.assertTrue(any(p.kind == "pokecenter" for p in tiles_ok), layout_id)
            from tileset import TilesetPair

            pair_ = TilesetPair.for_layout(layout)
            for p in tiles_ok:
                for _, _, b in p.blocks():
                    self.assertTrue(pair_.exists(b & consts().metatile_mask), (layout_id, p.name))

    def test_cells_text_sets_only_its_cells(self):
        g = blueprint.Blueprint(
            "layout LAYOUT_JUBILIFE_CITY\nsize 3 2\nbase none\nfill 0x001/c0/e3\n"
            + blueprint.cells_text({(1, 0): consts().pack(0x1D4, 1, 0), (2, 1): consts().pack(0x00D, 0, 3)})
        ).build()
        self.assertEqual([b & consts().metatile_mask for b in g.blocks], [0x001, 0x1D4, 0x001, 0x001, 0x001, 0x00D])


class Checks(unittest.TestCase):
    def test_whole_repo_runs(self):
        findings = check.run()
        self.assertTrue(all(level in ("error", "warning") for level, _, _ in findings))

    def test_view_window_reaches_seven_blocks_sideways(self):
        a = {"width": 10, "height": 10}
        b = {"width": 20, "height": 10}
        open_a = Blockdata(10, 10)
        seen = set(check._visible_neighbour_blocks("right", 0, a, open_a, b))
        self.assertIn((6, 0), seen)
        self.assertNotIn((7, 0), seen)


@unittest.skipUnless((platinum.cache_dir() / "include/data/map_headers.h").exists(), "no cached pokeplatinum checkout")
class Platinum(unittest.TestCase):
    def test_jubilife_reference(self):
        ref = platinum.load("JUBILIFE_CITY")
        self.assertEqual((ref.width, ref.height), (64, 64))
        warp = next(w for w in ref.warps if w["dest_header_id"] == "MAP_HEADER_JUBILIFE_CITY_POKECENTER_1F")
        door = next(p for p in ref.props if p.name == "pokecenter_door" and int(p.y) == warp["y"])
        self.assertEqual(int(door.x), warp["x"])

    def test_buildings_and_their_doors(self):
        ref = platinum.load("SANDGEM_TOWN")
        found = {b.kind: b for b in ref.buildings()}
        self.assertEqual(found["pokecenter"].box, (15, 7, 5, 4))
        self.assertEqual(found["pokecenter"].doors, [(17, 10)])
        self.assertEqual(found["mart"].doors, [(27, 10)])
        self.assertEqual(found["lab"].doors, [(8, 10)])

    def test_placed_buildings_put_doors_on_platinums(self):
        import buildings
        from tileset import TilesetPair

        ref = platinum.load("SANDGEM_TOWN")
        layout = resolve_layout("LAYOUT_SANDGEM_TOWN")
        placed, missing = buildings.plan(ref, layout, ref.grid(), (0, 0, ref.width, ref.height))
        self.assertEqual(missing, [])
        tiles = TilesetPair.for_layout(layout)
        doors = set()
        for p in placed:
            self.assertEqual(p.piece.kind if p.target.kind in ("pokecenter", "mart") else p.target.kind, p.target.kind)
            for x, y in p.doors:
                self.assertIn("DOOR", behaviors().get(tiles.behavior(p.cells[(x, y)] & consts().metatile_mask), ""))
                doors.add((x, y))
        self.assertEqual(doors, {(w["x"], w["y"]) for w in ref.warps})


if __name__ == "__main__":
    unittest.main()
