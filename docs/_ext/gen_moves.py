"""Sphinx extension: generate one move page per type from the decomp data.

Pages land in ``docs/moves/types/`` at build time so they cannot drift from
``src/data/battle_moves.h``. The directory is gitignored; edit the data, not
the output.
"""

from __future__ import annotations

import re
import shutil
from pathlib import Path

from gen_pokedex import const_name, md_escape, read

HERE = Path(__file__).resolve().parent
DOCS = HERE.parent
REPO = HERE.parents[1]
OUT = DOCS / "moves" / "types"

DATA = REPO / "src" / "data"

# Palladium implements the per-move physical/special split, so category is read
# from the move's own .category field rather than inferred from its type.
CATEGORY_LABELS = {
    "MOVE_CATEGORY_PHYSICAL": "Physical",
    "MOVE_CATEGORY_SPECIAL": "Special",
    "MOVE_CATEGORY_STATUS": "Status",
}
# The order categories are reported in, on both the index and the type pages.
CATEGORY_ORDER = ["Physical", "Special", "Status"]


def type_order() -> list[str]:
    """TYPE_ constants in the order the game defines them."""
    text = read(REPO / "include" / "constants" / "pokemon.h")
    found = []
    for name, value in re.findall(r"#define (TYPE_[A-Z_]+)\s+(\d+)", text):
        if name != "TYPE_NONE":
            found.append((int(value), name))
    return [n for _v, n in sorted(found)]


def parse_battle_moves() -> dict[str, dict]:
    text = read(DATA / "battle_moves.h")
    moves = {}
    parts = re.split(r"\[(MOVE_\w+)\]\s*=\s*", text)
    for const, body in zip(parts[1::2], parts[2::2]):
        body = body.split("\n    },", 1)[0]

        def num(field, default=0):
            m = re.search(rf"\.{field}\s*=\s*(\d+)", body)
            return int(m.group(1)) if m else default

        mtype = re.search(r"\.type\s*=\s*(TYPE_\w+)", body)
        effect = re.search(r"\.effect\s*=\s*(EFFECT_\w+)", body)
        cat = re.search(r"\.category\s*=\s*(MOVE_CATEGORY_\w+)", body)
        moves[const] = {
            "type": mtype.group(1) if mtype else "TYPE_NONE",
            "effect": effect.group(1) if effect else "",
            "category": cat.group(1) if cat else "",
            "power": num("power"),
            "pp": num("pp"),
            "accuracy": num("accuracy"),
            "chance": num("secondaryEffectChance"),
        }
    return moves


def parse_contest_moves() -> dict[str, str]:
    text = read(DATA / "contest_moves.h")
    out = {}
    parts = re.split(r"\[(MOVE_\w+)\]\s*=\s*", text)
    for const, body in zip(parts[1::2], parts[2::2]):
        body = body.split("\n    },", 1)[0]
        m = re.search(r"\.contestCategory\s*=\s*(CONTEST_CATEGORY_\w+)", body)
        if m:
            out[const] = const_name(m.group(1), "CONTEST_CATEGORY_")
    return out


def parse_move_descriptions() -> dict[str, str]:
    """MOVE_X -> the in-game description, as one line.

    Mapped through gMoveDescriptionPointers rather than by deriving the array
    name from the move name, so a renamed array cannot silently drop a row.
    """
    text = read(DATA / "text" / "move_descriptions.h")

    bodies = {}
    for name, block in re.findall(
        r"static const u8 (\w+)\[\]\s*=\s*_\((.*?)\);", text, re.S
    ):
        chunks = re.findall(r'"((?:[^"\\]|\\.)*)"', block)
        joined = "".join(chunks).replace("\\n", " ")
        bodies[name] = re.sub(r"\s+", " ", joined).strip()

    out = {}
    for const, arr in re.findall(r"\[(MOVE_\w+)\s*-\s*1\]\s*=\s*(\w+)", text):
        if bodies.get(arr):
            out[const] = bodies[arr]
    return out


def category(move: dict) -> str:
    """Physical, Special or Status, as the move itself declares.

    Read straight from .category rather than derived from power or type: since
    the physical/special split a type can carry moves of all three categories,
    and power alone cannot tell a status move from a fixed-damage one.
    """
    return CATEGORY_LABELS.get(move["category"], "—")


def power_cell(move: dict) -> str:
    if move["power"] == 0:
        return "—"
    if move["power"] == 1:
        # Sentinel: damage is computed by the move's effect, not from power.
        return "Varies"
    return str(move["power"])


def accuracy_cell(move: dict) -> str:
    """0 means no accuracy check: either a self-target or EFFECT_ALWAYS_HIT."""
    if move["accuracy"] == 0:
        return "—"
    return f"{move['accuracy']}%"


def effect_cell(const: str, move: dict, descriptions: dict[str, str]) -> str:
    text = descriptions.get(const, "")
    if not text:
        # Fall back to the effect constant when there is no description.
        effect = move["effect"]
        text = "" if not effect or effect == "EFFECT_HIT" else const_name(effect, "EFFECT_")
    if move["chance"]:
        chance = f"{move['chance']}% chance"
        text = f"{text} ({chance})" if text else chance
    if move["effect"]:
        text = f"{text} `{move['effect']}`" if text else f"`{move['effect']}`"
    return text or "—"


def category_counts(entries: list) -> dict[str, int]:
    """How many of each category, keyed by label, in CATEGORY_ORDER order."""
    counts: dict[str, int] = {}
    for _const, move in entries:
        label = category(move)
        counts[label] = counts.get(label, 0) + 1
    order = CATEGORY_ORDER + [c for c in counts if c not in CATEGORY_ORDER]
    return {c: counts[c] for c in order if c in counts}


def breakdown(entries: list) -> str:
    """Render the counts as prose: `2 physical, 10 special, 2 status`."""
    return ", ".join(
        f"{n} {label.lower()}" for label, n in category_counts(entries).items()
    )


def sort_key(item):
    """Real-power moves by power descending, then Varies, then status moves."""
    const, move = item
    power = move["power"]
    bucket = 0 if power > 1 else (1 if power == 1 else 2)
    return (bucket, -power, const_name(const, "MOVE_"))


def render_type_page(mtype: str, entries: list, contest: dict, descriptions: dict) -> str:
    label = const_name(mtype, "TYPE_")
    lines = [f"# {label} moves", ""]

    noun = "move" if len(entries) == 1 else "moves"
    lines += [
        f"{len(entries)} {label}-type {noun} — {breakdown(entries)}. Category is "
        "a property of the move rather than of its type, so a single type can "
        "carry physical, special and status moves alike. See "
        "{doc}`../../features/physical-special-split`.",
        "",
    ]

    lines += [
        "Rows are ordered by power, not grouped by category. Power **Varies** "
        "means damage is computed by the move's effect rather "
        "than from a power value. Accuracy **—** means the move skips the "
        "accuracy check, either because it cannot miss or because it targets "
        "the user. The `EFFECT_` constant names the implementing case in "
        "`src/battle_script_commands.c`.",
        "",
        "| Move | Category | Power | Accuracy | PP | Contest | Special effects |",
        "| --- | --- | --- | --- | --- | --- | --- |",
    ]
    for const, move in sorted(entries, key=sort_key):
        lines.append(
            f"| {md_escape(const_name(const, 'MOVE_'))} "
            f"| {category(move)} "
            f"| {power_cell(move)} "
            f"| {accuracy_cell(move)} "
            f"| {move['pp'] or '—'} "
            f"| {md_escape(contest.get(const, '—'))} "
            f"| {md_escape(effect_cell(const, move, descriptions))} |"
        )
    lines.append("")
    return "\n".join(lines).rstrip() + "\n"


def generate(app=None) -> int:
    moves = parse_battle_moves()
    contest = parse_contest_moves()
    descriptions = parse_move_descriptions()

    by_type: dict[str, list] = {}
    for const, move in moves.items():
        if const == "MOVE_NONE":
            continue
        by_type.setdefault(move["type"], []).append((const, move))

    ordered = [t for t in type_order() if t in by_type]
    # Any type the canonical list missed still gets a page.
    ordered += [t for t in by_type if t not in ordered]

    if OUT.exists():
        shutil.rmtree(OUT)
    OUT.mkdir(parents=True)

    entries = []
    for mtype in ordered:
        label = const_name(mtype, "TYPE_")
        fname = label.lower()
        (OUT / f"{fname}.md").write_text(
            render_type_page(mtype, by_type[mtype], contest, descriptions),
            encoding="utf-8",
        )
        entries.append(
            (fname, label, category_counts(by_type[mtype]), len(by_type[mtype]))
        )

    totals = {c: 0 for c in CATEGORY_ORDER}
    index = [
        "# Moves by type",
        "",
        f"{sum(n for _f, _l, _c, n in entries)} moves across {len(entries)} "
        "types, generated from `src/data/battle_moves.h` and "
        "`src/data/contest_moves.h` at build time.",
        "",
        "Since Palladium implements the per-move physical/special split, a "
        "type's damaging moves are no longer all one category — the counts "
        "below come from each move's own `.category`. See "
        "{doc}`../../features/physical-special-split`.",
        "",
        "| Type | Moves | " + " | ".join(CATEGORY_ORDER) + " |",
        "| --- | --- | " + " | ".join("---" for _c in CATEGORY_ORDER) + " |",
    ]
    for fname, label, counts, count in entries:
        cells = []
        for cat in CATEGORY_ORDER:
            n = counts.get(cat, 0)
            totals[cat] += n
            cells.append(str(n) if n else "—")
        index.append(
            f"| [{label}]({fname}.md) | {count} | " + " | ".join(cells) + " |"
        )
    index.append(
        f"| **Total** | **{sum(n for _f, _l, _c, n in entries)}** | "
        + " | ".join(f"**{totals[c]}**" for c in CATEGORY_ORDER)
        + " |"
    )
    index += ["", "```{toctree}", ":maxdepth: 1", ":hidden:", ""]
    index += [fname for fname, _l, _c, _n in entries]
    index += ["```", ""]
    (OUT / "index.md").write_text("\n".join(index), encoding="utf-8")

    return len(entries)


def on_builder_inited(app):
    count = generate(app)
    from sphinx.util import logging as sphinx_logging

    sphinx_logging.getLogger(__name__).info(
        f"[gen_moves] generated {count} move type pages"
    )


def setup(app):
    app.connect("builder-inited", on_builder_inited)
    return {"version": "1.0", "parallel_read_safe": True}


if __name__ == "__main__":
    print(f"generated {generate()} type pages into {OUT}")
