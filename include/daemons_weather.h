#ifndef GUARD_DAEMONS_WEATHER_H
#define GUARD_DAEMONS_WEATHER_H

// T-415: WEATHER, by season -- see src/daemons_weather.c.

// DEBUG's WEATHER row: 0 follows the date, then one of these + 1.
enum {
    DAEMONS_SKY_CLEAR,
    DAEMONS_SKY_RAIN,
    DAEMONS_SKY_STORM,
    DAEMONS_SKY_SNOW,
    DAEMONS_SKY_FOG,
    DAEMONS_SKY_WIND,
    DAEMONS_SKY_COUNT
};

extern u8 gDaemonsWeatherOverride;

// The weather a map shows: its header's own unless it is an ordinary outdoor map (header SUNNY, outdoor map type),
// where the day's events decide. `headerWeather` is already translated (field_weather_util.c).
u8 DaemonsWeatherForMap(u8 headerWeather, u8 mapType, u8 mapsec);

#endif // GUARD_DAEMONS_WEATHER_H
