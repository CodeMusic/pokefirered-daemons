#include "global.h"
#include "gflib.h"
#include "task.h"
#include "event_data.h"
#include "text_window.h"
#include "quest_log.h"
#include "region_map.h"
#include "strings.h"
#include "map_name_popup.h"
#include "constants/maps.h"
#include "daemons_time.h"

#define FLOOR_ROOFTOP 127

static void Task_MapNamePopup(u8 taskId);
static u16 MapNamePopupCreateWindow(bool32 palIntoFadedBuffer);
static void MapNamePopupPrintMapNameOnWindow(u16 windowId);
static u8 *MapNamePopupAppendFloorNum(u8 *dest, s8 flags);

#define tState              data[0]
#define tTimer              data[1]
#define tPos                data[2]
#define tReshow             data[3]
#define tWindowId           data[4]
#define tWindowExists       data[5]
#define tWindowCleared      data[6]
#define tWindowDestroyed    data[7]
#define tPalIntoFadedBuffer data[8]

//  T-196: AN ECHO SHOULD NOT LOOK LIKE AN EVENT. R says the last field line again, and in the ordinary
//  message box that is indistinguishable from something happening now -- which matters most in exactly the
//  case the feature exists for, a player who missed a line and is not sure whether they just triggered
//  something.
//
//  The label goes in the MAP NAME POPUP rather than in a window of its own. The popup is already a framed
//  label that slides in at the top, it owns its tiles and its palette, and its task handles the slide out --
//  so a new window would mean a new baseBlock guessed against the field's own, which is how a tilemap gets
//  corrupted in a way only playing it shows. The message box is at the bottom; the label is at the top;
//  nothing overlaps.
//
//  FRLG has no sign-style box to borrow (Emerald's MSGBOX_SIGN does not exist here), and prefixing the text
//  was the other candidate: the box holds two lines, most field lines use both, and a prefix pushes the
//  second one out of the frame.
//  T-196: set for one popup, consumed by the printer.
static EWRAM_DATA const u8 *sPopupOverride = NULL;

//  T-269: THE DAY, ON LEAVING A BUILDING (decided by the user 2026-09-25). Stepping out of a door, the popup shows the
//  town's name and then, sliding in again in the same frame, the day: "F · WEDNESDAY · NIGHT" -- the day's NOTE as
//  its mark (Sunday C to Saturday B, the user's seven and the Guide's order), then the weekday, then the watch. Ink
//  and paper, the popup's own; the day's colour is not here (9.4) but on the CHECKPOINT's trim (T-275), and the note
//  is heard at the terminal (T-274). Nothing says what either means. Wording DRAFT.
static EWRAM_DATA bool8 sDayBannerPending = FALSE;
static EWRAM_DATA u8 sDayBanner[28] = {0};

static const u8 *const sDayNotes[WEEKDAY_COUNT] = {
    gText_DayNoteC, gText_DayNoteD, gText_DayNoteE, gText_DayNoteF, gText_DayNoteG, gText_DayNoteA, gText_DayNoteB,
};
static const u8 *const sDayNames[WEEKDAY_COUNT] = {
    gText_DaySunday, gText_DayMonday, gText_DayTuesday, gText_DayWednesday, gText_DayThursday, gText_DayFriday,
    gText_DaySaturday,
};
static const u8 *const sWatchNames[WATCH_COUNT] = {
    [WATCH_DAY] = gText_WatchDay, [WATCH_DUSK] = gText_WatchDusk, [WATCH_NIGHT] = gText_WatchNight,
    [WATCH_DAWN] = gText_WatchDawn,
};

static const u8 *DayBannerText(void)
{
    u8 day = DaemonsWeekday();
    u8 *p = StringCopy(sDayBanner, sDayNotes[day]);
    p = StringCopy(p, gText_DayBannerDot);
    p = StringCopy(p, sDayNames[day]);
    p = StringCopy(p, gText_DayBannerDot);
    StringCopy(p, sWatchNames[DaemonsWatch()]);
    return sDayBanner;
}

//  Asked for when the player comes out of a door (overworld.c). The popup that shows the town's name then shows
//  this; a map that shows no name shows only this.
void MapNamePopup_QueueDayBanner(bool8 showNameFirst, bool32 palIntoFadedBuffer)
{
    if (showNameFirst)
    {
        sDayBannerPending = TRUE;
        ShowMapNamePopup(palIntoFadedBuffer);
        //  T-278: refused (quest log playback, or the flag) -- the day must not wait for the next town's name
        if (FindTaskIdByFunc(Task_MapNamePopup) == TASK_NONE)
            sDayBannerPending = FALSE;
    }
    else
    {
        sDayBannerPending = FALSE;
        ShowMapLabelPopup(DayBannerText(), palIntoFadedBuffer);
    }
}

void ShowMapNamePopup(bool32 palIntoFadedBuffer)
{
    u8 taskId;
    if (FlagGet(FLAG_DONT_SHOW_MAP_NAME_POPUP) != TRUE && !QL_IS_PLAYBACK_STATE)
    {
        taskId = FindTaskIdByFunc(Task_MapNamePopup);
        if (taskId == TASK_NONE)
        {
            taskId = CreateTask(Task_MapNamePopup, 90);
            ChangeBgX(0,  0x0000, 0);
            ChangeBgY(0, -0x1081, 0);
            gTasks[taskId].tState = 0;
            gTasks[taskId].tPos = 0;
            gTasks[taskId].tPalIntoFadedBuffer = palIntoFadedBuffer;
        }
        else
        {
            if (gTasks[taskId].tState != 4)
                gTasks[taskId].tState = 4;
            gTasks[taskId].tReshow = TRUE;
        }
    }
}

void ShowFieldLabelPopup(const u8 *text)
{
    ShowMapLabelPopup(text, FALSE);
}

void ShowMapLabelPopup(const u8 *text, bool32 palIntoFadedBuffer)
{
    sPopupOverride = text;
    ShowMapNamePopup(palIntoFadedBuffer);
    if (FindTaskIdByFunc(Task_MapNamePopup) == TASK_NONE)
        sPopupOverride = NULL;         // refused (quest log playback, or the flag): do not leave it armed
}

//  T-215: A SCHOOL FLOOR NAMES ITS SUBJECT AS YOU ARRIVE, the way a town names itself.
//
//  The brief: the section name must be readable "right away", without pressing anything. Vanilla's popup
//  already does exactly that for towns -- but it only fires when the map SECTION changes, and walking in
//  from CALLOW does not change it. So a floor with a subject gets a label of its own, keyed by map.
//  Every floor added later is one row here.
static const u8 sText_Floor_Language[] = _("1F  LANGUAGE");
static const u8 sText_Floor_Attention[] = _("2F  ATTENTION");
static const u8 sText_Floor_Memory[] = _("3F  MEMORY");
static const u8 sText_Floor_Categories[] = _("4F  CATEGORIES");
static const u8 sText_Floor_Learning[] = _("5F  LEARNING");
static const u8 sText_Floor_Bias[] = _("6F  BIAS");
static const u8 sText_Floor_Error[] = _("7F  ERROR");


static const struct { u16 map; const u8 *label; } sFloorLabels[] =
{
    { MAP_VIRIDIAN_CITY_SCHOOL, sText_Floor_Language },
    { MAP_VIRIDIAN_CITY_SCHOOL_2F, sText_Floor_Attention },
    { MAP_VIRIDIAN_CITY_SCHOOL_3F, sText_Floor_Memory },
    { MAP_VIRIDIAN_CITY_SCHOOL_4F, sText_Floor_Categories },
    { MAP_VIRIDIAN_CITY_SCHOOL_5F, sText_Floor_Learning },
    { MAP_VIRIDIAN_CITY_SCHOOL_6F, sText_Floor_Bias },
    { MAP_VIRIDIAN_CITY_SCHOOL_7F, sText_Floor_Error },
};

const u8 *GetMapFloorLabel(void)
{
    u32 i;
    u16 map = (gSaveBlock1Ptr->location.mapGroup << 8) | gSaveBlock1Ptr->location.mapNum;

    for (i = 0; i < ARRAY_COUNT(sFloorLabels); i++)
    {
        if (sFloorLabels[i].map == map)
            return sFloorLabels[i].label;
    }
    return NULL;
}

static void Task_MapNamePopup(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    switch (task->tState)
    {
    case 0:
        task->tWindowId = MapNamePopupCreateWindow(task->tPalIntoFadedBuffer);
        task->tWindowExists = TRUE;
        task->tState = 1;
        break;
    case 1:
        if (IsDma3ManagerBusyWithBgCopy())
            break;
        // fallthrough
    case 2:
        task->tPos -= 2;
        if (task->tPos <= -24)
        {
            task->tState = 3;
            task->tTimer = 0;
        }
        break;
    case 3:
        task->tTimer++;
        if (task->tTimer > 120)
        {
            task->tTimer = 0;
            task->tState = 4;
        }
        break;
    case 4:
        task->tPos += 2;
        if (task->tPos >= 0)
        {
            if (task->tReshow)
            {
                MapNamePopupPrintMapNameOnWindow(task->tWindowId);
                CopyWindowToVram(task->tWindowId, COPYWIN_GFX);
                task->tState = 1;
                task->tReshow = FALSE;
            }
            else if (sDayBannerPending)
            {
                //  T-269: the name has been seen; now the day, in the same frame.
                sDayBannerPending = FALSE;
                sPopupOverride = DayBannerText();
                MapNamePopupPrintMapNameOnWindow(task->tWindowId);
                CopyWindowToVram(task->tWindowId, COPYWIN_GFX);
                task->tState = 1;
            }
            else
            {
                task->tState = 6;
                return;
            }
        }
    case 5:
        break;
    case 6:
        if (task->tWindowExists && !task->tWindowCleared)
        {
            rbox_fill_rectangle(task->tWindowId);
            CopyWindowToVram(task->tWindowId, COPYWIN_MAP);
            task->tWindowCleared = TRUE;
        }
        task->tState = 7;
        return;
    case 7:
        if (!IsDma3ManagerBusyWithBgCopy())
        {
            if (task->tWindowExists)
            {
                RemoveWindow(task->tWindowId);
                task->tWindowExists = FALSE;
                task->tWindowDestroyed = TRUE;
            }
            task->tState = 8;
            ChangeBgY(0, 0x00000000, 0);
        }
        return;
    case 8:
        sDayBannerPending = FALSE;   // T-278: a banner never outlives the popup it was queued for
        DestroyTask(taskId);
        return;
    }
    SetGpuReg(REG_OFFSET_BG0VOFS, task->tPos);
}

void DismissMapNamePopup(void)
{
    u8 taskId;
    s16 *data;
    taskId = FindTaskIdByFunc(Task_MapNamePopup);
    if (taskId != TASK_NONE)
    {
        data = gTasks[taskId].data;
        if (tState < 6)
            tState = 6;
        sDayBannerPending = FALSE;
    }
}

bool32 IsMapNamePopupTaskActive(void)
{
    return FindTaskIdByFunc(Task_MapNamePopup) != TASK_NONE ? TRUE : FALSE;
}

#define WIN_PAL_NUM  13

static u16 MapNamePopupCreateWindow(bool32 palintoFadedBuffer)
{
    struct WindowTemplate windowTemplate = {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 29,
        .width = 14,
        .height = 2,
        .paletteNum = WIN_PAL_NUM,
        .baseBlock = 0x001
    };
    u16 windowId;
    u16 tileNum = 0x01D;
    if (gMapHeader.floorNum != 0 || sDayBannerPending || sPopupOverride == sDayBanner)
    {
        //  T-269: the day banner is wider than a town's name, so it takes the floor number's width.
        if (gMapHeader.floorNum == 0 || gMapHeader.floorNum != FLOOR_ROOFTOP)
        {
            windowTemplate.width += 5;
            tileNum = 0x027;
        }
        else
        {
            // ROOFTOP
            windowTemplate.width += 8;
            tileNum = 0x02D;
        }
    }
    windowId = AddWindow(&windowTemplate);
    if (palintoFadedBuffer)
        LoadPalette(GetTextWindowPalette(3), BG_PLTT_ID(WIN_PAL_NUM), PLTT_SIZE_4BPP);
    else
        CpuCopy16(GetTextWindowPalette(3), &gPlttBufferUnfaded[BG_PLTT_ID(WIN_PAL_NUM)], PLTT_SIZE_4BPP);
    LoadStdWindowTiles(windowId, tileNum);
    DrawTextBorderOuter(windowId, tileNum, WIN_PAL_NUM);
    PutWindowTilemap(windowId);
    MapNamePopupPrintMapNameOnWindow(windowId);
    CopyWindowToVram(windowId, COPYWIN_FULL);
    return windowId;
}

static void MapNamePopupPrintMapNameOnWindow(u16 windowId)
{
    u8 mapName[25];
    u32 maxWidth = 112;
    u32 xpos;
    u8 *ptr;

    if (sPopupOverride != NULL)
    {
        //  The override is consumed here rather than on teardown, so a map popup that happens to follow an
        //  echo shows the map's own name.
        if (sPopupOverride == sDayBanner)
            maxWidth = 152;
        xpos = (maxWidth - GetStringWidth(FONT_NORMAL, sPopupOverride, -1)) / 2;
        FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
        AddTextPrinterParameterized(windowId, FONT_NORMAL, sPopupOverride, xpos, 2, TEXT_SKIP_DRAW, NULL);
        sPopupOverride = NULL;
        return;
    }

    ptr = GetMapName(mapName, gMapHeader.regionMapSectionId, 0);
    if (sDayBannerPending)
        maxWidth = 152;     // T-269: the window was widened for the day that follows
    if (gMapHeader.floorNum != 0)
    {
        ptr = MapNamePopupAppendFloorNum(ptr, gMapHeader.floorNum);
        maxWidth = gMapHeader.floorNum != FLOOR_ROOFTOP ? 152 : 176;
    }
    xpos = (maxWidth - GetStringWidth(FONT_NORMAL, mapName, -1)) / 2;
    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    AddTextPrinterParameterized(windowId, FONT_NORMAL, mapName, xpos, 2, TEXT_SKIP_DRAW, NULL);
}

static u8 *MapNamePopupAppendFloorNum(u8 *dest, s8 floorNum)
{
    if (floorNum == 0)
        return dest;
    *dest++ = CHAR_SPACE;
    if (floorNum == FLOOR_ROOFTOP)
        return StringCopy(dest, gText_Rooftop2);
    if (floorNum < 0)
    {
        *dest++ = CHAR_B;
        floorNum *= -1;
    }
    dest = ConvertIntToDecimalStringN(dest, floorNum, STR_CONV_MODE_LEFT_ALIGN, 2);
    *dest++ = CHAR_F;
    *dest = EOS;
    return dest;
}

#undef tPalIntoFadedBuffer
#undef tWindowDestroyed
#undef tWindowCleared
#undef tWindowExists
#undef tWindowId
#undef tReshow
#undef tPos
#undef tTimer
#undef tState
