"""Sphinx extension: generate Pokedex pages from the decomp's data headers.

One page per evolution family, written into ``docs/pokedex/families/`` at build
time so the docs can never drift from ``src/data/pokemon/``. The generated
directory is gitignored; edit the data headers, not the output.
"""

from __future__ import annotations

import re
import shutil
from pathlib import Path

HERE = Path(__file__).resolve().parent
DOCS = HERE.parent
REPO = HERE.parents[1]
OUT = DOCS / "pokedex" / "families"

DATA = REPO / "src" / "data"
MON = DATA / "pokemon"


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace")


# --- name formatting -------------------------------------------------------

def titlecase(raw: str) -> str:
    """BULBASAUR -> Bulbasaur, FARFETCH'D -> Farfetch'd, HO-OH -> Ho-Oh."""
    out = []
    capitalise = True
    for ch in raw:
        out.append(ch.upper() if capitalise else ch.lower())
        capitalise = ch in " -."
    return "".join(out)


def const_name(const: str, prefix: str) -> str:
    """MOVE_LEECH_SEED -> Leech Seed; TYPE_GRASS -> Grass."""
    body = const[len(prefix):] if const.startswith(prefix) else const
    return titlecase(body.replace("_", " "))


def slug(const: str) -> str:
    """SPECIES_NIDORAN_F -> nidoran-f.

    Derived from the species constant, not the display name: NIDORAN female and
    male both display as "Nidoran" and would otherwise share a filename.
    """
    body = const[len("SPECIES_"):] if const.startswith("SPECIES_") else const
    s = re.sub(r"[^a-z0-9]+", "-", body.lower())
    return s.strip("-") or "unknown"


# --- parsers ---------------------------------------------------------------

def parse_species_names() -> dict[str, str]:
    text = read(DATA / "text" / "species_names.h")
    names = {}
    for const, disp in re.findall(r"\[(SPECIES_\w+)\]\s*=\s*_\(\"([^\"]*)\"\)", text):
        names[const] = titlecase(disp)
    return names


STAT_FIELDS = [
    ("baseHP", "HP"),
    ("baseAttack", "Attack"),
    ("baseDefense", "Defense"),
    ("baseSpAttack", "Sp. Attack"),
    ("baseSpDefense", "Sp. Defense"),
    ("baseSpeed", "Speed"),
]


def parse_species_info() -> dict[str, dict]:
    text = read(MON / "species_info.h")
    info = {}
    # Each entry: [SPECIES_X] = { ... },  -- split on the index markers.
    parts = re.split(r"\[(SPECIES_\w+)\]\s*=\s*", text)
    for const, body in zip(parts[1::2], parts[2::2]):
        types = re.search(r"\.types\s*=\s*\{([^}]*)\}", body)
        abils = re.search(r"\.abilities\s*=\s*\{([^}]*)\}", body)
        tl, al = [], []
        if types:
            tl = [t.strip() for t in types.group(1).split(",") if t.strip()]
        if abils:
            al = [a.strip() for a in abils.group(1).split(",") if a.strip()]

        stats = {}
        for field, label in STAT_FIELDS:
            m = re.search(rf"\.{field}\s*=\s*(\d+)", body)
            if m:
                stats[label] = int(m.group(1))

        info[const] = {"types": tl, "abilities": al, "stats": stats}
    return info


EVO_TEXT = {
    "EVO_FRIENDSHIP": lambda p: "High friendship",
    "EVO_FRIENDSHIP_DAY": lambda p: "High friendship, daytime",
    "EVO_FRIENDSHIP_NIGHT": lambda p: "High friendship, night",
    "EVO_LEVEL": lambda p: f"Level {p}",
    "EVO_TRADE": lambda p: "Trade",
    "EVO_TRADE_ITEM": lambda p: f"Trade holding {const_name(p, 'ITEM_')}",
    "EVO_ITEM": lambda p: f"Use {const_name(p, 'ITEM_')}",
    "EVO_LEVEL_ATK_GT_DEF": lambda p: f"Level {p}, Attack > Defense",
    "EVO_LEVEL_ATK_EQ_DEF": lambda p: f"Level {p}, Attack = Defense",
    "EVO_LEVEL_ATK_LT_DEF": lambda p: f"Level {p}, Attack < Defense",
    "EVO_LEVEL_SILCOON": lambda p: f"Level {p} (Silcoon personality)",
    "EVO_LEVEL_CASCOON": lambda p: f"Level {p} (Cascoon personality)",
    "EVO_LEVEL_NINJASK": lambda p: f"Level {p} (Ninjask)",
    "EVO_LEVEL_SHEDINJA": lambda p: f"Level {p} (Shedinja, needs a free party slot)",
    "EVO_BEAUTY": lambda p: f"Beauty {p}",
    "EVO_LEVEL_FEMALE": lambda p: f"Level {p}, female",
    "EVO_LEVEL_MALE": lambda p: f"Level {p}, male",
    "EVO_MOVE": lambda p: f"Level up knowing {const_name(p, 'MOVE_')}",
}


def parse_evolutions() -> dict[str, list[tuple[str, str]]]:
    """SPECIES_X -> [(target_const, human readable condition), ...]"""
    text = read(MON / "evolution.h")
    evos: dict[str, list[tuple[str, str]]] = {}
    # Split on the [SPECIES_X] = markers: entries span multiple lines when a
    # species has branching evolutions (Eevee, Wurmple, Nincada), so matching
    # balanced braces with a regex is not reliable here.
    parts = re.split(r"\[(SPECIES_\w+)\]\s*=\s*", text)
    for const, body in zip(parts[1::2], parts[2::2]):
        found = []
        for method, param, target in re.findall(
            r"\{\s*(EVO_\w+)\s*,\s*([A-Za-z0-9_]+)\s*,\s*(SPECIES_\w+)\s*\}", body
        ):
            fmt = EVO_TEXT.get(method)
            cond = fmt(param) if fmt else f"{const_name(method, 'EVO_')} ({param})"
            found.append((target, cond))
        if found:
            evos[const] = found
    return evos


def parse_level_up() -> dict[str, list[tuple[int, str]]]:
    ptr_text = read(MON / "level_up_learnset_pointers.h")
    set_text = read(MON / "level_up_learnsets.h")

    arrays: dict[str, list[tuple[int, str]]] = {}
    for name, body in re.findall(
        r"static const u16 (\w+)\[\]\s*=\s*\{(.*?)LEVEL_UP_END", set_text, re.S
    ):
        moves = [
            (int(lvl), mv)
            for lvl, mv in re.findall(r"LEVEL_UP_MOVE\(\s*(\d+)\s*,\s*(MOVE_\w+)\s*\)", body)
        ]
        arrays[name] = moves

    out = {}
    for const, arr in re.findall(r"\[(SPECIES_\w+)\]\s*=\s*(\w+)", ptr_text):
        if const != "SPECIES_NONE" and arr in arrays:
            out[const] = arrays[arr]
    return out


def parse_egg_moves() -> dict[str, list[str]]:
    text = read(MON / "egg_moves.h")
    out = {}
    for name, body in re.findall(r"egg_moves\(\s*(\w+)\s*,(.*?)\)", text, re.S):
        moves = re.findall(r"(MOVE_\w+)", body)
        if moves:
            out[f"SPECIES_{name}"] = moves
    return out


def parse_tmhm() -> tuple[dict[str, list[str]], dict[str, str]]:
    """Returns (species -> [tmhm ids], tmhm id -> label like 'TM06')."""
    tms_text = read(REPO / "include" / "constants" / "tms_hms.h")

    def macro_list(macro: str) -> list[str]:
        m = re.search(rf"#define {macro}\(F\)(.*?)(?=\n#define|\n#endif)", tms_text, re.S)
        return re.findall(r"F\((\w+)\)", m.group(1)) if m else []

    tms = macro_list("FOREACH_TM")
    hms = macro_list("FOREACH_HM")
    labels = {}
    for i, mid in enumerate(tms, 1):
        labels[mid] = f"TM{i:02d}"
    for i, mid in enumerate(hms, 1):
        labels[mid] = f"HM{i:02d}"

    learn_text = read(MON / "tmhm_learnsets.h")
    out: dict[str, list[str]] = {}
    for const, body in re.findall(
        r"\[(SPECIES_\w+)\]\s*=\s*\{\s*\.learnset\s*=\s*\{(.*?)\}\s*\}", learn_text, re.S
    ):
        ids = re.findall(r"\.(\w+)\s*=\s*TRUE", body)
        if ids:
            out[const] = ids
    return out, labels


# --- family grouping -------------------------------------------------------

def build_families(species: list[str], evos: dict[str, list[tuple[str, str]]]):
    """Group species into evolution families, returning roots -> ordered members."""
    pre: dict[str, str] = {}
    for src, targets in evos.items():
        for target, _cond in targets:
            pre.setdefault(target, src)

    known = set(species)

    def root_of(s: str) -> str:
        seen = {s}
        while s in pre and pre[s] in known:
            s = pre[s]
            if s in seen:  # defensive: cyclic data
                break
            seen.add(s)
        return s

    families: dict[str, list[str]] = {}
    for s in species:
        families.setdefault(root_of(s), [])

    # Order members breadth-first from the root so chains read in evolution order.
    for root in families:
        ordered, queue = [], [root]
        while queue:
            cur = queue.pop(0)
            if cur in ordered or cur not in known:
                continue
            ordered.append(cur)
            for target, _c in evos.get(cur, []):
                queue.append(target)
        families[root] = ordered

    # Drop members that got claimed by a family they aren't the root of.
    claimed = set()
    for root in sorted(families, key=species.index):
        families[root] = [m for m in families[root] if m not in claimed]
        claimed.update(families[root])
    return {r: m for r, m in families.items() if m}


# --- rendering -------------------------------------------------------------

def md_escape(text: str) -> str:
    return text.replace("|", "\\|")


def render_species(m, members, names, info, evos, levelup, eggmoves, tmhm, tm_labels):
    """One species: an overview table, then three moveset tables."""
    meta = info.get(m, {})
    # A one-species family already names it in the page title; don't repeat it.
    lines = [] if len(members) == 1 else [f"## {names.get(m, m)}", ""]

    types = [const_name(t, "TYPE_") for t in meta.get("types", [])]
    # A single-typed mon repeats its type in the data; collapse the duplicate.
    if len(types) == 2 and types[0] == types[1]:
        types = types[:1]
    abils = [
        const_name(a, "ABILITY_")
        for a in meta.get("abilities", [])
        if a != "ABILITY_NONE"
    ]
    evo_cell = (
        "<br>".join(f"{names.get(t, t)} ({c})" for t, c in evos.get(m, [])) or "—"
    )

    stats = meta.get("stats", {})
    lines += ["| Attribute | Value |", "| --- | --- |"]
    lines.append(f"| Types | {md_escape(' · '.join(types) or '—')} |")
    lines.append(f"| Abilities | {md_escape(', '.join(abils) or '—')} |")
    lines.append(f"| Evolves into | {md_escape(evo_cell)} |")
    for _field, label in STAT_FIELDS:
        lines.append(f"| {label} | {stats.get(label, '—')} |")
    if stats:
        lines.append(f"| **Stat total** | **{sum(stats.values())}** |")
    lines.append("")

    h = "##" if len(members) == 1 else "###"

    # Level-up moves.
    lines += [f"{h} Level-up moves", ""]
    lu = sorted(levelup.get(m, []), key=lambda p: (p[0], p[1]))
    if lu:
        lines += ["| Level | Move |", "| --- | --- |"]
        for lvl, mv in lu:
            label = "Start" if lvl <= 1 else str(lvl)
            lines.append(f"| {label} | {md_escape(const_name(mv, 'MOVE_'))} |")
    else:
        lines.append("None.")
    lines.append("")

    # TM/HM moves, in TM/HM number order rather than struct order.
    lines += [f"{h} TM/HM moves", ""]
    ids = tmhm.get(m, [])
    ordered = sorted(ids, key=lambda i: tmhm_sort_key(i, tm_labels))
    if ordered:
        lines += ["| TM/HM | Move |", "| --- | --- |"]
        for mid in ordered:
            lines.append(
                f"| {tm_labels.get(mid, 'TM/HM')} | {md_escape(const_name(mid, ''))} |"
            )
    else:
        lines.append("None.")
    lines.append("")

    # Egg moves.
    lines += [f"{h} Egg moves", ""]
    eggs = eggmoves.get(m, [])
    if eggs:
        lines += ["| Move |", "| --- |"]
        for mv in eggs:
            lines.append(f"| {md_escape(const_name(mv, 'MOVE_'))} |")
    else:
        lines.append("None.")
    lines.append("")

    return lines


def tmhm_sort_key(mid: str, tm_labels: dict[str, str]) -> tuple[int, int]:
    """Sort TM01..TM50 before HM01..HM08, numerically within each."""
    label = tm_labels.get(mid, "")
    kind = 0 if label.startswith("TM") else 1
    num = int(label[2:]) if label[2:].isdigit() else 0
    return (kind, num)


def render_family(root, members, names, info, evos, levelup, eggmoves, tmhm, tm_labels) -> str:
    title = names.get(root, root)
    heading = f"{title} line" if len(members) > 1 else title

    lines = [f"# {heading}", ""]

    # Evolution chain, as prose arrows -- readable and diff-friendly.
    if len(members) > 1:
        chain_bits = []
        for m in members:
            for target, cond in evos.get(m, []):
                if target in members:
                    chain_bits.append(
                        f"**{names.get(m, m)}** → *{cond}* → **{names.get(target, target)}**"
                    )
        if chain_bits:
            lines += ["## Evolution", ""] + [f"- {b}" for b in chain_bits] + [""]

    for m in members:
        lines += render_species(
            m, members, names, info, evos, levelup, eggmoves, tmhm, tm_labels
        )

    return "\n".join(lines).rstrip() + "\n"


def generate(app=None) -> int:
    names = parse_species_names()
    info = parse_species_info()
    evos = parse_evolutions()
    levelup = parse_level_up()
    eggmoves = parse_egg_moves()
    tmhm, tm_labels = parse_tmhm()

    # Real species only: skip the SPECIES_NONE sentinel and the OLD_UNOWN padding.
    species = [
        s
        for s in info
        if s != "SPECIES_NONE"
        and not s.startswith("SPECIES_OLD_UNOWN")
        and names.get(s, "?") != "?"
    ]

    families = build_families(species, evos)

    if OUT.exists():
        shutil.rmtree(OUT)
    OUT.mkdir(parents=True)

    order = sorted(families, key=species.index)
    entries = []
    seen_names: dict[str, str] = {}
    for root in order:
        members = families[root]
        name = names.get(root, root)
        fname = slug(root) + ("-line" if len(members) > 1 else "")
        if fname in seen_names:
            raise ValueError(
                f"duplicate generated filename {fname!r} "
                f"(roots {seen_names[fname]} and {root})"
            )
        seen_names[fname] = root
        body = render_family(
            root, members, names, info, evos, levelup, eggmoves, tmhm, tm_labels
        )
        (OUT / f"{fname}.md").write_text(body, encoding="utf-8")
        entries.append((fname, name, members))

    index = [
        "# Evolution families",
        "",
        f"{len(species)} species across {len(entries)} evolution families, "
        "generated from `src/data/pokemon/` at build time.",
        "",
        "| Family | Members |",
        "| --- | --- |",
    ]
    for fname, name, members in entries:
        label = f"{name} line" if len(members) > 1 else name
        joined = ", ".join(names.get(m, m) for m in members)
        index.append(f"| [{md_escape(label)}]({fname}.md) | {md_escape(joined)} |")
    index += ["", "```{toctree}", ":maxdepth: 1", ":hidden:", ""]
    index += [fname for fname, _n, _m in entries]
    index += ["```", ""]
    (OUT / "index.md").write_text("\n".join(index), encoding="utf-8")

    return len(entries)


def on_builder_inited(app):
    count = generate(app)
    from sphinx.util import logging as sphinx_logging

    sphinx_logging.getLogger(__name__).info(
        f"[gen_pokedex] generated {count} evolution family pages"
    )


def setup(app):
    app.connect("builder-inited", on_builder_inited)
    return {"version": "1.0", "parallel_read_safe": True}


if __name__ == "__main__":
    print(f"generated {generate()} family pages into {OUT}")
