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
from project import Blockdata, consts, layouts, resolve_layout  # noqa: E402


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


if __name__ == "__main__":
    unittest.main()
