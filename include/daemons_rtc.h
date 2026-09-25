#ifndef GUARD_DAEMONS_RTC_H
#define GUARD_DAEMONS_RTC_H

// T-265: the cartridge's own clock, when it has one. Read-only -- see src/daemons_rtc.c.
struct DaemonsClock
{
    u16 year;       // 2000-2099
    u8 month;       // 1-12
    u8 day;         // 1-31
    u8 hour;        // 0-23
    u8 minute;
    u8 second;
};

bool8 DaemonsRtc_Read(struct DaemonsClock *clock);
void DaemonsRtc_MixIntoRng(void);

#endif // GUARD_DAEMONS_RTC_H
