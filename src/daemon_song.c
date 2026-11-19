// T-595: a daemon's SONG in the engine (DAEMONS vision 9.36; the user, 2026-11-18). DRAFT.
//
// "in the index you can still look at a past form and hear its song ... if the current form hasn't reached maturity
// ... its song as it is at that point in time"; "When any daemon evolves in the game you get to hear its song, after
// it sings its song perfectly it then changes into the new form." The songs are T-594's (DAEMONS tools/gensongs.py),
// written here as one table by its --write (src/data/daemon_songs.h).
//
// THE PLAYER is a task, a frame at a time, on PLAY's own sound path (play.c's Play_SoundNote, which plays a note as
// the HARP's natural bent to its pitch):
//     the tune    the HARP on SE2, a note at its frame (its start counted in half beats from the part's start, so the
//                 tune and the bass never drift apart)
//     the bass    the HARP on SE3, a root a bar, quieter until the part is known
//     the static  a note not yet learned: the same HARP note, its pitch flickering every FLICKER frames among the notes
//                 in the key that agree with it (sSongStatic, the least agreeing leaving first as it is learned) and the
//                 true note more often the nearer it is; and under it the cartridge's noise channel on SE1, a hiss
//                 that fades as the note closes. A bend never re-plucks, so it is one voice tuning in, as a radio does.
// T-596, THE HUM: a daemon near its evolution hums one bar of its own part (SONG_HUM, DaemonSong_Hum), every voice of
// it at HUM_VOLUME and the map's music left as it is, so it sits under the music where a song sits over it; the note
// sounding and the pitch heard are kept in the task (tHeard), so the follower can draw what it hums as it hums it.
// It costs no RAM but the task's own data: no static, no heap. The noise is a song of one note defined here (a voice
// and a track in ROM), so no song table row, no voicegroup and no .mid were added for it.
#include "global.h"
#include "daemon_song.h"
#include "m4a.h"
#include "play.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "sound.h"
#include "task.h"
#include "constants/sound.h"
#include "constants/species.h"

extern const struct Evolution gEvolutionTable[][EVOS_PER_MON];

struct DaemonSongPart
{
    u16 note;        // its first note in sSongNotes
    u16 bass;        // its first root in sSongBass
    u16 parent;      // the form its part continues, SPECIES_NONE for a line's first
    u8 noteCount;
    u8 bassCount;
    u8 openCount;    // the opening's notes (its cry), the first it learns
    u8 bpm;
    u8 tonic;        // its key: the tonic's pitch class, and whether minor
    u8 minor;
    u8 from;         // the level its part begins
    u8 to;           // the level it is perfect at
};

#include "data/daemon_songs.h"

#define FLICKER      4      // a static note's pitch moves every this many frames
#define PART_GAP     36     // frames between one part and the next (gensongs: 0.6 s)
#define MAX_PARTS    3
#define ALLUSION_MAX 4      // an allusion sings at most this many of a part's opening notes
#define BGM_DUCKED   85     // the map's music, under a song, as under a cry (sound.c)
#define HISS_MAX     80     // the noise at its loudest, a note with nothing of it learned (gensongs' STATIC_FLOOR)
// T-596: a hum's every voice at 3/8 of a song's, the map's music not ducked. A song ducks the music to a third
// (BGM_DUCKED) and sings over it; the hum is that ratio turned over -- about a third of the music's level, under it, so
// it is heard as something the daemon does beside the player and never as the game's music stopping for it. DRAFT,
// for the user's ear.
#define HUM_VOLUME   96
#define SHARE_HUM    0x100  // tShare's high byte: the song is a hum
#define BAR_HALVES   8      // a bar is four beats (gensongs: the bass's root a bar), eight half beats

// The task's data: the whole state of a song.
#define tPart0    0   // data[0..2]: the parts' species, oldest first
#define tParts    3
#define tPart     4   // the part playing
#define tShare    5   // the last part's share; the earlier are whole
#define tLimit    6   // notes of the part to sing, 0 for all (an allusion)
#define tFrame    7   // frames since the part began
#define tNote     8   // the next note
#define tMelCum   9   // where the next note starts, in half beats
#define tBass     10
#define tBassCum  11
#define tRng      12
#define tNear     13  // the note sounding: how near it is to learned, 0..255, or -1 learned
#define tTrue     14  // its true pitch, semitones from middle C
#define tHeard    15  // T-596: the pitch heard this frame, semitones from middle C
#define NOTHING_HEARD 0x7FFF

// The noise under a static note: the noise channel, 15-bit (white), held, in a song of one long note. Stopped at the
// note's end; left alone it ends by itself after four seconds.
static const struct ToneData sHissVoice[] =
{
    {.type = 12, .key = 60, .length = 0, .pan_sweep = 0, .wav = NULL, .attack = 0, .decay = 0, .sustain = 15, .release = 0},
};
static const u8 sHissTrack[] =
{
    0xBE, 127,        // VOL
    0xBB, 30,         // TEMPO: 60 beats a minute
    0xBD, 0,          // VOICE 0
    0xFF, 60, 127,    // N96 Cn3 v127: four beats, four seconds
    0xB0,             // W96
    0xB1,             // FINE
};
static const struct SongHeader sHissSong =
{
    .trackCount = 1,
    .blockCount = 0,
    .priority = 5,
    .reverb = 0,
    .tone = (struct ToneData *)sHissVoice,
    .part = {(u8 *)sHissTrack},
};

// The natural each pitch class is played from (play.c bends a sharp up from its natural), so a bend can be set
// against it without re-plucking: C C D D E F F G G A A B.
static const u8 sNaturalSemitone[12] = {0, 0, 2, 2, 4, 5, 5, 7, 7, 9, 9, 11};

static void Task_DaemonSong(u8 taskId);

// T-596: a voice's volume, a hum's turned down to HUM_VOLUME
static u16 Loud(s16 *data, u16 volume)
{
    return (data[tShare] & SHARE_HUM) ? volume * HUM_VOLUME / 256 : volume;
}

bool8 DaemonSong_Has(u16 species)
{
    return species < ARRAY_COUNT(sSongParts) && sSongParts[species].noteCount != 0;
}

bool8 DaemonSong_GrowsOn(u16 species)
{
    u32 i;

    for (i = 0; i < EVOS_PER_MON; i++)
        if (gEvolutionTable[species][i].method != 0)
            return TRUE;
    return FALSE;
}

u8 DaemonSong_ShareOf(u16 species, u32 exp)
{
    const struct DaemonSongPart *p;
    u32 lo, hi;

    if (!DaemonSong_Has(species))
        return SONG_SHARE_WHOLE;
    p = &sSongParts[species];
    lo = gExperienceTables[gSpeciesInfo[species].growthRate][p->from];
    hi = gExperienceTables[gSpeciesInfo[species].growthRate][p->to];
    if (exp >= hi)
        return SONG_SHARE_WHOLE;
    if (exp <= lo)
        return 0;
    lo = (exp - lo) * 255 / (hi - lo);
    return lo > 254 ? 254 : lo;   // perfect is every note, and the last is learned only at the level itself
}

static u8 BoxMonShare(struct BoxPokemon *box)
{
    return DaemonSong_ShareOf(GetBoxMonData(box, MON_DATA_SPECIES), GetBoxMonData(box, MON_DATA_EXP));
}

u8 DaemonSong_MonShare(struct Pokemon *mon)
{
    return BoxMonShare(&mon->box);
}

// ------------------------------------------------------------------------------------------------------- the key

static bool8 InKey(s16 midi, u8 tonic, u8 minor)
{
    u16 scale = minor ? 0x5AD : 0xAB5;   // 0 2 3 5 7 8 10 / 0 2 4 5 7 9 11
    s16 pc = (midi - tonic) % 12;

    if (pc < 0)
        pc += 12;
    return (scale >> pc) & 1;
}

// k steps along the key from a note (a note off the key steps from the key note below it), as gensongs.key_step
static s16 KeyStep(s16 midi, s8 k, u8 tonic, u8 minor)
{
    s16 n = midi;
    s8 dir = k > 0 ? 1 : -1;

    while (!InKey(n, tonic, minor))
        n--;
    for (; k != 0; k -= dir)
    {
        n += dir;
        while (!InKey(n, tonic, minor))
            n += dir;
    }
    return n;
}

static u16 NextRng(s16 *data)
{
    data[tRng] = (u16)data[tRng] * 25173 + 13849;
    return (u16)data[tRng];
}

static s32 FrameAt(const struct DaemonSongPart *p, s32 half)
{
    return half * 1800 / p->bpm;   // a half beat at bpm is 30 / bpm seconds
}

// ------------------------------------------------------------------------------------------------------ the static

// The note's pitch for the next FLICKER frames: true, or one of the candidates still left, as gensongs' model has it
static void Flicker(s16 *data, const struct DaemonSongPart *p)
{
    u8 near = data[tNear];
    u8 keep = (8 * (255 - near) + 127) / 255;
    u16 trueOdds = 64 + 191 * near / 255;
    s16 pitch = data[tTrue];
    s16 pc = ((data[tTrue] % 12) + 12) % 12;

    if (keep != 0 && (NextRng(data) >> 8) >= trueOdds)
        pitch = KeyStep(data[tTrue] + 60, sSongStatic[(NextRng(data) >> 8) % keep], p->tonic, p->minor) - 60;
    data[tHeard] = pitch;   // T-596: what is heard, for the note drawn
    m4aMPlayPitchControl(&gMPlayInfo_SE2, TRACKS_ALL, (pitch - sNaturalSemitone[pc]) * 256);
}

static void StartHiss(s16 *data, s16 midi, u8 near)
{
    u16 vol = Loud(data, HISS_MAX * (255 - near) * (255 - near) / (255 * 255));

    m4aMPlayStop(&gMPlayInfo_SE1);
    if (vol == 0)
        return;
    MPlayStart(&gMPlayInfo_SE1, (struct SongHeader *)&sHissSong);
    m4aMPlayImmInit(&gMPlayInfo_SE1);
    m4aMPlayPitchControl(&gMPlayInfo_SE1, TRACKS_ALL, (midi - 60) * 256);   // the hiss tinted toward the note
    m4aMPlayVolumeControl(&gMPlayInfo_SE1, TRACKS_ALL, vol);
}

// ------------------------------------------------------------------------------------------------------- a part

static const struct DaemonSongPart *Part(s16 *data)
{
    return &sSongParts[data[tPart0 + data[tPart]]];
}

static u8 PartShare(s16 *data)
{
    return data[tPart] == data[tParts] - 1 ? (data[tShare] & 0xFF) : SONG_SHARE_WHOLE;
}

static u8 PartNotes(s16 *data)
{
    const struct DaemonSongPart *p = Part(data);

    return (data[tLimit] != 0 && data[tLimit] < p->noteCount) ? data[tLimit] : p->noteCount;
}

static void BeginPart(s16 *data)
{
    data[tFrame] = 0;
    data[tNote] = 0;
    data[tMelCum] = 0;
    data[tBass] = 0;
    data[tBassCum] = 0;
    data[tNear] = -1;
    data[tHeard] = NOTHING_HEARD;   // T-596
}

static void StartNote(s16 *data)
{
    const struct DaemonSongPart *p = Part(data);
    const u8 *n = sSongNotes[p->note + data[tNote]];
    u8 share = PartShare(data);
    s32 near;

    data[tTrue] = n[0] - 60;
    data[tHeard] = data[tTrue];
    Play_SoundNote(PLAY_HARP, data[tTrue], FALSE);
    if (share >= n[2])
    {
        data[tNear] = -1;
        m4aMPlayStop(&gMPlayInfo_SE1);
        if (data[tShare] & SHARE_HUM)
            m4aMPlayVolumeControl(&gMPlayInfo_SE2, TRACKS_ALL, Loud(data, 256));
    }
    else
    {
        near = ((s32)share - (n[2] - SONG_WINDOW)) * 255 / SONG_WINDOW;
        if (near < 0)
            near = 0;
        if (near > 255)
            near = 255;
        data[tNear] = near;
        data[tRng] = data[tPart0 + data[tPart]] * 31 + data[tNote] * 7 + 1;
        m4aMPlayVolumeControl(&gMPlayInfo_SE2, TRACKS_ALL, Loud(data, 200 + 56 * near / 255));
        Flicker(data, p);
        StartHiss(data, n[0], near);
    }
    data[tMelCum] += n[1];
    data[tNote]++;
}

static void StartBass(s16 *data)
{
    const struct DaemonSongPart *p = Part(data);
    const u8 *b = sSongBass[p->bass + data[tBass]];

    Play_SoundNote(PLAY_HARP, b[0] - 60, TRUE);
    m4aMPlayVolumeControl(&gMPlayInfo_SE3, TRACKS_ALL, 70 + 80 * PartShare(data) / 255);
    data[tBassCum] += b[1];
    data[tBass]++;
}

static void EndSong(u8 taskId, bool8 cut)
{
    m4aMPlayStop(&gMPlayInfo_SE1);
    if (cut)
    {
        m4aMPlayStop(&gMPlayInfo_SE2);
        m4aMPlayStop(&gMPlayInfo_SE3);
    }
    if (!(gTasks[taskId].data[tShare] & SHARE_HUM))   // T-596: a hum never ducked it
        m4aMPlayVolumeControl(&gMPlayInfo_BGM, TRACKS_ALL, 256);
    DestroyTask(taskId);
}

static void Task_DaemonSong(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    const struct DaemonSongPart *p = Part(data);
    u8 count = PartNotes(data);

    if (data[tNote] < count && data[tFrame] >= FrameAt(p, data[tMelCum]))
    {
        StartNote(data);
    }
    else if (data[tNear] >= 0)
    {
        // the note's frames are counted from its own start, so it flickers as gensongs' model does
        s32 since = data[tFrame] - FrameAt(p, data[tMelCum] - sSongNotes[p->note + data[tNote] - 1][1]);

        if (data[tNote] >= count && data[tFrame] >= FrameAt(p, data[tMelCum]))
        {
            data[tNear] = -1;                      // the part's last note is over: the hiss with it
            m4aMPlayStop(&gMPlayInfo_SE1);
        }
        else if (since % FLICKER == 0)
        {
            Flicker(data, p);
        }
    }
    if (data[tLimit] == 0 && data[tBass] < p->bassCount && data[tFrame] >= FrameAt(p, data[tBassCum]))
        StartBass(data);

    if (data[tNote] >= count && data[tFrame] >= FrameAt(p, data[tMelCum]) + PART_GAP)
    {
        if (++data[tPart] >= data[tParts])
        {
            EndSong(taskId, FALSE);   // the last note rings out by itself
            return;
        }
        BeginPart(data);
        return;
    }
    data[tFrame]++;
}

// ------------------------------------------------------------------------------------------------------ the calls

bool8 DaemonSong_IsPlaying(void)
{
    return FuncIsActiveTask(Task_DaemonSong);
}

void DaemonSong_Stop(void)
{
    u8 taskId = FindTaskIdByFunc(Task_DaemonSong);

    if (taskId != TASK_NONE)
        EndSong(taskId, TRUE);
}

bool8 DaemonSong_Start(u16 species, u8 share, u8 mode)
{
    u16 line[MAX_PARTS];
    u8 count = 0, taskId, i;
    s16 *data;

    DaemonSong_Stop();
    if (!DaemonSong_Has(species))
        return FALSE;
    line[count++] = species;
    if (!(mode & SONG_OWN_PART))
    {
        while (count < MAX_PARTS && DaemonSong_Has(sSongParts[line[count - 1]].parent))
        {
            line[count] = sSongParts[line[count - 1]].parent;
            count++;
        }
    }

    taskId = CreateTask(Task_DaemonSong, 80);
    data = gTasks[taskId].data;
    for (i = 0; i < count; i++)
        data[tPart0 + i] = line[count - 1 - i];   // oldest first
    data[tParts] = count;
    data[tPart] = 0;
    data[tShare] = share;
    data[tLimit] = 0;
    if (mode & SONG_ALLUSION)
    {
        data[tLimit] = sSongParts[species].openCount;
        if (data[tLimit] == 0 || data[tLimit] > ALLUSION_MAX)
            data[tLimit] = ALLUSION_MAX;
    }
    BeginPart(data);
    m4aMPlayVolumeControl(&gMPlayInfo_BGM, TRACKS_ALL, BGM_DUCKED);
    return TRUE;
}

void DaemonSong_PlayForIndex(u16 species)
{
    u32 i, j;
    u8 share = SONG_SHARE_WHOLE;
    bool8 found = FALSE;

    if (DaemonSong_IsPlaying())
    {
        DaemonSong_Stop();
        return;
    }
    for (i = 0; i < gPlayerPartyCount && !found; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG) == species)
        {
            share = DaemonSong_MonShare(&gPlayerParty[i]);
            found = TRUE;
        }
    }
    for (i = 0; i < TOTAL_BOXES_COUNT && !found; i++)
    {
        for (j = 0; j < IN_BOX_COUNT && !found; j++)
        {
            struct BoxPokemon *box = GetBoxedMonPtr(i, j);

            if (GetBoxMonData(box, MON_DATA_SPECIES_OR_EGG) == species)
            {
                share = BoxMonShare(box);
                found = TRUE;
            }
        }
    }
    if (!DaemonSong_Start(species, share, SONG_LINE))
        PlayCry_NormalNoDucking(species, 0, CRY_VOLUME_RS, CRY_PRIORITY_NORMAL);
}

// ------------------------------------------------------------------------------------------- T-596: the wish to sing

u8 DaemonSong_Wish(struct Pokemon *mon)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG);
    u8 share;

    if (species == SPECIES_NONE || species == SPECIES_EGG || !DaemonSong_Has(species) || !DaemonSong_GrowsOn(species))
        return 0;
    share = DaemonSong_MonShare(mon);
    if (share < SONG_WISH_FROM)
        return 0;
    return 1 + (share - SONG_WISH_FROM) * 254 / (SONG_SHARE_WHOLE - SONG_WISH_FROM);
}

// The bar it hums: the one holding the first note it has not learned (the last bar, when it has them all) -- or, one
// time in three, any bar before that, one it already sings. (A bar of under three notes takes the one before it in.)
bool8 DaemonSong_Hum(u16 species, u8 share, u16 seed)
{
    const struct DaemonSongPart *p;
    u8 i, first, end, bar, frontier;
    u16 cum, startHalf;
    s16 *data;
    u8 taskId;

    if (!DaemonSong_Has(species))
        return FALSE;
    DaemonSong_Stop();
    p = &sSongParts[species];
    frontier = 0xFF;
    for (i = 0, cum = 0; i < p->noteCount; cum += sSongNotes[p->note + i][1], i++)
    {
        if (share < sSongNotes[p->note + i][2])
        {
            frontier = cum / BAR_HALVES;
            break;
        }
        frontier = cum / BAR_HALVES;   // so far: the bar of the last note (a part learned whole hums its close)
    }
    bar = frontier;
    if (frontier > 0 && seed % 3 == 0)
        bar = (seed / 3) % frontier;
    first = end = p->noteCount;
    startHalf = 0;
    for (i = 0, cum = 0; i < p->noteCount; cum += sSongNotes[p->note + i][1], i++)
    {
        if (first == p->noteCount && cum >= bar * BAR_HALVES)
        {
            first = i;
            startHalf = cum;
        }
        if (cum >= (bar + 1) * BAR_HALVES)
        {
            end = i;
            break;
        }
    }
    if (first >= end)
        return FALSE;
    // A bar of fewer than three notes (a part's closing note can fill one alone) takes in the bar before it, so the
    // hum is always a phrase and never one held note.
    if (end - first < 3 && bar > 0)
    {
        for (i = 0, cum = 0; i < first; cum += sSongNotes[p->note + i][1], i++)
        {
            if (cum >= (bar - 1) * BAR_HALVES)
            {
                first = i;
                startHalf = cum;
                break;
            }
        }
    }

    taskId = CreateTask(Task_DaemonSong, 80);
    data = gTasks[taskId].data;
    data[tPart0] = species;
    data[tParts] = 1;
    data[tPart] = 0;
    data[tShare] = share | SHARE_HUM;
    data[tLimit] = end;     // non-zero: no bass under it
    BeginPart(data);
    data[tNote] = first;
    data[tMelCum] = startHalf;
    data[tFrame] = FrameAt(p, startHalf);
    return TRUE;
}

bool8 DaemonSong_IsHumming(void)
{
    u8 taskId = FindTaskIdByFunc(Task_DaemonSong);

    return taskId != TASK_NONE && (gTasks[taskId].data[tShare] & SHARE_HUM);
}

bool8 DaemonSong_Heard(u16 *serial, s16 *heard, s16 *truePitch, s16 *near)
{
    u8 taskId = FindTaskIdByFunc(Task_DaemonSong);
    s16 *data;

    if (taskId == TASK_NONE)
        return FALSE;
    data = gTasks[taskId].data;
    if (data[tHeard] == NOTHING_HEARD)
        return FALSE;   // between parts, or before the first note
    *serial = (data[tPart] << 8) | data[tNote];
    *heard = data[tHeard];
    *truePitch = data[tTrue];
    *near = data[tNear];
    return TRUE;
}

#if DAEMONS_DEBUG
u32 DaemonSong_ExpAtShare(u16 species, u8 share)
{
    const struct DaemonSongPart *p;
    u32 lo, hi;

    if (!DaemonSong_Has(species))
        return 0;
    p = &sSongParts[species];
    lo = gExperienceTables[gSpeciesInfo[species].growthRate][p->from];
    hi = gExperienceTables[gSpeciesInfo[species].growthRate][p->to];
    return lo + (hi - lo) * share / 255 + 1;
}
#endif
