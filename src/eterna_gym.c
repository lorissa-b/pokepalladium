// Eterna Gym's floral clock, after Platinum's (src/overlay008/gym_features.c in
// pret/pokeplatinum). The clock face is a flower bed the player can't cross;
// its two hands are paths. Beating each Gym Trainer, then Gardenia, moves the
// clock to its next state (VAR_ETERNA_GYM_CLOCK_STATE), which changes where
// the hands point and so which parts of the Gym can be reached. The second and
// third Trainers also drain a fountain that blocks one side of the Gym.
//
// Platinum turns the hands as 3D models and blocks tiles with a collision
// table per state. Here each state's table is drawn into the map: blocked
// cells of the bed are flowers, open cells and the hands are paving.

#include "global.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_camera.h"
#include "fieldmap.h"
#include "script.h"
#include "sound.h"
#include "task.h"
#include "trig.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/songs.h"

#define CLOCK_LEFT     5   // map coordinates of the clock's 13x13 region
#define CLOCK_TOP      6
#define CLOCK_SIZE     13
#define CLOCK_CENTER_X 11
#define CLOCK_CENTER_Y 12
#define HAND_LENGTH    6

#define FOUNTAIN_Y          18
#define LEFT_FOUNTAIN_X     2  // where the camera looks at each fountain
#define RIGHT_FOUNTAIN_X    20

// Blocks, as map grid entries (metatile, collision, elevation 3).
#define BLOCK(metatile, collision) ((metatile) | PACK_COLLISION(collision) | PACK_ELEVATION(3))
#define METATILE_GRASS    0x001 // General_Grass
#define METATILE_FLOWERS  0x004 // General's red flowers, made solid for the bed
#define METATILE_PAVING   0x109 // General's pale stone path
#define METATILE_WATER    0x0A1 // General_ReflectiveWater, solid: the fountain

enum {
    ETERNA_CLOCK_INITIAL,
    ETERNA_CLOCK_DEFEATED_FIRST_TRAINER,
    ETERNA_CLOCK_DEFEATED_SECOND_TRAINER,
    ETERNA_CLOCK_DEFEATED_THIRD_TRAINER,
    ETERNA_CLOCK_DEFEATED_GYM_LEADER,
    ETERNA_CLOCK_STATE_COUNT
};

// The time the clock shows in each state, as in Platinum.
static const u8 sClockTimes[ETERNA_CLOCK_STATE_COUNT][2] = {
    [ETERNA_CLOCK_INITIAL]                 = { 7, 25 },
    [ETERNA_CLOCK_DEFEATED_FIRST_TRAINER]  = { 6, 15 },
    [ETERNA_CLOCK_DEFEATED_SECOND_TRAINER] = { 9, 15 },
    [ETERNA_CLOCK_DEFEATED_THIRD_TRAINER]  = { 0, 45 },
    [ETERNA_CLOCK_DEFEATED_GYM_LEADER]     = { 0, 30 },
};

// Platinum's sEternaGymFountainCollision, per side: whether the fountain's
// row (x 1-4 on the left, 18-21 on the right) is blocked. The second Trainer
// drains the right fountain and the third the left.
static const bool8 sFountainBlocked[ETERNA_CLOCK_STATE_COUNT][2] = {
    [ETERNA_CLOCK_INITIAL]                 = { TRUE, TRUE },
    [ETERNA_CLOCK_DEFEATED_FIRST_TRAINER]  = { TRUE, TRUE },
    [ETERNA_CLOCK_DEFEATED_SECOND_TRAINER] = { TRUE, FALSE },
    [ETERNA_CLOCK_DEFEATED_THIRD_TRAINER]  = { FALSE, FALSE },
    [ETERNA_CLOCK_DEFEATED_GYM_LEADER]     = { FALSE, FALSE },
};

// Which cells of the clock region the clock draws: 0 = a hedge it leaves alone,
// 1 = the flower bed, 2 = floor beside it (the region's corners).
static const u8 sClockCells[CLOCK_SIZE * CLOCK_SIZE] = {
    2, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 2,
    0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
    2, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 2,
};

// Platinum's sEternaGymClockCollision: 1 where the clock blocks the player.
// clang-format off
static const u8 sClockCollision[ETERNA_CLOCK_STATE_COUNT][CLOCK_SIZE * CLOCK_SIZE] = {
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
    },
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
        0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
    },
    {
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
    },
    {
        0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
        0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
    },
    {
        0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
        0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
        0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
    },
};
// clang-format on

// field_specials.c
void SpawnCameraObject(void);
void RemoveCameraObject(void);

static EWRAM_DATA s16 sCameraStartX = 0;
static EWRAM_DATA s16 sCameraStartY = 0;
static EWRAM_DATA bool8 sCameraMovedXFirst = FALSE;
static EWRAM_DATA bool8 sCameraAtFountain = FALSE;

static u16 GetClockState(void)
{
    u16 state = VarGet(VAR_ETERNA_GYM_CLOCK_STATE);
    if (state >= ETERNA_CLOCK_STATE_COUNT)
        state = ETERNA_CLOCK_DEFEATED_GYM_LEADER;
    return state;
}

// Round (value / 4096) to the nearest whole number, halves away from zero.
static s32 RoundQ12(s32 value)
{
    if (value < 0)
        return -((-value + 2048) >> 12);
    return (value + 2048) >> 12;
}

// Marks the cells a hand covers at the angle given in degrees clockwise from
// 12 o'clock.
static void MarkHand(bool8 *cells, u16 angle)
{
    s32 r;
    for (r = 1; r <= HAND_LENGTH; r++)
    {
        s32 x = CLOCK_CENTER_X - CLOCK_LEFT + RoundQ12(r * Sin2(angle % 360));
        s32 y = CLOCK_CENTER_Y - CLOCK_TOP - RoundQ12(r * Cos2(angle % 360));
        if (x >= 0 && x < CLOCK_SIZE && y >= 0 && y < CLOCK_SIZE)
            cells[y * CLOCK_SIZE + x] = TRUE;
    }
}

static void MarkHands(bool8 *cells, u16 hourAngle, u16 minuteAngle)
{
    memset(cells, FALSE, CLOCK_SIZE * CLOCK_SIZE);
    cells[(CLOCK_CENTER_Y - CLOCK_TOP) * CLOCK_SIZE + CLOCK_CENTER_X - CLOCK_LEFT] = TRUE;
    MarkHand(cells, hourAngle);
    MarkHand(cells, minuteAngle);
}

static void SetBlock(s32 x, s32 y, u16 entry, bool8 redraw)
{
    if (MapGridGetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET) == (entry & MAPGRID_METATILE_ID_MASK)
     && MapGridGetCollisionAt(x + MAP_OFFSET, y + MAP_OFFSET) == UNPACK_COLLISION(entry))
        return;
    MapGridSetMetatileEntryAt(x + MAP_OFFSET, y + MAP_OFFSET, entry);
    if (redraw)
        CurrentMapDrawMetatileAt(x + MAP_OFFSET, y + MAP_OFFSET);
}

// Draws the clock with the hands over `hands`. The paving's collision and the
// open cells come from `collisionState`; cells open in `alsoOpenState` too
// are paved (during a turn, the paths both states share stay put).
static void DrawClockCells(const bool8 *hands, u16 collisionState, u16 alsoOpenState, bool8 redraw)
{
    s32 i;
    for (i = 0; i < CLOCK_SIZE * CLOCK_SIZE; i++)
    {
        s32 x = CLOCK_LEFT + i % CLOCK_SIZE;
        s32 y = CLOCK_TOP + i / CLOCK_SIZE;
        u8 blocked = sClockCollision[collisionState][i];
        u16 metatile;

        if (sClockCells[i] == 0)
            continue;
        if (sClockCells[i] == 2)
            metatile = METATILE_GRASS;
        else if (hands[i] || (!blocked && !sClockCollision[alsoOpenState][i]))
            metatile = METATILE_PAVING;
        else
            metatile = METATILE_FLOWERS, blocked = TRUE;
        SetBlock(x, y, BLOCK(metatile, blocked), redraw);
    }
}

static void DrawFountains(u16 state, bool8 redraw)
{
    s32 x;
    for (x = 1; x <= 4; x++)
    {
        bool8 left = sFountainBlocked[state][0];
        bool8 right = sFountainBlocked[state][1];
        SetBlock(x, FOUNTAIN_Y, left ? BLOCK(METATILE_WATER, 1) : BLOCK(METATILE_PAVING, 0), redraw);
        SetBlock(x + 17, FOUNTAIN_Y, right ? BLOCK(METATILE_WATER, 1) : BLOCK(METATILE_PAVING, 0), redraw);
    }
}

static void DrawClockState(u16 state, bool8 redraw)
{
    bool8 hands[CLOCK_SIZE * CLOCK_SIZE];
    MarkHands(hands, sClockTimes[state][0] * 30, sClockTimes[state][1] * 6);
    DrawClockCells(hands, state, state, redraw);
}

// The Gym's ON_LOAD script: draw the clock and fountains as they stand.
void EternaGym_DrawClock(void)
{
    u16 state = GetClockState();
    DrawClockState(state, FALSE);
    DrawFountains(state, FALSE);
}

// ---------------------------------------------------------------- camera

#define tDestX    data[0]
#define tDestY    data[1]
#define tXFirst   data[2]
#define tDelay    data[3]
#define tState    data[4]

static struct ObjectEvent *GetCamera(void)
{
    u8 id = GetObjectEventIdByLocalIdAndMap(LOCALID_CAMERA, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
    return &gObjectEvents[id];
}

static void GetCameraPos(s16 *x, s16 *y)
{
    struct ObjectEvent *camera = GetCamera();
    *x = camera->currentCoords.x - MAP_OFFSET;
    *y = camera->currentCoords.y - MAP_OFFSET;
}

// Walks the camera one tile at a time toward the task's destination; TRUE
// once it's there.
static bool8 StepCamera(struct Task *task)
{
    struct ObjectEvent *camera = GetCamera();
    s16 x, y;
    u8 action;

    if (camera->heldMovementActive && !ObjectEventClearHeldMovementIfFinished(camera))
        return FALSE;
    GetCameraPos(&x, &y);
    if (x == task->tDestX && y == task->tDestY)
        return TRUE;
    if ((task->tXFirst && x != task->tDestX) || y == task->tDestY)
        action = (x < task->tDestX) ? MOVEMENT_ACTION_WALK_NORMAL_RIGHT : MOVEMENT_ACTION_WALK_NORMAL_LEFT;
    else
        action = (y < task->tDestY) ? MOVEMENT_ACTION_WALK_NORMAL_DOWN : MOVEMENT_ACTION_WALK_NORMAL_UP;
    ObjectEventSetHeldMovement(camera, action);
    return FALSE;
}

// Platinum moves along the shorter distance first.
static bool8 ShorterAxisIsX(s16 fromX, s16 fromY, s16 toX, s16 toY)
{
    return abs(toX - fromX) < abs(toY - fromY);
}

static void Task_MoveCamera(u8 taskId)
{
    if (StepCamera(&gTasks[taskId]))
    {
        DestroyTask(taskId);
        ScriptContext_Enable();
    }
}

static void StartCameraMove(s16 destX, s16 destY, bool8 xFirst, TaskFunc func)
{
    u8 taskId = CreateTask(func, 80);
    gTasks[taskId].tDestX = destX;
    gTasks[taskId].tDestY = destY;
    gTasks[taskId].tXFirst = xFirst;
}

// Spawns the camera on the player and moves it to the clock's centre.
void EternaGym_CameraToClock(void)
{
    SpawnCameraObject();
    sCameraStartX = gSaveBlock1Ptr->pos.x;
    sCameraStartY = gSaveBlock1Ptr->pos.y;
    sCameraMovedXFirst = ShorterAxisIsX(sCameraStartX, sCameraStartY, CLOCK_CENTER_X, CLOCK_CENTER_Y);
    sCameraAtFountain = FALSE;
    StartCameraMove(CLOCK_CENTER_X, CLOCK_CENTER_Y, sCameraMovedXFirst, Task_MoveCamera);
}

// Moves the camera back to the player and hands the view back.
static void Task_CameraToPlayer(u8 taskId)
{
    if (StepCamera(&gTasks[taskId]))
    {
        RemoveCameraObject();
        DestroyTask(taskId);
        ScriptContext_Enable();
    }
}

void EternaGym_CameraToPlayer(void)
{
    s16 x, y;
    bool8 xFirst;

    if (sCameraAtFountain)
    {
        GetCameraPos(&x, &y);
        xFirst = ShorterAxisIsX(x, y, sCameraStartX, sCameraStartY);
    }
    else
    {
        xFirst = !sCameraMovedXFirst; // retrace the way it came
    }
    StartCameraMove(sCameraStartX, sCameraStartY, xFirst, Task_CameraToPlayer);
}

// ---------------------------------------------------------------- clock

#define tMinutes     data[5]  // the time shown, in minutes from 12:00
#define tDestMinutes data[6]
#define tFromState   data[7]
#define tToState     data[8]
#define tTick        data[9]

#define MINUTES_PER_FRAME 2   // Platinum's speed at 30fps is 2 a frame; this runs at 60fps
#define TICK_INTERVAL     16

static void DrawTurningClock(struct Task *task)
{
    bool8 hands[CLOCK_SIZE * CLOCK_SIZE];
    s32 hour = task->tMinutes / 60;
    s32 minute = task->tMinutes % 60;
    u16 hourAngle = hour * 30;

    // As in Platinum, the hour hand creeps with the minutes until it reaches
    // its hour; after Gardenia only the minute hand moves.
    if (task->tToState == ETERNA_CLOCK_DEFEATED_GYM_LEADER)
        hourAngle = sClockTimes[task->tToState][0] * 30;
    else if (hour != sClockTimes[task->tToState][0])
        hourAngle += minute / 2;
    MarkHands(hands, hourAngle, minute * 6);
    DrawClockCells(hands, task->tFromState, task->tToState, TRUE);
}

static void Task_TurnClock(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
    case 0:
        if (++task->tDelay < 8)
            break;
        task->tDelay = 0;
        task->tState++;
        // fallthrough
    case 1:
        if (task->tTick++ % TICK_INTERVAL == 0)
            PlaySE(SE_CLICK);
        if (task->tMinutes != task->tDestMinutes)
        {
            s32 left = (task->tDestMinutes - task->tMinutes + 720) % 720;
            task->tMinutes = (task->tMinutes + min(left, MINUTES_PER_FRAME)) % 720;
            DrawTurningClock(task);
            break;
        }
        DrawClockState(task->tToState, TRUE);
        PlaySE(SE_DING_DONG);
        task->tState++;
        break;
    case 2:
        if (++task->tDelay < 8)
            break;
        DestroyTask(taskId);
        ScriptContext_Enable();
        break;
    }
}

static void Task_EnableScript(u8 taskId)
{
    DestroyTask(taskId);
    ScriptContext_Enable();
}

// Moves the clock on to the state in VAR_0x8004 (the Trainer just beaten),
// turning the hands forward as Platinum does. Does nothing if it's there.
void EternaGym_TurnClock(void)
{
    u16 from = GetClockState();
    u16 to = gSpecialVar_0x8004;
    u8 taskId;

    if (to >= ETERNA_CLOCK_STATE_COUNT || to <= from)
    {
        // Let the script's waitstate run first, then carry on.
        CreateTask(Task_EnableScript, 80);
        return;
    }
    VarSet(VAR_ETERNA_GYM_CLOCK_STATE, to);
    taskId = CreateTask(Task_TurnClock, 80);
    gTasks[taskId].tFromState = from;
    gTasks[taskId].tToState = to;
    gTasks[taskId].tMinutes = sClockTimes[from][0] * 60 + sClockTimes[from][1];
    gTasks[taskId].tDestMinutes = sClockTimes[to][0] * 60 + sClockTimes[to][1];
    if (to == ETERNA_CLOCK_DEFEATED_GYM_LEADER)
        gTasks[taskId].tDestMinutes += 60; // 0:45 to 0:30 goes forward round the minute hand
}

// ---------------------------------------------------------------- fountains

static void Task_DrainFountain(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
    case 0: // down from the clock's centre to the fountains' row
        if (!StepCamera(task))
            break;
        task->tDestX = VarGet(VAR_ETERNA_GYM_CLOCK_STATE) == ETERNA_CLOCK_DEFEATED_THIRD_TRAINER ? LEFT_FOUNTAIN_X : RIGHT_FOUNTAIN_X;
        task->tXFirst = TRUE;
        task->tState++;
        break;
    case 1: // along to the fountain
        if (!StepCamera(task))
            break;
        task->tState++;
        break;
    case 2:
        if (++task->tDelay < 4)
            break;
        task->tDelay = 0;
        DrawFountains(GetClockState(), TRUE);
        PlaySE(SE_RAIN);
        task->tState++;
        break;
    case 3:
        if (++task->tDelay < 60)
            break;
        PlaySE(SE_RAIN_STOP);
        sCameraAtFountain = TRUE;
        DestroyTask(taskId);
        ScriptContext_Enable();
        break;
    }
}

// After the second and third Trainers: show the fountain that has stopped.
void EternaGym_CameraToFountain(void)
{
    StartCameraMove(CLOCK_CENTER_X, FOUNTAIN_Y, FALSE, Task_DrainFountain);
}
