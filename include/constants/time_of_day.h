#ifndef GUARD_CONSTANTS_TIME_OF_DAY_H
#define GUARD_CONSTANTS_TIME_OF_DAY_H

// Periods of the in-game day, in the order their wild encounter tables are
// listed when a map has one table per period (see GetCurrentMapWildMonHeaderId).
#define TIME_MORNING        0
#define TIME_DAY            1
#define TIME_EVENING        2
#define TIME_NIGHT          3
#define TIMES_OF_DAY_COUNT  4

// The hour each period starts. Night runs until MORNING_HOUR_BEGIN.
#define MORNING_HOUR_BEGIN  6
#define DAY_HOUR_BEGIN      10
#define EVENING_HOUR_BEGIN  17
#define NIGHT_HOUR_BEGIN    20

#endif // GUARD_CONSTANTS_TIME_OF_DAY_H
