#ifndef GUARD_CONFIG_H
#define GUARD_CONFIG_H

// In the Generation 3 games, Asserts were used in various debug builds.
// Ruby/Sapphire and Emerald do not have these asserts while Fire Red
// still has them in the ROM. This is because the developers forgot
// to define NDEBUG before release, however this has been changed as
// Ruby's actual debug build does not use the AGBPrint features.
#define NDEBUG

// To enable printf debugging, comment out "#define NDEBUG". This allows
// the various AGBPrint functions to be used. (See include/gba/isagbprint.h).
// See below for enabling different pretty printing versions.

#ifndef NDEBUG

#define PRETTY_PRINT_MINI_PRINTF (0)
#define PRETTY_PRINT_LIBC (1)

#define LOG_HANDLER_AGB_PRINT (0)
#define LOG_HANDLER_NOCASH_PRINT (1)
#define LOG_HANDLER_MGBA_PRINT (2)

// Use this switch to choose a handler for pretty printing.
// NOTE: mini_printf supports a custom pretty printing formatter to display preproc encoded strings. (%S)
//       some libc distributions (especially dkp arm-libc) will fail to link pretty printing.
#define PRETTY_PRINT_HANDLER (PRETTY_PRINT_MINI_PRINTF)

// Use this switch to choose a handler for printf output.
// NOTE: These will only work on the respective emulators and should not be used in a productive environment.
//       Some emulators or real hardware might (and is allowed to) crash if they are used.
//       AGB_PRINT is supported on respective debug units.

#define LOG_HANDLER (LOG_HANDLER_MGBA_PRINT)
#endif

#define ENGLISH

#ifdef ENGLISH
#define UNITS_IMPERIAL
#define CHAR_DEC_SEPARATOR CHAR_PERIOD // Period is used as a decimal separator only in the UK and the US.
#else
#define UNITS_METRIC
#define CHAR_DEC_SEPARATOR CHAR_COMMA
#endif

// Replace the cartridge's real-time clock with a clock stored in the save file
// that only runs while the game is being played. FAKE_RTC_SPEED is how many
// in-game seconds pass per real second: at 60, an in-game hour takes about a
// real minute and a full day about 24 minutes. Set FAKE_RTC to FALSE to go back
// to the hardware clock.
#define FAKE_RTC       TRUE
#define FAKE_RTC_SPEED 60

// Tint the overworld and battle backgrounds by time of day, darkening towards
// night. Indoor and underground maps are left alone.
#define DAY_NIGHT_TINT TRUE

// Uncomment to fix some identified minor bugs
//#define BUGFIX

// Various undefined behavior bugs may or may not prevent compilation with
// newer compilers. So always fix them when using a modern compiler.
#if MODERN || defined(BUGFIX)
#ifndef UBFIX
#define UBFIX
#endif
#endif

#endif // GUARD_CONFIG_H
