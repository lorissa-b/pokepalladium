"""Sphinx extension: generate Pokedex pages from the decomp's data headers.

One page per evolution family, written into ``docs/pokedex/families/`` at build
time so the docs can never drift from ``src/data/pokemon/``. The generated
directory is gitignored; edit the data headers, not the output.
"""

from __future__ import annotations

import html
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
    "EVO_ITEM_MALE": lambda p: f"Use {const_name(p, 'ITEM_')}, male",
    "EVO_ITEM_FEMALE": lambda p: f"Use {const_name(p, 'ITEM_')}, female",
    "EVO_ITEM_HOLD_DAY": lambda p: f"Level up holding {const_name(p, 'ITEM_')}, daytime",
    "EVO_ITEM_HOLD_NIGHT": lambda p: f"Level up holding {const_name(p, 'ITEM_')}, night",
    "EVO_PARTY_SPECIES": lambda p: f"Level up with {const_name(p, 'SPECIES_')} in the party",
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


def parse_sprites() -> dict[str, Path]:
    """SPECIES_X -> the front sprite PNG the game uses for its summary screen.

    Follows the still-front-pic table to the INCGFX path so forms like Unown A
    resolve to the right file. Castform's front pic is built from a combined
    .4bpp, so fall back to its normal-form PNG in that case.
    """
    paths = {}
    gfx = read(DATA / "graphics" / "pokemon.h")
    for sym, path in re.findall(r"const u32 (gMonStillFrontPic_\w+)\[\]\s*=\s*\w+\(\"([^\"]+)\"", gfx):
        paths[sym] = REPO / path

    table = read(DATA / "pokemon_graphics" / "still_front_pic_table.h")
    out = {}
    for name, sym in re.findall(r"SPECIES_SPRITE\(\s*(\w+)\s*,\s*(\w+)\s*\)", table):
        path = paths.get(sym)
        if path is None:
            continue
        if path.suffix != ".png":
            path = path.parent / "normal" / "front.png"
        if path.exists():
            out[f"SPECIES_{name}"] = path
    return out


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


def species_heading(level: str, m: str, names: dict[str, str], sprites: dict[str, Path]) -> str:
    """A heading with the species' front sprite in front of its name."""
    name = names.get(m, m)
    if m not in sprites:
        return f"{level} {name}"
    return f"{level} ![{name}](sprites/{slug(m)}.png) {name}"


def render_species(m, members, names, info, evos, levelup, eggmoves, tmhm, tm_labels, sprites):
    """One species: its sprite and name, an overview table, then its moves."""
    meta = info.get(m, {})
    # A one-species family names it in the page title, sprite and all.
    lines = [] if len(members) == 1 else [species_heading("##", m, names, sprites), ""]

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
    lines += [f"{h} Moves", ""]

    lu = [
        ("Start" if lvl <= 1 else str(lvl), const_name(mv, "MOVE_"))
        for lvl, mv in sorted(levelup.get(m, []), key=lambda p: (p[0], p[1]))
    ]
    # TM/HM moves in TM/HM number order rather than struct order.
    tms = [
        (tm_labels.get(mid, "TM/HM"), const_name(mid, ""))
        for mid in sorted(tmhm.get(m, []), key=lambda i: tmhm_sort_key(i, tm_labels))
    ]
    eggs = [const_name(mv, "MOVE_") for mv in eggmoves.get(m, [])]

    lines += render_moves_table(lu, tms, eggs)
    lines.append("")
    return lines


def render_moves_table(lu, tms, eggs) -> list[str]:
    """Level-up, TM/HM and egg moves side by side in one table.

    Markdown tables can't span columns, so this is raw HTML. The columns are
    independent lists; each row just pairs up the nth entry of each.
    """
    e = html.escape
    none = '<td colspan="{}"><em>None</em></td>'

    rows = []
    for i in range(max(len(lu), len(tms), len(eggs), 1)):
        cells = []
        if i < len(lu):
            cells += [f"<td>{e(lu[i][0])}</td>", f"<td>{e(lu[i][1])}</td>"]
        else:
            cells.append(none.format(2) if i == 0 else '<td colspan="2"></td>')
        if i < len(tms):
            cells += [f"<td>{e(tms[i][0])}</td>", f"<td>{e(tms[i][1])}</td>"]
        else:
            cells.append(none.format(2) if i == 0 else '<td colspan="2"></td>')
        if i < len(eggs):
            cells.append(f"<td>{e(eggs[i])}</td>")
        else:
            cells.append(none.format(1) if i == 0 else "<td></td>")
        rows.append("<tr>" + "".join(cells) + "</tr>")

    return [
        '<table class="docutils align-default moves-table">',
        "<thead>",
        '<tr><th colspan="2">Level-up</th><th colspan="2">TM / HM</th>'
        '<th rowspan="2">Egg</th></tr>',
        "<tr><th>Level</th><th>Move</th><th>TM / HM</th><th>Move</th></tr>",
        "</thead>",
        "<tbody>",
        *rows,
        "</tbody>",
        "</table>",
    ]


def tmhm_sort_key(mid: str, tm_labels: dict[str, str]) -> tuple[int, int]:
    """Sort TM01..TM50 before HM01..HM08, numerically within each."""
    label = tm_labels.get(mid, "")
    kind = 0 if label.startswith("TM") else 1
    num = int(label[2:]) if label[2:].isdigit() else 0
    return (kind, num)


def render_family(root, members, names, info, evos, levelup, eggmoves, tmhm, tm_labels, sprites) -> str:
    title = names.get(root, root)
    if len(members) > 1:
        heading = f"# {title} line"
    else:
        heading = species_heading("#", root, names, sprites)

    lines = [heading, ""]

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
            m, members, names, info, evos, levelup, eggmoves, tmhm, tm_labels, sprites
        )

    return "\n".join(lines).rstrip() + "\n"


def generate(app=None) -> int:
    names = parse_species_names()
    info = parse_species_info()
    evos = parse_evolutions()
    levelup = parse_level_up()
    eggmoves = parse_egg_moves()
    tmhm, tm_labels = parse_tmhm()
    sprites = parse_sprites()

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
    (OUT / "sprites").mkdir()

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
            root, members, names, info, evos, levelup, eggmoves, tmhm, tm_labels, sprites
        )
        for m in members:
            if m in sprites:
                shutil.copyfile(sprites[m], OUT / "sprites" / f"{slug(m)}.png")
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
