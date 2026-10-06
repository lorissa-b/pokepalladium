# Buildings and metatiles

The draft places a whole Emerald building on every Platinum building (town
or route), with its door on Platinum's door. The `.notes` file lists each
placement ("Buildings placed"); the blueprint's header lists anything
unmatched (`no Emerald building fits`, `no door for Platinum's door`).

- A name with `~WxH` was made from parts to Platinum's exact size.
  `draft --originals-only` keeps to the originals' own buildings.
- To see what fits: `buildings <LAYOUT> --render build/mapkit/pieces.png`
  (or `--kind house|mart|pokecenter|gym|lab|gate|other` for a text list).
- To swap one: `buildings <LAYOUT> --piece <NAME> --at X,Y` prints it as a
  blueprint grid to paste over the old one. Add `--size WxH --doors 1,5` to
  make it another size from its parts.
- To copy from another map: `extract <Map> --region X,Y,W,H`, or a `stamp`
  line in the blueprint.
- Keep each building's door on the tile where Platinum has its door warp.

Finding metatiles: `tileset <Map>` draws the catalog as a PNG. For text, use
`tileset <Map> --list --behavior <NAME>` or `--material <NAME>`, never
unfiltered (it prints every metatile, about 15k tokens).

Details: the "Buildings" and "Buildings made from parts" sections of
`docs/map/tooling.md`.
