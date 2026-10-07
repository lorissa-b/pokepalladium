# Handing the layout to a subagent

Steps 1-6 (reference, target, draft, detail, build, score) produce most of
the output: grids, notes, renders and compare runs. Run them in a subagent
(the Agent tool) so that output stays in its context and only a summary
comes back. Keep steps 7-8 (events and checks) in the main conversation.

The subagent starts with no context, so brief it fully. Fill in this brief:

```
Redraw <Hoenn map dir> (layout <LAYOUT>) as Platinum's <HEADER> using
tools/mapkit/mapkit.py. Follow steps 1-6 of .claude/skills/sinnoh-map/SKILL.md
and its Rules; read its reference files only when a step needs them.

What this map should be: <what the user asked for: whole map or a region,
style or cave, buildings to keep or change, anything deliberately
different from Platinum>.
Neighbours: <maps it connects to, from `info`>.

Build the map.bin and stop before moving events. Don't commit.

Reply with only:
- the build and compare summary lines (blocks changed, score, origin);
- the mismatch areas you left on purpose, and why;
- Platinum warps compare lists as missing;
- the paths of the blueprint, its .notes file and the final compare PNG.
```

When it reports back, Read the final compare PNG yourself before moving on,
then do the events from the `.notes` file (`grep`, not Read).

Skip the subagent for a small interior, where the whole job is a few short
outputs.
