"""Sphinx extension: generate a page for each town, city and route.

Each page lists the map's wild encounters (split by time of day), its trainer
battles and the items found there, including the buildings and underwater
areas that belong to it. Pages land in ``docs/map/towns/`` and
``docs/map/routes/`` at build time so they cannot drift from the game data.
Both directories are gitignored; edit the data, not the output.
"""

from __future__ import annotations

import html
import json
import re
import shutil
import struct
import zlib
from pathlib import Path

from gen_pokedex import const_name, md_escape, parse_species_names, read, slug, titlecase

HERE = Path(__file__).resolve().parent
DOCS = HERE.parent
REPO = HERE.parents[1]
OUT = DOCS / "map"

DATA = REPO / "src" / "data"
MAPS = REPO / "data" / "maps"

PERIODS = ["Morning", "Day", "Evening", "Night"]

# Which table each period uses, by how many tables the map has. Mirrors
# GetTimeBasedWildMonHeaderId in src/wild_encounter.c.
PERIOD_TABLES = {
    1: [0, 0, 0, 0],
    2: [0, 0, 1, 1],
    4: [0, 1, 2, 3],
}

# Unused maps left over in the decomp, which would otherwise be filed under Route 203.
EXCLUDED_MAPS = {"MAP_ROUTE203_PROTOTYPE", "MAP_ROUTE203_PROTOTYPE_PRETTY_PETAL_FLOWER_SHOP"}

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

# (directory, index title, map types) for each section of the Map docs.
SECTIONS = [
    ("towns", "Towns and cities", {"MAP_TYPE_TOWN", "MAP_TYPE_CITY"}),
    ("routes", "Routes", {"MAP_TYPE_ROUTE", "MAP_TYPE_OCEAN_ROUTE"}),
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
    """Route201 -> Route 201; MtPyre_Summit -> Mt. Pyre (Summit);
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
            rows.append((label, species, names.get(species, species), row))
    return rows


def load_maps() -> dict[str, dict]:
    maps = {}
    for path in MAPS.glob("*/map.json"):
        data = json.loads(read(path))
        maps[data["id"]] = data
    return maps


def area_name(parent: str, name: str) -> str:
    """The part of a building's map name after its town or route:
    JubilifeCity_PoketchCompany_1F -> Poketch Company 1F; Underwater_Route124 -> Underwater."""
    if name == parent:
        return "Outside"
    if name == f"Underwater_{parent}":
        return "Underwater"
    rest = name[len(parent) + 1:].split("_")
    words = " ".join(re.sub(r"(?<=[a-z])(?=[A-Z0-9])", " ", p) for p in rest)
    return words.replace("Pokemon", "Pokémon")


def parent_name(name: str) -> str:
    """The map a building or underwater area belongs to, by its name."""
    parts = name.split("_")
    if parts[0] == "Underwater" and len(parts) == 2:
        return parts[1]
    return parts[0]


def parse_icons() -> dict[str, Path]:
    """SPECIES_X -> its party icon PNG, following gMonIconTable so forms like
    Unown A resolve to the right file."""
    paths = {}
    gfx = read(DATA / "graphics" / "pokemon.h")
    for sym, path in re.findall(r"const u8 (gMonIcon_\w+)\[\]\s*=\s*\w+\(\"([^\"]+)\"", gfx):
        paths[sym] = REPO / path
    table = read(REPO / "src" / "pokemon_icon.c")
    table = table[table.index("gMonIconTable[]"):]
    table = table[: table.index("};")]
    out = {}
    for species, sym in re.findall(r"\[(SPECIES_\w+)\]\s*=\s*(gMonIcon_\w+)", table):
        if species != "SPECIES_NONE" and sym in paths and paths[sym].exists():
            out[species] = paths[sym]
    return out


def write_icon_frame(src: Path, dst: Path) -> None:
    """Copy the first frame of a party icon: icon.png holds two 32x32 animation
    frames stacked vertically. PNG filters only look at the row above, so the
    top rows of the image data decode on their own and can be kept as-is."""
    data = src.read_bytes()
    chunks, pos = [], 8
    while pos < len(data):
        length, kind = struct.unpack(">I4s", data[pos:pos + 8])
        chunks.append((kind, data[pos + 8:pos + 8 + length]))
        pos += 12 + length

    width, height, depth, colour, *rest = struct.unpack(">IIBBBBB", chunks[0][1])
    if rest[2]:  # interlaced: rows aren't stored in order, so keep both frames
        shutil.copyfile(src, dst)
        return
    frame = min(width, height)
    channels = {0: 1, 2: 3, 3: 1, 4: 2, 6: 4}[colour]
    row = 1 + (width * depth * channels + 7) // 8
    pixels = zlib.decompress(b"".join(body for kind, body in chunks if kind == b"IDAT"))

    def chunk(kind: bytes, body: bytes) -> bytes:
        return struct.pack(">I", len(body)) + kind + body + struct.pack(">I", zlib.crc32(kind + body))

    out = [b"\x89PNG\r\n\x1a\n", chunk(b"IHDR", struct.pack(">IIBBBBB", width, frame, depth, colour, *rest))]
    out += [chunk(kind, body) for kind, body in chunks[1:] if kind not in (b"IDAT", b"IEND")]
    out += [chunk(b"IDAT", zlib.compress(pixels[: row * frame])), chunk(b"IEND", b"")]
    dst.write_bytes(b"".join(out))


# --- game text ---------------------------------------------------------------

def game_text(raw: str) -> str:
    """TEAM AQUA -> Team Aqua; {PKMN} TRAINER -> Pokémon Trainer."""
    return titlecase(raw.replace("{PKMN}", "POKéMON"))


def item_title(raw: str) -> str:
    """POKé BALL -> Poké Ball, HP UP -> HP Up, TM01 -> TM01."""
    keep = {"HP", "PP"}
    words = []
    for word in raw.split(" "):
        if word in keep or re.fullmatch(r"[TH]M\d+", word):
            words.append(word)
        else:
            words.append(titlecase(word))
    return " ".join(words)


def parse_items() -> dict[str, str]:
    """ITEM_X -> display name, with TMs and HMs naming their move."""
    text = read(DATA / "items.h")
    names = {}
    for const, raw in re.findall(r"\[(ITEM_\w+)\]\s*=\s*\{\s*\.name\s*=\s*_\(\"([^\"]*)\"\)", text):
        name = item_title(raw)
        m = re.fullmatch(r"ITEM_[TH]M_(\w+)", const)
        if m:
            name = f"{name} {const_name(m.group(1), '')}"
        names[const] = name
    return names


def item_name(const: str, items: dict[str, str]) -> str:
    return items.get(const) or const_name(const, "ITEM_")


def parse_item_balls() -> dict[str, str]:
    """Script label -> the item an item ball with that script gives."""
    out = {}
    for path in list((REPO / "data").rglob("*.inc")):
        for label, item in re.findall(r"^(\w+)::\s*\n\s*finditem\s+(ITEM_\w+)", read(path), re.M):
            out[label] = item
    return out


def brace_groups(text: str) -> list[str]:
    """The contents of each top-level {...} in text."""
    groups, depth, start = [], 0, 0
    for i, ch in enumerate(text):
        if ch == "{":
            if depth == 0:
                start = i + 1
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0:
                groups.append(text[start:i])
    return groups


def parse_trainers() -> dict[str, dict]:
    """TRAINER_X -> {"name", "double", "party": [(species, level, item, [moves])]}."""
    classes = dict(
        re.findall(
            r"\[(TRAINER_CLASS_\w+)\]\s*=\s*_\(\"([^\"]*)\"\)",
            read(DATA / "text" / "trainer_class_names.h"),
        )
    )

    parties = {}
    for name, body in re.findall(
        r"static const struct \w+ (sParty_\w+)\[\]\s*=\s*\{(.*?)\};",
        read(DATA / "trainer_parties.h"),
        re.S,
    ):
        party = []
        for mon in brace_groups(body):
            species = re.search(r"\.species\s*=\s*(SPECIES_\w+)", mon)
            level = re.search(r"\.lvl\s*=\s*(\d+)", mon)
            item = re.search(r"\.heldItem\s*=\s*(ITEM_\w+)", mon)
            moves = re.search(r"\.moves\s*=\s*\{([^}]*)\}", mon)
            if not species or not level:
                continue
            party.append(
                (
                    species.group(1),
                    int(level.group(1)),
                    item.group(1) if item and item.group(1) != "ITEM_NONE" else None,
                    [m for m in re.findall(r"MOVE_\w+", moves.group(1)) if m != "MOVE_NONE"]
                    if moves
                    else [],
                )
            )
        parties[name] = party

    trainers = {}
    text = read(DATA / "trainers.h")
    parts = re.split(r"\[(TRAINER_\w+)\]\s*=\s*", text)
    for const, body in zip(parts[1::2], parts[2::2]):
        cls = re.search(r"\.trainerClass\s*=\s*(TRAINER_CLASS_\w+)", body)
        name = re.search(r"\.trainerName\s*=\s*_\(\"([^\"]*)\"\)", body)
        party = re.search(r"\.party\s*=\s*\w+\(\s*(sParty_\w+)\s*\)", body)
        if not cls or not party:
            continue
        cls_name = game_text(classes.get(cls.group(1), ""))
        own_name = game_text(name.group(1) if name else "")
        pic = re.search(r"\.trainerPic\s*=\s*(TRAINER_PIC_\w+)", body)
        trainers[const] = {
            "name": " ".join(bit for bit in (cls_name, own_name) if bit),
            "class_name": cls_name,
            "own_name": own_name,
            "class": cls.group(1),
            "pic": pic.group(1) if pic else None,
            "female": bool(re.search(r"\.encounterMusic_gender\s*=\s*F_TRAINER_FEMALE", body)),
            "double": bool(re.search(r"\.doubleBattle\s*=\s*TRUE", body)),
            "party": parties.get(party.group(1), []),
        }
    return trainers


def parse_trainer_money() -> tuple[dict[str, int], int]:
    """(TRAINER_CLASS_X -> money multiplier, the default for unlisted classes)."""
    text = read(REPO / "src" / "battle_main.c")
    table = text[text.index("gTrainerMoneyTable[]"):]
    table = table[: table.index("};")]
    money = {cls: int(v) for cls, v in re.findall(r"\{\s*(TRAINER_CLASS_\w+)\s*,\s*(\d+)\s*\}", table)}
    default = re.search(r"\{\s*0xFF\s*,\s*(\d+)\s*\}", table)
    return money, int(default.group(1)) if default else 5


def prize_money(trainer: dict, money: tuple[dict[str, int], int]) -> int:
    """Mirrors GetTrainerMoneyToGive: 4 x the last Pokémon's level x the class's
    rate, doubled for double battles. Without an Amulet Coin."""
    if not trainer["party"]:
        return 0
    rates, default = money
    reward = 4 * trainer["party"][-1][1] * rates.get(trainer["class"], default)
    return reward * 2 if trainer["double"] else reward


def parse_gender_ratios() -> dict[str, int]:
    """SPECIES_X -> genderRatio: 0 all male, 254 all female, 255 genderless."""
    fixed = {"MON_MALE": 0, "MON_FEMALE": 254, "MON_GENDERLESS": 255}
    text = read(DATA / "pokemon" / "species_info.h")
    parts = re.split(r"\[(SPECIES_\w+)\]\s*=\s*", text)
    out = {}
    for const, body in zip(parts[1::2], parts[2::2]):
        m = re.search(r"\.genderRatio\s*=\s*(\w+)(?:\(([\d.]+)\))?", body)
        if not m:
            continue
        if m.group(1) in fixed:
            out[const] = fixed[m.group(1)]
        elif m.group(1) == "PERCENT_FEMALE":
            out[const] = min(254, int(float(m.group(2)) * 255 / 100))
    return out


def trainer_mon_gender(trainer: dict, species: str, ratios: dict[str, int]) -> str:
    """♂, ♀ or "" for a trainer's Pokémon. CreateNPCTrainerParty only adds a
    name hash to the upper bytes of the personality, so the low byte that picks
    the gender is fixed by the battle type and the trainer's gender."""
    ratio = ratios.get(species, 255)
    if ratio == 255:
        return ""
    if ratio in (0, 254):
        return "♀" if ratio == 254 else "♂"
    low = 0x80 if trainer["double"] else 0x78 if trainer["female"] else 0x88
    return "♀" if ratio > low else "♂"


def parse_trainer_pics() -> dict[str, Path]:
    """TRAINER_PIC_X -> its front pic PNG."""
    paths = {}
    gfx = read(DATA / "graphics" / "trainers.h")
    for sym, path in re.findall(r"const u32 (gTrainerFrontPic_\w+)\[\]\s*=\s*\w+\(\"([^\"]+)\"", gfx):
        paths[sym] = REPO / path
    table = read(DATA / "trainer_graphics" / "front_pic_tables.h")
    out = {}
    for pic, sym in re.findall(r"TRAINER_SPRITE\(\s*(\w+)\s*,\s*(gTrainerFrontPic_\w+)", table):
        if sym in paths and paths[sym].exists():
            out[f"TRAINER_PIC_{pic}"] = paths[sym]
    return out


def parse_rematches() -> dict[str, list[str]]:
    """First battle's TRAINER_X -> its rematch trainers, in order."""
    text = read(REPO / "src" / "battle_setup.c")
    out = {}
    for args in re.findall(r"\]\s*=\s*REMATCH\(([^)]*)\)", text):
        ids = [a.strip() for a in args.split(",")][:-1]  # the last is the map
        rematches = []
        for t in ids[1:]:
            if t != ids[0] and t not in rematches:
                rematches.append(t)
        out[ids[0]] = rematches
    return out


def map_scripts(name: str) -> str:
    return "\n".join(read(p) for p in sorted((MAPS / name).glob("scripts.*")))


def map_trainers(name: str) -> list[str]:
    """Trainer constants battled in a map's scripts, in script order."""
    found = []
    for const in re.findall(
        r"trainerbattle\w*\s+(?:TRAINER_BATTLE_\w+\s*,\s*)?(TRAINER_\w+)", map_scripts(name)
    ):
        if const not in found and const != "TRAINER_NONE":
            found.append(const)
    return found


def merge_name_variants(consts: list[str], trainers: dict[str, dict]) -> list[str]:
    """Fold trainers that differ only by name into one entry. The rival is one
    battle per starter, but a script picks May or Brendan by the player's
    gender: both trainers have the same class, sprite and team, and constants
    that match apart from the name. The merged entry is added to ``trainers``
    under a key joining the constants, named "May / Brendan"."""
    groups: dict[str, list[str]] = {}
    order = []
    for const in consts:
        t = trainers.get(const)
        if t is None:
            order.append(const)
            continue
        # The constant without the name: TRAINER_MAY_ROUTE_202_TREECKO -> ROUTE_202_TREECKO.
        # A constant that is only a name (TRAINER_GWEN) stays whole, so it never matches.
        rest = re.sub(rf"^TRAINER_{re.escape(t['own_name'].upper())}_(?=.)", "", const)
        key = repr((rest, t["class"], t["pic"], t["double"], t["party"]))
        if key not in groups:
            groups[key] = []
            order.append(key)
        groups[key].append(const)
    out = []
    for key in order:
        members = groups.get(key)
        if members is None:
            out.append(key)
            continue
        names = list(dict.fromkeys(trainers[c]["own_name"] for c in members))
        if len(names) == 1:
            out += members
            continue
        merged = "|".join(members)
        t = dict(trainers[members[0]], own_name=" / ".join(names))
        t["name"] = " ".join(bit for bit in (t["class_name"], t["own_name"]) if bit)
        trainers[merged] = t
        out.append(merged)
    return out


def map_items(info: dict, item_balls: dict[str, str]) -> list[tuple[str, str]]:
    """(ITEM_X, how it's found) for each item ball, hidden item and gift on a map."""
    out = []
    for obj in info.get("object_events", []):
        item = item_balls.get(obj.get("script", ""))
        if obj.get("graphics_id") == "OBJ_EVENT_GFX_ITEM_BALL" and item:
            out.append((item, "Item ball"))
    for bg in info.get("bg_events", []):
        if bg.get("type") == "hidden_item" and bg.get("item"):
            out.append((bg["item"], "Hidden"))
    gifts = []
    for item in re.findall(r"^\s*giveitem\s+(ITEM_\w+)", map_scripts(info["name"]), re.M):
        if item not in gifts:
            gifts.append(item)
    out += [(item, "Gift") for item in gifts]
    return out


# --- rendering ---------------------------------------------------------------

# Encounter types grouped under a header row each, in table order.
ENCOUNTER_GROUPS = [
    ("Walking", {"Grass", "Cave"}),
    ("Surfing", {"Surf"}),
    ("Fishing", {"Old Rod", "Good Rod", "Super Rod"}),
]
SPECIAL_GROUP = "Special"  # Rock Smash, seaweed and anything else


def encounter_group(label: str) -> str:
    return next((g for g, labels in ENCOUNTER_GROUPS if label in labels), SPECIAL_GROUP)


def render_encounters(rows, split: bool, icons: dict[str, Path]) -> list[str]:
    """A table of wild Pokémon, with a chance per period when the map's tables
    change with the time of day and a single chance column otherwise.

    Rows sit under Walking/Surfing/Fishing/Special header rows spanning the
    table, which Markdown tables can't do, so this is raw HTML. Icons are
    copied next to the pages by copy_images, since Sphinx doesn't collect
    images from raw HTML.
    """
    e = html.escape
    periods = PERIODS if split else ["Chance"]
    width = 3 + len(periods)
    lines = [
        '<table class="docutils align-default encounters-table">',
        "<thead><tr>"
        + "".join(f"<th>{e(h)}</th>" for h in ["Type", "Pokémon", "Levels", *periods])
        + "</tr></thead>",
        "<tbody>",
    ]
    for group in [g for g, _l in ENCOUNTER_GROUPS] + [SPECIAL_GROUP]:
        grouped = [r for r in rows if encounter_group(r[0]) == group]
        if not grouped:
            continue
        lines.append(f'<tr><th colspan="{width}">{group}</th></tr>')
        for label, species, name, row in grouped:
            chances = row["chance"] if split else row["chance"][:1]
            mon = e(name)
            if species in icons:
                mon = f'<img alt="" src="../icons/{slug(species)}.png"> {mon}'
            if row["min"] == row["max"]:
                levels = str(row["min"])
            else:
                levels = f"{row['min']} - {row['max']}"
            cells = [e(label), mon, levels] + [percent(c) if c else "—" for c in chances]
            lines.append("<tr>" + "".join(f"<td>{c}</td>" for c in cells) + "</tr>")
    lines += ["</tbody>", "</table>"]
    return lines


def render_mon(species, level, item, moves, trainer, ctx) -> str:
    e = html.escape
    icon = ""
    if species in ctx["icons"]:
        icon = f'<img class="mon-icon" alt="" src="../icons/{slug(species)}.png">'
    gender = trainer_mon_gender(trainer, species, ctx["ratios"])
    gender_html = ""
    if gender:
        kind = "female" if gender == "♀" else "male"
        gender_html = f'<span class="gender-{kind}">{gender}</span>'
    lines = [
        f'<span class="mon-name">{e(ctx["names"].get(species, species))}</span>{gender_html} '
        f'<span class="mon-level">Lv. {level}</span>',
        e(item_name(item, ctx["items"])) if item else "No item",
    ]
    if moves:
        lines.append(f'<span class="mon-moves">{e(", ".join(const_name(m, "MOVE_") for m in moves))}</span>')
    return f'<div class="trainer-mon">{icon}<div>' + "<br>".join(lines) + "</div></div>"


def render_trainer(const, ctx, note="") -> list[str]:
    """One trainer: their sprite, name and prize money in a cell spanning a row
    per Pokémon in their party."""
    e = html.escape
    t = ctx["trainers"][const]
    pic = ""
    if t["pic"] in ctx["pics"]:
        pic = f'<img class="trainer-pic" alt="" src="../trainers/{t["pic"][len("TRAINER_PIC_"):].lower()}.png"><br>'
    bits = [f'<span class="trainer-class">{e(t["class_name"])}</span> <strong>{e(t["own_name"])}</strong>']
    if note:
        bits.append(f"<em>{e(note)}</em>")
    if t["double"]:
        bits.append("<em>Double battle</em>")
    bits.append(f"Reward: ₽{prize_money(t, ctx['money'])}")
    party = t["party"] or [None]
    cell = f'<td class="trainer" rowspan="{len(party)}">{pic}' + "<br>".join(bits) + "</td>"
    rows = []
    for i, mon in enumerate(party):
        mon_cell = f"<td>{render_mon(*mon, t, ctx)}</td>" if mon else "<td>—</td>"
        rows.append("<tr>" + (cell if i == 0 else "") + mon_cell + "</tr>")
    return rows


def render_trainers(areas, ctx) -> list[str]:
    """The trainer table: full-width rows name each area when there's more than
    one, and head the rematch battles at the end."""
    show_area = len(areas) > 1
    lines = [
        '<table class="docutils align-default trainers-table">',
        "<thead><tr><th>Trainer</th><th>Pokémon</th></tr></thead>",
        "<tbody>",
    ]
    rematches = []
    for area, _info, consts, _i in areas:
        consts = [c for c in consts if c in ctx["trainers"]]
        if not consts:
            continue
        if show_area:
            lines.append(f'<tr><th colspan="2">{html.escape(area)}</th></tr>')
        for const in consts:
            lines += render_trainer(const, ctx)
            for n, rematch in enumerate(ctx["rematches"].get(const, []), 1):
                if rematch in ctx["trainers"]:
                    rematches.append((rematch, f"Rematch {n}"))
    if rematches:
        lines.append('<tr><th colspan="2">Rematch</th></tr>')
        for const, note in rematches:
            lines += render_trainer(const, ctx, note)
    lines += ["</tbody>", "</table>"]
    return lines


def render_page(title, areas, encounter_areas, hours, ctx) -> str:
    """areas: [(area name, map info, [trainer consts], [(item, how)])]."""
    show_area = len(areas) > 1
    lines = [f"# {title}", ""]

    # Wild encounters, one table per area that has any.
    lines += ["## Wild encounters", ""]
    if not encounter_areas:
        lines += ["No wild Pokémon.", ""]
    if any(split for _a, _c, _r, split in encounter_areas):
        lines += [" · ".join(f"**{p}** {h}" for p, h in zip(PERIODS, hours)), ""]
    for area, map_const, rows, split in encounter_areas:
        if len(encounter_areas) > 1 or area != "Outside":
            lines += [f"### {area}", ""]
        lines += render_encounters(rows, split, ctx["icons"]) + [""]
        if map_const in NOTES:
            lines += ["```{note}", NOTES[map_const], "```", ""]

    # Trainers.
    lines += ["## Trainers", ""]
    consts = [c for _a, _info, found, _i in areas for c in found if c in ctx["trainers"]]
    if consts:
        lines += render_trainers(areas, ctx)
        repeated = [ctx["trainers"][c]["name"] for c in consts]
        if len(repeated) != len(set(repeated)):
            lines += [
                "",
                "A trainer listed more than once has a different team depending on "
                "your choices, such as your starter; you only battle one of them.",
            ]
    else:
        lines.append("No trainers.")
    lines.append("")

    # Items.
    lines += ["## Items", ""]
    rows = [(area, item, how) for area, _info, _t, found in areas for item, how in found]
    if rows:
        lines += [("| Area " if show_area else "") + "| Item | How |"]
        lines += [("| --- " if show_area else "") + "| --- | --- |"]
        for area, item, how in rows:
            cells = f"| {md_escape(item_name(item, ctx['items']))} | {how} |"
            lines.append((f"| {area} " if show_area else "") + cells)
    else:
        lines.append("No items.")
    lines.append("")

    return "\n".join(lines)


def generate(app=None) -> int:
    encounters = json.loads(read(DATA / "wild_encounters.json"))
    group = next(g for g in encounters["wild_encounter_groups"] if g["label"] == "gWildMonHeaders")
    fields = group["fields"]
    maps = {k: v for k, v in load_maps().items() if k not in EXCLUDED_MAPS}
    section_names = load_section_names()
    section_counts: dict[str, int] = {}
    for info in maps.values():
        if info["map_type"] in NAMED_BY_SECTION:
            section = info.get("region_map_section")
            section_counts[section] = section_counts.get(section, 0) + 1
    names = parse_species_names()
    hours = period_hours()
    trainers = parse_trainers()
    items = parse_items()
    item_balls = parse_item_balls()
    icons = parse_icons()
    pics = parse_trainer_pics()
    ctx = {
        "names": names,
        "items": items,
        "icons": icons,
        "pics": pics,
        "trainers": trainers,
        "rematches": parse_rematches(),
        "money": parse_trainer_money(),
        "ratios": parse_gender_ratios(),
    }

    tables_by_map: dict[str, list[dict]] = {}
    for table in group["encounters"]:
        tables_by_map.setdefault(table["map"], []).append(table)

    # Top-level maps get a page; their buildings and underwater areas join it.
    tops = {
        info["name"]: info
        for info in maps.values()
        if any(info["map_type"] in types for _d, _t, types in SECTIONS)
    }
    children: dict[str, list[dict]] = {name: [] for name in tops}
    for info in maps.values():
        parent = parent_name(info["name"])
        if info["name"] not in tops and parent in tops:
            children[parent].append(info)
    # Only the ones the map actually leads to, by warps (events or script commands, as the
    # Trick House's rooms are) or diving, directly or through each other: a building left
    # behind when its route was redrawn isn't part of it.
    for name, kids in children.items():
        by_id = {k["id"]: k for k in kids}
        reached, todo = set(), [tops[name]]
        while todo:
            info = todo.pop()
            links = [w.get("dest_map") for w in info.get("warp_events") or []]
            links += [c.get("map") for c in info.get("connections") or [] if c.get("direction") in ("dive", "emerge")]
            links += re.findall(r"^\s*(?:warp\w*|setdynamicwarp|setwarp)\s+(MAP_\w+)", map_scripts(info["name"]), re.M)
            for dest in links:
                if dest in by_id and dest not in reached:
                    reached.add(dest)
                    todo.append(by_id[dest])
        children[name] = [k for k in kids if k["id"] in reached]

    for directory in [d for d, _t, _types in SECTIONS] + ["icons", "trainers"]:
        out = OUT / directory
        if out.exists():
            shutil.rmtree(out)
        out.mkdir(parents=True)
    for species, path in icons.items():
        write_icon_frame(path, OUT / "icons" / f"{slug(species)}.png")
    for pic, path in pics.items():
        shutil.copyfile(path, OUT / "trainers" / f"{pic[len('TRAINER_PIC_'):].lower()}.png")

    pages = []  # (directory, title, filename)
    used: set[str] = set()
    for name, top in tops.items():
        title = map_title(top, section_names, section_counts)
        # Pages named after the game's name for the map use it for the file too
        # (route201.md), others the map's own name (mtpyre_summit.md)
        if title == map_display_name(name):
            fname = name.lower()
        else:
            fname = re.sub(r"[^a-z0-9]", "", title.lower())
        if fname in used:
            raise ValueError(f"two maps would both generate {fname}.md")
        used.add(fname)

        members = [top] + sorted(children[name], key=lambda i: natural_key(i["name"]))
        areas, encounter_areas = [], []
        for info in members:
            area = area_name(name, info["name"])
            tables = tables_by_map.get(info["id"], [])
            if len(tables) in PERIOD_TABLES:
                period_tables = PERIOD_TABLES[len(tables)]
                rows = build_rows(tables, period_tables, fields, info["map_type"], names)
                if rows:
                    encounter_areas.append((area, info["id"], rows, len(set(period_tables)) > 1))
            found_trainers = merge_name_variants(map_trainers(info["name"]), ctx["trainers"])
            found_items = map_items(info, item_balls)
            if info is top or found_trainers or found_items:
                areas.append((area, info, found_trainers, found_items))

        directory = next(d for d, _t, types in SECTIONS if top["map_type"] in types)
        (OUT / directory / f"{fname}.md").write_text(
            render_page(title, areas, encounter_areas, hours, ctx),
            encoding="utf-8",
        )
        pages.append((directory, title, fname))

    for directory, heading, _types in SECTIONS:
        entries = sorted((p for p in pages if p[0] == directory), key=lambda p: natural_key(p[1]))
        index = [
            f"# {heading}",
            "",
            f"{len(entries)} maps, each with its wild encounters, trainers and items, "
            "including the buildings and underwater areas that belong to it. "
            "Generated from the game data at build time. "
            "See {doc}`../../features/time-of-day` for how encounters change "
            "through the day.",
            "",
        ]
        index += [f"- [{title}]({fname}.md)" for _d, title, fname in entries]
        index += ["", "```{toctree}", ":maxdepth: 1", ":hidden:", ""]
        index += [fname for _d, _t, fname in entries] + ["```", ""]
        (OUT / directory / "index.md").write_text("\n".join(index), encoding="utf-8")

    return len(pages)


def on_builder_inited(app):
    count = generate(app)
    from sphinx.util import logging as sphinx_logging

    sphinx_logging.getLogger(__name__).info(f"[gen_maps] generated {count} town, city and route pages")


def copy_images(app, exception):
    """Put the icons and trainer pics where the tables' raw <img> tags expect
    them, since Sphinx only collects images from Markdown image syntax."""
    if exception is None and app.builder.format == "html":
        for directory in ("icons", "trainers"):
            dest = Path(app.outdir) / "map" / directory
            if dest.exists():
                shutil.rmtree(dest)
            shutil.copytree(OUT / directory, dest)


def setup(app):
    app.connect("builder-inited", on_builder_inited)
    app.connect("build-finished", copy_images)
    return {"version": "1.0", "parallel_read_safe": True}


if __name__ == "__main__":
    print(f"generated {generate()} map pages into {OUT}")
