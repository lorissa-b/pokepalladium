# Fast in-game clock

## What changed

In-game time runs **60 times faster than real time** and only moves while the
game is being played. An in-game hour takes about a real minute, and a full
day–night cycle takes about 24 minutes.

Vanilla Emerald reads the cartridge's real-time clock, so time of day tracks
the real world and keeps moving while the game is off. Palladium replaces it
with a clock kept in the save file and advanced every frame, so the day cycle is
something you actually see during a play session. As a side effect, the game no
longer needs a working RTC — no more "internal battery has run dry" message on
emulators or flash carts without one.

Everything that runs off the clock speeds up with it: berries grow, daily events
(Shoal Cave tides, the lottery, Dewford trends, Mirage Island) roll over, and
day-based timers such as Pokérus tick down at the in-game rate.

The day/night tint, time-of-day evolutions and encounters built on top of this
are covered in {doc}`time-of-day`.

## Where it lives

| What | Where |
| --- | --- |
| `FAKE_RTC` / `FAKE_RTC_SPEED` build flags | `include/config.h` |
| Clock storage (`fakeRtc`, days/hours/minutes/seconds) | `struct SaveBlock2` in `include/global.h`, in what was `filler_90` |
| Per-frame tick | `RtcAdvanceFakeClock` in `src/rtc.c`, called from `PlayTimeCounter_Update` in `src/play_time.c` |
| Clock reads | `RtcGetInfo` in `src/rtc.c` returns the fake clock instead of the hardware |
| Clock reset (new game, reset-clock screen) | `RtcReset` in `src/rtc.c` |

The rest of the game is untouched: it still reads time through `RtcGetInfo`,
and the player's wall-clock setting is stored as the usual offset in
`localTimeOffset`.

## Tuning

`FAKE_RTC_SPEED` is the number of in-game seconds per real second. Any whole
number works; each frame adds `FAKE_RTC_SPEED / 60` in-game seconds, carrying
the remainder. The GBA runs at about 59.73 frames per second, so at 60 an hour
is 60.3 real seconds and a day is 24.1 real minutes.

| `FAKE_RTC_SPEED` | Real time per in-game hour | Real time per in-game day |
| --- | --- | --- |
| 1 | 1 hour | 24 hours |
| 30 | 2 minutes | 48 minutes |
| 60 | 1 minute | 24 minutes |
| 120 | 30 seconds | 12 minutes |

Set `FAKE_RTC` to `FALSE` to go back to the cartridge clock.

## Limits

- The clock stops counting days after day 32767 so the game's 16-bit day
  counters don't overflow. At 60× that's roughly 540 hours of play on one save.
  Time of day keeps cycling past that point, but daily events stop.
- Saves made before this change have no clock stored. The first time one is
  loaded, the fake clock is started from the save's last recorded local time
  (`StartFakeClockIfUnset` in `src/rtc.c`), so the game carries on from where
  it left off instead of jumping.
