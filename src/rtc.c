#include "global.h"
#include "rtc.h"
#include "string_util.h"
#include "text.h"

// iwram bss
static u16 sErrorStatus;
static struct SiiRtcInfo sRtc;
static u8 sProbeResult;
static u16 sSavedIme;
#if FAKE_RTC
static u8 sFakeRtcSubseconds;
#endif

// iwram common
COMMON_DATA struct Time gLocalTime = {0};

// const rom

static const struct SiiRtcInfo sRtcDummy = {0, MONTH_JAN, 1}; // 2000 Jan 1

static const s32 sNumDaysInMonths[MONTH_COUNT] =
{
    [MONTH_JAN - 1] = 31,
    [MONTH_FEB - 1] = 28,
    [MONTH_MAR - 1] = 31,
    [MONTH_APR - 1] = 30,
    [MONTH_MAY - 1] = 31,
    [MONTH_JUN - 1] = 30,
    [MONTH_JUL - 1] = 31,
    [MONTH_AUG - 1] = 31,
    [MONTH_SEP - 1] = 30,
    [MONTH_OCT - 1] = 31,
    [MONTH_NOV - 1] = 30,
    [MONTH_DEC - 1] = 31,
};

void RtcDisableInterrupts(void)
{
    sSavedIme = REG_IME;
    REG_IME = 0;
}

void RtcRestoreInterrupts(void)
{
    REG_IME = sSavedIme;
}

u32 ConvertBcdToBinary(u8 bcd)
{
    if (bcd > 0x9F)
        return 0xFF;

    if ((bcd & 0xF) <= 9)
        return (10 * ((bcd >> 4) & 0xF)) + (bcd & 0xF);
    else
        return 0xFF;
}

bool8 IsLeapYear(u32 year)
{
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        return TRUE;

    return FALSE;
}

u16 ConvertDateToDayCount(u8 year, u8 month, u8 day)
{
    s32 i;
    u16 dayCount = 0;

    for (i = year - 1; i >= 0; i--)
    {
        dayCount += 365;

        if (IsLeapYear(i) == TRUE)
            dayCount++;
    }

    for (i = 0; i < month - 1; i++)
        dayCount += sNumDaysInMonths[i];

    if (month > MONTH_FEB && IsLeapYear(year) == TRUE)
        dayCount++;

    dayCount += day;

    return dayCount;
}

u16 RtcGetDayCount(struct SiiRtcInfo *rtc)
{
    u8 year = ConvertBcdToBinary(rtc->year);
    u8 month = ConvertBcdToBinary(rtc->month);
    u8 day = ConvertBcdToBinary(rtc->day);
    return ConvertDateToDayCount(year, month, day);
}

#if FAKE_RTC
static u8 ConvertBinaryToBcd(u8 value)
{
    return ((value / 10) << 4) | (value % 10);
}

// Inverse of ConvertDateToDayCount. Day 1 is 1 January 2000.
static void ConvertDayCountToDate(u16 dayCount, struct SiiRtcInfo *rtc)
{
    u8 year = 0;
    u8 month = MONTH_JAN;
    u16 daysInMonth;

    if (dayCount < 1)
        dayCount = 1;

    while (dayCount > 365 + IsLeapYear(year))
    {
        dayCount -= 365 + IsLeapYear(year);
        year++;
    }

    while (TRUE)
    {
        daysInMonth = sNumDaysInMonths[month - 1];
        if (month == MONTH_FEB && IsLeapYear(year) == TRUE)
            daysInMonth++;
        if (dayCount <= daysInMonth)
            break;
        dayCount -= daysInMonth;
        month++;
    }

    rtc->year = ConvertBinaryToBcd(year);
    rtc->month = ConvertBinaryToBcd(month);
    rtc->day = ConvertBinaryToBcd(dayCount);
}

// Saves from before the fake clock have it zeroed. Start it so that the local
// time carries on from the last time the save recorded, rather than jumping.
static void StartFakeClockIfUnset(void)
{
    struct Time *time = &gSaveBlock2Ptr->fakeRtc;
    struct Time *offset = &gSaveBlock2Ptr->localTimeOffset;
    struct Time *lastLocalTime = &gSaveBlock2Ptr->lastBerryTreeUpdate;
    s32 carry;

    if (time->days >= 1)
        return;

    carry = offset->seconds + lastLocalTime->seconds;
    time->seconds = carry % SECONDS_PER_MINUTE;
    carry = offset->minutes + lastLocalTime->minutes + carry / SECONDS_PER_MINUTE;
    time->minutes = carry % MINUTES_PER_HOUR;
    carry = offset->hours + lastLocalTime->hours + carry / MINUTES_PER_HOUR;
    time->hours = carry % HOURS_PER_DAY;
    carry = offset->days + lastLocalTime->days + carry / HOURS_PER_DAY;

    if (carry >= 1 && carry <= SHRT_MAX)
        time->days = carry;
    else
        RtcReset();
}

static void RtcGetFakeInfo(struct SiiRtcInfo *rtc)
{
    struct Time *time = &gSaveBlock2Ptr->fakeRtc;

    StartFakeClockIfUnset();
    ConvertDayCountToDate(time->days, rtc);
    rtc->dayOfWeek = (time->days + 5) % 7; // 1 January 2000 was a Saturday
    rtc->hour = ConvertBinaryToBcd(time->hours);
    rtc->minute = ConvertBinaryToBcd(time->minutes);
    rtc->second = ConvertBinaryToBcd(time->seconds);
    rtc->status = SIIRTCINFO_24HOUR;
    rtc->alarmHour = 0;
    rtc->alarmMinute = 0;
}

// Called once per frame while a game is in progress.
void RtcAdvanceFakeClock(void)
{
    struct Time *time = &gSaveBlock2Ptr->fakeRtc;
    u32 seconds;

    StartFakeClockIfUnset();

    // Accumulate FAKE_RTC_SPEED per frame and carry every 60 into a whole
    // in-game second, so any speed works (at 60, that's one second per frame).
    seconds = sFakeRtcSubseconds + FAKE_RTC_SPEED;
    sFakeRtcSubseconds = seconds % 60;
    seconds = time->seconds + seconds / 60;

    time->seconds = seconds % SECONDS_PER_MINUTE;
    seconds = time->minutes + seconds / SECONDS_PER_MINUTE;
    time->minutes = seconds % MINUTES_PER_HOUR;
    seconds = time->hours + seconds / MINUTES_PER_HOUR;
    time->hours = seconds % HOURS_PER_DAY;

    // Stop counting days before the s16 day counters used by the rest of the
    // game overflow. That's roughly 540 hours of play at 60x speed.
    if (time->days <= SHRT_MAX - (s32)(seconds / HOURS_PER_DAY))
        time->days += seconds / HOURS_PER_DAY;
}
#endif

void RtcInit(void)
{
    sErrorStatus = 0;

#if !FAKE_RTC // The fake clock doesn't use the cartridge's clock, so there's nothing to check.
    RtcDisableInterrupts();
    SiiRtcUnprotect();
    sProbeResult = SiiRtcProbe();
    RtcRestoreInterrupts();

    if ((sProbeResult & 0xF) != 1)
    {
        sErrorStatus = RTC_INIT_ERROR;
        return;
    }

    if (sProbeResult & 0xF0)
        sErrorStatus = RTC_INIT_WARNING;
    else
        sErrorStatus = 0;

    RtcGetRawInfo(&sRtc);
    sErrorStatus = RtcCheckInfo(&sRtc);
#endif
}

u16 RtcGetErrorStatus(void)
{
    return sErrorStatus;
}

void RtcGetInfo(struct SiiRtcInfo *rtc)
{
#if FAKE_RTC
    RtcGetFakeInfo(rtc);
#else
    if (sErrorStatus & RTC_ERR_FLAG_MASK)
        *rtc = sRtcDummy;
    else
        RtcGetRawInfo(rtc);
#endif
}

void RtcGetDateTime(struct SiiRtcInfo *rtc)
{
    RtcDisableInterrupts();
    SiiRtcGetDateTime(rtc);
    RtcRestoreInterrupts();
}

void RtcGetStatus(struct SiiRtcInfo *rtc)
{
    RtcDisableInterrupts();
    SiiRtcGetStatus(rtc);
    RtcRestoreInterrupts();
}

void RtcGetRawInfo(struct SiiRtcInfo *rtc)
{
    RtcGetStatus(rtc);
    RtcGetDateTime(rtc);
}

u16 RtcCheckInfo(struct SiiRtcInfo *rtc)
{
    u16 errorFlags = 0;
    s32 year;
    s32 month;
    s32 value;

    if (rtc->status & SIIRTCINFO_POWER)
        errorFlags |= RTC_ERR_POWER_FAILURE;

    if (!(rtc->status & SIIRTCINFO_24HOUR))
        errorFlags |= RTC_ERR_12HOUR_CLOCK;

    year = ConvertBcdToBinary(rtc->year);

    if (year == 0xFF)
        errorFlags |= RTC_ERR_INVALID_YEAR;

    month = ConvertBcdToBinary(rtc->month);

    if (month == 0xFF || month == 0 || month > MONTH_COUNT)
        errorFlags |= RTC_ERR_INVALID_MONTH;

    value = ConvertBcdToBinary(rtc->day);

    if (value == 0xFF)
        errorFlags |= RTC_ERR_INVALID_DAY;

    if (month == MONTH_FEB)
    {
        if (value > IsLeapYear(year) + sNumDaysInMonths[month - 1])
            errorFlags |= RTC_ERR_INVALID_DAY;
    }
    else
    {
        if (value > sNumDaysInMonths[month - 1])
            errorFlags |= RTC_ERR_INVALID_DAY;
    }

    value = ConvertBcdToBinary(rtc->hour);

    if (value > HOURS_PER_DAY)
        errorFlags |= RTC_ERR_INVALID_HOUR;

    value = ConvertBcdToBinary(rtc->minute);

    if (value > MINUTES_PER_HOUR)
        errorFlags |= RTC_ERR_INVALID_MINUTE;

    value = ConvertBcdToBinary(rtc->second);

    if (value > SECONDS_PER_MINUTE)
        errorFlags |= RTC_ERR_INVALID_SECOND;

    return errorFlags;
}

void RtcReset(void)
{
#if FAKE_RTC
    gSaveBlock2Ptr->fakeRtc.days = 1;
    gSaveBlock2Ptr->fakeRtc.hours = 0;
    gSaveBlock2Ptr->fakeRtc.minutes = 0;
    gSaveBlock2Ptr->fakeRtc.seconds = 0;
    sFakeRtcSubseconds = 0;
#else
    RtcDisableInterrupts();
    SiiRtcReset();
    RtcRestoreInterrupts();
#endif
}

static void UNUSED FormatDecimalTime(u8 *dest, s32 hour, s32 minute, s32 second)
{
    dest = ConvertIntToDecimalStringN(dest, hour, STR_CONV_MODE_LEADING_ZEROS, 2);
    *dest++ = CHAR_COLON;
    dest = ConvertIntToDecimalStringN(dest, minute, STR_CONV_MODE_LEADING_ZEROS, 2);
    *dest++ = CHAR_COLON;
    dest = ConvertIntToDecimalStringN(dest, second, STR_CONV_MODE_LEADING_ZEROS, 2);
    *dest = EOS;
}

static void UNUSED FormatHexTime(u8 *dest, s32 hour, s32 minute, s32 second)
{
    dest = ConvertIntToHexStringN(dest, hour, STR_CONV_MODE_LEADING_ZEROS, 2);
    *dest++ = CHAR_COLON;
    dest = ConvertIntToHexStringN(dest, minute, STR_CONV_MODE_LEADING_ZEROS, 2);
    *dest++ = CHAR_COLON;
    dest = ConvertIntToHexStringN(dest, second, STR_CONV_MODE_LEADING_ZEROS, 2);
    *dest = EOS;
}

static void UNUSED FormatHexRtcTime(u8 *dest)
{
    FormatHexTime(dest, sRtc.hour, sRtc.minute, sRtc.second);
}

static void UNUSED FormatDecimalDate(u8 *dest, s32 year, s32 month, s32 day)
{
    dest = ConvertIntToDecimalStringN(dest, year, STR_CONV_MODE_LEADING_ZEROS, 4);
    *dest++ = CHAR_HYPHEN;
    dest = ConvertIntToDecimalStringN(dest, month, STR_CONV_MODE_LEADING_ZEROS, 2);
    *dest++ = CHAR_HYPHEN;
    dest = ConvertIntToDecimalStringN(dest, day, STR_CONV_MODE_LEADING_ZEROS, 2);
    *dest = EOS;
}

static void UNUSED FormatHexDate(u8 *dest, s32 year, s32 month, s32 day)
{
    dest = ConvertIntToHexStringN(dest, year, STR_CONV_MODE_LEADING_ZEROS, 4);
    *dest++ = CHAR_HYPHEN;
    dest = ConvertIntToHexStringN(dest, month, STR_CONV_MODE_LEADING_ZEROS, 2);
    *dest++ = CHAR_HYPHEN;
    dest = ConvertIntToHexStringN(dest, day, STR_CONV_MODE_LEADING_ZEROS, 2);
    *dest = EOS;
}

void RtcCalcTimeDifference(struct SiiRtcInfo *rtc, struct Time *result, struct Time *t)
{
    u16 days = RtcGetDayCount(rtc);
    result->seconds = ConvertBcdToBinary(rtc->second) - t->seconds;
    result->minutes = ConvertBcdToBinary(rtc->minute) - t->minutes;
    result->hours = ConvertBcdToBinary(rtc->hour) - t->hours;
    result->days = days - t->days;

    if (result->seconds < 0)
    {
        result->seconds += SECONDS_PER_MINUTE;
        --result->minutes;
    }

    if (result->minutes < 0)
    {
        result->minutes += MINUTES_PER_HOUR;
        --result->hours;
    }

    if (result->hours < 0)
    {
        result->hours += HOURS_PER_DAY;
        --result->days;
    }
}

void RtcCalcLocalTime(void)
{
    RtcGetInfo(&sRtc);
    RtcCalcTimeDifference(&sRtc, &gLocalTime, &gSaveBlock2Ptr->localTimeOffset);
}

void RtcInitLocalTimeOffset(s32 hour, s32 minute)
{
    RtcCalcLocalTimeOffset(0, hour, minute, 0);
}

void RtcCalcLocalTimeOffset(s32 days, s32 hours, s32 minutes, s32 seconds)
{
    gLocalTime.days = days;
    gLocalTime.hours = hours;
    gLocalTime.minutes = minutes;
    gLocalTime.seconds = seconds;
    RtcGetInfo(&sRtc);
    RtcCalcTimeDifference(&sRtc, &gSaveBlock2Ptr->localTimeOffset, &gLocalTime);
}

void CalcTimeDifference(struct Time *result, struct Time *t1, struct Time *t2)
{
    result->seconds = t2->seconds - t1->seconds;
    result->minutes = t2->minutes - t1->minutes;
    result->hours = t2->hours - t1->hours;
    result->days = t2->days - t1->days;

    if (result->seconds < 0)
    {
        result->seconds += SECONDS_PER_MINUTE;
        --result->minutes;
    }

    if (result->minutes < 0)
    {
        result->minutes += MINUTES_PER_HOUR;
        --result->hours;
    }

    if (result->hours < 0)
    {
        result->hours += HOURS_PER_DAY;
        --result->days;
    }
}

u32 RtcGetMinuteCount(void)
{
    RtcGetInfo(&sRtc);
    return (HOURS_PER_DAY * MINUTES_PER_HOUR) * RtcGetDayCount(&sRtc) + MINUTES_PER_HOUR * sRtc.hour + sRtc.minute;
}

u32 RtcGetLocalDayCount(void)
{
    return RtcGetDayCount(&sRtc);
}
