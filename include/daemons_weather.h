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
// T-417 P4: HARMONY IN BALANCE turns this section's sky for the rest of the watch. FALSE where the day decides nothing.
bool8 DaemonsSongSky(u8 headerWeather, u8 mapType, u8 mapsec);
// T-416: the sky over a section now (DAEMONS_SKY_*), and whether a thunderstorm touched the islands today.
u8 DaemonsSkyAt(u8 mapsec);
bool8 DaemonsStormOnIslandsToday(void);

// T-415 phase 2: the sky a field weather is (DAEMONS_SKY_*), and the sky over the player now.
u8 DaemonsSkyOfWeather(u8 weather);
u8 DaemonsSkyNow(void);
// The sky's own boosts (x1.25): a thunderstorm SIGNAL, fog LATENT, wind VECTOR.
bool8 DaemonsSkyBoostsType(u8 type);
// The battle's light weather tint over the terrain's palettes, and whether a boosted routine may glint here.
void DaemonsTintBattleTerrain(void);
bool8 DaemonsSkyGlintAllowed(void);

#endif // GUARD_DAEMONS_WEATHER_H
