"""Tests for the encounter analyser. Run with: python3 -m unittest discover tools/encounters

They read the repo's data but never write to it.
"""

from __future__ import annotations

import contextlib
import io
import re
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

import encounters as enc  # noqa: E402


class Tables(unittest.TestCase):
    def test_every_table_adds_up_to_100_percent(self):
        for map_id, m in enc.encounters().items():
            for header in m.headers:
                for method, table in header.items():
                    total = sum(p for p, _, _ in table.composition().values())
                    self.assertAlmostEqual(total, 100, delta=len(table.composition()), msg=f"{map_id} {method}")

    def test_fishing_is_split_by_rod(self):
        header = enc.encounters()["MAP_ROUTE204"].headers[0]
        self.assertIn("old_rod", header)
        self.assertIn("super_rod", header)
        self.assertNotIn("fishing_mons", header)

    def test_times_follow_the_number_of_tables(self):
        a, b, c, d = ({"land_mons": enc.Table("land_mons", 20, [])} for _ in range(4))
        four = enc.MapEncounters("X", [a, b, c, d]).by_time()
        self.assertEqual([four[t] for t in enc.TIMES], [a, b, c, d])
        two = enc.MapEncounters("X", [a, b]).by_time()
        self.assertEqual([two[t] for t in enc.TIMES], [a, a, b, b])
        three = enc.MapEncounters("X", [a, b, c]).by_time()
        self.assertEqual([three[t] for t in enc.TIMES], [a, a, a, a])

    def test_identical_times_are_grouped(self):
        slot = enc.Slot("SPECIES_ZUBAT", 100, 5, 7)
        same = [{"land_mons": enc.Table("land_mons", 10, [slot])} for _ in range(4)]
        self.assertEqual([times for times, _ in enc.MapEncounters("X", same).grouped("land_mons")], [enc.TIMES])


class Selection(unittest.TestCase):
    def test_sinnoh_maps_are_named_after_their_section(self):
        chosen = {m["id"] for m in enc.select_maps([], all_maps=False)}
        self.assertIn("MAP_ROUTE201", chosen)
        self.assertIn("MAP_RAVAGED_PATH", chosen)
        # These share a Sinnoh section without being that place.
        self.assertNotIn("MAP_ROUTE105", chosen)
        self.assertNotIn("MAP_RUSTURF_TUNNEL", chosen)

    def test_names_can_span_words_and_be_shortened(self):
        self.assertEqual([enc.find_map(n)["id"] for n in enc.join_names(["ravaged", "path", "route201"])],
                         ["MAP_RAVAGED_PATH", "MAP_ROUTE201"])
        self.assertEqual(enc.find_map("twinleaf")["id"], "MAP_TWINLEAF_TOWN")
        self.assertEqual(enc.resolve_species(["mr", "mime", "shinx"]), ["SPECIES_MR_MIME", "SPECIES_SHINX"])


class Species(unittest.TestCase):
    def test_types_and_families(self):
        self.assertEqual(enc.species("SPECIES_SHINX").types, ("Electric",))
        self.assertEqual(enc.species("SPECIES_ODDISH").types, ("Grass", "Poison"))
        self.assertEqual(enc.family("SPECIES_LUXRAY"), "SPECIES_SHINX")
        self.assertEqual(enc.family("SPECIES_SHINX"), "SPECIES_SHINX")

    def test_generations_follow_the_national_dex(self):
        self.assertEqual(enc.generation("SPECIES_BULBASAUR"), 1)
        self.assertEqual(enc.generation("SPECIES_HOOTHOOT"), 2)
        self.assertEqual(enc.generation("SPECIES_MUDKIP"), 3)
        self.assertEqual(enc.generation("SPECIES_SHINX"), 4)

    def test_starters_are_read(self):
        self.assertEqual(len(enc.starters()), 3)


class Trainers(unittest.TestCase):
    def test_only_reachable_battles_count(self):
        # Route 202's scripts still hold Route 103's battles, but only its
        # placed trainers can start one.
        info = enc.maps()["MAP_ROUTE202"]
        found = {t.const for t in enc.map_trainers(info)}
        placed = [e for e in info["object_events"] if e.get("trainer_type") == "TRAINER_TYPE_NORMAL"]
        in_scripts = set(re.findall(r"trainerbattle\w*\s+(?:\w+\s*,\s*)?(TRAINER_\w+)",
                                    (enc.REPO / "data/maps/Route202/scripts.inc").read_text()))
        self.assertEqual(len(found), len(placed))
        self.assertLess(found, in_scripts)

    def test_parties_are_read(self):
        party = enc.trainers()["TRAINER_CALVIN_1"].party
        self.assertTrue(party)
        self.assertTrue(all(s.startswith("SPECIES_") and lv > 0 for s, lv in party))


class Commands(unittest.TestCase):
    def run_command(self, *argv: str) -> tuple[int, str]:
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            code = enc.main(list(argv))
        return code, out.getvalue()

    def test_every_command_runs(self):
        for argv in (["summary"], ["overview"], ["show", "route201", "--types"], ["species", "starly"], ["check", "--max-species", "0"]):
            code, text = self.run_command(*argv)
            self.assertIn(code, (0, 1), argv)
            self.assertTrue(text.strip(), argv)


if __name__ == "__main__":
    unittest.main()
