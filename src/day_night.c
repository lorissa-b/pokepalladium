#include "global.h"
#include "battle_main.h"
#include "day_night.h"
#include "main.h"
#include "overworld.h"
#include "palette.h"
#include "rtc.h"
#include "constants/map_types.h"
#include "constants/rgb.h"

// Tints the overworld and battle backgrounds by time of day. Each frame, the
// faded palette buffer is copied into a tinted buffer, which is what gets sent
// to palette RAM instead. The unfaded and faded buffers are never changed, so
// the rest of the game is unaware of the tint.
//
// Based on the Day Night System tutorial by Ashingda
// (https://github.com/Ashingda/pokeemerald-public/wiki/Day-Night-System),
// itself based on Xhyz's dns.

#if DAY_NIGHT_TINT

// The screen brightens over the first hour of the morning and darkens over
// the first hour of the evening, and stays dark from then until morning.
#define DAWN_HOUR   MORNING_HOUR_BEGIN
#define DUSK_HOUR   EVENING_HOUR_BEGIN

// Palettes to tint, one bit per palette: bits 0-15 are the background
// palettes and bits 16-31 the sprite palettes.
#define OVERWORLD_TINTED_PALETTES   0xFFFF1FFF // All but BG 13-15, which hold the text and menu windows
#define BATTLE_TINTED_PALETTES      0x0000001C // Only BG 2-4, the battle background

// How often to re-read the clock. One in-game minute at FAKE_RTC_SPEED 60.
#define FRAMES_PER_CLOCK_CHECK      60

// Each filter is how far to darken each channel, out of 32.
static const u16 sNightFilter = RGB2(14, 14, 5);

// Indexed by minute / 2 during DAWN_HOUR.
static const u16 sDawnFilters[] =
{
    RGB2(13, 13, 4), RGB2(13, 13, 4), RGB2(12, 13, 4), RGB2(12, 13, 4),
    RGB2(11, 12, 4), RGB2(10, 12, 4), RGB2( 9, 12, 4), RGB2( 8, 11, 4),
    RGB2( 7, 11, 3), RGB2( 6, 11, 3), RGB2( 5, 10, 3), RGB2( 4, 10, 3),
    RGB2( 3,  9, 3), RGB2( 2,  9, 3), RGB2( 1,  8, 3), RGB2( 0,  8, 3),
    RGB2( 0,  7, 2), RGB2( 0,  7, 2), RGB2( 0,  6, 2), RGB2( 0,  6, 2),
    RGB2( 0,  5, 2), RGB2( 0,  5, 2), RGB2( 0,  4, 2), RGB2( 0,  4, 1),
    RGB2( 0,  3, 1), RGB2( 0,  3, 1), RGB2( 0,  2, 1), RGB2( 0,  2, 1),
    RGB2( 0,  1, 1), RGB2( 0,  1, 1),
};

// Indexed by minute / 2 during DUSK_HOUR.
static const u16 sDuskFilters[] =
{
    RGB2( 0,  1, 1), RGB2( 0,  1, 1), RGB2( 0,  2, 1), RGB2( 0,  2, 1),
    RGB2( 0,  3, 1), RGB2( 0,  3, 1), RGB2( 0,  4, 1), RGB2( 0,  4, 1),
    RGB2( 0,  5, 2), RGB2( 0,  5, 2), RGB2( 0,  6, 2), RGB2( 0,  6, 2),
    RGB2( 0,  7, 2), RGB2( 0,  7, 2), RGB2( 0,  8, 2), RGB2( 1,  8, 3),
    RGB2( 2,  9, 3), RGB2( 3,  9, 3), RGB2( 4, 10, 3), RGB2( 5, 10, 3),
    RGB2( 6, 10, 3), RGB2( 7, 11, 3), RGB2( 8, 11, 4), RGB2( 9, 11, 4),
    RGB2(10, 12, 4), RGB2(11, 12, 4), RGB2(12, 12, 4), RGB2(12, 13, 4),
    RGB2(13, 13, 4), RGB2(13, 13, 4),
};

STATIC_ASSERT(ARRAY_COUNT(sDawnFilters) == MINUTES_PER_HOUR / 2, DawnFiltersCoverAnHour);
STATIC_ASSERT(ARRAY_COUNT(sDuskFilters) == MINUTES_PER_HOUR / 2, DuskFiltersCoverAnHour);

static bool8 sTintActive;
static u8 sFramesUntilClockCheck;
static u16 sCurrentFilter;
ALIGNED(4) EWRAM_DATA static u16 sTintedPlttBuffer[PLTT_BUFFER_SIZE] = {0};
// The tinted value of each 5-bit red, green and blue level under sCurrentFilter.
EWRAM_DATA static u8 sTintedChannels[3][32] = {0};

static bool8 IsTintedMap(void)
{
    switch (gMapHeader.mapType)
    {
    case MAP_TYPE_NONE:
    case MAP_TYPE_INDOOR:
    case MAP_TYPE_UNDERGROUND:
    case MAP_TYPE_SECRET_BASE:
        return FALSE;
    default:
        return TRUE;
    }
}

static u16 GetCurrentFilter(void)
{
    s32 hour = gLocalTime.hours;
    s32 minute = gLocalTime.minutes;

    if (minute < 0 || minute >= MINUTES_PER_HOUR)
        minute = 0;

    if (hour == DAWN_HOUR)
        return sDawnFilters[minute / 2];
    else if (hour == DUSK_HOUR)
        return sDuskFilters[minute / 2];
    else if (hour > DAWN_HOUR && hour < DUSK_HOUR)
        return RGB_BLACK; // No tint
    else
        return sNightFilter;
}

static void SetFilter(u16 filter)
{
    u32 level;

    if (filter == sCurrentFilter)
        return;

    sCurrentFilter = filter;
    for (level = 0; level < 32; level++)
    {
        sTintedChannels[0][level] = (level * (31 - GET_R(filter))) >> 5;
        sTintedChannels[1][level] = (level * (31 - GET_G(filter))) >> 5;
        sTintedChannels[2][level] = (level * (31 - GET_B(filter))) >> 5;
    }
}

// Called once per frame by the overworld and battle main callbacks, after
// the frame's palette fade has been applied.
void DayNight_UpdateTint(void)
{
    const u16 *src = gPlttBufferFaded;
    u16 *dest = sTintedPlttBuffer;
    u32 tintedPalettes;
    u32 palNum, i;
    u16 filter;

    if (sFramesUntilClockCheck == 0)
    {
        RtcCalcLocalTime();
        sFramesUntilClockCheck = FRAMES_PER_CLOCK_CHECK;
    }
    sFramesUntilClockCheck--;

    filter = GetCurrentFilter();
    if (!IsTintedMap() || filter == RGB_BLACK)
    {
        sTintActive = FALSE;
        return;
    }

    SetFilter(filter);
    tintedPalettes = gMain.inBattle ? BATTLE_TINTED_PALETTES : OVERWORLD_TINTED_PALETTES;
    for (palNum = 0; palNum < PLTT_BUFFER_SIZE / 16; palNum++, tintedPalettes >>= 1)
    {
        if (tintedPalettes & 1)
        {
            for (i = 0; i < 16; i++, src++)
            {
                *dest++ = sTintedChannels[0][GET_R(*src)]
                       | (sTintedChannels[1][GET_G(*src)] << 5)
                       | (sTintedChannels[2][GET_B(*src)] << 10);
            }
        }
        else
        {
            for (i = 0; i < 16; i++)
                *dest++ = *src++;
        }
    }
    sTintActive = TRUE;
}

// The buffer to copy to palette RAM this frame.
const u16 *DayNight_GetPlttBufferToTransfer(void)
{
    // The tinted buffer is only kept up to date by the callbacks that call
    // DayNight_UpdateTint. Anywhere else, drop it until they run again.
    if (gMain.callback2 != CB2_Overworld
     && gMain.callback2 != CB2_OverworldBasic
     && gMain.callback2 != BattleMainCB2)
    {
        sTintActive = FALSE;
        sFramesUntilClockCheck = 0;
    }

    if (sTintActive)
        return sTintedPlttBuffer;
    return gPlttBufferFaded;
}

#else

void DayNight_UpdateTint(void)
{
}

const u16 *DayNight_GetPlttBufferToTransfer(void)
{
    return gPlttBufferFaded;
}

#endif // DAY_NIGHT_TINT
