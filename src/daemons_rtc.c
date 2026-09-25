#include "global.h"
#include "random.h"
#include "daemons_rtc.h"

// T-265: THE CARTRIDGE'S CLOCK (the user, 2026-09-25, on an EZ-Flash with a clock battery in it).
//
// FireRed's cartridges had no clock, so the game only ever knew its own play time. Ruby, Sapphire and Emerald carried
// a Seiko S-3511A on the cartridge's GPIO port, and the flash carts and emulators that run them provide one there.
// This reads it, and only reads it: the driver below is pret's pokeemerald src/siirtc.c cut down to the two reads,
// so nothing in DAEMONS can reset or set the player's clock -- Emerald's RtcInit resets a clock it finds in 12-hour
// mode, which on a flash cart would wipe the time the player set in its menu.
//
// Where there is no clock -- a retail FireRed board, an emulator with its clock off -- the port reads back ROM bytes
// and the date fails the checks in DaemonsRtc_Read, so every caller sees "no clock" and the game plays as it did.
//
// USED FOR: the random number generator's seed (DaemonsRtc_MixIntoRng), and the terminal's TIME code.

#define SCK_HI  1
#define SIO_HI  2
#define CS_HI   4

#define DIR_ALL_OUT 7
#define DIR_READ    5   // SCK and CS out, SIO in

#define CMD_STATUS_RD   (0x60 | (1 << 1) | 1)
#define CMD_DATETIME_RD (0x60 | (2 << 1) | 1)

#define STATUS_24HOUR 0x40

#define GPIO_PORT_DATA        (*(vu16 *)0x80000C4)
#define GPIO_PORT_DIRECTION   (*(vu16 *)0x80000C6)
#define GPIO_PORT_READ_ENABLE (*(vu16 *)0x80000C8)

// The library's name, as Emerald's cartridges carry it. Kept in the ROM on purpose: tools that decide whether a game
// has a clock look for this string.
const char gDaemonsRtcLibVersion[] = "SIIRTC_V001";

static void WriteCommand(u8 value)
{
    u8 i, bit;

    for (i = 0; i < 8; i++)
    {
        bit = (value >> (7 - i)) & 1;
        GPIO_PORT_DATA = (bit << 1) | CS_HI;
        GPIO_PORT_DATA = (bit << 1) | CS_HI;
        GPIO_PORT_DATA = (bit << 1) | CS_HI;
        GPIO_PORT_DATA = (bit << 1) | SCK_HI | CS_HI;
    }
}

static u8 ReadData(void)
{
    u8 i, value = 0;

    for (i = 0; i < 8; i++)
    {
        GPIO_PORT_DATA = CS_HI;
        GPIO_PORT_DATA = CS_HI;
        GPIO_PORT_DATA = CS_HI;
        GPIO_PORT_DATA = CS_HI;
        GPIO_PORT_DATA = CS_HI;
        GPIO_PORT_DATA = SCK_HI | CS_HI;
        value = (value >> 1) | (((GPIO_PORT_DATA & SIO_HI) >> 1) << 7);
    }
    return value;
}

// One command, then `len` bytes back.
static void ReadRegister(u8 command, u8 *out, u8 len)
{
    u8 i;

    GPIO_PORT_READ_ENABLE = TRUE;
    GPIO_PORT_DATA = SCK_HI;
    GPIO_PORT_DATA = SCK_HI | CS_HI;
    GPIO_PORT_DIRECTION = DIR_ALL_OUT;
    WriteCommand(command);
    GPIO_PORT_DIRECTION = DIR_READ;
    for (i = 0; i < len; i++)
        out[i] = ReadData();
    GPIO_PORT_DATA = SCK_HI;
    GPIO_PORT_DATA = SCK_HI;
    GPIO_PORT_READ_ENABLE = FALSE;
}

// Binary-coded decimal: each nibble a digit. Anything else is not a clock talking.
static bool8 FromBcd(u8 bcd, u8 max, u8 *out)
{
    if ((bcd & 0xF) > 9 || (bcd >> 4) > 9)
        return FALSE;
    *out = (bcd >> 4) * 10 + (bcd & 0xF);
    return *out <= max;
}

bool8 DaemonsRtc_Read(struct DaemonsClock *clock)
{
    u8 status, year, hour, raw[7];   // year, month, day, weekday, hour, minute, second

    ReadRegister(CMD_STATUS_RD, &status, 1);
    ReadRegister(CMD_DATETIME_RD, raw, 7);   // year, month, day, weekday, hour, minute, second
    if (!(status & STATUS_24HOUR))
        return FALSE;                         // a clock nobody has set up, or no clock at all
    if (!FromBcd(raw[0], 99, &year)
     || !FromBcd(raw[1], 12, &clock->month) || clock->month == 0
     || !FromBcd(raw[2], 31, &clock->day) || clock->day == 0
     || !FromBcd(raw[3] & 7, 6, &clock->weekday)
     || !FromBcd(raw[4] & 0x3F, 23, &hour)
     || !FromBcd(raw[5], 59, &clock->minute)
     || !FromBcd(raw[6] & 0x7F, 59, &clock->second))
        return FALSE;
    clock->year = 2000 + year;
    clock->hour = hour;
    //  T-277: A CLOCK NOBODY SET IS NO CLOCK. mGBA keeps an unset clock as a zeroed record and counts it on from
    //  2000 as if 1970 were its start, so it reads 56 years ahead and hours off (engine.md trap 31) -- and a flash
    //  cart's clock nobody has set sits at 2000-01-01. Either would put night at noon, so a year outside 2020-2049
    //  is read as no clock at all and the day falls back to play time.
    if (clock->year < 2020 || clock->year > 2049)
        return FALSE;
    return TRUE;
}

// The seed has come from a timer since FireRed shipped: how long the title screen was up, to the cycle. On a
// cartridge that is plenty. An emulator replaying the same presses lands on the same seed, and the clock is the one
// thing that is never the same twice, so it is folded in when there is one. Without a clock this does nothing.
void DaemonsRtc_MixIntoRng(void)
{
    struct DaemonsClock clock;

    if (!DaemonsRtc_Read(&clock))
        return;
    gRngValue ^= ((u32)clock.second << 26) ^ ((u32)clock.minute << 20) ^ ((u32)clock.hour << 15)
               ^ ((u32)clock.day << 10) ^ ((u32)clock.month << 6) ^ clock.year;
    Random();
}
