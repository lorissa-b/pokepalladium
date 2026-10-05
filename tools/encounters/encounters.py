#!/usr/bin/env python3
"""Analyse which Pokémon each map offers: wild encounters and trainers.

    tools/encounters/encounters.py summary              # the Sinnoh maps at a glance
    tools/encounters/encounters.py show route201 ravaged path
    tools/encounters/encounters.py species shinx pachirisu
    tools/encounters/encounters.py check --max-species 5

Maps are named as mapkit names them: MAP_* id, directory or a loose spelling.
With no maps given, the commands look at the converted Sinnoh maps: those in a
Sinnoh region map section and named after it (so Route 105, which only shares
Route 218's section, is left out). --all looks at every map with encounters.

Output is Markdown: headings, tables and lists. Rates are the percentages
produced by the slot weights in wild_encounters.json.
Times of day follow GetTimeBasedWildMonHeaderId: four tables are morning, day,
evening and night; two are morning+day and evening+night; any other number
means the first table is used all day.
"""

from __future__ import annotations

import argparse
import json
import re
import signal
import sys
from collections import defaultdict
from dataclasses import dataclass, field
from functools import lru_cache
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "mapkit"))

from project import REPO, maps, read, resolve_map  # noqa: E402

ENCOUNTERS_JSON = "src/data/wild_encounters.json"
TIMES = ["Morning", "Day", "Evening", "Night"]
TIME_ABBR = {"Morning": "Morn", "Day": "Day", "Evening": "Eve", "Night": "Night"}
METHOD_NAMES = {
    "land_mons": "Grass",
    "water_mons": "Surf",
    "rock_smash_mons": "Rock Smash",
    "old_rod": "Old Rod",
    "good_rod": "Good Rod",
    "super_rod": "Super Rod",
}
METHOD_ORDER = list(METHOD_NAMES)

# Region map sections (without MAPSEC_) that are Sinnoh places.
SINNOH_PLACES = {
    "TWINLEAF_TOWN", "SANDGEM_TOWN", "JUBILIFE_CITY", "OREBURGH_CITY", "FLOAROMA_TOWN",
    "ETERNA_CITY", "HEARTHOME_CITY", "SOLACEON_TOWN", "VEILSTONE_CITY", "PASTORIA_CITY",
    "CELESTIC_TOWN", "CANALAVE_CITY", "SNOWPOINT_CITY", "SUNYSHORE_CITY", "POKEMON_LEAGUE",
    "FIGHT_AREA", "SURVIVAL_AREA", "RESORT_AREA", "LAKE_VERITY", "LAKE_VALOR", "LAKE_ACUITY",
    "VERITY_LAKEFRONT", "VALOR_LAKEFRONT", "ACUITY_LAKEFRONT", "RAVAGED_PATH", "OREBURGH_GATE",
    "OREBURGH_MINE", "VALLEY_WINDWORKS", "ETERNA_FOREST", "FUEGO_IRONWORKS", "MT_CORONET",
    "SPEAR_PILLAR", "GREAT_MARSH", "SOLACEON_RUINS", "VICTORY_ROAD_SINNOH", "WAYWARD_CAVE",
    "LOST_TOWER", "IRON_ISLAND", "OLD_CHATEAU", "TROPHY_GARDEN", "STARK_MOUNTAIN",
    "SNOWPOINT_TEMPLE", "SENDOFF_SPRING", "TURNBACK_CAVE", "FLOWER_PARADISE", "SNOWPOINT_LAKE",
    "FULLMOON_ISLAND", "NEWMOON_ISLAND", "DISTORTION_WORLD", "MANIAC_TUNNEL",
}


def squash(name: str) -> str:
    return re.sub(r"[^a-z0-9]", "", name.lower())


def is_sinnoh_map(info: dict) -> bool:
    section = info.get("region_map_section", "").removeprefix("MAPSEC_")
    sinnoh = section in SINNOH_PLACES or re.fullmatch(r"ROUTE_2[0-3]\d", section)
    return bool(sinnoh) and squash(info["name"]) == squash(section)


# ---------------------------------------------------------------- species


@dataclass
class Species:
    const: str
    name: str
    types: tuple[str, ...]


@lru_cache(maxsize=None)
def species_table() -> dict[str, Species]:
    names = dict(re.findall(r"\[(SPECIES_\w+)\]\s*=\s*_\(\"(.*?)\"\)", read("src/data/text/species_names.h")))
    out = {}
    for const, body in re.findall(r"\[(SPECIES_\w+)\]\s*=\s*\{(.*?)\n    \}", read("src/data/pokemon/species_info.h"), re.S):
        m = re.search(r"\.types\s*=\s*\{\s*TYPE_(\w+)\s*,\s*TYPE_(\w+)", body)
        types = tuple(dict.fromkeys(t.title() for t in m.groups())) if m else ()
        out[const] = Species(const, names.get(const, const[8:]).title(), types)
    return out


def species(const: str) -> Species:
    return species_table().get(const) or Species(const, const.removeprefix("SPECIES_").title(), ())


def find_species(name: str) -> str | None:
    key = squash(name.removeprefix("SPECIES_"))
    for const, info in species_table().items():
        if key in (squash(const[8:]), squash(info.name)):
            return const
    return None


def resolve_species(words: list[str]) -> list[str]:
    """Species from the command line, letting names span words (`mr mime`)."""
    out, i = [], 0
    while i < len(words):
        for j in range(len(words), i, -1):
            const = find_species(" ".join(words[i:j]))
            if const:
                out.append(const)
                i = j
                break
        else:
            raise SystemExit(f"error: no species called {words[i]!r}")
    return out


@lru_cache(maxsize=None)
def families() -> dict[str, str]:
    """Species -> the first species of its evolution family."""
    parent = {}
    for base, evos in re.findall(r"\[(SPECIES_\w+)\]\s*=\s*\{(\{.*?\})\}", read("src/data/pokemon/evolution.h")):
        for target in re.findall(r"(SPECIES_\w+)\s*\}", evos):
            parent.setdefault(target, base)

    def root(s: str) -> str:
        seen = set()
        while s in parent and s not in seen:
            seen.add(s)
            s = parent[s]
        return s

    return {s: root(s) for s in set(parent) | set(parent.values())}


def family(const: str) -> str:
    return families().get(const, const)


@lru_cache(maxsize=None)
def starters() -> set[str]:
    m = re.search(r"sStarterMon\[\w*\]\s*=\s*\{(.*?)\}", read("src/starter_choose.c"), re.S)
    return set(re.findall(r"SPECIES_\w+", m.group(1))) if m else set()


# ---------------------------------------------------------------- encounters


@dataclass
class Slot:
    species: str
    rate: int
    min_level: int
    max_level: int


@dataclass
class Table:
    """One method's slots in one encounter header."""

    method: str
    encounter_rate: int
    slots: list[Slot]

    def composition(self) -> dict[str, tuple[int, int, int]]:
        """Species -> (percent, min level, max level), most common first."""
        agg: dict[str, list[int]] = {}
        for s in self.slots:
            a = agg.setdefault(s.species, [0, s.min_level, s.max_level])
            a[0] += s.rate
            a[1] = min(a[1], s.min_level)
            a[2] = max(a[2], s.max_level)
        total = sum(s.rate for s in self.slots) or 1
        out = {sp: (round(v[0] * 100 / total), v[1], v[2]) for sp, v in agg.items()}
        return dict(sorted(out.items(), key=lambda kv: -kv[1][0]))

    def key(self) -> tuple:
        return (self.encounter_rate, tuple(sorted(self.composition().items())))


@dataclass
class MapEncounters:
    map_id: str
    headers: list[dict[str, Table]] = field(default_factory=list)

    def by_time(self) -> dict[str, dict[str, Table]]:
        n = len(self.headers)
        if n == 4:
            return dict(zip(TIMES, self.headers))
        if n == 2:
            return {t: self.headers[0 if t in ("Morning", "Day") else 1] for t in TIMES}
        return {t: self.headers[0] for t in TIMES}

    def methods(self) -> list[str]:
        found = {m for h in self.headers for m in h}
        return [m for m in METHOD_ORDER if m in found]

    def grouped(self, method: str) -> list[tuple[list[str], Table | None]]:
        """Times sharing an identical table for this method, in time order."""
        groups: list[tuple[list[str], Table | None]] = []
        for t, header in self.by_time().items():
            table = header.get(method)
            for times, other in groups:
                if (other and table and other.key() == table.key()) or (other is None and table is None):
                    times.append(t)
                    break
            else:
                groups.append(([t], table))
        return groups


@lru_cache(maxsize=None)
def encounters() -> dict[str, MapEncounters]:
    data = json.loads(read(ENCOUNTERS_JSON))
    out: dict[str, MapEncounters] = {}
    for group in data["wild_encounter_groups"]:
        if group.get("label") != "gWildMonHeaders":
            continue
        fields = {f["type"]: f for f in group["fields"]}
        for entry in group["encounters"]:
            if "map" not in entry:
                continue
            header: dict[str, Table] = {}
            for method, f in fields.items():
                if method not in entry:
                    continue
                mons, rates = entry[method]["mons"], f["encounter_rates"]
                slots = [Slot(m["species"], rates[i], m["min_level"], m["max_level"]) for i, m in enumerate(mons)]
                rate = entry[method]["encounter_rate"]
                if "groups" in f:
                    for sub, idxs in f["groups"].items():
                        header[sub] = Table(sub, rate, [slots[i] for i in idxs])
                else:
                    header[method] = Table(method, rate, slots)
            out.setdefault(entry["map"], MapEncounters(entry["map"])).headers.append(header)
    return out


# ---------------------------------------------------------------- trainers


@dataclass
class Trainer:
    const: str
    name: str
    trainer_class: str
    party: list[tuple[str, int]]

    def label(self) -> str:
        return f"{self.trainer_class} {self.name}"


@lru_cache(maxsize=None)
def trainers() -> dict[str, Trainer]:
    parties = {}
    for sym, body in re.findall(r"\b(sParty_\w+)\[\]\s*=\s*\{(.*?)\n\};", read("src/data/trainer_parties.h"), re.S):
        parties[sym] = [(sp, int(lvl)) for lvl, sp in re.findall(r"\.lvl\s*=\s*(\d+),\s*\.species\s*=\s*(SPECIES_\w+)", body)]
    out = {}
    for const, body in re.findall(r"\[(TRAINER_\w+)\]\s*=\s*\{(.*?)\n    \},", read("src/data/trainers.h"), re.S):
        name = re.search(r'trainerName\s*=\s*_\("(.*?)"\)', body)
        cls = re.search(r"trainerClass\s*=\s*TRAINER_CLASS_(\w+)", body)
        party = re.search(r"party\s*=\s*\w+\((sParty_\w+)\)", body)
        out[const] = Trainer(
            const,
            name.group(1).title() if name else "",
            cls.group(1).replace("_", " ").title() if cls else "",
            parties.get(party.group(1), []) if party else [],
        )
    return out


def map_trainers(info: dict) -> list[Trainer]:
    """Trainers whose battle scripts are reachable from the map's events.

    Starts at the map's object, coord and bg event scripts and its map scripts,
    and follows every label they mention, so leftover battles nothing points to
    are left out.
    """
    path = REPO / "data" / "maps" / info["dir"] / "scripts.inc"
    if not path.exists():
        return []
    blocks: dict[str, str] = {}
    label = None
    for line in path.read_text(encoding="utf-8").splitlines():
        m = re.match(r"^(\w+)::?", line)
        if m:
            label = m.group(1)
            blocks[label] = ""
        elif label:
            blocks[label] += line + "\n"
    # A block without a terminating end/return falls through to the next label.
    order = list(blocks)
    starts = [info["name"] + "_MapScripts"]
    for kind in ("object_events", "coord_events", "bg_events"):
        starts += [e["script"] for e in info.get(kind) or [] if e.get("script")]
    seen, todo = set(), [s for s in starts if s in blocks]
    while todo:
        lbl = todo.pop()
        if lbl in seen:
            continue
        seen.add(lbl)
        body = blocks[lbl]
        todo += [r for r in re.findall(r"\b\w+\b", body) if r in blocks]
        if not re.search(r"^\s*(end|return|goto)\b", body, re.M):
            i = order.index(lbl)
            if i + 1 < len(order):
                todo.append(order[i + 1])
    ids = []
    for lbl in order:
        if lbl in seen:
            for t in re.findall(r"trainerbattle\w*\s+(?:\w+\s*,\s*)?(TRAINER_\w+)", blocks[lbl]):
                if t not in ids:
                    ids.append(t)
    table = trainers()
    return [table[t] for t in ids if t in table]


# ---------------------------------------------------------------- selection


def find_map(name: str) -> dict | None:
    """resolve_map, or else the shortest map whose name starts with this (`twinleaf`)."""
    info = resolve_map(name)
    if info or not squash(name):
        return info
    key = squash(name)
    matches = [m for m in maps().values() if squash(m["name"]).startswith(key)]
    return min(matches, key=lambda m: (m["id"] not in encounters(), len(m["name"]))) if matches else None


def select_maps(names: list[str], all_maps: bool) -> list[dict]:
    if names:
        out = []
        for raw in names:
            info = find_map(raw)
            if not info:
                raise SystemExit(f"error: no map called {raw!r}")
            out.append(info)
        return out
    with_encounters = encounters()
    pool = [m for m in maps().values() if m["id"] in with_encounters]
    if not all_maps:
        pool = [m for m in pool if is_sinnoh_map(m)]
    order = {m: i for i, m in enumerate(with_encounters)}
    return sorted(pool, key=lambda m: order[m["id"]])


def join_names(args: list[str]) -> list[str]:
    """Let maps be given with spaces (`ravaged path`) when no other map matches."""
    out, i = [], 0
    while i < len(args):
        for j in range(len(args), i, -1):
            candidate = " ".join(args[i:j])
            if j - i == 1 or find_map(candidate):
                out.append(candidate)
                i = j
                break
    return out


def display(info: dict) -> str:
    return re.sub(r"(?<=[a-z])(?=[A-Z0-9])", " ", info["name"]).replace("_", " ")


def times_label(times: list[str]) -> str:
    if len(times) == 4:
        return "All day"
    return "/".join(TIME_ABBR[t] for t in times)


def levels(a: int, b: int) -> str:
    return f"{a}" if a == b else f"{a}-{b}"


def md_table(headings: list[str], rows: list[list[str]]) -> str:
    lines = ["| " + " | ".join(headings) + " |", "|" + "|".join("---" for _ in headings) + "|"]
    lines += ["| " + " | ".join(r) + " |" for r in rows]
    return "\n".join(lines)


def type_mix(tables: list[Table]) -> dict[str, float]:
    """Each type's share of the encounters in these tables, a dual type counting half to each."""
    mix: dict[str, float] = defaultdict(float)
    for table in tables:
        for sp, (pct, _, _) in table.composition().items():
            types = species(sp).types or ("?",)
            for t in types:
                mix[t] += pct / len(types) / len(tables)
    return dict(sorted(mix.items(), key=lambda kv: -kv[1]))


def method_table(enc: MapEncounters, method: str) -> str:
    """Species down the side, times of day across, levels at the end."""
    groups = [(times, table) for times, table in enc.grouped(method)]
    order: dict[str, list[int]] = {}
    for _, table in groups:
        for s, (_, a, b) in (table.composition() if table else {}).items():
            lv = order.setdefault(s, [a, b])
            lv[0], lv[1] = min(lv[0], a), max(lv[1], b)
    rows = []
    for s, (a, b) in order.items():
        cells = [species(s).name]
        for _, table in groups:
            comp = table.composition() if table else {}
            cells.append(f"{comp[s][0]}%" if s in comp else "–")
        rows.append(cells + [levels(a, b)])
    return md_table(["Species"] + [times_label(t) for t, _ in groups] + ["Lv."], rows)


# ---------------------------------------------------------------- commands
#
# Output is Markdown (headings, tables and lists), so it reads as rendered
# tables wherever it is shown.


def cmd_show(args) -> None:
    for info in select_maps(join_names(args.maps), args.all):
        enc = encounters().get(info["id"])
        print(f"## {display(info)}\n")
        if not enc:
            print("No wild encounters.\n")
        else:
            n = len(enc.headers)
            note = {1: "One table, all day", 2: "Day and night tables", 4: "A table per time of day"}.get(n, f"{n} tables, first used all day")
            print(f"*{note} ({info['id']})*\n")
            for method in enc.methods():
                print(f"**{METHOD_NAMES[method]}**\n")
                print(method_table(enc, method) + "\n")
                if args.types and method == "land_mons":
                    mix = type_mix([h[method] for h in enc.headers if method in h])
                    print("Types: " + ", ".join(f"{t} {v:.0f}%" for t, v in mix.items() if v >= 0.5) + "\n")
        if not args.no_trainers:
            found = map_trainers(info)
            if found:
                print("**Trainers**\n")
                rows = [[t.label(), ", ".join(f"{species(s).name} Lv. {lv}" for s, lv in t.party)] for t in found]
                print(md_table(["Trainer", "Team"], rows) + "\n")


def cmd_summary(args) -> None:
    chosen = select_maps(join_names(args.maps), args.all)
    rows = []
    where: dict[str, dict[str, set[str]]] = defaultdict(lambda: defaultdict(set))
    for info in chosen:
        enc = encounters().get(info["id"])
        if not enc:
            continue
        grass = [h["land_mons"] for h in enc.headers if "land_mons" in h]
        per_table = [len(t.composition()) for t in grass]
        grass_species = {s for t in grass for s in t.composition()}
        all_species = {s for h in enc.headers for t in h.values() for s in t.composition()}
        top3 = (
            round(sum(sum(p for p, _, _ in list(t.composition().values())[:3]) for t in grass) / len(grass))
            if grass else None
        )
        lvs = [lv for t in grass for s in t.slots for lv in (s.min_level, s.max_level)]
        rows.append([
            display(info),
            str(len(enc.headers)),
            levels(min(per_table), max(per_table)) if grass else "–",
            str(len(grass_species)) if grass else "–",
            str(len(all_species)),
            f"{top3}%" if top3 is not None else "–",
            levels(min(lvs), max(lvs)) if lvs else "–",
            str(len({t.label() for t in map_trainers(info)})),
        ])
        for h in enc.headers:
            for method, t in h.items():
                for s in t.composition():
                    where[s][display(info)].add(METHOD_NAMES[method])

    print(md_table(["Map", "Tables", "Grass/table", "Grass total", "All species", "Top 3", "Grass Lv.", "Trainers"], rows))
    print()
    print("- **Grass/table:** species in each grass table. **Grass total:** across all times of day.")
    print("- **Top 3:** the three most common species' share of a grass table, averaged over its tables.")
    print("- **Trainers:** distinct trainers, so a rival with a team per starter counts once.")
    print()
    print("**Species by number of maps**\n")
    method_order = {name: i for i, name in enumerate(METHOD_NAMES.values())}
    srows = []
    for s, places in sorted(where.items(), key=lambda kv: (-len(kv[1]), species(kv[0]).name)):
        text = ", ".join(
            f"{m} ({', '.join(sorted(ms, key=method_order.get))})" for m, ms in places.items()
        )
        srows.append([species(s).name, str(len(places)), text])
    print(md_table(["Species", "Maps", "Where"], srows))


def cmd_species(args) -> None:
    for target in resolve_species(args.species):
        print(f"## {species(target).name} ({'/'.join(species(target).types)})\n")
        rows = []
        for map_id, enc in encounters().items():
            info = maps().get(map_id)
            if not info or (not args.all and not is_sinnoh_map(info)):
                continue
            for method in enc.methods():
                for times, table in enc.grouped(method):
                    if table and target in table.composition():
                        p, a, b = table.composition()[target]
                        rows.append([display(info), METHOD_NAMES[method], times_label(times), f"{p}%", levels(a, b)])
        for info in maps().values():
            if not args.all and not is_sinnoh_map(info):
                continue
            for t in map_trainers(info):
                for s, lv in t.party:
                    if s == target:
                        rows.append([display(info), f"Trainer: {t.label()}", "–", "–", str(lv)])
        if rows:
            print(md_table(["Map", "How", "Times", "Rate", "Lv."], rows) + "\n")
        else:
            print("Not found" + (".\n" if args.all else " on the Sinnoh maps (try --all).\n"))


def cmd_check(args) -> int:
    chosen = select_maps(join_names(args.maps), args.all)
    # The starters are given, not caught, so the rival's are fine.
    obtainable = {family(s) for s in starters()} | {
        family(s)
        for info in chosen
        for h in (encounters().get(info["id"]).headers if info["id"] in encounters() else [])
        for t in h.values()
        for s in t.composition()
    }
    warnings, notes = [], []
    for info in chosen:
        name = display(info)
        enc = encounters().get(info["id"])
        if enc:
            for method in enc.methods():
                for times, table in enc.grouped(method):
                    if not table:
                        continue
                    comp = table.composition()
                    where = f"{name} {METHOD_NAMES[method]} ({times_label(times)})"
                    if method == "land_mons" and args.max_species and len(comp) > args.max_species:
                        warnings.append(f"**{where}:** {len(comp)} species, more than {args.max_species}")
                    mids = sorted((a + b) / 2 for a, b in ((v[1], v[2]) for v in comp.values()))
                    median = mids[len(mids) // 2]
                    for s, (p, a, b) in comp.items():
                        if b * 2 < median or a > median * 2:
                            warnings.append(f"**{where}:** {species(s).name} at Lv. {levels(a, b)} is far from the table's usual Lv. {median:g}")
            if len(enc.headers) == 4 and len({tuple(sorted((m, t.key()) for m, t in h.items())) for h in enc.headers}) == 1:
                notes.append(f"**{name}:** its four tables are identical, so one would do")
        for t in map_trainers(info):
            for s, lv in t.party:
                if family(s) not in obtainable:
                    warnings.append(f"**{name}:** {t.label()}'s {species(s).name} can't be caught on any of these maps")
    if warnings:
        print(f"**Warnings ({len(warnings)})**\n")
        print("\n".join(f"- {w}" for w in warnings) + "\n")
    else:
        print(f"No problems found on {len(chosen)} maps.\n")
    if notes:
        print("**Notes**\n")
        print("\n".join(f"- {n}" for n in notes))
    return 1 if warnings else 0


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="command", required=True)

    def add(name: str, help_text: str, takes_maps: bool = True) -> argparse.ArgumentParser:
        p = sub.add_parser(name, help=help_text, description=help_text)
        if takes_maps:
            p.add_argument("maps", nargs="*", help="maps to look at (default: the converted Sinnoh maps)")
        p.add_argument("--all", action="store_true", help="every map with encounters, not just the Sinnoh ones")
        return p

    p = add("summary", "How varied each map's encounters are, and which species turn up where")
    p.set_defaults(func=cmd_summary)
    p = add("show", "Every encounter table and trainer on the given maps")
    p.add_argument("--types", action="store_true", help="also show each map's grass type mix")
    p.add_argument("--no-trainers", action="store_true", help="leave out trainers")
    p.set_defaults(func=cmd_show)
    p = add("species", "Where the given species can be caught or fought", takes_maps=False)
    p.add_argument("species", nargs="+", help="species names, e.g. shinx or SPECIES_SHINX")
    p.set_defaults(func=cmd_species)
    p = add("check", "Flag crowded tables, level outliers and trainer Pokémon that can't be caught; exits 1 on a warning")
    p.add_argument("--max-species", type=int, default=5, help="most species a grass table should have (0 to skip; default 5)")
    p.set_defaults(func=cmd_check)

    args = parser.parse_args(argv)
    return args.func(args) or 0


if __name__ == "__main__":
    signal.signal(signal.SIGPIPE, signal.SIG_DFL)  # quiet when piped into head
    sys.exit(main())
