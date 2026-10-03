# Bag capacity

## What changed

- **Stacks hold 999.** Every bag pocket stacks an item up to 999, up from 99.
  The bag and its toss and sell prompts show three digits.
- **The Items pocket holds 120 different items**, up from 30.

With 120 stacks of 999, the bag can hold up to 119,880 of one item, so the
Poké Mart's "IN BAG" count shows six digits.

## Where it lives

| What | Where |
| --- | --- |
| `MAX_BAG_ITEM_CAPACITY`, `BAG_ITEM_CAPACITY_DIGITS` | `include/constants/items.h` |
| `BAG_ITEMS_COUNT` | `include/constants/global.h` |
| Battle Pyramid bag quantities (now `u16`) | `struct PyramidBag` in `include/global.h` |
| Poké Mart "IN BAG" count | `CountTotalItemQuantityInBag` in `src/item.c`, `Task_BuyHowManyDialogueInit` in `src/shop.c` |

The other pockets' sizes (`BAG_KEYITEMS_COUNT` and so on) sit next to
`BAG_ITEMS_COUNT`. Each slot is 4 bytes of SaveBlock1, and changing any of them
breaks existing saves.

## Upstream credit

The pokeemerald wiki's
[Increase bag item capacity to 999 items in a stack](https://github.com/pret/pokeemerald/wiki/Increase-item-bag-capacity)
(tenaya15's `999_item_stack` branch, after MapleFall and Lunos) and
[Make the Bag Able to Hold 120 Items Instead of 30](https://github.com/pret/pokeemerald/wiki/Make-the-Bag-Able-to-Hold-120-Items-Instead-of-30).
