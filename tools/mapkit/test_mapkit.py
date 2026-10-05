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

    def test_cave_fill_uses_cave_blocks_and_fills_deep_rock(self):
        import autotile
        from compare import GROUP_OF, emerald_symbol
        from tileset import TilesetPair

        layout = resolve_layout("LAYOUT_RAVAGED_PATH")
        grid = ["#" * 12] * 3 + ["###......###", "###..~~..###", "###......###"] + ["#" * 12] * 3
        blocks, tally, _ = autotile.fill(grid, layout, "cave", water=None)
        tiles = TilesetPair.for_layout(layout)
        c = consts()
        for y, row in enumerate(grid):
            for x, ch in enumerate(row):
                b = blocks.get(x, y)
                self.assertEqual(GROUP_OF[emerald_symbol(tiles, b)], GROUP_OF[ch], (x, y, hex(b)))
                if ch != "~":
                    self.assertGreaterEqual(b & c.metatile_mask, c.metatiles_in_primary,
                                            f"{hex(b)} at {(x, y)} is an outdoor block")
        deep = {blocks.get(x, y) for x, y in ((0, 0), (11, 0), (0, 8), (11, 8))}
        self.assertEqual(len(deep), 1, "deep rock is one filling block")
        self.assertGreater(tally["deep"], 0)


class Access(unittest.TestCase):
    """Finishing sprites may move a walkable/solid border, never change where the player can go."""

    @staticmethod
    def kinds(rows):
        import autotile

        return [autotile._kind(ch) for row in rows for ch in row]

    def test_a_new_shore_is_a_change(self):
        import autotile

        before = ["....#~~", "....#~~", "....#~~"]
        after = ["....#~~", ".....~~", "....#~~"]  # the tree between field and water opened
        diff = autotile.access_differences(self.kinds(before), self.kinds(after), 7, 3, set())
        self.assertTrue(any(line.startswith("now touch") for line in diff), diff)

    def test_a_way_around_a_cut_tree_is_a_change(self):
        import autotile

        before = ["#####", "..@..", "#####"]
        pocket = ["#.###", "..@..", "#####"]  # a dead end off the west side: nothing new to reach
        self.assertTrue(autotile._same_access(self.kinds(before), self.kinds(pocket), 5, 3, {1 * 5 + 2}))
        behind = ["#####", "..@..", "##.##"]  # a spot only reachable through the tree: new
        self.assertFalse(autotile._same_access(self.kinds(before), self.kinds(behind), 5, 3, {1 * 5 + 2}))
        bypass = ["#.#.#", "..@..", "#...#"]
        bypass_before = ["#.#.#", "..@..", "#####"]
        self.assertFalse(autotile._same_access(self.kinds(bypass_before), self.kinds(bypass), 5, 3, {1 * 5 + 2}))

    def test_an_unreachable_pocket_is_not_a_change(self):
        import autotile

        before = ["#####", "#####", "....."]
        after = ["#.###", "#####", "....."]  # a sealed walkable tile, like a roof's top row
        self.assertEqual(autotile.access_differences(self.kinds(before), self.kinds(after), 5, 3, set()), [])

    def test_finished_blocks_move_as_their_classes_say(self):
        import autotile
        from compare import GROUP_OF, emerald_symbol
        from tileset import TilesetPair

        layout = resolve_layout("LAYOUT_ROUTE218")
        grid = ["##########", "#........#", "#.#......#", "#........#", "##########"]
        blocks, _, _ = autotile.fill(grid, layout)
        tiles = TilesetPair.for_layout(layout)
        for y, row in enumerate(grid):
            for x, ch in enumerate(row):
                self.assertEqual(GROUP_OF[emerald_symbol(tiles, blocks.get(x, y))], GROUP_OF[ch], (x, y))

    def test_flexible_cells_keep_access(self):
        import autotile
        from compare import emerald_symbol
        from tileset import TilesetPair

        layout = resolve_layout("LAYOUT_ROUTE218")
        # A tree line one block thick between a field and the water, and a lone tree.
        grid = ["##########", "#.....#~~#", "#.#...#~~#", "#.....#~~#", "##########"]
        flexible = {(x, y) for y in range(1, 4) for x in range(1, 9) if grid[y][x] in "#."}
        blocks, tally, _ = autotile.fill(grid, layout, flexible=flexible)
        tiles = TilesetPair.for_layout(layout)
        after = [autotile._kind(emerald_symbol(tiles, blocks.get(x, y))) for y in range(5) for x in range(10)]
        self.assertEqual(autotile.access_differences(self.kinds(grid), after, 10, 5, set()), [])


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


class Parts(unittest.TestCase):
    def test_a_wider_house_with_its_door_moved(self):
        import buildings
        import parts
        from tileset import TilesetPair

        layout = resolve_layout("LAYOUT_SANDGEM_TOWN")
        seed = next(p for p in buildings.library(layout) if p.name == "PETALBURG_CITY@19,21")
        made = parts.make(seed, layout, 7, 4, (3,))
        self.assertIsNotNone(made)
        piece, _ = made
        self.assertEqual((piece.w, piece.h, piece.doors), (7, 4, ((3, 3),)))
        tiles = TilesetPair.for_layout(layout)
        j = parts.joins(layout["primary_tileset"], layout["secondary_tileset"])
        mid = consts().metatile_mask
        for dx, dy, b in piece.blocks():
            is_door = "DOOR" in behaviors().get(tiles.behavior(b & mid), "")
            self.assertEqual(is_door, (dx, dy) == (3, 3), (dx, dy))
            if dx + 1 < piece.w and piece.cells[dy][dx + 1] is not None:
                self.assertLessEqual(j.h(b & mid, piece.cells[dy][dx + 1] & mid), parts.JOIN_MAX)
            if dy + 1 < piece.h and piece.cells[dy + 1][dx] is not None:
                self.assertLessEqual(j.v(b & mid, piece.cells[dy + 1][dx] & mid), parts.JOIN_MAX)

    def test_signs_and_emblems_are_never_repeated(self):
        import buildings
        import parts

        layout = resolve_layout("LAYOUT_SANDGEM_TOWN")
        centre = next(p for p in buildings.library(layout) if p.name == "OLDALE_TOWN@5,13")
        self.assertIsNone(parts.make(centre, layout, 6, 4, (2,)))

    def test_sequences_keep_ends_and_cap_runs(self):
        import parts

        # Four inputs to six: the middle two each repeat once, ends stay ends.
        cost, seq = parts._sequence(6, 4, lambda a, b: 0.0 if b == a + 1 else 1.0, max_run=2, forward=True)
        self.assertEqual(seq, [0, 1, 1, 2, 2, 3])
        self.assertEqual(cost, 2.0)
        # Three inputs can't make six when one middle input may only run twice.
        self.assertEqual(parts._sequence(6, 3, lambda a, b: 0.0, max_run=2, forward=True), (float("inf"), None))


class Ground(unittest.TestCase):
    def test_material_kinds_and_path_looks(self):
        import ground

        self.assertEqual(ground.base_name("nsandp_lm2"), "nsandp")
        self.assertEqual(ground.base_name("lakep.1_pl"), "lakep")
        self.assertEqual(ground.kind("nsand_lm2"), "path")
        self.assertEqual(ground.kind("hage"), "path")
        self.assertEqual(ground.kind("c1_r1_ud"), "path")
        self.assertEqual(ground.kind("nhana_lm2"), "flowers")
        self.assertEqual(ground.kind("conttree_b_lm2"), "tree")
        self.assertEqual(ground.kind("c1_g1"), "unknown")
        self.assertEqual(ground.path_look("nsand_lm2"), "sandy")
        self.assertEqual(ground.path_look("c4_road_u"), "stone")
        self.assertIsNone(ground.path_look("ngrass"))

    def test_emerald_paths_are_their_own_class_with_a_look(self):
        import materials
        from compare import GROUP_OF, emerald_symbol
        from tileset import TilesetPair

        tiles = TilesetPair.for_layout(resolve_layout("LAYOUT_TWINLEAF_TOWN"))
        c = consts()
        self.assertEqual(emerald_symbol(tiles, c.pack(0x121, 0, 3)), "p", "Littleroot's sand pit")
        self.assertEqual(emerald_symbol(tiles, c.pack(0x001, 0, 3)), ".")
        self.assertEqual(emerald_symbol(tiles, c.pack(0x124, 0, 3)), ":", "beach sand stays sand")
        self.assertEqual(GROUP_OF["p"], GROUP_OF["."])
        self.assertEqual(materials.family(materials.of(tiles, 0x121), "path"), "sandy")
        self.assertEqual(materials.family(materials.of(tiles, 0x000), "path"), "stone")

    def test_path_cells_draw_as_paths_of_one_look(self):
        import autotile
        import materials
        from compare import GROUP_OF, emerald_symbol
        from tileset import TilesetPair

        layout = resolve_layout("LAYOUT_TWINLEAF_TOWN")
        grid = ["##########", "#........#", "#.pppppp.#", "#.pppppp.#", "#.pppppp.#", "#........#", "##########"]
        blocks, _, _ = autotile.fill(grid, layout, "route", path="sandy")
        tiles = TilesetPair.for_layout(layout)
        for y, row in enumerate(grid):
            for x, ch in enumerate(row):
                b = blocks.get(x, y)
                self.assertEqual(GROUP_OF[emerald_symbol(tiles, b)], GROUP_OF[ch], (x, y))
                if ch == "p":
                    mats = materials.of(tiles, b & consts().metatile_mask)
                    self.assertIn("path", mats, (x, y, hex(b)))
                    self.assertEqual(materials.family(mats, "path"), "sandy", (x, y, hex(b)))


    def test_one_tile_paths_use_the_middle_block(self):
        import autotile

        layout = resolve_layout("LAYOUT_TWINLEAF_TOWN")
        m = autotile.model(layout["primary_tileset"], layout["secondary_tileset"])
        centre = m.blocks[autotile.path_centre(m, "sandy")]
        self.assertEqual(centre, 0x121, "Littleroot's sand pit centre")
        # A wide path with a one-tile spur west (row 3) and a single worn tile below a door (5, 6).
        grid = ["##########", "#........#", "#....ppp.#", "#ppppppp.#", "#....ppp.#", "#........#", "#....p...#",
                "#........#", "##########"]
        blocks, tally, _ = autotile.fill(grid, layout, "route", path="sandy")
        mid = consts().metatile_mask
        for x in (1, 2, 3, 4):
            self.assertEqual(blocks.get(x, 3) & mid, centre, (x, 3))
        self.assertEqual(blocks.get(5, 6) & mid, centre)
        self.assertEqual(tally["thin_path"], 5)


    def test_snow_is_the_white_sand_tile(self):
        import autotile
        import materials
        from compare import emerald_symbol
        from tileset import TilesetPair

        layout = resolve_layout("LAYOUT_TWINLEAF_TOWN")
        tiles = TilesetPair.for_layout(layout)
        c = consts()
        self.assertIn("snow", materials.of(tiles, 0x0B5))
        self.assertEqual(emerald_symbol(tiles, c.pack(0x0B5, 0, 3)), "s")
        self.assertEqual(behaviors().get(tiles.behavior(0x0B5)), "MB_SAND", "footprints, like sand")
        # Its pixels are the sand pit's with the sand shades swapped for whites.
        sand, snow = tiles.draw(0x121).convert("RGB"), tiles.draw(0x0B5).convert("RGB")
        self.assertEqual(sand.size, snow.size)
        self.assertTrue(all(min(p) >= 180 for p in snow.getdata()), "every pixel is white or a pale grey")
        grid = ["########", "#......#", "#.ssss.#", "#.ssss.#", "#......#", "########"]
        blocks, tally, _ = autotile.fill(grid, layout, "route")
        for x in range(2, 6):
            for y in (2, 3):
                self.assertEqual(blocks.get(x, y) & c.metatile_mask, 0x0B5, (x, y))
        self.assertEqual(tally["snow"], 8)


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

    def test_unreachable_void_counts_as_solid(self):
        ref = platinum.load("RAVAGED_PATH")
        self.assertEqual(ref.symbol(0, 0), ".", "Platinum stores the void as floor")
        self.assertGreater(ref.solidify_unreachable(), 0)
        self.assertEqual(ref.symbol(0, 0), "#")
        for w in ref.warps:
            self.assertNotEqual(ref.symbol(w["x"], w["y"]), "#", w)

    def test_ground_model_lines_up_with_the_tiles(self):
        import ground

        ref = platinum.load("TWINLEAF_TOWN")
        water = [(x, y) for y in range(ref.height) for x in range(ref.width) if ref.symbol(x, y) == "~"]
        self.assertGreater(len(water), 20)
        agree = sum(ref.ground_kind(x, y) == "water" for x, y in water)
        self.assertGreater(agree, 0.9 * len(water), "the model's lake sits on the water tiles")
        # The road south from Route 201 is painted as sand.
        self.assertEqual([ref.symbol(x, 5) for x in range(14, 18)], ["p"] * 4)
        self.assertEqual(ground.path_look(ref.ground[5][15]), "sandy")

    def test_maps_side_by_side_join_into_one_reference(self):
        route = platinum.load("ROUTE_201")
        joined = platinum.load("ROUTE_201+VERITY_LAKEFRONT")
        self.assertEqual(joined.header, "MAP_HEADER_ROUTE_201+MAP_HEADER_VERITY_LAKEFRONT")
        dx, dy = route.origin[0] - joined.origin[0], route.origin[1] - joined.origin[1]
        self.assertEqual(joined.symbol(dx + 20, dy + 21), route.symbol(20, 21))
        # Both maps' events, in the joined map's coordinates.
        self.assertTrue(any(w["dest_header_id"].startswith("MAP_HEADER_LAKE_VERITY") for w in joined.warps))
        barry = next(o for o in route.objects if o["graphics_id"] == "OBJ_EVENT_GFX_BARRY")
        self.assertIn((barry["x"] + dx, barry["y"] + dy), {(o["x"], o["y"]) for o in joined.objects})
        self.assertNotIn("left", joined.neighbours, "the lakefront is part of it, not a neighbour")

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
        # The lab is made from parts to Platinum's footprint exactly.
        lab = next(p for p in placed if p.target.kind == "lab")
        self.assertEqual((lab.piece.w, lab.piece.h), lab.target.box[2:])
        self.assertTrue(lab.piece.made)
        originals, _ = buildings.plan(ref, layout, ref.grid(), (0, 0, ref.width, ref.height), make_new=False)
        self.assertFalse(any(p.piece.made for p in originals))


if __name__ == "__main__":
    unittest.main()
