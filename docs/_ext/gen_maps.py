"""Sphinx extension: generate a wild encounter page for each map with
time-of-day encounter tables.

Pages land in ``docs/map/encounters/`` at build time so they cannot drift from
``src/data/wild_encounters.json``. The directory is gitignored; edit the data,
not the output.
"""

from __future__ import annotations

import json
import re
import shutil
from pathlib import Path

from gen_pokedex import md_escape, parse_species_names, read

HERE = Path(__file__).resolve().parent
DOCS = HERE.parent
REPO = HERE.parents[1]
OUT = DOCS / "map" / "encounters"

DATA = REPO / "src" / "data"
MAPS = REPO / "data" / "maps"

PERIODS = ["Morning", "Day", "Evening", "Night"]

# Which table each period uses, by how many tables the map has. Mirrors
# GetTimeBasedWildMonHeaderId in src/wild_encounter.c.
PERIOD_TABLES = {
    2: [0, 0, 1, 1],
    4: [0, 1, 2, 3],
}

# Altering Cave's tables are picked by a variable, not by time of day.
EXCLUDED_MAPS = {"MAP_ALTERING_CAVE"}

# Encounters that don't come from the tables, added to the map's page.
NOTES = {
    "MAP_ROUTE119": (
        "Feebas isn't in the tables. It can be fished with any rod on 6 of the "
        "route's water tiles, which change with the Dewford trend; on those tiles, "
        "half of all bites are a level 20–25 Feebas. See `CheckFeebas` in "
        "`src/wild_encounter.c`."
    ),
}

# Outdoor map types whose pages are titled with the region map section's name.
NAMED_BY_SECTION = {"MAP_TYPE_TOWN", "MAP_TYPE_CITY", "MAP_TYPE_ROUTE", "MAP_TYPE_OCEAN_ROUTE"}

# Page sections on the index, in order, keyed by map type.
SECTIONS = [
    ("Towns and cities", {"MAP_TYPE_TOWN", "MAP_TYPE_CITY"}),
    ("Routes", {"MAP_TYPE_ROUTE"}),
    ("Sea routes", {"MAP_TYPE_OCEAN_ROUTE"}),
    ("Underwater", {"MAP_TYPE_UNDERWATER"}),
]


def period_hours() -> list[str]:
    """Each period's hours, e.g. "06:00–09:59", from time_of_day.h."""
    text = read(REPO / "include" / "constants" / "time_of_day.h")
    starts = []
    for period in PERIODS:
        m = re.search(rf"#define {period.upper()}_HOUR_BEGIN\s+(\d+)", text)
        starts.append(int(m.group(1)))
    hours = []
    for i, start in enumerate(starts):
        end = (starts[(i + 1) % len(starts)] - 1) % 24
        hours.append(f"{start:02d}:00–{end:02d}:59")
    return hours


def encounter_types(fields: list[dict]) -> list[tuple[str, str, list[int]]]:
    """(JSON key, rod group or "", slot rates) for each way to meet a Pokémon.

    Fishing is split by rod, since each rod draws from its own slots.
    """
    out = []
    for field in fields:
        rates = field["encounter_rates"]
        if "groups" in field:
            for group, slots in field["groups"].items():
                out.append((field["type"], group, [rates[i] if i in slots else 0 for i in range(len(rates))]))
        else:
            out.append((field["type"], "", rates))
    return out


def type_label(key: str, group: str, map_type: str) -> str:
    if key == "land_mons":
        return "Cave" if map_type == "MAP_TYPE_UNDERGROUND" else "Grass"
    if key == "water_mons":
        # Underwater maps use their water table for Pokémon met in seaweed.
        return "Seaweed" if map_type == "MAP_TYPE_UNDERWATER" else "Surf"
    if key == "rock_smash_mons":
        return "Rock Smash"
    if key == "fishing_mons":
        return group.replace("_", " ").title()
    return key


def map_display_name(name: str) -> str:
    """Route101 -> Route 101; MtPyre_Summit -> Mt. Pyre (Summit);
    Underwater_Route124 -> Route 124 (Underwater)."""

    def words(part: str) -> str:
        part = re.sub(r"(?<=[a-z])(?=[A-Z0-9])", " ", part)
        return re.sub(r"\bMt\b", "Mt.", part)

    parts = name.split("_")
    if parts[0] == "Underwater" and len(parts) == 2:
        return f"{words(parts[1])} (Underwater)"
    if len(parts) == 1:
        return words(parts[0])
    return f"{words(parts[0])} ({' '.join(words(p) for p in parts[1:])})"


def load_section_names() -> dict[str, str]:
    """Region map section id -> the name the game shows, e.g. "Twinleaf Town"."""
    data = json.loads(read(DATA / "region_map" / "region_map_sections.json"))
    return {s["id"]: title_case(s["name"]) for s in data["map_sections"] if "name" in s}


def title_case(name: str) -> str:
    """ROUTE 201 -> Route 201; MT. PYRE -> Mt. Pyre."""
    return " ".join(word[:1] + word[1:].lower() for word in name.split())


def map_title(info: dict, section_names: dict[str, str], section_counts: dict[str, int]) -> str:
    """The in-game name for outdoor maps that are the only outdoor map in their
    region map section, so repurposed maps (Littleroot Town is Twinleaf Town,
    Route 101 is Route 201) are listed under their new names. Maps sharing a
    section, like the Safari Zone areas, and underwater maps (whose sections
    are all just "Underwater") keep a name built from the map's own name."""
    section = info.get("region_map_section")
    if (
        info["map_type"] in NAMED_BY_SECTION
        and section_counts.get(section) == 1
        and section in section_names
    ):
        return section_names[section]
    return map_display_name(info["name"])


def natural_key(text: str):
    return [int(t) if t.isdigit() else t for t in re.split(r"(\d+)", text)]


def percent(value: float) -> str:
    return f"{value:.0f}%" if value == int(value) else f"{value:.1f}%"


def build_rows(tables: list[dict], period_tables: list[int], fields, map_type, names):
    """One row per Pokémon per encounter type, with its slots combined."""
    rows = []
    for key, group, rates in encounter_types(fields):
        total = sum(rates)
        by_species: dict[str, dict] = {}
        for period, table_index in enumerate(period_tables):
            table = tables[table_index].get(key)
            if not table:
                continue
            for slot, mon in enumerate(table["mons"]):
                if not rates[slot]:
                    continue  # Slot belongs to another rod
                row = by_species.setdefault(
                    mon["species"],
                    {"min": mon["min_level"], "max": mon["max_level"], "chance": [0] * len(PERIODS)},
                )
                row["min"] = min(row["min"], mon["min_level"])
                row["max"] = max(row["max"], mon["max_level"])
                row["chance"][period] += rates[slot] * 100 / total
        label = type_label(key, group, map_type)
        ordered = sorted(
            by_species.items(),
            key=lambda item: (-sum(item[1]["chance"]), names.get(item[0], item[0])),
        )
        for species, row in ordered:
            rows.append((label, names.get(species, species), row))
    return rows


def render_map_page(title: str, map_const: str, rows, hours: list[str], labels: list[str]) -> str:
    lines = [
        f"# {title}",
        "",
        f"Wild encounters on `{map_const}`, from the "
        f"{', '.join(f'`{label}`' for label in labels)} tables in "
        "`src/data/wild_encounters.json`.",
        "",
        "Each chance is the odds that an encounter of that type is that Pokémon, "
        "with all of its encounter slots added together. Levels are the lowest and "
        "highest across all of its slots and times of day.",
        "",
        " · ".join(f"**{p}** {h}" for p, h in zip(PERIODS, hours)),
        "",
        "| Type | Pokémon | Min Lv. | Max Lv. | " + " | ".join(PERIODS) + " |",
        "| --- | --- | --- | --- | " + " | ".join("---" for _p in PERIODS) + " |",
    ]
    for label, name, row in rows:
        chances = " | ".join(percent(c) if c else "—" for c in row["chance"])
        lines.append(f"| {label} | {md_escape(name)} | {row['min']} | {row['max']} | {chances} |")
    lines.append("")
    if map_const in NOTES:
        lines += ["```{note}", NOTES[map_const], "```", ""]
    return "\n".join(lines)


def load_maps() -> dict[str, dict]:
    maps = {}
    for path in MAPS.glob("*/map.json"):
        data = json.loads(read(path))
        maps[data["id"]] = data
    return maps


def generate(app=None) -> int:
    encounters = json.loads(read(DATA / "wild_encounters.json"))
    group = next(g for g in encounters["wild_encounter_groups"] if g["label"] == "gWildMonHeaders")
    fields = group["fields"]
    maps = load_maps()
    section_names = load_section_names()
    section_counts: dict[str, int] = {}
    for info in maps.values():
        if info["map_type"] in NAMED_BY_SECTION:
            section = info.get("region_map_section")
            section_counts[section] = section_counts.get(section, 0) + 1
    names = parse_species_names()
    hours = period_hours()

    tables_by_map: dict[str, list[dict]] = {}
    for table in group["encounters"]:
        tables_by_map.setdefault(table["map"], []).append(table)

    if OUT.exists():
        shutil.rmtree(OUT)
    OUT.mkdir(parents=True)

    pages = []  # (section, title, filename)
    used: set[str] = set()
    for map_const, tables in tables_by_map.items():
        if map_const in EXCLUDED_MAPS or len(tables) not in PERIOD_TABLES:
            continue
        info = maps[map_const]
        title = map_title(info, section_names, section_counts)
        # Pages named after the game's name for the map use it for the file too
        # (route201.md), others the map's own name (mtpyre_summit.md)
        if title == map_display_name(info["name"]):
            fname = info["name"].lower()
        else:
            fname = re.sub(r"[^a-z0-9]", "", title.lower())
        if fname in used:
            raise ValueError(f"two maps would both generate {fname}.md")
        used.add(fname)
        rows = build_rows(tables, PERIOD_TABLES[len(tables)], fields, info["map_type"], names)
        labels = [t["base_label"] for t in tables]
        (OUT / f"{fname}.md").write_text(
            render_map_page(title, map_const, rows, hours, labels), encoding="utf-8"
        )
        section = next((s for s, types in SECTIONS if info["map_type"] in types), "Other")
        pages.append((section, title, fname))

    index = [
        "# Wild encounters",
        "",
        f"{len(pages)} maps have separate wild encounter tables for each time "
        "of day, generated from `src/data/wild_encounters.json` at build time. "
        "Caves and building interiors use one table all day and aren't listed. "
        "See {doc}`../../features/time-of-day`.",
        "",
        "| Period | Hours |",
        "| --- | --- |",
    ]
    index += [f"| {p} | {h} |" for p, h in zip(PERIODS, hours)]
    index.append("")

    toctree = []
    for section in [s for s, _t in SECTIONS] + ["Other"]:
        entries = sorted((p for p in pages if p[0] == section), key=lambda p: natural_key(p[1]))
        if not entries:
            continue
        index += [f"## {section}", ""]
        index += [f"- [{title}]({fname}.md)" for _s, title, fname in entries]
        index.append("")
        toctree += [fname for _s, _t, fname in entries]

    index += ["```{toctree}", ":maxdepth: 1", ":hidden:", ""] + toctree + ["```", ""]
    (OUT / "index.md").write_text("\n".join(index), encoding="utf-8")

    return len(pages)


def on_builder_inited(app):
    count = generate(app)
    from sphinx.util import logging as sphinx_logging

    sphinx_logging.getLogger(__name__).info(f"[gen_maps] generated {count} map encounter pages")


def setup(app):
    app.connect("builder-inited", on_builder_inited)
    return {"version": "1.0", "parallel_read_safe": True}


if __name__ == "__main__":
    print(f"generated {generate()} map pages into {OUT}")
