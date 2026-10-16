#include "global.h"
#include "bg.h"
#include "decompress.h"
#include "event_data.h"
#include "gpu_regs.h"
#include "item.h"
#include "item_icon.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "mining.h"
#include "overworld.h"
#include "palette.h"
#include "random.h"
#include "scanline_effect.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "window.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/rgb.h"
#include "constants/songs.h"

// The Explorer Kit's rock wall: Platinum's Underground wall-mining minigame
// (pokeplatinum src/underground/mining.c), drawn with Emerald's own message
// box, window frames, Yes/No menu, region map cursor and bag item icons.

#define MINING_WIDTH  13
#define MINING_HEIGHT 10
#define CELL_TILES    2  // each cell is 16x16 pixels

#define MAX_BURIED_ITEMS       8
#define MAX_BURIED_OBJECTS     8
#define INITIAL_WALL_INTEGRITY 196
#define PICKAXE_WALL_DAMAGE    4
#define HAMMER_WALL_DAMAGE     8

#define TAG_CURSOR    0x5600
#define TAG_ITEM_ICON 0x5610

enum {
    TOOL_PICKAXE,
    TOOL_HAMMER,
};

// 16x16 metatiles in graphics/mining/tiles.png, left to right
enum {
    METATILE_CLEAR,
    METATILE_DIRT_1,  // METATILE_DIRT_1 to METATILE_DIRT_6 follow the dirt depth
    METATILE_BARE_WALL = 7,
    METATILE_TREASURE,
    METATILE_ROCK,
    METATILE_COUNT
};

enum {
    WIN_MSG,  // the standard message box, as in the overworld
    WIN_WALL_METER,
};

// Where the standard frames and text box sit, as in the overworld (see src/menu.c)
#define DLG_WINDOW_PALETTE_NUM 15

#define MAX_CURSOR_SPRITES 9

struct MiningObject
{
    const u8 *shape;  // 'x' for a cell the object fills, 'o' for an empty one; NULL if it fills its whole rectangle
    u16 itemId;       // ITEM_NONE for a rock
    u8 width;
    u8 height;
    u8 weights[4];
};

struct BuriedObject
{
    const struct MiningObject *object;
    u8 x;
    u8 y;
    bool8 dugUp;
    u8 iconSpriteId;
};

struct MiningGame
{
    u8 dirt[MINING_HEIGHT][MINING_WIDTH];
    u8 objectGrid[MINING_HEIGHT][MINING_WIDTH];  // index into buried + 1, or 0
    struct BuriedObject buried[MAX_BURIED_OBJECTS];
    u8 objectCount;
    u8 itemCount;  // the first itemCount buried objects are items; the rest are rocks
    u8 wallIntegrity;
    u8 tool;
    s8 cursorX;
    s8 cursorY;
    u8 cursorSpriteIds[MAX_CURSOR_SPRITES];
    u8 shakeTimer;
    u8 collapseTimer;
    u8 sparkleTimer;
    u8 nextItem;
    u8 timer;
    u16 dirtTilemap[BG_SCREEN_SIZE / 2];
    u16 wallTilemap[BG_SCREEN_SIZE / 2];
};

static EWRAM_DATA struct MiningGame *sMining = NULL;

static void CB2_MiningGame(void);
static void VBlankCB_MiningGame(void);
static void Task_MiningGame_WaitFadeIn(u8 taskId);
static void Task_MiningGame_ShowHint(u8 taskId);
static void Task_MiningGame_WaitForMessage(u8 taskId);
static void Task_MiningGame_HandleInput(u8 taskId);
static void Task_MiningGame_ConfirmStop(u8 taskId);
static void Task_MiningGame_HandleStopYesNo(u8 taskId);
static void Task_MiningGame_EverythingDugUp(u8 taskId);
static void Task_MiningGame_Collapse(u8 taskId);
static void Task_MiningGame_GiveNextItem(u8 taskId);
static void Task_MiningGame_WaitForItemMessage(u8 taskId);
static void Task_MiningGame_FadeOut(u8 taskId);
static void Task_MiningGame_Exit(u8 taskId);

static const u8 sText_SomethingPinged[] = _("Something pinged in the wall!\n{STR_VAR_1} confirmed!");
static const u8 sText_ControlsHint[] = _("Press {A_BUTTON} to dig, and {R_BUTTON} to switch\nbetween the pickaxe and the hammer.");
static const u8 sText_StopDigging[] = _("Stop digging?");
static const u8 sText_EverythingDugUp[] = _("Everything was dug up!");
static const u8 sText_WallCollapsed[] = _("The wall collapsed!");
static const u8 sText_ItemObtained[] = _("{STR_VAR_1}\nwas obtained!");
static const u8 sText_BagIsFull[] = _("Too bad! The BAG is full…\n{STR_VAR_1} was left behind.");

static const u16 sMiningTiles_Pal[] = INCGFX_U16("graphics/mining/tiles.png", ".gbapal");
static const u32 sMiningTiles_Gfx[] = INCGFX_U32("graphics/mining/tiles.png", ".4bpp.lz");

// The region map's blinking cursor marks every cell the current tool hits.
static const u16 sCursor_Pal[] = INCGFX_U16("graphics/pokenav/region_map/cursor.pal", ".gbapal");
static const u32 sCursor_Gfx[] = INCGFX_U32("graphics/pokenav/region_map/cursor_small.png", ".4bpp.lz");

static const struct CompressedSpriteSheet sSpriteSheet_Cursor = {sCursor_Gfx, 0x100, TAG_CURSOR};
static const struct SpritePalette sSpritePalette_Cursor = {sCursor_Pal, TAG_CURSOR};

static const struct OamData sOam_Cursor =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .size = SPRITE_SIZE(16x16),
    .priority = 1,
};

static const union AnimCmd sAnim_Cursor[] =
{
    ANIMCMD_FRAME(0, 20),
    ANIMCMD_FRAME(4, 20),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd *const sAnims_Cursor[] =
{
    sAnim_Cursor
};

static const struct SpriteTemplate sSpriteTemplate_Cursor =
{
    .tileTag = TAG_CURSOR,
    .paletteTag = TAG_CURSOR,
    .oam = &sOam_Cursor,
    .anims = sAnims_Cursor,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

// A dug-up item shows its bag icon, scaled up to fill its spot in the wall.
static const struct OamData sOam_ItemIcon =
{
    .affineMode = ST_OAM_AFFINE_DOUBLE,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .size = SPRITE_SIZE(32x32),
    .priority = 1,
};

static const union AnimCmd sAnim_ItemIcon[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_ItemIcon[] =
{
    sAnim_ItemIcon
};

enum {
    ICON_SCALE_SMALL,  // for items 2 cells across
    ICON_SCALE_LARGE,  // for items 3 or more cells across
};

static const union AffineAnimCmd sAffineAnim_ItemIconSmall[] =
{
    AFFINEANIMCMD_FRAME(256, 256, 0, 0),
    AFFINEANIMCMD_FRAME(8, 8, 0, 11),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_ItemIconLarge[] =
{
    AFFINEANIMCMD_FRAME(256, 256, 0, 0),
    AFFINEANIMCMD_FRAME(16, 16, 0, 16),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sAffineAnims_ItemIcon[] =
{
    [ICON_SCALE_SMALL] = sAffineAnim_ItemIconSmall,
    [ICON_SCALE_LARGE] = sAffineAnim_ItemIconLarge,
};

static const struct SpriteTemplate sSpriteTemplate_ItemIcon =
{
    .tileTag = 0,
    .paletteTag = 0,
    .oam = &sOam_ItemIcon,
    .anims = sAnims_ItemIcon,
    .images = NULL,
    .affineAnims = sAffineAnims_ItemIcon,
    .callback = SpriteCallbackDummy,
};

static const struct BgTemplate sBgTemplates[] =
{
    {   // Message box and Yes/No menu
        .bg = 0,
        .charBaseIndex = 2,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    },
    {   // Dirt
        .bg = 1,
        .charBaseIndex = 0,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0
    },
    {   // The wall behind the dirt, with the buried items and rocks
        .bg = 2,
        .charBaseIndex = 0,
        .mapBaseIndex = 29,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 3,
        .baseTile = 0
    },
    {   // Wall meter
        .bg = 3,
        .charBaseIndex = 2,
        .mapBaseIndex = 28,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0
    },
};

static const struct WindowTemplate sWindowTemplates[] =
{
    [WIN_MSG] = {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 15,
        .width = 27,
        .height = 4,
        .paletteNum = DLG_WINDOW_PALETTE_NUM,
        .baseBlock = 0x194
    },
    [WIN_WALL_METER] = {
        .bg = 3,
        .tilemapLeft = 27,
        .tilemapTop = 1,
        .width = 2,
        .height = 18,
        .paletteNum = DLG_WINDOW_PALETTE_NUM,
        .baseBlock = 0x001
    },
    DUMMY_WIN_TEMPLATE
};

// The wall meter's bar, inside its window
#define METER_BAR_X      4
#define METER_BAR_Y      4
#define METER_BAR_WIDTH  8
#define METER_BAR_HEIGHT 136

// Platinum's dirt stamps: 10 patches of depth 4, then 15 of depth 6 on top
static const u8 sDirtDepth4Stamp[8][8] =
{
    { 0, 0, 4, 4, 4, 4, 0, 0 },
    { 0, 4, 4, 4, 4, 4, 4, 0 },
    { 4, 4, 4, 4, 4, 4, 0, 0 },
    { 4, 4, 4, 4, 4, 4, 0, 0 },
    { 4, 4, 4, 4, 4, 4, 0, 0 },
    { 4, 4, 4, 4, 4, 4, 0, 0 },
    { 0, 4, 4, 4, 4, 4, 4, 0 },
    { 0, 0, 4, 4, 4, 4, 0, 0 },
};

static const u8 sDirtDepth6Stamp[5][5] =
{
    { 0, 6, 6, 6, 0 },
    { 6, 6, 6, 6, 6 },
    { 6, 6, 6, 6, 6 },
    { 6, 6, 6, 6, 6 },
    { 0, 6, 6, 6, 0 },
};

// Platinum's buried objects, minus the spheres, Oval Stone and Odd Keystone
static const u8 sSunStoneShape[] = {
    'o', 'x', 'o',
    'x', 'x', 'x',
    'x', 'x', 'x',
};

static const u8 sStarPieceReviveShape[] = {
    'o', 'x', 'o',
    'x', 'x', 'x',
    'o', 'x', 'o',
};

static const u8 sMoonStoneShape[] = {
    'o', 'x', 'x', 'x',
    'x', 'x', 'x', 'o',
};

static const u8 sMoonStoneShape90deg[] = {
    'x', 'o',
    'x', 'x',
    'x', 'x',
    'o', 'x',
};

static const u8 sThunderstoneShape[] = {
    'o', 'x', 'x',
    'x', 'x', 'x',
    'x', 'x', 'o',
};

static const u8 sWaterStoneShape[] = {
    'x', 'x', 'x',
    'x', 'x', 'x',
    'x', 'x', 'o',
};

static const u8 sLeafStoneShape[] = {
    'o', 'x', 'o',
    'x', 'x', 'x',
    'x', 'x', 'x',
    'o', 'x', 'o',
};

static const u8 sLeafStoneShape90deg[] = {
    'o', 'x', 'x', 'o',
    'x', 'x', 'x', 'x',
    'o', 'x', 'x', 'o',
};

static const u8 sHelixFossilShape[] = {
    'o', 'x', 'x', 'x',
    'x', 'x', 'x', 'x',
    'x', 'x', 'x', 'x',
    'x', 'x', 'x', 'o',
};

static const u8 sHelixFossilShape90deg[] = {
    'x', 'x', 'x', 'o',
    'x', 'x', 'x', 'x',
    'x', 'x', 'x', 'x',
    'o', 'x', 'x', 'x',
};

static const u8 sDomeFossilShape[] = {
    'x', 'x', 'x', 'x', 'x',
    'x', 'x', 'x', 'x', 'x',
    'x', 'x', 'x', 'x', 'x',
    'o', 'x', 'x', 'x', 'o',
};

static const u8 sClawFossilShape[] = {
    'o', 'o', 'x', 'x',
    'o', 'x', 'x', 'x',
    'o', 'x', 'x', 'x',
    'x', 'x', 'x', 'o',
    'x', 'x', 'o', 'o',
};

static const u8 sClawFossilShape90deg[] = {
    'x', 'x', 'o', 'o', 'o',
    'x', 'x', 'x', 'x', 'o',
    'o', 'x', 'x', 'x', 'x',
    'o', 'o', 'x', 'x', 'x',
};

static const u8 sClawFossilShape180deg[] = {
    'o', 'o', 'x', 'x',
    'o', 'x', 'x', 'x',
    'x', 'x', 'x', 'o',
    'x', 'x', 'x', 'o',
    'x', 'x', 'o', 'o',
};

static const u8 sClawFossilShape270deg[] = {
    'x', 'x', 'x', 'o', 'o',
    'x', 'x', 'x', 'x', 'o',
    'o', 'x', 'x', 'x', 'x',
    'o', 'o', 'o', 'x', 'x',
};

static const u8 sRootFossilShape[] = {
    'x', 'x', 'x', 'x', 'o',
    'x', 'x', 'x', 'x', 'x',
    'x', 'x', 'o', 'x', 'x',
    'o', 'o', 'o', 'x', 'x',
    'o', 'o', 'x', 'x', 'o',
};

static const u8 sRootFossilShape90deg[] = {
    'o', 'o', 'x', 'x', 'x',
    'o', 'o', 'x', 'x', 'x',
    'x', 'o', 'o', 'x', 'x',
    'x', 'x', 'x', 'x', 'x',
    'o', 'x', 'x', 'x', 'o',
};

static const u8 sRootFossilShape180deg[] = {
    'o', 'x', 'x', 'o', 'o',
    'x', 'x', 'o', 'o', 'o',
    'x', 'x', 'o', 'x', 'x',
    'x', 'x', 'x', 'x', 'x',
    'o', 'x', 'x', 'x', 'x',
};

static const u8 sRootFossilShape270deg[] = {
    'o', 'x', 'x', 'x', 'o',
    'x', 'x', 'x', 'x', 'x',
    'x', 'x', 'o', 'o', 'x',
    'x', 'x', 'x', 'o', 'o',
    'x', 'x', 'x', 'o', 'o',
};

static const u8 sOldAmberShape[] = {
    'o', 'x', 'x', 'x',
    'x', 'x', 'x', 'x',
    'x', 'x', 'x', 'x',
    'x', 'x', 'x', 'o',
};

static const u8 sOldAmberShape90deg[] = {
    'x', 'x', 'x', 'o',
    'x', 'x', 'x', 'x',
    'x', 'x', 'x', 'x',
    'o', 'x', 'x', 'x',
};

static const u8 sRareBoneShape[] = {
    'x', 'x', 'x',
    'o', 'x', 'o',
    'o', 'x', 'o',
    'o', 'x', 'o',
    'o', 'x', 'o',
    'x', 'x', 'x',
};

static const u8 sRareBoneShape90deg[] = {
    'x', 'o', 'o', 'o', 'o', 'x',
    'x', 'x', 'x', 'x', 'x', 'x',
    'x', 'o', 'o', 'o', 'o', 'x',
};

static const u8 sRedShardShape[] = {
    'x', 'x', 'x',
    'x', 'x', 'o',
    'x', 'x', 'x',
};

static const u8 sBlueShardShape[] = {
    'x', 'x', 'x',
    'x', 'x', 'x',
    'x', 'x', 'o',
};

static const u8 sYellowShardShape[] = {
    'x', 'o', 'x', 'o',
    'x', 'x', 'x', 'o',
    'x', 'x', 'x', 'x',
};

static const u8 sGreenShardShape[] = {
    'x', 'x', 'x', 'x',
    'x', 'x', 'x', 'x',
    'x', 'x', 'o', 'x',
};

static const u8 sHeartScaleShape[] = {
    'x', 'o',
    'x', 'x',
};

static const u8 sArmorFossilShape[] = {
    'o', 'x', 'x', 'x', 'o',
    'o', 'x', 'x', 'x', 'o',
    'x', 'x', 'x', 'x', 'x',
    'o', 'x', 'x', 'x', 'o',
};

static const u8 sSkullFossilShape[] = {
    'x', 'x', 'x', 'x',
    'x', 'x', 'x', 'x',
    'x', 'x', 'x', 'x',
    'o', 'x', 'x', 'o',
};

static const u8 sLightClayShape[] = {
    'x', 'o', 'x', 'o',
    'x', 'x', 'x', 'x',
    'x', 'x', 'x', 'x',
    'o', 'x', 'o', 'x',
};

static const u8 sIcyRockShape[] = {
    'o', 'x', 'x', 'o',
    'x', 'x', 'x', 'x',
    'x', 'x', 'x', 'x',
    'x', 'o', 'o', 'x',
};

static const u8 sSmoothRockShape[] = {
    'o', 'o', 'x', 'o',
    'x', 'x', 'x', 'o',
    'o', 'x', 'x', 'x',
    'o', 'x', 'o', 'o',
};

static const u8 sHeatRockShape[] = {
    'x', 'o', 'x', 'o',
    'x', 'x', 'x', 'x',
    'x', 'x', 'x', 'x',
};

static const u8 sDampRockShape[] = {
    'x', 'x', 'x',
    'x', 'x', 'x',
    'x', 'o', 'x',
};

static const u8 sRockTShape[] = {
    'x', 'x', 'x',
    'o', 'x', 'o',
};

static const u8 sRockTShape90deg[] = {
    'o', 'x',
    'x', 'x',
    'o', 'x',
};

static const u8 sRockTShape180deg[] = {
    'o', 'x', 'o',
    'x', 'x', 'x',
};

static const u8 sRockTShape270deg[] = {
    'x', 'o',
    'x', 'x',
    'x', 'o',
};

static const u8 sRockZShape[] = {
    'x', 'x', 'o',
    'o', 'x', 'x',
};

static const u8 sRockZShape90deg[] = {
    'o', 'x',
    'x', 'x',
    'x', 'o',
};

static const u8 sRockSShape[] = {
    'o', 'x', 'x',
    'x', 'x', 'o',
};

static const u8 sRockSShape90deg[] = {
    'x', 'o',
    'x', 'x',
    'o', 'x',
};

// Weights: odd Trainer ID, even Trainer ID, then the same after the National Pokédex.
static const struct MiningObject sMiningItems[] =
{
    { .itemId = ITEM_SUN_STONE, .width = 3, .height = 3, .shape = sSunStoneShape, .weights = {4, 1, 15, 3} },
    { .itemId = ITEM_STAR_PIECE, .width = 3, .height = 3, .shape = sStarPieceReviveShape, .weights = {2, 2, 10, 10} },
    { .itemId = ITEM_MOON_STONE, .width = 4, .height = 2, .shape = sMoonStoneShape, .weights = {1, 2, 1, 8} },
    { .itemId = ITEM_MOON_STONE, .width = 2, .height = 4, .shape = sMoonStoneShape90deg, .weights = {1, 2, 2, 7} },
    { .itemId = ITEM_HARD_STONE, .width = 2, .height = 2, .shape = NULL, .weights = {4, 4, 20, 20} },
    { .itemId = ITEM_THUNDER_STONE, .width = 3, .height = 3, .shape = sThunderstoneShape, .weights = {4, 1, 30, 5} },
    { .itemId = ITEM_EVERSTONE, .width = 4, .height = 2, .shape = NULL, .weights = {4, 4, 20, 20} },
    { .itemId = ITEM_FIRE_STONE, .width = 3, .height = 3, .shape = NULL, .weights = {4, 1, 30, 5} },
    { .itemId = ITEM_WATER_STONE, .width = 3, .height = 3, .shape = sWaterStoneShape, .weights = {1, 4, 5, 30} },
    { .itemId = ITEM_LEAF_STONE, .width = 3, .height = 4, .shape = sLeafStoneShape, .weights = {1, 2, 3, 15} },
    { .itemId = ITEM_LEAF_STONE, .width = 4, .height = 3, .shape = sLeafStoneShape90deg, .weights = {1, 2, 2, 15} },
    { .itemId = ITEM_HELIX_FOSSIL, .width = 4, .height = 4, .shape = sHelixFossilShape, .weights = {0, 0, 3, 1} },
    { .itemId = ITEM_HELIX_FOSSIL, .width = 4, .height = 4, .shape = sHelixFossilShape90deg, .weights = {0, 0, 3, 1} },
    { .itemId = ITEM_HELIX_FOSSIL, .width = 4, .height = 4, .shape = sHelixFossilShape, .weights = {0, 0, 3, 1} },
    { .itemId = ITEM_HELIX_FOSSIL, .width = 4, .height = 4, .shape = sHelixFossilShape90deg, .weights = {0, 0, 3, 1} },
    { .itemId = ITEM_DOME_FOSSIL, .width = 5, .height = 4, .shape = sDomeFossilShape, .weights = {0, 0, 1, 13} },
    { .itemId = ITEM_CLAW_FOSSIL, .width = 4, .height = 5, .shape = sClawFossilShape, .weights = {0, 0, 3, 1} },
    { .itemId = ITEM_CLAW_FOSSIL, .width = 5, .height = 4, .shape = sClawFossilShape90deg, .weights = {0, 0, 3, 1} },
    { .itemId = ITEM_CLAW_FOSSIL, .width = 4, .height = 5, .shape = sClawFossilShape180deg, .weights = {0, 0, 3, 1} },
    { .itemId = ITEM_CLAW_FOSSIL, .width = 5, .height = 4, .shape = sClawFossilShape270deg, .weights = {0, 0, 3, 1} },
    { .itemId = ITEM_ROOT_FOSSIL, .width = 5, .height = 5, .shape = sRootFossilShape, .weights = {0, 0, 1, 3} },
    { .itemId = ITEM_ROOT_FOSSIL, .width = 5, .height = 5, .shape = sRootFossilShape90deg, .weights = {0, 0, 1, 3} },
    { .itemId = ITEM_ROOT_FOSSIL, .width = 5, .height = 5, .shape = sRootFossilShape180deg, .weights = {0, 0, 1, 3} },
    { .itemId = ITEM_ROOT_FOSSIL, .width = 5, .height = 5, .shape = sRootFossilShape270deg, .weights = {0, 0, 1, 3} },
    { .itemId = ITEM_OLD_AMBER, .width = 4, .height = 4, .shape = sOldAmberShape, .weights = {0, 0, 2, 2} },
    { .itemId = ITEM_OLD_AMBER, .width = 4, .height = 4, .shape = sOldAmberShape90deg, .weights = {0, 0, 3, 3} },
    { .itemId = ITEM_RARE_BONE, .width = 3, .height = 6, .shape = sRareBoneShape, .weights = {1, 1, 5, 5} },
    { .itemId = ITEM_RARE_BONE, .width = 6, .height = 3, .shape = sRareBoneShape90deg, .weights = {1, 1, 5, 5} },
    { .itemId = ITEM_REVIVE, .width = 3, .height = 3, .shape = sStarPieceReviveShape, .weights = {8, 8, 10, 10} },
    { .itemId = ITEM_MAX_REVIVE, .width = 3, .height = 3, .shape = NULL, .weights = {1, 1, 2, 2} },
    { .itemId = ITEM_RED_SHARD, .width = 3, .height = 3, .shape = sRedShardShape, .weights = {13, 13, 17, 17} },
    { .itemId = ITEM_BLUE_SHARD, .width = 3, .height = 3, .shape = sBlueShardShape, .weights = {13, 13, 17, 17} },
    { .itemId = ITEM_YELLOW_SHARD, .width = 4, .height = 3, .shape = sYellowShardShape, .weights = {13, 13, 17, 17} },
    { .itemId = ITEM_GREEN_SHARD, .width = 4, .height = 3, .shape = sGreenShardShape, .weights = {13, 13, 17, 17} },
    { .itemId = ITEM_HEART_SCALE, .width = 2, .height = 2, .shape = sHeartScaleShape, .weights = {33, 33, 30, 30} },
    { .itemId = ITEM_ARMOR_FOSSIL, .width = 5, .height = 4, .shape = sArmorFossilShape, .weights = {0, 25, 0, 12} },
    { .itemId = ITEM_SKULL_FOSSIL, .width = 4, .height = 4, .shape = sSkullFossilShape, .weights = {25, 0, 12, 0} },
    { .itemId = ITEM_LIGHT_CLAY, .width = 4, .height = 4, .shape = sLightClayShape, .weights = {1, 1, 5, 2} },
    { .itemId = ITEM_IRON_BALL, .width = 3, .height = 3, .shape = NULL, .weights = {1, 1, 2, 5} },
    { .itemId = ITEM_ICY_ROCK, .width = 4, .height = 4, .shape = sIcyRockShape, .weights = {2, 1, 11, 5} },
    { .itemId = ITEM_SMOOTH_ROCK, .width = 4, .height = 4, .shape = sSmoothRockShape, .weights = {1, 2, 5, 11} },
    { .itemId = ITEM_HEAT_ROCK, .width = 4, .height = 3, .shape = sHeatRockShape, .weights = {2, 1, 11, 5} },
    { .itemId = ITEM_DAMP_ROCK, .width = 3, .height = 3, .shape = sDampRockShape, .weights = {1, 2, 5, 11} },
    { .itemId = ITEM_FLAME_PLATE, .width = 4, .height = 3, .shape = NULL, .weights = {1, 1, 1, 1} },
    { .itemId = ITEM_SPLASH_PLATE, .width = 4, .height = 3, .shape = NULL, .weights = {1, 1, 1, 1} },
    { .itemId = ITEM_ZAP_PLATE, .width = 4, .height = 3, .shape = NULL, .weights = {1, 1, 1, 1} },
    { .itemId = ITEM_MEADOW_PLATE, .width = 4, .height = 3, .shape = NULL, .weights = {1, 1, 1, 1} },
    { .itemId = ITEM_ICICLE_PLATE, .width = 4, .height = 3, .shape = NULL, .weights = {1, 1, 1, 1} },
    { .itemId = ITEM_FIST_PLATE, .width = 4, .height = 3, .shape = NULL, .weights = {1, 1, 1, 1} },
    { .itemId = ITEM_TOXIC_PLATE, .width = 4, .height = 3, .shape = NULL, .weights = {1, 1, 1, 1} },
    { .itemId = ITEM_EARTH_PLATE, .width = 4, .height = 3, .shape = NULL, .weights = {1, 1, 1, 1} },
    { .itemId = ITEM_SKY_PLATE, .width = 4, .height = 3, .shape = NULL, .weights = {1, 1, 1, 1} },
    { .itemId = ITEM_MIND_PLATE, .width = 4, .height = 3, .shape = NULL, .weights = {1, 1, 1, 1} },
    { .itemId = ITEM_INSECT_PLATE, .width = 4, .height = 3, .shape = NULL, .weights = {1, 1, 1, 1} },
    { .itemId = ITEM_STONE_PLATE, .width = 4, .height = 3, .shape = NULL, .weights = {1, 1, 1, 1} },
    { .itemId = ITEM_SPOOKY_PLATE, .width = 4, .height = 3, .shape = NULL, .weights = {1, 1, 1, 1} },
    { .itemId = ITEM_DRACO_PLATE, .width = 4, .height = 3, .shape = NULL, .weights = {1, 1, 1, 1} },
    { .itemId = ITEM_DREAD_PLATE, .width = 4, .height = 3, .shape = NULL, .weights = {1, 1, 1, 1} },
    { .itemId = ITEM_IRON_PLATE, .width = 4, .height = 3, .shape = NULL, .weights = {1, 1, 1, 1} },
};

static const struct MiningObject sMiningRocks[] =
{
    { .itemId = ITEM_NONE, .width = 4, .height = 1, .shape = NULL },
    { .itemId = ITEM_NONE, .width = 1, .height = 4, .shape = NULL },
    { .itemId = ITEM_NONE, .width = 2, .height = 2, .shape = NULL },
    { .itemId = ITEM_NONE, .width = 3, .height = 2, .shape = sRockTShape },
    { .itemId = ITEM_NONE, .width = 2, .height = 3, .shape = sRockTShape90deg },
    { .itemId = ITEM_NONE, .width = 3, .height = 2, .shape = sRockTShape180deg },
    { .itemId = ITEM_NONE, .width = 2, .height = 3, .shape = sRockTShape270deg },
    { .itemId = ITEM_NONE, .width = 3, .height = 2, .shape = sRockZShape },
    { .itemId = ITEM_NONE, .width = 2, .height = 3, .shape = sRockZShape90deg },
    { .itemId = ITEM_NONE, .width = 3, .height = 2, .shape = sRockSShape },
    { .itemId = ITEM_NONE, .width = 2, .height = 3, .shape = sRockSShape90deg },
    { .itemId = ITEM_NONE, .width = 3, .height = 3, .shape = NULL },
    { .itemId = ITEM_NONE, .width = 2, .height = 4, .shape = NULL },
    { .itemId = ITEM_NONE, .width = 4, .height = 2, .shape = NULL },
};

// Opens the minigame from a script; the script carries on once it closes.
void StartMiningGame(void)
{
    CleanupOverworldWindowsAndTilemaps();
    gMain.savedCallback = CB2_ReturnToFieldContinueScript;
    SetMainCallback2(CB2_StartMiningGame);
    LockPlayerFieldControls();
}

static bool8 ObjectFillsCell(const struct MiningObject *object, int x, int y)
{
    if (object->shape == NULL)
        return TRUE;
    return object->shape[y * object->width + x] != 'o';
}

static bool8 TryPlaceObject(const struct MiningObject *object, int x, int y)
{
    int i, j;

    if (sMining->objectCount >= MAX_BURIED_OBJECTS)
        return FALSE;
    if (x + object->width > MINING_WIDTH || y + object->height > MINING_HEIGHT)
        return FALSE;

    for (j = 0; j < object->height; j++)
    {
        for (i = 0; i < object->width; i++)
        {
            if (ObjectFillsCell(object, i, j) && sMining->objectGrid[y + j][x + i] != 0)
                return FALSE;
        }
    }

    sMining->buried[sMining->objectCount].object = object;
    sMining->buried[sMining->objectCount].x = x;
    sMining->buried[sMining->objectCount].y = y;
    sMining->buried[sMining->objectCount].dugUp = FALSE;
    sMining->buried[sMining->objectCount].iconSpriteId = MAX_SPRITES;
    sMining->objectCount++;

    for (j = 0; j < object->height; j++)
    {
        for (i = 0; i < object->width; i++)
        {
            if (ObjectFillsCell(object, i, j))
                sMining->objectGrid[y + j][x + i] = sMining->objectCount;
        }
    }
    return TRUE;
}

static u8 GetWeightIndex(void)
{
    u8 index = (gSaveBlock2Ptr->playerTrainerId[0] & 1) ? 0 : 1;
    if (IsNationalPokedexEnabled())
        index += 2;
    return index;
}

static int PickItem(int roll, u8 weightIndex)
{
    int i;

    for (i = 0; i < (int)ARRAY_COUNT(sMiningItems); i++)
    {
        roll -= sMiningItems[i].weights[weightIndex];
        if (roll < 0)
            return i;
    }
    return 0;
}

static bool8 IsPlate(u16 itemId)
{
    return itemId >= FIRST_PLATE && itemId <= LAST_PLATE;
}

// Each plate can only be dug up once.
static bool8 HasPlateBeenMined(u16 itemId)
{
    return IsPlate(itemId) && FlagGet(FLAG_MINED_FLAME_PLATE + (itemId - FIRST_PLATE));
}

static void GenerateWall(void)
{
    int i, x, y, startX, startY;
    int totalWeight = 0;
    int itemCount, attempts;
    u8 weightIndex = GetWeightIndex();
    bool8 canStamp;

    for (y = 0; y < MINING_HEIGHT; y++)
    {
        for (x = 0; x < MINING_WIDTH; x++)
        {
            sMining->dirt[y][x] = 2;
            sMining->objectGrid[y][x] = 0;
        }
    }

    // Platinum offsets the depth-4 stamps' rows by the depth-6 stamp's size; kept as is.
    for (i = 0; i < 10; i++)
    {
        startX = (Random() % (MINING_WIDTH + 8)) - 8;
        startY = (Random() % (MINING_HEIGHT + 8)) - 5;
        for (y = startY; y < startY + 8; y++)
        {
            if (y < 0 || y >= MINING_HEIGHT)
                continue;
            for (x = startX; x < startX + 8; x++)
            {
                if (x < 0 || x >= MINING_WIDTH || sDirtDepth4Stamp[y - startY][x - startX] == 0)
                    continue;
                sMining->dirt[y][x] = sDirtDepth4Stamp[y - startY][x - startX];
            }
        }
    }

    // A depth-6 stamp only lands where the dirt is already at least 4 deep. Platinum
    // checks this against the depth-4 stamp's shape; kept as is.
    for (i = 0; i < 15; i++)
    {
        startX = (Random() % (MINING_WIDTH + 5)) - 5;
        startY = (Random() % (MINING_HEIGHT + 5)) - 5;
        canStamp = TRUE;
        for (y = startY; y < startY + 5 && canStamp; y++)
        {
            if (y < 0 || y >= MINING_HEIGHT)
                continue;
            for (x = startX; x < startX + 5; x++)
            {
                if (x < 0 || x >= MINING_WIDTH || sDirtDepth4Stamp[y - startY][x - startX] == 0)
                    continue;
                if (sMining->dirt[y][x] < 4)
                {
                    canStamp = FALSE;
                    break;
                }
            }
        }
        if (!canStamp)
            continue;
        for (y = startY; y < startY + 5; y++)
        {
            if (y < 0 || y >= MINING_HEIGHT)
                continue;
            for (x = startX; x < startX + 5; x++)
            {
                if (x < 0 || x >= MINING_WIDTH || sDirtDepth6Stamp[y - startY][x - startX] == 0)
                    continue;
                sMining->dirt[y][x] = sDirtDepth6Stamp[y - startY][x - startX];
            }
        }
    }

    for (i = 0; i < (int)ARRAY_COUNT(sMiningItems); i++)
        totalWeight += sMiningItems[i].weights[weightIndex];

    // 2 to 8 items, or 3 on the first wall
    itemCount = (Random() % (MAX_BURIED_ITEMS - 1)) + 2;
    if (!FlagGet(FLAG_HAS_MINED_WALL))
        itemCount = 3;

    sMining->objectCount = 0;
    for (attempts = 0; sMining->objectCount < itemCount && attempts < 1000; attempts++)
    {
        const struct MiningObject *item = &sMiningItems[PickItem(Random() % totalWeight, weightIndex)];

        if (HasPlateBeenMined(item->itemId))
            continue;
        if (IsPlate(item->itemId))
        {
            bool8 alreadyInWall = FALSE;
            for (i = 0; i < sMining->objectCount; i++)
            {
                if (sMining->buried[i].object->itemId == item->itemId)
                    alreadyInWall = TRUE;
            }
            if (alreadyInWall)
                continue;
        }
        TryPlaceObject(item, Random() % MINING_WIDTH, Random() % MINING_HEIGHT);
    }
    sMining->itemCount = sMining->objectCount;

    // No rocks in the first wall
    if (FlagGet(FLAG_HAS_MINED_WALL))
    {
        for (i = 0; i < 100; i++)
            TryPlaceObject(&sMiningRocks[Random() % ARRAY_COUNT(sMiningRocks)], Random() % MINING_WIDTH, Random() % MINING_HEIGHT);
    }
}

static bool8 IsItemObject(u8 gridValue)
{
    return gridValue != 0 && gridValue <= sMining->itemCount;
}

static bool8 IsRockObject(u8 gridValue)
{
    return gridValue > sMining->itemCount;
}

static void SetCellMetatile(u16 *tilemap, int x, int y, u8 metatile)
{
    u16 tile = metatile * CELL_TILES;
    int pos = (y * CELL_TILES) * 32 + x * CELL_TILES;

    tilemap[pos] = tile;
    tilemap[pos + 1] = tile + 1;
    tilemap[pos + 32] = tile + METATILE_COUNT * CELL_TILES;
    tilemap[pos + 33] = tile + METATILE_COUNT * CELL_TILES + 1;
}

static void DrawWall(void)
{
    int x, y;

    for (y = 0; y < MINING_HEIGHT; y++)
    {
        for (x = 0; x < MINING_WIDTH; x++)
        {
            u8 gridValue = sMining->objectGrid[y][x];
            u8 metatile = METATILE_BARE_WALL;

            if (IsItemObject(gridValue))
                metatile = METATILE_TREASURE;
            else if (IsRockObject(gridValue))
                metatile = METATILE_ROCK;
            SetCellMetatile(sMining->wallTilemap, x, y, metatile);
        }
    }
    ScheduleBgCopyTilemapToVram(2);
}

static void DrawDirt(void)
{
    int x, y;

    for (y = 0; y < MINING_HEIGHT; y++)
    {
        for (x = 0; x < MINING_WIDTH; x++)
            SetCellMetatile(sMining->dirtTilemap, x, y, METATILE_CLEAR + sMining->dirt[y][x]);
    }
    ScheduleBgCopyTilemapToVram(1);
}

static void DrawWallMeter(void)
{
    int filled = METER_BAR_HEIGHT * sMining->wallIntegrity / INITIAL_WALL_INTEGRITY;
    u8 color;

    if (sMining->wallIntegrity > INITIAL_WALL_INTEGRITY / 2)
        color = TEXT_COLOR_GREEN;
    else if (sMining->wallIntegrity > INITIAL_WALL_INTEGRITY / 4)
        color = TEXT_COLOR_LIGHT_RED;
    else
        color = TEXT_COLOR_RED;

    FillWindowPixelBuffer(WIN_WALL_METER, PIXEL_FILL(TEXT_COLOR_WHITE));
    FillWindowPixelRect(WIN_WALL_METER, PIXEL_FILL(TEXT_COLOR_DARK_GRAY), METER_BAR_X - 1, METER_BAR_Y - 1, METER_BAR_WIDTH + 2, METER_BAR_HEIGHT + 2);
    FillWindowPixelRect(WIN_WALL_METER, PIXEL_FILL(TEXT_COLOR_LIGHT_GRAY), METER_BAR_X, METER_BAR_Y, METER_BAR_WIDTH, METER_BAR_HEIGHT - filled);
    if (filled > 0)
        FillWindowPixelRect(WIN_WALL_METER, PIXEL_FILL(color), METER_BAR_X, METER_BAR_Y + METER_BAR_HEIGHT - filled, METER_BAR_WIDTH, filled);
    CopyWindowToVram(WIN_WALL_METER, COPYWIN_GFX);
}

// The pickaxe hits a plus shape, the hammer a 3x3 square.
static bool8 ToolHitsOffset(int dx, int dy)
{
    if (sMining->tool == TOOL_HAMMER)
        return TRUE;
    return dx == 0 || dy == 0;
}

static void UpdateCursor(void)
{
    int dx, dy, i = 0;

    for (dy = -1; dy <= 1; dy++)
    {
        for (dx = -1; dx <= 1; dx++, i++)
        {
            struct Sprite *sprite = &gSprites[sMining->cursorSpriteIds[i]];
            int x = sMining->cursorX + dx;
            int y = sMining->cursorY + dy;

            sprite->x = x * 16 + 8;
            sprite->y = y * 16 + 8;
            sprite->invisible = !ToolHitsOffset(dx, dy) || x < 0 || x >= MINING_WIDTH || y < 0 || y >= MINING_HEIGHT;
        }
    }
}

static void SetCursorVisible(bool8 visible)
{
    int i;

    if (visible)
    {
        UpdateCursor();
    }
    else
    {
        for (i = 0; i < MAX_CURSOR_SPRITES; i++)
            gSprites[sMining->cursorSpriteIds[i]].invisible = TRUE;
    }
}

static void LowerDirt(int x, int y, int amount)
{
    if (x < 0 || x >= MINING_WIDTH || y < 0 || y >= MINING_HEIGHT)
        return;
    while (amount-- > 0 && sMining->dirt[y][x] != 0)
        sMining->dirt[y][x]--;
}

// Platinum's Mining_RemoveDirt
static void Strike(void)
{
    static const s8 sAdjacent[4][2] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
    static const s8 sDiagonal[4][2] = {{1, 1}, {-1, -1}, {-1, 1}, {1, -1}};
    int x = sMining->cursorX;
    int y = sMining->cursorY;
    int i;
    bool8 hammer = (sMining->tool == TOOL_HAMMER);

    LowerDirt(x, y, 2);

    if (sMining->dirt[y][x] == 0 && IsItemObject(sMining->objectGrid[y][x]))
        sMining->sparkleTimer = 15;

    if (sMining->dirt[y][x] == 0 && IsRockObject(sMining->objectGrid[y][x]))
    {
        // A bare rock stops the blow from spreading.
        PlaySE(SE_BREAKABLE_DOOR);
    }
    else
    {
        PlaySE(hammer ? SE_M_MEGA_KICK2 : SE_M_COMET_PUNCH);
        if (hammer)
        {
            for (i = 0; i < 4; i++)
                LowerDirt(x + sDiagonal[i][0], y + sDiagonal[i][1], 1);
        }
        for (i = 0; i < 4; i++)
            LowerDirt(x + sAdjacent[i][0], y + sAdjacent[i][1], hammer ? 2 : 1);
    }

    i = hammer ? HAMMER_WALL_DAMAGE : PICKAXE_WALL_DAMAGE;
    if (sMining->wallIntegrity > i)
        sMining->wallIntegrity -= i;
    else
        sMining->wallIntegrity = 0;

    sMining->shakeTimer = 1;
    DrawDirt();
    DrawWallMeter();
}

static void ShowItemIcon(u8 index)
{
    struct BuriedObject *buried = &sMining->buried[index];
    const struct MiningObject *object = buried->object;
    u8 spriteId = AddCustomItemIconSprite(&sSpriteTemplate_ItemIcon, TAG_ITEM_ICON + index, TAG_ITEM_ICON + index, object->itemId);
    bool8 small;

    if (spriteId == MAX_SPRITES)
        return;
    // The 24x24 icon sits in the top left of its 32x32 sprite, so it's nudged
    // to be centred once scaled up.
    small = min(object->width, object->height) <= 2;
    buried->iconSpriteId = spriteId;
    gSprites[spriteId].x = buried->x * 16 + object->width * 8 + (small ? 5 : 8);
    gSprites[spriteId].y = buried->y * 16 + object->height * 8 + (small ? 5 : 8);
    StartSpriteAffineAnim(&gSprites[spriteId], small ? ICON_SCALE_SMALL : ICON_SCALE_LARGE);
}

// Marks newly uncovered items, and returns whether every item is uncovered.
static bool8 CheckItemsDugUp(void)
{
    bool8 uncovered[MAX_BURIED_ITEMS];
    bool8 allDugUp = TRUE;
    int x, y, i;

    for (i = 0; i < sMining->itemCount; i++)
        uncovered[i] = TRUE;

    for (y = 0; y < MINING_HEIGHT; y++)
    {
        for (x = 0; x < MINING_WIDTH; x++)
        {
            u8 gridValue = sMining->objectGrid[y][x];
            if (IsItemObject(gridValue) && sMining->dirt[y][x] != 0)
                uncovered[gridValue - 1] = FALSE;
        }
    }

    for (i = 0; i < sMining->itemCount; i++)
    {
        if (!uncovered[i])
        {
            allDugUp = FALSE;
        }
        else if (!sMining->buried[i].dugUp)
        {
            sMining->buried[i].dugUp = TRUE;
            ShowItemIcon(i);
        }
    }
    return allDugUp;
}

static void SetShakeOffset(int dx, int dy)
{
    int i;

    ChangeBgX(1, dx << 8, BG_COORD_SET);
    ChangeBgY(1, dy << 8, BG_COORD_SET);
    ChangeBgX(2, dx << 8, BG_COORD_SET);
    ChangeBgY(2, dy << 8, BG_COORD_SET);
    for (i = 0; i < MAX_CURSOR_SPRITES; i++)
    {
        gSprites[sMining->cursorSpriteIds[i]].x2 = -dx;
        gSprites[sMining->cursorSpriteIds[i]].y2 = -dy;
    }
    for (i = 0; i < sMining->itemCount; i++)
    {
        if (sMining->buried[i].iconSpriteId != MAX_SPRITES)
        {
            gSprites[sMining->buried[i].iconSpriteId].x2 = -dx;
            gSprites[sMining->buried[i].iconSpriteId].y2 = -dy;
        }
    }
}

// The wall shakes after each blow, longer and harder as it weakens, as in Platinum.
static void UpdateShake(void)
{
    int damage = INITIAL_WALL_INTEGRITY - sMining->wallIntegrity;
    int range;

    if (sMining->collapseTimer != 0)
    {
        sMining->collapseTimer--;
        range = (sMining->collapseTimer != 0) ? 7 : 0;
    }
    else if (sMining->shakeTimer != 0)
    {
        sMining->shakeTimer++;
        if (sMining->shakeTimer > damage / 15)
        {
            sMining->shakeTimer = 0;
            range = 0;
        }
        else
        {
            range = 3 + damage / 50;
        }
    }
    else
    {
        return;
    }

    if (range == 0)
        SetShakeOffset(0, 0);
    else
        SetShakeOffset((Random() % range) - range / 2, (Random() % range) - range / 2);
}

static void ShowMessage(const u8 *text)
{
    StringExpandPlaceholders(gStringVar4, text);
    DrawDialogueFrame(WIN_MSG, FALSE);
    AddTextPrinterParameterized2(WIN_MSG, FONT_NORMAL, gStringVar4, GetPlayerTextSpeedDelay(), NULL, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_WHITE, TEXT_COLOR_LIGHT_GRAY);
    gTextFlags.canABSpeedUpPrint = TRUE;
    CopyWindowToVram(WIN_MSG, COPYWIN_FULL);
}

static bool8 IsMessageDone(void)
{
    return !IsTextPrinterActive(WIN_MSG) && JOY_NEW(A_BUTTON | B_BUTTON);
}

static void CloseMessage(void)
{
    ClearDialogWindowAndFrame(WIN_MSG, TRUE);
}

static void LoadMiningGraphics(void)
{
    int i;

    SetVBlankCallback(NULL);
    SetGpuReg(REG_OFFSET_DISPCNT, 0);
    SetGpuReg(REG_OFFSET_BG3CNT, 0);
    SetGpuReg(REG_OFFSET_BG2CNT, 0);
    SetGpuReg(REG_OFFSET_BG1CNT, 0);
    SetGpuReg(REG_OFFSET_BG0CNT, 0);
    for (i = 0; i < NUM_BACKGROUNDS; i++)
    {
        ChangeBgX(i, 0, BG_COORD_SET);
        ChangeBgY(i, 0, BG_COORD_SET);
    }
    DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000);
    DmaClear32(3, (void *)OAM, OAM_SIZE);
    DmaClear16(3, (void *)PLTT, PLTT_SIZE);
    ScanlineEffect_Stop();
    ResetTasks();
    ResetSpriteData();
    ResetPaletteFade();
    FreeAllSpritePalettes();

    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sBgTemplates, ARRAY_COUNT(sBgTemplates));
    SetBgTilemapBuffer(1, sMining->dirtTilemap);
    SetBgTilemapBuffer(2, sMining->wallTilemap);
    InitWindows(sWindowTemplates);
    DeactivateAllTextPrinters();
    LoadMessageBoxAndBorderGfx();
    ClearScheduledBgCopiesToVram();

    LZ77UnCompVram(sMiningTiles_Gfx, (void *)BG_CHAR_ADDR(0));
    LoadPalette(sMiningTiles_Pal, BG_PLTT_ID(0), PLTT_SIZE_4BPP);

    LoadCompressedSpriteSheet(&sSpriteSheet_Cursor);
    LoadSpritePalette(&sSpritePalette_Cursor);
    for (i = 0; i < MAX_CURSOR_SPRITES; i++)
        sMining->cursorSpriteIds[i] = CreateSprite(&sSpriteTemplate_Cursor, 0, 0, 0);
}

void CB2_StartMiningGame(void)
{
    sMining = AllocZeroed(sizeof(*sMining));
    sMining->wallIntegrity = INITIAL_WALL_INTEGRITY;
    sMining->tool = TOOL_PICKAXE;
    sMining->cursorX = MINING_WIDTH / 2;
    sMining->cursorY = MINING_HEIGHT / 2;

    LoadMiningGraphics();
    GenerateWall();
    DrawWall();
    DrawDirt();
    DrawStdWindowFrame(WIN_WALL_METER, FALSE);
    DrawWallMeter();
    PutWindowTilemap(WIN_WALL_METER);
    CopyWindowToVram(WIN_WALL_METER, COPYWIN_FULL);
    SetCursorVisible(FALSE);

    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    EnableInterrupts(INTR_FLAG_VBLANK);
    SetVBlankCallback(VBlankCB_MiningGame);
    SetMainCallback2(CB2_MiningGame);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    SetGpuReg(REG_OFFSET_BLDY, 0);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);
    ShowBg(0);
    ShowBg(1);
    ShowBg(2);
    ShowBg(3);
    CreateTask(Task_MiningGame_WaitFadeIn, 0);
}

static void CB2_MiningGame(void)
{
    RunTasks();
    RunTextPrinters();
    if (sMining != NULL)
        UpdateShake();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void VBlankCB_MiningGame(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void Task_MiningGame_WaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        ConvertIntToDecimalStringN(gStringVar1, sMining->itemCount, STR_CONV_MODE_LEFT_ALIGN, 1);
        ShowMessage(sText_SomethingPinged);
        gTasks[taskId].func = Task_MiningGame_ShowHint;
    }
}

static void Task_MiningGame_ShowHint(u8 taskId)
{
    if (IsMessageDone())
    {
        if (!FlagGet(FLAG_HAS_MINED_WALL))
        {
            ShowMessage(sText_ControlsHint);
            gTasks[taskId].func = Task_MiningGame_WaitForMessage;
        }
        else
        {
            CloseMessage();
            SetCursorVisible(TRUE);
            gTasks[taskId].func = Task_MiningGame_HandleInput;
        }
    }
}

static void Task_MiningGame_WaitForMessage(u8 taskId)
{
    if (IsMessageDone())
    {
        CloseMessage();
        SetCursorVisible(TRUE);
        gTasks[taskId].func = Task_MiningGame_HandleInput;
    }
}

static void Task_MiningGame_HandleInput(u8 taskId)
{
    if (sMining->sparkleTimer != 0 && --sMining->sparkleTimer == 0)
        PlaySE(SE_SHINY);

    if (JOY_NEW(A_BUTTON))
    {
        Strike();
        // An uncovered wall counts even if the last blow also brought it down.
        if (CheckItemsDugUp())
        {
            FlagSet(FLAG_HAS_MINED_WALL);
            SetCursorVisible(FALSE);
            gTasks[taskId].data[0] = 25;
            gTasks[taskId].func = Task_MiningGame_EverythingDugUp;
        }
        else if (sMining->wallIntegrity == 0)
        {
            FlagSet(FLAG_HAS_MINED_WALL);
            SetCursorVisible(FALSE);
            sMining->shakeTimer = 0;
            sMining->collapseTimer = 45;
            PlaySE(SE_M_EARTHQUAKE);
            gTasks[taskId].func = Task_MiningGame_Collapse;
        }
        return;
    }
    if (JOY_NEW(R_BUTTON | L_BUTTON | SELECT_BUTTON))
    {
        PlaySE(SE_SELECT);
        sMining->tool ^= 1;
        UpdateCursor();
        return;
    }
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        SetCursorVisible(FALSE);
        ShowMessage(sText_StopDigging);
        gTasks[taskId].func = Task_MiningGame_ConfirmStop;
        return;
    }

    if (JOY_REPEAT(DPAD_UP) && sMining->cursorY > 0)
        sMining->cursorY--;
    else if (JOY_REPEAT(DPAD_DOWN) && sMining->cursorY < MINING_HEIGHT - 1)
        sMining->cursorY++;
    else if (JOY_REPEAT(DPAD_LEFT) && sMining->cursorX > 0)
        sMining->cursorX--;
    else if (JOY_REPEAT(DPAD_RIGHT) && sMining->cursorX < MINING_WIDTH - 1)
        sMining->cursorX++;
    else
        return;
    UpdateCursor();
}

static void Task_MiningGame_ConfirmStop(u8 taskId)
{
    if (!IsTextPrinterActive(WIN_MSG))
    {
        DisplayYesNoMenuDefaultYes();
        gTasks[taskId].func = Task_MiningGame_HandleStopYesNo;
    }
}

static void Task_MiningGame_HandleStopYesNo(u8 taskId)
{
    switch (Menu_ProcessInputNoWrapClearOnChoose())
    {
    case 0:  // Yes
        FlagSet(FLAG_HAS_MINED_WALL);
        CloseMessage();
        sMining->nextItem = 0;
        gTasks[taskId].func = Task_MiningGame_GiveNextItem;
        break;
    case 1:  // No
    case MENU_B_PRESSED:
        CloseMessage();
        SetCursorVisible(TRUE);
        gTasks[taskId].func = Task_MiningGame_HandleInput;
        break;
    }
}

static void Task_MiningGame_EverythingDugUp(u8 taskId)
{
    if (gTasks[taskId].data[0] != 0)
    {
        gTasks[taskId].data[0]--;
        return;
    }
    if (gTasks[taskId].data[1] == 0)
    {
        PlaySE(SE_SUCCESS);
        ShowMessage(sText_EverythingDugUp);
        gTasks[taskId].data[1] = 1;
    }
    else if (IsMessageDone())
    {
        CloseMessage();
        sMining->nextItem = 0;
        gTasks[taskId].func = Task_MiningGame_GiveNextItem;
    }
}

static void Task_MiningGame_Collapse(u8 taskId)
{
    if (sMining->collapseTimer != 0)
        return;
    if (gTasks[taskId].data[1] == 0)
    {
        PlaySE(SE_BREAKABLE_DOOR);
        ShowMessage(sText_WallCollapsed);
        gTasks[taskId].data[1] = 1;
    }
    else if (IsMessageDone())
    {
        CloseMessage();
        sMining->nextItem = 0;
        gTasks[taskId].func = Task_MiningGame_GiveNextItem;
    }
}

// Gives each uncovered item in turn, as Platinum does once the wall is done.
static void Task_MiningGame_GiveNextItem(u8 taskId)
{
    while (sMining->nextItem < sMining->itemCount && !sMining->buried[sMining->nextItem].dugUp)
        sMining->nextItem++;

    if (sMining->nextItem >= sMining->itemCount)
    {
        gTasks[taskId].func = Task_MiningGame_FadeOut;
        return;
    }

    {
        u16 itemId = sMining->buried[sMining->nextItem].object->itemId;

        CopyItemName(itemId, gStringVar1);
        if (CheckBagHasSpace(itemId, 1))
        {
            AddBagItem(itemId, 1);
            if (IsPlate(itemId))
                FlagSet(FLAG_MINED_FLAME_PLATE + (itemId - FIRST_PLATE));
            PlayFanfare(MUS_OBTAIN_ITEM);
            ShowMessage(sText_ItemObtained);
        }
        else
        {
            ShowMessage(sText_BagIsFull);
        }
    }
    sMining->nextItem++;
    gTasks[taskId].func = Task_MiningGame_WaitForItemMessage;
}

static void Task_MiningGame_WaitForItemMessage(u8 taskId)
{
    if (IsFanfareTaskInactive() && IsMessageDone())
    {
        CloseMessage();
        gTasks[taskId].func = Task_MiningGame_GiveNextItem;
    }
}

static void Task_MiningGame_FadeOut(u8 taskId)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    gTasks[taskId].func = Task_MiningGame_Exit;
}

static void Task_MiningGame_Exit(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        ResetSpriteData();
        FreeAllSpritePalettes();
        FreeAllWindowBuffers();
        FREE_AND_SET_NULL(sMining);
        SetMainCallback2(gMain.savedCallback);
    }
}
