#include "global.h"
#include "option_plus_menu.h"
#include "main.h"
#include "menu.h"
#include "scanline_effect.h"
#include "palette.h"
#include "sprite.h"
#include "task.h"
#include "malloc.h"
#include "bg.h"
#include "gpu_regs.h"
#include "window.h"
#include "text.h"
#include "text_window.h"
#include "international_string_util.h"
#include "strings.h"
#include "gba/m4a_internal.h"
#include "constants/rgb.h"
#include "menu_helpers.h"
#include "decompress.h"
#include "event_data.h"
#include "constants/flags.h"

enum
{
    MENU_GENERAL, // General - non battle and non sound related, basically everything else
    MENU_BATTLE,  // Battle - anything that affects battles
    MENU_SOUND,   // Sound - anything that affects sound
    MENU_COUNT,
};

// Menu items
// General options
enum
{
    MENUITEM_GENERAL_INSTANTTEXT,
    MENUITEM_GENERAL_BUTTONMODE,
    MENUITEM_GENERAL_FRAMETYPE,
    MENUITEM_GENERAL_FOLLOWERS,
    MENUITEM_GENERAL_AUTORUN,
    MENUITEM_GENERAL_MATCHCALL,
    MENUITEM_GENERAL_CANCEL,
    MENUITEM_GENERAL_COUNT,
};

// Battle options
enum
{
    MENUITEM_BATTLE_BATTLESCENE,
    MENUITEM_BATTLE_BATTLESTYLE,
    MENUITEM_BATTLE_BATTLESPEED,
    MENUITEM_BATTLE_BAGUSE,
    MENUITEM_BATTLE_QUICKRUN,
    MENUITEM_BATTLE_DOUBLEBATTLE,
    MENUITEM_BATTLE_MOVEINFO,
    MENUITEM_BATTLE_CANCEL,
    MENUITEM_BATTLE_COUNT,
};

// Sound options
enum
{
    MENUITEM_SOUND_SOUNDMODE,
    MENUITEM_SOUND_BIKEMUSIC,
    MENUITEM_SOUND_SURFMUSIC,
    MENUITEM_SOUND_CANCEL,
    MENUITEM_SOUND_COUNT,
};

// Window Ids
enum
{
    WIN_TOPBAR,
    WIN_OPTIONS,
    WIN_DESCRIPTION
};

static const struct WindowTemplate sOptionMenuWinTemplates[] =
{
    {//WIN_TOPBAR
        .bg = 1,
        .tilemapLeft = 0,
        .tilemapTop = 0,
        .width = 30,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 2
    },
    {//WIN_OPTIONS
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 3,
        .width = 26,
        .height = 10,
        .paletteNum = 1,
        .baseBlock = 62
    },
    {//WIN_DESCRIPTION
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 15,
        .width = 26,
        .height = 4,
        .paletteNum = 1,
        .baseBlock = 500
    },
    DUMMY_WIN_TEMPLATE
};

static const struct BgTemplate sOptionMenuBgTemplates[] =
{
    {
       .bg = 0,
       .charBaseIndex = 1,
       .mapBaseIndex = 30,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 1,
    },
    {
       .bg = 1,
       .charBaseIndex = 1,
       .mapBaseIndex = 31,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 0,
    },
    {
       .bg = 2,
       .charBaseIndex = 0,
       .mapBaseIndex = 29,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 1,
    },
    {
       .bg = 3,
       .charBaseIndex = 3,
       .mapBaseIndex = 27,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 2,
    },
};

struct OptionMenu
{
    u8 submenu;
    u8 sel_general[MENUITEM_GENERAL_COUNT];
    u8 sel_battle[MENUITEM_BATTLE_COUNT];
    u8 sel_sound[MENUITEM_SOUND_COUNT];
    int menuCursor[MENU_COUNT];
    int visibleCursor[MENU_COUNT];
    u8 arrowTaskId;
    u8 gfxLoadState;
};

#define Y_DIFF 16 // Difference in pixels between items.
#define OPTIONS_ON_SCREEN 5
#define NUM_OPTIONS_FROM_BORDER 1

// local functions
static void MainCB2(void);
static void VBlankCB(void);
static void DrawTopBarText(void); //top Option text
static void DrawLeftSideOptionText(int selection, int y);
static void DrawRightSideChoiceText(const u8 *str, int x, int y, bool8 choosen, bool8 active);
static void DrawOptionMenuTexts(void); //left side text;
static void DrawChoices(u32 id, int y); //right side draw function
static void HighlightOptionMenuItem(void);
static void Task_OptionMenuFadeIn(u8 taskId);
static void Task_OptionMenuProcessInput(u8 taskId);
static void Task_OptionMenuSave(u8 taskId);
static void Task_OptionMenuFadeOut(u8 taskId);
static void ScrollMenu(int direction);
static void ScrollAll(int direction); // to bottom or top
static int GetMiddleX(const u8 *txt1, const u8 *txt2, const u8 *txt3);
static int XOptions_ProcessInput(int x, int selection);
static int TwoOptions_ProcessInput(int selection);
static int ThreeOptions_ProcessInput(int selection);
static int FourOptions_ProcessInput(int selection);
static int UNUSED ElevenOptions_ProcessInput(int selection);
static int Sound_ProcessInput(int selection);
static int FrameType_ProcessInput(int selection);
static const u8 *const OptionTextDescription(void);
static const u8 *const OptionTextRight(u8 menuItem);
static u8 MenuItemCount(void);
static u8 MenuItemCancel(void);
static void DrawDescriptionText(void);
static void DrawOptionMenuChoice(const u8 *text, u8 x, u8 y, u8 style, bool8 active);
static void UNUSED DrawChoices_Options_Four(const u8 *const *const strings, int selection, int y, bool8 active);
static void ReDrawAll(void);
static void InstantText_DrawChoices(int selection, int y);
static void ButtonMode_DrawChoices(int selection, int y);
static void FrameType_DrawChoices(int selection, int y);
static void Followers_DrawChoices(int selection, int y);
static void AutoRun_DrawChoices(int selection, int y);
static void MatchCall_DrawChoices(int selection, int y);
static void BattleScene_DrawChoices(int selection, int y);
static void BattleStyle_DrawChoices(int selection, int y);
static void BattleSpeed_DrawChoices(int selection, int y);
static void BagUse_DrawChoices(int selection, int y);
static void QuickRun_DrawChoices(int selection, int y);
static void DoubleBattle_DrawChoices(int selection, int y);
static void MoveInfo_DrawChoices(int selection, int y);
static void SoundMode_DrawChoices(int selection, int y);
static void BikeMusic_DrawChoices(int selection, int y);
static void SurfMusic_DrawChoices(int selection, int y);
static void DrawBgWindowFrames(void);

// EWRAM vars
EWRAM_DATA static struct OptionMenu *sOptions = NULL;
static EWRAM_DATA u8 *sBg2TilemapBuffer = NULL;
static EWRAM_DATA u8 *sBg3TilemapBuffer = NULL;

// const data
static const u8 sEqualSignGfx[] = INCBIN_U8("graphics/interface/option_menu_equals_sign.4bpp"); // note: this is only used in the Japanese release
static const u16 sOptionMenuBg_Pal[] = {RGB(17, 18, 31)};
static const u16 sOptionMenuText_Pal[] = INCBIN_U16("graphics/interface/option_menu_text_custom.gbapal");

static const u32 sOptionsPlusTiles[] = INCBIN_U32("graphics/ui_options_plus/options_plus_tiles.4bpp.smol");
static const u16 sOptionsPlusPalette[] = INCBIN_U16("graphics/ui_options_plus/options_plus_tiles.gbapal");
static const u32 sOptionsPlusTilemap[] = INCBIN_U32("graphics/ui_options_plus/options_plus_tiles.bin.smolTM");

// Scrolling Background
static const u32 sScrollBgTiles[] = INCBIN_U32("graphics/ui_options_plus/scroll_tiles.4bpp.smol");
static const u32 sScrollBgTilemap[] = INCBIN_U32("graphics/ui_options_plus/scroll_tiles.bin.smolTM");
static const u16 sScrollBgPalette[] = INCBIN_U16("graphics/ui_options_plus/scroll_tiles.gbapal");

#define TEXT_COLOR_OPTIONS_WHITE                1
#define TEXT_COLOR_OPTIONS_GRAY_FG              2
#define TEXT_COLOR_OPTIONS_GRAY_SHADOW          3
#define TEXT_COLOR_OPTIONS_GRAY_LIGHT_FG        4
#define TEXT_COLOR_OPTIONS_ORANGE_FG            5
#define TEXT_COLOR_OPTIONS_ORANGE_SHADOW        6
#define TEXT_COLOR_OPTIONS_RED_FG               7
#define TEXT_COLOR_OPTIONS_RED_SHADOW           8
#define TEXT_COLOR_OPTIONS_GREEN_FG             9
#define TEXT_COLOR_OPTIONS_GREEN_SHADOW         10
#define TEXT_COLOR_OPTIONS_GREEN_DARK_FG        11
#define TEXT_COLOR_OPTIONS_GREEN_DARK_SHADOW    12
#define TEXT_COLOR_OPTIONS_RED_DARK_FG          13
#define TEXT_COLOR_OPTIONS_RED_DARK_SHADOW      14

// Menu draw and input functions
typedef struct {
    void (*drawChoices)(int selection, int y);
    int (*processInput)(int selection);
} MenuItemFunctions;

static const MenuItemFunctions sItemFunctionsGeneral[MENUITEM_GENERAL_COUNT] =
{
    [MENUITEM_GENERAL_INSTANTTEXT]  = {InstantText_DrawChoices,   TwoOptions_ProcessInput},
    [MENUITEM_GENERAL_BUTTONMODE]   = {ButtonMode_DrawChoices,    ThreeOptions_ProcessInput},
    [MENUITEM_GENERAL_FRAMETYPE]    = {FrameType_DrawChoices,     FrameType_ProcessInput},
    [MENUITEM_GENERAL_FOLLOWERS]    = {Followers_DrawChoices,     TwoOptions_ProcessInput},
    [MENUITEM_GENERAL_AUTORUN]      = {AutoRun_DrawChoices,       TwoOptions_ProcessInput},
    [MENUITEM_GENERAL_MATCHCALL]    = {MatchCall_DrawChoices,     TwoOptions_ProcessInput},
    [MENUITEM_GENERAL_CANCEL]       = {NULL, NULL},
};

static const MenuItemFunctions sItemFunctionsBattle[MENUITEM_BATTLE_COUNT] =
{
    [MENUITEM_BATTLE_BATTLESCENE]  = {BattleScene_DrawChoices,    TwoOptions_ProcessInput},
    [MENUITEM_BATTLE_BATTLESTYLE]  = {BattleStyle_DrawChoices,    TwoOptions_ProcessInput},
    [MENUITEM_BATTLE_BATTLESPEED]  = {BattleSpeed_DrawChoices,    FourOptions_ProcessInput},
    [MENUITEM_BATTLE_BAGUSE]       = {BagUse_DrawChoices,         TwoOptions_ProcessInput},
    [MENUITEM_BATTLE_QUICKRUN]     = {QuickRun_DrawChoices,       ThreeOptions_ProcessInput},
    [MENUITEM_BATTLE_DOUBLEBATTLE] = {DoubleBattle_DrawChoices,   TwoOptions_ProcessInput},
    [MENUITEM_BATTLE_MOVEINFO]     = {MoveInfo_DrawChoices,       TwoOptions_ProcessInput},
    [MENUITEM_BATTLE_CANCEL]       = {NULL, NULL},
};

static const MenuItemFunctions sItemFunctionsSound[MENUITEM_SOUND_COUNT] =
{
    [MENUITEM_SOUND_SOUNDMODE]    = {SoundMode_DrawChoices,       Sound_ProcessInput},
    [MENUITEM_SOUND_BIKEMUSIC]    = {BikeMusic_DrawChoices,       TwoOptions_ProcessInput},
    [MENUITEM_SOUND_SURFMUSIC]    = {SurfMusic_DrawChoices,       TwoOptions_ProcessInput},
    [MENUITEM_SOUND_CANCEL]       = {NULL, NULL},
};

static const u8 *const sOptionMenuItemsNamesGeneral[MENUITEM_GENERAL_COUNT] =
{
    [MENUITEM_GENERAL_INSTANTTEXT]  = gText_InstantText,
    [MENUITEM_GENERAL_BUTTONMODE]   = gText_ButtonMode,
    [MENUITEM_GENERAL_FRAMETYPE]    = gText_Frame,
    [MENUITEM_GENERAL_FOLLOWERS]    = gText_Followers,
    [MENUITEM_GENERAL_AUTORUN]      = gText_AutoRun,
    [MENUITEM_GENERAL_MATCHCALL]    = gText_MatchCalls,
    [MENUITEM_GENERAL_CANCEL]       = gText_OptionMenuSave,
};

static const u8 *const sOptionMenuItemsNamesBattle[MENUITEM_BATTLE_COUNT] =
{
    [MENUITEM_BATTLE_BATTLESCENE]   = gText_BattleScene,
    [MENUITEM_BATTLE_BATTLESTYLE]   = gText_BattleStyle,
    [MENUITEM_BATTLE_BATTLESPEED]   = gText_BattleSpeed,
    [MENUITEM_BATTLE_BAGUSE]        = gText_BagUse,
    [MENUITEM_BATTLE_QUICKRUN]      = gText_QuickRun,
    [MENUITEM_BATTLE_DOUBLEBATTLE]  = gText_DoubleBattles,
    [MENUITEM_BATTLE_MOVEINFO]      = gText_MoveInfo,
    [MENUITEM_BATTLE_CANCEL]        = gText_OptionMenuSave,
};

static const u8 *const sOptionMenuItemsNamesSound[MENUITEM_SOUND_COUNT] =
{
    [MENUITEM_SOUND_SOUNDMODE]  = gText_Sound,
    [MENUITEM_SOUND_BIKEMUSIC]  = gText_BikeMusic,
    [MENUITEM_SOUND_SURFMUSIC]  = gText_SurfMusic,
    [MENUITEM_SOUND_CANCEL]     = gText_OptionMenuSave,
};

static const u8 *const OptionTextRight(u8 menuItem)
{
    switch (sOptions->submenu)
    {
    case MENU_BATTLE:
        return sOptionMenuItemsNamesBattle[menuItem];
    case MENU_SOUND:
        return sOptionMenuItemsNamesSound[menuItem];
    default:
    case MENU_GENERAL:
        return sOptionMenuItemsNamesGeneral[menuItem];
    }
}

// Menu left side text conditions
static bool8 CheckConditions(int selection)
{
    switch (sOptions->submenu)
    {
    case MENU_GENERAL:
        switch(selection)
        {
        case MENUITEM_GENERAL_AUTORUN:
            return FlagGet(FLAG_SYS_B_DASH);
        case MENUITEM_GENERAL_INSTANTTEXT:
        case MENUITEM_GENERAL_BUTTONMODE:
        case MENUITEM_GENERAL_FRAMETYPE:
        case MENUITEM_GENERAL_FOLLOWERS:
        case MENUITEM_GENERAL_MATCHCALL:
        case MENUITEM_GENERAL_CANCEL:
        case MENUITEM_GENERAL_COUNT:
            return TRUE;
        }
    case MENU_BATTLE:
        switch(selection)
        {
        case MENUITEM_BATTLE_BATTLESCENE:
        case MENUITEM_BATTLE_BATTLESTYLE:
        case MENUITEM_BATTLE_BATTLESPEED:
        case MENUITEM_BATTLE_BAGUSE:
        case MENUITEM_BATTLE_QUICKRUN:
        case MENUITEM_BATTLE_DOUBLEBATTLE:
        case MENUITEM_BATTLE_MOVEINFO:
        case MENUITEM_BATTLE_CANCEL:
        case MENUITEM_BATTLE_COUNT:
            return TRUE;
        }
    case MENU_SOUND:
        switch(selection)
        {
        case MENUITEM_SOUND_SOUNDMODE:
        case MENUITEM_SOUND_BIKEMUSIC:
        case MENUITEM_SOUND_SURFMUSIC:
        case MENUITEM_SOUND_CANCEL:
        case MENUITEM_SOUND_COUNT:
            return TRUE;
        }
    }
    return FALSE;
}

// Descriptions
static const u8 sText_Empty[]                   = _("");
static const u8 sText_Desc_Save[]               = _("Save your settings.");

static const u8 sText_Desc_InstantTextOn[]      = _("Instant text is enabled.");
static const u8 sText_Desc_InstantTextOff[]     = _("Instant text is disabled.");
static const u8 sText_Desc_ButtonMode[]         = _("All buttons work as normal.");
static const u8 sText_Desc_ButtonMode_LR[]      = _("On some screens the L and R buttons\nact as left and right.");
static const u8 sText_Desc_ButtonMode_LA[]      = _("The L button acts as another A\nbutton for one-handed play.");
static const u8 sText_Desc_FrameType[]          = _("Choose the frame surrounding the\nwindows.");
static const u8 sText_Desc_MatchCallOn[]        = _("TRAINERs will be able to call you,\noffering rematches and info.");
static const u8 sText_Desc_MatchCallOff[]       = _("You will not receive calls.\nSpecial events will still occur.");
static const u8 sText_Desc_FollowersOn[]        = _("Your first party POKéMON will\nfollow you in the overworld.");
static const u8 sText_Desc_FollowersOff[]       = _("Following POKéMON will be disabled.\nYour POKéMON will not follow you.");
static const u8 sText_Desc_AutoRunOn[]          = _("Enables auto running in the overworld.\nPress {B_BUTTON} to walk.");
static const u8 sText_Desc_AutoRunOff[]         = _("Disabled auto running in the overworld.\nPress {B_BUTTON} to run.");

static const u8 sText_Desc_BattleScene_On[]     = _("Show the POKéMON battle animations.");
static const u8 sText_Desc_BattleScene_Off[]    = _("Skip the POKéMON battle animations.");
static const u8 sText_Desc_BattleStyle_Shift[]  = _("Get the option to switch your\nPOKéMON after the enemies faints.");
static const u8 sText_Desc_BattleStyle_Set[]    = _("No free switch after fainting the\nenemies POKéMON.");
static const u8 sText_Desc_BagUse_On[]          = _("Enables the use of items from the\nbag.");
static const u8 sText_Desc_BagUse_Off[]         = _("Disables the use of items from the\nbag.");
static const u8 sText_Desc_QuickRunOptionR[]    = _("Run from wild battles by pressing\nthe {R_BUTTON} button.");
static const u8 sText_Desc_QuickRunOptionBA[]   = _("Move the cursor to RUN by pressing\nthe {B_BUTTON} button.");
static const u8 sText_Desc_QuickRunOptionOff[]  = _("Disables quick running from wild\nbattles.");
static const u8 sText_Desc_DoubleBattles_On[]   = _("All Trainer battles will be double\nbattles.");
static const u8 sText_Desc_DoubleBattles_Off[]  = _("All Trainer battles will be single\nbattles, unless forced.");
static const u8 sText_Desc_MoveInfo_On[]        = _("Shows a window with information of\nmoves.");
static const u8 sText_Desc_MoveInfo_Off[]       = _("Disables move information window.");
static const u8 sText_Desc_BattleSpeed_1x[]     = _("Battle animations will play at default\nspeed.");
static const u8 sText_Desc_BattleSpeed_2x[]     = _("Battle animations will play in 2x\n speed.");
static const u8 sText_Desc_BattleSpeed_3x[]     = _("Battle animations will play in 3x\n speed.");
static const u8 sText_Desc_BattleSpeed_4x[]     = _("Battle animations will play in 4x\n speed.");

static const u8 sText_Desc_SoundMono[]          = _("Sound is the same in all speakers.\nRecommended for original hardware.");
static const u8 sText_Desc_SoundStereo[]        = _("Play the left and right audio channel\nseperatly. Great with headphones.");
static const u8 sText_Desc_BikeMusicOn[]        = _("Bike theme music will play\nwhile cycling.");
static const u8 sText_Desc_BikeMusicOff[]       = _("Normal route music continues\nwhile cycling.");
static const u8 sText_Desc_SurfMusicOn[]        = _("Surf theme music will play\nwhile on water.");
static const u8 sText_Desc_SurfMusicOff[]       = _("Normal water route music continues\nwhile surfing.");

static const u8 *const sOptionMenuItemDescriptionsGeneral[MENUITEM_GENERAL_COUNT][3] =
{
    [MENUITEM_GENERAL_INSTANTTEXT] = {sText_Desc_InstantTextOn,        sText_Desc_InstantTextOff,       sText_Empty},
    [MENUITEM_GENERAL_BUTTONMODE]  = {sText_Desc_ButtonMode,           sText_Desc_ButtonMode_LR,        sText_Desc_ButtonMode_LA},
    [MENUITEM_GENERAL_FRAMETYPE]   = {sText_Desc_FrameType,            sText_Empty,                     sText_Empty},
    [MENUITEM_GENERAL_FOLLOWERS]   = {sText_Desc_FollowersOn,          sText_Desc_FollowersOff,         sText_Empty},
    [MENUITEM_GENERAL_AUTORUN]     = {sText_Desc_AutoRunOn,            sText_Desc_AutoRunOff,           sText_Empty},
    [MENUITEM_GENERAL_MATCHCALL]   = {sText_Desc_MatchCallOn,          sText_Desc_MatchCallOff,         sText_Empty},
    [MENUITEM_GENERAL_CANCEL]      = {sText_Desc_Save,                 sText_Empty,                     sText_Empty},
};

static const u8 *const sOptionMenuItemDescriptionsBattle[MENUITEM_BATTLE_COUNT][4] =
{
    [MENUITEM_BATTLE_BATTLESCENE]  = {sText_Desc_BattleScene_On,       sText_Desc_BattleScene_Off,       sText_Empty,                    sText_Empty},
    [MENUITEM_BATTLE_BATTLESTYLE]  = {sText_Desc_BattleStyle_Shift,    sText_Desc_BattleStyle_Set,       sText_Empty,                    sText_Empty},
    [MENUITEM_BATTLE_BATTLESPEED]  = {sText_Desc_BattleSpeed_1x,       sText_Desc_BattleSpeed_2x,        sText_Desc_BattleSpeed_3x,      sText_Desc_BattleSpeed_4x},
    [MENUITEM_BATTLE_BAGUSE]       = {sText_Desc_BagUse_On,            sText_Desc_BagUse_Off,            sText_Empty,                    sText_Empty},
    [MENUITEM_BATTLE_QUICKRUN]     = {sText_Desc_QuickRunOptionR,      sText_Desc_QuickRunOptionBA,      sText_Desc_QuickRunOptionOff,   sText_Empty},
    [MENUITEM_BATTLE_DOUBLEBATTLE] = {sText_Desc_DoubleBattles_On,    sText_Desc_DoubleBattles_Off,     sText_Empty,                    sText_Empty},
    [MENUITEM_BATTLE_MOVEINFO]     = {sText_Desc_MoveInfo_On,          sText_Desc_MoveInfo_Off,          sText_Empty,                    sText_Empty},
    [MENUITEM_BATTLE_CANCEL]       = {sText_Desc_Save,                 sText_Empty,                      sText_Empty,                    sText_Empty},
};

static const u8 *const sOptionMenuItemDescriptionsSound[MENUITEM_SOUND_COUNT][3] =
{
    [MENUITEM_SOUND_SOUNDMODE]     = {sText_Desc_SoundMono,            sText_Desc_SoundStereo,            sText_Empty},
    [MENUITEM_SOUND_BIKEMUSIC]     = {sText_Desc_BikeMusicOn,          sText_Desc_BikeMusicOff,           sText_Empty},
    [MENUITEM_SOUND_SURFMUSIC]     = {sText_Desc_SurfMusicOn,          sText_Desc_SurfMusicOff,           sText_Empty},
    [MENUITEM_SOUND_CANCEL]        = {sText_Desc_Save,                 sText_Empty,                       sText_Empty},
};

// Disabled Descriptions
static const u8 sText_Desc_Disabled_Textspeed[]     = _("Only active if xyz.");
static const u8 sText_Desc_Disabled_AutoRun[]       = _("Only active if running shows are\nreceived.");

static const u8 *const sOptionMenuItemDescriptionsDisabledGeneral[MENUITEM_GENERAL_COUNT] =
{
    [MENUITEM_GENERAL_INSTANTTEXT] = sText_Desc_Disabled_Textspeed,
    [MENUITEM_GENERAL_BUTTONMODE]  = sText_Empty,
    [MENUITEM_GENERAL_FRAMETYPE]   = sText_Empty,
    [MENUITEM_GENERAL_FOLLOWERS]   = sText_Empty,
    [MENUITEM_GENERAL_AUTORUN]     = sText_Desc_Disabled_AutoRun,
    [MENUITEM_GENERAL_MATCHCALL]   = sText_Empty,
    [MENUITEM_GENERAL_CANCEL]      = sText_Empty,
};

static const u8 *const sOptionMenuItemDescriptionsDisabledBattle[MENUITEM_BATTLE_COUNT] =
{
    [MENUITEM_BATTLE_BATTLESCENE] = sText_Empty,
    [MENUITEM_BATTLE_BATTLESTYLE] = sText_Empty,
    [MENUITEM_BATTLE_BATTLESPEED] = sText_Empty,
    [MENUITEM_BATTLE_BAGUSE]      = sText_Empty,
    [MENUITEM_BATTLE_QUICKRUN]    = sText_Empty,
    [MENUITEM_BATTLE_DOUBLEBATTLE]  = sText_Empty,
    [MENUITEM_BATTLE_MOVEINFO]    = sText_Empty,
    [MENUITEM_BATTLE_CANCEL]      = sText_Empty,
};

static const u8 *const sOptionMenuItemDescriptionsDisabledSound[MENUITEM_SOUND_COUNT] =
{
    [MENUITEM_SOUND_SOUNDMODE]   = sText_Empty,
    [MENUITEM_SOUND_BIKEMUSIC]   = sText_Empty,
    [MENUITEM_SOUND_SURFMUSIC]   = sText_Empty,
    [MENUITEM_SOUND_CANCEL]      = sText_Empty,
};

static const u8 *const OptionTextDescription(void)
{
    u8 menuItem = sOptions->menuCursor[sOptions->submenu];
    u8 selection;

    switch (sOptions->submenu)
    {
    case MENU_BATTLE:
        if (menuItem >= MENUITEM_BATTLE_COUNT || !CheckConditions(menuItem))
            return sOptionMenuItemDescriptionsDisabledBattle[menuItem];
        selection = sOptions->sel_battle[menuItem];
        return sOptionMenuItemDescriptionsBattle[menuItem][selection];
    case MENU_SOUND:
        if (menuItem >= MENUITEM_SOUND_COUNT || !CheckConditions(menuItem))
            return sOptionMenuItemDescriptionsDisabledSound[menuItem];
        selection = sOptions->sel_sound[menuItem];
        return sOptionMenuItemDescriptionsSound[menuItem][selection];
    default:
    case MENU_GENERAL:
        if (menuItem >= MENUITEM_GENERAL_COUNT || !CheckConditions(menuItem))
            return sOptionMenuItemDescriptionsDisabledGeneral[menuItem];
        selection = sOptions->sel_general[menuItem];
        if (menuItem == MENUITEM_GENERAL_FRAMETYPE)
            selection = 0;
        return sOptionMenuItemDescriptionsGeneral[menuItem][selection];
    }
}

static u8 MenuItemCount(void)
{
    switch (sOptions->submenu)
    {
    case MENU_BATTLE:
        return MENUITEM_BATTLE_COUNT;
    case MENU_SOUND:
        return MENUITEM_SOUND_COUNT;
    default:
    case MENU_GENERAL:
        return MENUITEM_GENERAL_COUNT;
    }
}

static u8 MenuItemCancel(void)
{
    switch (sOptions->submenu)
    {
    case MENU_BATTLE:
        return MENUITEM_BATTLE_CANCEL;
    case MENU_SOUND:
        return MENUITEM_SOUND_CANCEL;
    default:
    case MENU_GENERAL:
        return MENUITEM_GENERAL_CANCEL;
    }
}

// Main code
static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    ChangeBgY(3, 96, BG_COORD_ADD);
}

static const u8 sText_TopBar_General[]         = _("GENERAL");
static const u8 sText_TopBar_General_Right[]   = _("{R_BUTTON}BATTLE");
static const u8 sText_TopBar_General_Left[]    = _("{L_BUTTON}SOUND");
static const u8 sText_TopBar_Battle[]          = _("BATTLE");
static const u8 sText_TopBar_Battle_Left[]     = _("{L_BUTTON}GENERAL");
static const u8 sText_TopBar_Battle_Right[]    = _("{R_BUTTON}SOUND");
static const u8 sText_TopBar_Sound[]           = _("SOUND");
static const u8 sText_TopBar_Sound_Left[]      = _("{L_BUTTON}BATTLE");
static const u8 sText_TopBar_Sound_Right[]     = _("{R_BUTTON}GENERAL");
static void DrawTopBarText(void)
{
    const u8 color[3] = { 0, TEXT_COLOR_WHITE, TEXT_COLOR_OPTIONS_GRAY_FG };

    FillWindowPixelBuffer(WIN_TOPBAR, PIXEL_FILL(0));
    switch (sOptions->submenu)
    {
        case MENU_GENERAL:
            AddTextPrinterParameterized3(WIN_TOPBAR, FONT_SMALL, 105, 1, color, 0, sText_TopBar_General);
            AddTextPrinterParameterized3(WIN_TOPBAR, FONT_SMALL, 2, 1, color, 0, sText_TopBar_General_Left);  // Show L to go to SOUND
            AddTextPrinterParameterized3(WIN_TOPBAR, FONT_SMALL, 190, 1, color, 0, sText_TopBar_General_Right);
            break;
        case MENU_BATTLE:
            AddTextPrinterParameterized3(WIN_TOPBAR, FONT_SMALL, 105, 1, color, 0, sText_TopBar_Battle);
            AddTextPrinterParameterized3(WIN_TOPBAR, FONT_SMALL, 2, 1, color, 0, sText_TopBar_Battle_Left);
            AddTextPrinterParameterized3(WIN_TOPBAR, FONT_SMALL, 190, 1, color, 0, sText_TopBar_Battle_Right);
            break;
        case MENU_SOUND:
            AddTextPrinterParameterized3(WIN_TOPBAR, FONT_SMALL, 105, 1, color, 0, sText_TopBar_Sound);
            AddTextPrinterParameterized3(WIN_TOPBAR, FONT_SMALL, 2, 1, color, 0, sText_TopBar_Sound_Left);
            AddTextPrinterParameterized3(WIN_TOPBAR, FONT_SMALL, 190, 1, color, 0, sText_TopBar_Sound_Right);  // Show R to go to GENERAL
            break;
    }
    PutWindowTilemap(WIN_TOPBAR);
    CopyWindowToVram(WIN_TOPBAR, COPYWIN_FULL);
}

static void DrawOptionMenuTexts(void) //left side text
{
    u8 i;

    FillWindowPixelBuffer(WIN_OPTIONS, PIXEL_FILL(0));
    for (i = 0; i < MenuItemCount(); i++)
        DrawLeftSideOptionText(i, (i * Y_DIFF) + 1);
    CopyWindowToVram(WIN_OPTIONS, COPYWIN_FULL);
}

static void DrawDescriptionText(void)
{
    u8 color_gray[3];
    color_gray[0] = TEXT_COLOR_TRANSPARENT;
    color_gray[1] = TEXT_COLOR_OPTIONS_GRAY_FG;
    color_gray[2] = TEXT_COLOR_OPTIONS_GRAY_SHADOW;
        
    FillWindowPixelBuffer(WIN_DESCRIPTION, PIXEL_FILL(1));
    AddTextPrinterParameterized4(WIN_DESCRIPTION, FONT_NORMAL, 5, 1, 0, 0, color_gray, TEXT_SKIP_DRAW, OptionTextDescription());
    CopyWindowToVram(WIN_DESCRIPTION, COPYWIN_FULL);
}

static void DrawLeftSideOptionText(int selection, int y)
{
    u8 color_yellow[3];
    u8 color_gray[3];

    color_yellow[0] = TEXT_COLOR_TRANSPARENT;
    color_yellow[1] = TEXT_COLOR_WHITE;
    color_yellow[2] = TEXT_COLOR_OPTIONS_GRAY_FG;
    color_gray[0] = TEXT_COLOR_TRANSPARENT;
    color_gray[1] = TEXT_COLOR_WHITE;
    color_gray[2] = TEXT_COLOR_OPTIONS_GRAY_SHADOW;

    if (CheckConditions(selection))
        AddTextPrinterParameterized4(WIN_OPTIONS, FONT_NORMAL, 8, y, 0, 0, color_yellow, TEXT_SKIP_DRAW, OptionTextRight(selection));
    else
        AddTextPrinterParameterized4(WIN_OPTIONS, FONT_NORMAL, 8, y, 0, 0, color_gray, TEXT_SKIP_DRAW, OptionTextRight(selection));
}

static void DrawRightSideChoiceText(const u8 *text, int x, int y, bool8 choosen, bool8 active)
{
    u8 color_red[3];
    u8 color_gray[3];

    if (active)
    {
        color_red[0] = TEXT_COLOR_TRANSPARENT;
        color_red[1] = TEXT_COLOR_OPTIONS_ORANGE_FG;
        color_red[2] = TEXT_COLOR_OPTIONS_GRAY_FG;
        color_gray[0] = TEXT_COLOR_TRANSPARENT;
        color_gray[1] = TEXT_COLOR_OPTIONS_WHITE;
        color_gray[2] = TEXT_COLOR_OPTIONS_GRAY_FG;
    }
    else
    {
        color_red[0] = TEXT_COLOR_TRANSPARENT;
        color_red[1] = TEXT_COLOR_OPTIONS_WHITE;
        color_red[2] = TEXT_COLOR_OPTIONS_GRAY_FG;
        color_gray[0] = TEXT_COLOR_TRANSPARENT;
        color_gray[1] = TEXT_COLOR_OPTIONS_WHITE;
        color_gray[2] = TEXT_COLOR_OPTIONS_GRAY_FG;
    }

    if (choosen)
        AddTextPrinterParameterized4(WIN_OPTIONS, FONT_NORMAL, x, y, 0, 0, color_red, TEXT_SKIP_DRAW, text);
    else
        AddTextPrinterParameterized4(WIN_OPTIONS, FONT_NORMAL, x, y, 0, 0, color_gray, TEXT_SKIP_DRAW, text);
}

static void DrawChoices(u32 id, int y) //right side draw function
{
    switch (sOptions->submenu)
    {
        case MENU_GENERAL:
            if (sItemFunctionsGeneral[id].drawChoices != NULL)
                sItemFunctionsGeneral[id].drawChoices(sOptions->sel_general[id], y);
            break;
        case MENU_BATTLE:
            if (sItemFunctionsBattle[id].drawChoices != NULL)
                sItemFunctionsBattle[id].drawChoices(sOptions->sel_battle[id], y);
            break;
        case MENU_SOUND:
            if (sItemFunctionsSound[id].drawChoices != NULL)
                sItemFunctionsSound[id].drawChoices(sOptions->sel_sound[id], y);
            break;
    }
}

static void HighlightOptionMenuItem(void)
{
    int cursor = sOptions->visibleCursor[sOptions->submenu];

    SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(8, 232));
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(cursor * Y_DIFF + 24, cursor * Y_DIFF + 40));
}

static bool8 OptionsMenu_LoadGraphics(void) // Load all the tilesets, tilemaps, spritesheets, and palettes
{
    switch (sOptions->gfxLoadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(2, sOptionsPlusTiles, 0, 0, 0);
        sOptions->gfxLoadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            DecompressDataWithHeaderWram(sOptionsPlusTilemap, sBg2TilemapBuffer);
            sOptions->gfxLoadState++;
        }
        break;
    case 2:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(3, sScrollBgTiles, 0, 0, 0);
        sOptions->gfxLoadState++;
        break;
    case 3:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            DecompressDataWithHeaderWram(sScrollBgTilemap, sBg3TilemapBuffer);
            sOptions->gfxLoadState++;
        }
        break;
    case 4:
        LoadPalette(sOptionsPlusPalette, 64, 32);
        LoadPalette(sScrollBgPalette, 32, 32);
        sOptions->gfxLoadState++;
        break;
    default:
        sOptions->gfxLoadState = 0;
        return TRUE;
    }
    return FALSE;
}

void CB2_InitOptionPlusMenu(void)
{
    u32 i;
    switch (gMain.state)
    {
    default:
    case 0:
        SetVBlankHBlankCallbacksToNull();
        ClearScheduledBgCopiesToVram();
        ResetVramOamAndBgCntRegs();
        sOptions = AllocZeroed(sizeof(*sOptions));
        FreeAllSpritePalettes();
        ResetTasks();
        ResetSpriteData();
        gMain.state++;
        break;
    case 1:
        DmaClearLarge16(3, (void *)(VRAM), VRAM_SIZE, 0x1000);
        DmaClear32(3, OAM, OAM_SIZE);
        DmaClear16(3, PLTT, PLTT_SIZE);
        ResetBgsAndClearDma3BusyFlags(0);
        ResetBgPositions();
        
        DeactivateAllTextPrinters();
        SetGpuReg(REG_OFFSET_WIN0H, 0);
        SetGpuReg(REG_OFFSET_WIN0V, 0);
        SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_BG_ALL | WININ_WIN0_OBJ);
        SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG_ALL | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR);
        SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_EFFECT_DARKEN | BLDCNT_TGT1_BG0 | BLDCNT_TGT1_BG2);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 4);
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON | DISPCNT_WIN1_ON);
        
        ResetAllBgsCoordinates();
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sOptionMenuBgTemplates, NELEMS(sOptionMenuBgTemplates));
        InitWindows(sOptionMenuWinTemplates);

        sBg2TilemapBuffer = Alloc(0x800);
        memset(sBg2TilemapBuffer, 0, 0x800);
        SetBgTilemapBuffer(2, sBg2TilemapBuffer);
        ScheduleBgCopyTilemapToVram(2);

        sBg3TilemapBuffer = Alloc(0x800);
        memset(sBg3TilemapBuffer, 0, 0x800);
        SetBgTilemapBuffer(3, sBg3TilemapBuffer);
        ScheduleBgCopyTilemapToVram(3);
        gMain.state++;
        break;
    case 2:
        ResetPaletteFade();
        ScanlineEffect_Stop();
        gMain.state++;
        sOptions->gfxLoadState = 0;
        break;
    case 3:
        if (OptionsMenu_LoadGraphics() == TRUE)
        {
            gMain.state++;
            LoadBgTiles(1, GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->tiles, 0x120, 0x1A2);
        }
        break;
    case 4:
        LoadPalette(sOptionMenuBg_Pal, 0, sizeof(sOptionMenuBg_Pal));
        LoadPalette(GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->pal, 0x70, 0x20);
        gMain.state++;
        break;
    case 5:
        LoadPalette(sOptionMenuText_Pal, 16, sizeof(sOptionMenuText_Pal));
        gMain.state++;
        break;
    case 6:
        sOptions->sel_general[MENUITEM_GENERAL_INSTANTTEXT] = gSaveBlock2Ptr->optionsInstantTextOff;
        sOptions->sel_general[MENUITEM_GENERAL_BUTTONMODE]  = gSaveBlock2Ptr->optionsButtonMode;
        sOptions->sel_general[MENUITEM_GENERAL_FRAMETYPE]   = gSaveBlock2Ptr->optionsWindowFrameType;
        sOptions->sel_general[MENUITEM_GENERAL_FOLLOWERS]   = gSaveBlock2Ptr->optionsFollowersOff;
        sOptions->sel_general[MENUITEM_GENERAL_AUTORUN]     = gSaveBlock2Ptr->optionsAutoRunOff;
        sOptions->sel_general[MENUITEM_GENERAL_MATCHCALL]   = gSaveBlock2Ptr->optionsDisableMatchCall;

        sOptions->sel_battle[MENUITEM_BATTLE_BATTLESCENE]   = gSaveBlock2Ptr->optionsBattleSceneOff;
        sOptions->sel_battle[MENUITEM_BATTLE_BATTLESTYLE]   = gSaveBlock2Ptr->optionsBattleStyle;
        sOptions->sel_battle[MENUITEM_BATTLE_BATTLESPEED]   = gSaveBlock2Ptr->optionsBattleSpeed;
        sOptions->sel_battle[MENUITEM_BATTLE_BAGUSE]        = gSaveBlock2Ptr->optionsDisableBagUse;
        sOptions->sel_battle[MENUITEM_BATTLE_QUICKRUN]      = gSaveBlock2Ptr->optionsQuickRunButton;
        sOptions->sel_battle[MENUITEM_BATTLE_DOUBLEBATTLE]  = gSaveBlock2Ptr->optionsDoubleBattlesOff;
        sOptions->sel_battle[MENUITEM_BATTLE_MOVEINFO]      = gSaveBlock2Ptr->optionsShowBattleMoveInfoOff;

        sOptions->sel_sound[MENUITEM_SOUND_SOUNDMODE]       = gSaveBlock2Ptr->optionsSound;
        sOptions->sel_sound[MENUITEM_SOUND_BIKEMUSIC]       = gSaveBlock2Ptr->optionsBikeMusicOff;
        sOptions->sel_sound[MENUITEM_SOUND_SURFMUSIC]       = gSaveBlock2Ptr->optionsSurfMusicOff;

        sOptions->submenu = MENU_GENERAL;

        gMain.state++;
        break;
    case 7:
        PutWindowTilemap(WIN_TOPBAR);
        DrawTopBarText();
        gMain.state++;
        break;
    case 8:
        PutWindowTilemap(WIN_DESCRIPTION);
        DrawDescriptionText();
        gMain.state++;
        break;
    case 9:
        PutWindowTilemap(WIN_OPTIONS);
        DrawOptionMenuTexts();
        gMain.state++;
        break;
    case 10:
        u8 UNUSED taskId = CreateTask(Task_OptionMenuFadeIn, 0); // No idea, but sure, ig
        
        sOptions->arrowTaskId = AddScrollIndicatorArrowPairParameterized(SCROLL_ARROW_UP, 240 / 2, 20, 110, MENUITEM_GENERAL_COUNT - 1, 110, 110, 0);

        for (i = 0; i < min(OPTIONS_ON_SCREEN, MenuItemCount()); i++)
            DrawChoices(i, i * Y_DIFF);

        HighlightOptionMenuItem();

        CopyWindowToVram(WIN_OPTIONS, COPYWIN_FULL);
        gMain.state++;
        break;
    case 11:
        DrawBgWindowFrames();
        gMain.state++;
        break;
    case 12:
        ShowBg(0);
        ShowBg(1);
        ShowBg(2);
        ShowBg(3);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0x10, 0, RGB_BLACK);
        SetVBlankCallback(VBlankCB);
        SetMainCallback2(MainCB2);
        return;
    }
}

static void Task_OptionMenuFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gTasks[taskId].func = Task_OptionMenuProcessInput;
        SetGpuReg(REG_OFFSET_WIN0H, 0); // Idk man Im just trying to stop this stupid graphical bug from happening dont judge me
        SetGpuReg(REG_OFFSET_WIN0V, 0);
        SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_BG_ALL | WININ_WIN0_OBJ);
        SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG_ALL | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR);
        SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_EFFECT_DARKEN | BLDCNT_TGT1_BG0 | BLDCNT_TGT1_BG2);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 4);
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON | DISPCNT_WIN1_ON | DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
        ShowBg(0);
        ShowBg(1);
        ShowBg(2);
        ShowBg(3);
        HighlightOptionMenuItem();
        return;
    }
}

static void Task_OptionMenuProcessInput(u8 taskId)
{
    u8 optionsToDraw = min(OPTIONS_ON_SCREEN , MenuItemCount());
    if (JOY_NEW(A_BUTTON))
    {
        if (sOptions->menuCursor[sOptions->submenu] == MenuItemCancel())
            gTasks[taskId].func = Task_OptionMenuSave;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].func = Task_OptionMenuSave;
    }
    else if (JOY_NEW(DPAD_UP))
    {
        if (sOptions->visibleCursor[sOptions->submenu] == NUM_OPTIONS_FROM_BORDER) // don't advance visible cursor until scrolled to the bottom
        {
            if (--sOptions->menuCursor[sOptions->submenu] == 0)
                sOptions->visibleCursor[sOptions->submenu]--;
            else
                ScrollMenu(1);
        }
        else
        {
            if (--sOptions->menuCursor[sOptions->submenu] < 0) // Scroll all the way to the bottom.
            {
                sOptions->visibleCursor[sOptions->submenu] = sOptions->menuCursor[sOptions->submenu] = optionsToDraw-2;
                ScrollAll(0);
                sOptions->visibleCursor[sOptions->submenu] = optionsToDraw-1;
                sOptions->menuCursor[sOptions->submenu] = MenuItemCount() - 1;
            }
            else
            {
                sOptions->visibleCursor[sOptions->submenu]--;
            }
        }
        HighlightOptionMenuItem();
        DrawDescriptionText();
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        if (sOptions->visibleCursor[sOptions->submenu] == optionsToDraw-2) // don't advance visible cursor until scrolled to the bottom
        {
            if (++sOptions->menuCursor[sOptions->submenu] == MenuItemCount() - 1)
                sOptions->visibleCursor[sOptions->submenu]++;
            else
                ScrollMenu(0);
        }
        else
        {
            if (++sOptions->menuCursor[sOptions->submenu] >= MenuItemCount()-1) // Scroll all the way to the top.
            {
                sOptions->visibleCursor[sOptions->submenu] = optionsToDraw-2;
                sOptions->menuCursor[sOptions->submenu] = MenuItemCount() - optionsToDraw-1;
                ScrollAll(1);
                sOptions->visibleCursor[sOptions->submenu] = sOptions->menuCursor[sOptions->submenu] = 0;
            }
            else
            {
                sOptions->visibleCursor[sOptions->submenu]++;
            }
        }
        HighlightOptionMenuItem();
        DrawDescriptionText();
    }
    else if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT))
    {
        switch (sOptions->submenu)
        {
            case MENU_GENERAL:
            {
                int cursor = sOptions->menuCursor[sOptions->submenu];
                u8 previousOption = sOptions->sel_general[cursor];
                if (CheckConditions(cursor))
                {
                    if (sItemFunctionsGeneral[cursor].processInput != NULL)
                    {
                        sOptions->sel_general[cursor] = sItemFunctionsGeneral[cursor].processInput(previousOption);
                        ReDrawAll();
                        DrawDescriptionText();
                    }

                    if (previousOption != sOptions->sel_general[cursor])
                        DrawChoices(cursor, sOptions->visibleCursor[sOptions->submenu] * Y_DIFF);
                }
                break;
            }
            case MENU_BATTLE:
            {
                int cursor = sOptions->menuCursor[sOptions->submenu];
                u8 previousOption = sOptions->sel_battle[cursor];
                if (CheckConditions(cursor))
                {
                    if (sItemFunctionsBattle[cursor].processInput != NULL)
                    {
                        sOptions->sel_battle[cursor] = sItemFunctionsBattle[cursor].processInput(previousOption);
                        ReDrawAll();
                        DrawDescriptionText();
                    }

                    if (previousOption != sOptions->sel_battle[cursor])
                        DrawChoices(cursor, sOptions->visibleCursor[sOptions->submenu] * Y_DIFF);
                }
                break;
            }
            case MENU_SOUND:
            {
                int cursor = sOptions->menuCursor[sOptions->submenu];
                u8 previousOption = sOptions->sel_sound[cursor];
                if (CheckConditions(cursor))
                {
                    if (sItemFunctionsSound[cursor].processInput != NULL)
                    {
                        sOptions->sel_sound[cursor] = sItemFunctionsSound[cursor].processInput(previousOption);
                        ReDrawAll();
                        DrawDescriptionText();
                    }

                    if (previousOption != sOptions->sel_sound[cursor])
                        DrawChoices(cursor, sOptions->visibleCursor[sOptions->submenu] * Y_DIFF);
                }
                break;
            }
        }
    }
    else if (JOY_NEW(R_BUTTON))
    {
        // Circular navigation: GENERAL → BATTLE → SOUND → GENERAL
        sOptions->submenu = (sOptions->submenu + 1) % MENU_COUNT;
        
        DrawTopBarText();
        ReDrawAll();
        HighlightOptionMenuItem();
        DrawDescriptionText();
    }
    else if (JOY_NEW(L_BUTTON))
    {
        // Circular navigation: GENERAL ← BATTLE ← SOUND ← GENERAL
        sOptions->submenu = (sOptions->submenu + MENU_COUNT - 1) % MENU_COUNT;
        
        DrawTopBarText();
        ReDrawAll();
        HighlightOptionMenuItem();
        DrawDescriptionText();
    }
}

static void Task_OptionMenuSave(u8 taskId)
{
    gSaveBlock2Ptr->optionsInstantTextOff   = sOptions->sel_general[MENUITEM_GENERAL_INSTANTTEXT];
    gSaveBlock2Ptr->optionsButtonMode       = sOptions->sel_general[MENUITEM_GENERAL_BUTTONMODE];
    gSaveBlock2Ptr->optionsWindowFrameType  = sOptions->sel_general[MENUITEM_GENERAL_FRAMETYPE];
    gSaveBlock2Ptr->optionsFollowersOff     = sOptions->sel_general[MENUITEM_GENERAL_FOLLOWERS];
    gSaveBlock2Ptr->optionsAutoRunOff       = sOptions->sel_general[MENUITEM_GENERAL_AUTORUN];
    gSaveBlock2Ptr->optionsDisableMatchCall = sOptions->sel_general[MENUITEM_GENERAL_MATCHCALL];

    gSaveBlock2Ptr->optionsBattleSceneOff   = sOptions->sel_battle[MENUITEM_BATTLE_BATTLESCENE];
    gSaveBlock2Ptr->optionsBattleStyle      = sOptions->sel_battle[MENUITEM_BATTLE_BATTLESTYLE];
    gSaveBlock2Ptr->optionsBattleSpeed      = sOptions->sel_battle[MENUITEM_BATTLE_BATTLESPEED];
    gSaveBlock2Ptr->optionsDisableBagUse    = sOptions->sel_battle[MENUITEM_BATTLE_BAGUSE];
    gSaveBlock2Ptr->optionsQuickRunButton   = sOptions->sel_battle[MENUITEM_BATTLE_QUICKRUN];
    gSaveBlock2Ptr->optionsDoubleBattlesOff = sOptions->sel_battle[MENUITEM_BATTLE_DOUBLEBATTLE];
    gSaveBlock2Ptr->optionsShowBattleMoveInfoOff = sOptions->sel_battle[MENUITEM_BATTLE_MOVEINFO];

    gSaveBlock2Ptr->optionsSound            = sOptions->sel_sound[MENUITEM_SOUND_SOUNDMODE];
    gSaveBlock2Ptr->optionsBikeMusicOff     = sOptions->sel_sound[MENUITEM_SOUND_BIKEMUSIC];
    gSaveBlock2Ptr->optionsSurfMusicOff     = sOptions->sel_sound[MENUITEM_SOUND_SURFMUSIC];

    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
    gTasks[taskId].func = Task_OptionMenuFadeOut;
}

#define try_free(ptr) ({        \
    void ** ptr__ = (void **)&(ptr);   \
    if (*ptr__ != NULL)                \
        Free(*ptr__);                  \
})

static void Task_OptionMenuFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        FreeAllWindowBuffers();
        FREE_AND_SET_NULL(sOptions);
        try_free(sBg2TilemapBuffer);
        try_free(sBg3TilemapBuffer);
        SetGpuReg(REG_OFFSET_WIN0H, 0);
        SetGpuReg(REG_OFFSET_WIN0V, 0);
        SetGpuReg(REG_OFFSET_WININ, 0);
        SetGpuReg(REG_OFFSET_WINOUT, 0);
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 4);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        HideBg(2);
        HideBg(3);
        SetMainCallback2(gMain.savedCallback);
    }
}

static void ScrollMenu(int direction)
{
    int menuItem, pos;
    u8 optionsToDraw = min(OPTIONS_ON_SCREEN, MenuItemCount());

    if (direction == 0) // scroll down
        menuItem = sOptions->menuCursor[sOptions->submenu] + NUM_OPTIONS_FROM_BORDER, pos = optionsToDraw - 1;
    else
        menuItem = sOptions->menuCursor[sOptions->submenu] - NUM_OPTIONS_FROM_BORDER, pos = 0;

    // Hide one
    ScrollWindow(WIN_OPTIONS, direction, Y_DIFF, PIXEL_FILL(0));
    // Show one
    FillWindowPixelRect(WIN_OPTIONS, PIXEL_FILL(0), 0, Y_DIFF * pos, 26 * 8, Y_DIFF);
    // Print
    DrawChoices(menuItem, pos * Y_DIFF);
    DrawLeftSideOptionText(menuItem, (pos * Y_DIFF) + 1);
    CopyWindowToVram(WIN_OPTIONS, COPYWIN_GFX);
}
static void ScrollAll(int direction) // to bottom or top
{
    int i, y, menuItem, pos;
    int scrollCount;
    u8 optionsToDraw = min(OPTIONS_ON_SCREEN, MenuItemCount());

    scrollCount = MenuItemCount() - optionsToDraw;

    // Move items up/down
    ScrollWindow(WIN_OPTIONS, direction, Y_DIFF * scrollCount, PIXEL_FILL(1));

    // Clear moved items
    if (direction == 0)
    {
        y = optionsToDraw - scrollCount;
        if (y < 0)
            y = optionsToDraw;
        y *= Y_DIFF;
    }
    else
    {
        y = 0;
    }

    FillWindowPixelRect(WIN_OPTIONS, PIXEL_FILL(0), 0, y, 26 * 8, Y_DIFF * scrollCount);
    // Print new texts
    for (i = 0; i < scrollCount; i++)
    {
        if (direction == 0) // From top to bottom
            menuItem = MenuItemCount() - 1 - i, pos = optionsToDraw - 1 - i;
        else // From bottom to top
            menuItem = i, pos = i;
        DrawChoices(menuItem, pos * Y_DIFF);
        DrawLeftSideOptionText(menuItem, (pos * Y_DIFF) + 1);
    }
    CopyWindowToVram(WIN_OPTIONS, COPYWIN_GFX);
}

// Process Input functions ****GENERIC****
static int GetMiddleX(const u8 *txt1, const u8 *txt2, const u8 *txt3)
{
    int xMid;
    int widthLeft = GetStringWidth(1, txt1, 0);
    int widthMid = GetStringWidth(1, txt2, 0);
    int widthRight = GetStringWidth(1, txt3, 0);

    widthMid -= (198 - 104);
    xMid = (widthLeft - widthMid - widthRight) / 2 + 104;
    return xMid;
}

static int XOptions_ProcessInput(int x, int selection)
{
    if (JOY_NEW(DPAD_RIGHT))
    {
        if (++selection > (x - 1))
            selection = 0;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        if (--selection < 0)
            selection = (x - 1);
    }
    return selection;
}

static int TwoOptions_ProcessInput(int selection)
{
    if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT))
        selection ^= 1;

    return selection;
}

static int ThreeOptions_ProcessInput(int selection)
{
    return XOptions_ProcessInput(3, selection);
}

static int FourOptions_ProcessInput(int selection)
{
    return XOptions_ProcessInput(4, selection);
}

static int UNUSED ElevenOptions_ProcessInput(int selection)
{
    return XOptions_ProcessInput(11, selection);
}

// Process Input functions ****SPECIFIC****
static int Sound_ProcessInput(int selection)
{
    if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT))
    {
        selection ^= 1;
        SetPokemonCryStereo(selection);
    }

    return selection;
}

static int FrameType_ProcessInput(int selection)
{
    if (JOY_NEW(DPAD_RIGHT))
    {
        if (selection < WINDOW_FRAMES_COUNT - 1)
            selection++;
        else
            selection = 0;

        LoadBgTiles(1, GetWindowFrameTilesPal(selection)->tiles, 0x120, 0x1A2);
        LoadPalette(GetWindowFrameTilesPal(selection)->pal, 0x70, 0x20);
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        if (selection != 0)
            selection--;
        else
            selection = WINDOW_FRAMES_COUNT - 1;

        LoadBgTiles(1, GetWindowFrameTilesPal(selection)->tiles, 0x120, 0x1A2);
        LoadPalette(GetWindowFrameTilesPal(selection)->pal, 0x70, 0x20);
    }
    return selection;
}

// Draw Choices functions ****GENERIC****
static void DrawOptionMenuChoice(const u8 *text, u8 x, u8 y, u8 style, bool8 active)
{
    bool8 choosen = FALSE;
    if (style != 0)
        choosen = TRUE;

    DrawRightSideChoiceText(text, x, y+1, choosen, active);
}

static void UNUSED DrawChoices_Options_Four(const u8 *const *const strings, int selection, int y, bool8 active)
{
    static const u8 choiceOrders[][3] =
    {
        {0, 1, 2},
        {0, 1, 2},
        {1, 2, 3},
        {1, 2, 3},
    };
    u8 styles[4] = {0};
    int xMid;
    const u8 *order = choiceOrders[selection];

    styles[selection] = 1;
    xMid = GetMiddleX(strings[order[0]], strings[order[1]], strings[order[2]]);

    DrawOptionMenuChoice(strings[order[0]], 104, y, styles[order[0]], active);
    DrawOptionMenuChoice(strings[order[1]], xMid, y, styles[order[1]], active);
    DrawOptionMenuChoice(strings[order[2]], GetStringRightAlignXOffset(1, strings[order[2]], 198), y, styles[order[2]], active);
}

static void ReDrawAll(void)
{
    u8 menuItem = sOptions->menuCursor[sOptions->submenu] - sOptions->visibleCursor[sOptions->submenu];
    u8 i;
    u8 optionsToDraw = min(OPTIONS_ON_SCREEN, MenuItemCount());

    if (MenuItemCount() <= OPTIONS_ON_SCREEN) // Draw or delete the scrolling arrows based on options in the menu
    {
        if (sOptions->arrowTaskId != TASK_NONE)
        {
            RemoveScrollIndicatorArrowPair(sOptions->arrowTaskId);
            sOptions->arrowTaskId = TASK_NONE;
        }
    }
    else
    {
        if (sOptions->arrowTaskId == TASK_NONE)
            sOptions->arrowTaskId = AddScrollIndicatorArrowPairParameterized(SCROLL_ARROW_UP, 240 / 2, 20, 110, MenuItemCount() - 1, 110, 110, 0);

    }

    FillWindowPixelBuffer(WIN_OPTIONS, PIXEL_FILL(0));
    for (i = 0; i < optionsToDraw; i++)
    {
        DrawChoices(menuItem+i, i * Y_DIFF);
        DrawLeftSideOptionText(menuItem+i, (i * Y_DIFF) + 1);
    }
    CopyWindowToVram(WIN_OPTIONS, COPYWIN_GFX);
}

// Process Input functions ****SPECIFIC****
static void InstantText_DrawChoices(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_GENERAL_INSTANTTEXT);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_BattleSceneOn, 104, y, styles[0], active);
    DrawOptionMenuChoice(gText_BattleSceneOff, GetStringRightAlignXOffset(FONT_NORMAL, gText_BattleSceneOff, 198), y, styles[1], active);
}

static void ButtonMode_DrawChoices(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_GENERAL_BUTTONMODE);
    u8 styles[3] = {0};
    int xMid = GetMiddleX(gText_ButtonTypeNormal, gText_ButtonTypeLR, gText_ButtonTypeLEqualsA);
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_ButtonTypeNormal, 104, y, styles[0], active);
    DrawOptionMenuChoice(gText_ButtonTypeLR, xMid, y, styles[1], active);
    DrawOptionMenuChoice(gText_ButtonTypeLEqualsA, GetStringRightAlignXOffset(1, gText_ButtonTypeLEqualsA, 198), y, styles[2], active);
}

static void FrameType_DrawChoices(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_GENERAL_FRAMETYPE);
    u8 text[16];
    u8 n = selection + 1;
    u16 i;

    for (i = 0; gText_FrameTypeNumber[i] != EOS && i <= 5; i++)
        text[i] = gText_FrameTypeNumber[i];

    // Convert a number to decimal string
    if (n / 10 != 0)
    {
        text[i] = n / 10 + CHAR_0;
        i++;
        text[i] = n % 10 + CHAR_0;
        i++;
    }
    else
    {
        text[i] = n % 10 + CHAR_0;
        i++;
        text[i] = 0x77;
        i++;
    }

    text[i] = EOS;

    DrawOptionMenuChoice(gText_FrameType, 104, y, 0, active);
    DrawOptionMenuChoice(text, 128, y, 1, active);
}

static void Followers_DrawChoices(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_GENERAL_FOLLOWERS);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_BattleSceneOn, 104, y, styles[0], active);
    DrawOptionMenuChoice(gText_BattleSceneOff, GetStringRightAlignXOffset(1, gText_BattleSceneOff, 198), y, styles[1], active);
}

static void AutoRun_DrawChoices(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_GENERAL_AUTORUN);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_BattleSceneOn, 104, y, styles[0], active);
    DrawOptionMenuChoice(gText_BattleSceneOff, GetStringRightAlignXOffset(1, gText_BattleSceneOff, 198), y, styles[1], active);
}

static void MatchCall_DrawChoices(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_GENERAL_MATCHCALL);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_BattleSceneOn, 104, y, styles[0], active);
    DrawOptionMenuChoice(gText_BattleSceneOff, GetStringRightAlignXOffset(1, gText_BattleSceneOff, 198), y, styles[1], active);
}

static void BattleScene_DrawChoices(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_BATTLE_BATTLESCENE);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_BattleSceneOn, 104, y, styles[0], active);
    DrawOptionMenuChoice(gText_BattleSceneOff, GetStringRightAlignXOffset(FONT_NORMAL, gText_BattleSceneOff, 198), y, styles[1], active);
}

static void BattleStyle_DrawChoices(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_BATTLE_BATTLESTYLE);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_BattleStyleShift, 104, y, styles[0], active);
    DrawOptionMenuChoice(gText_BattleStyleSet, GetStringRightAlignXOffset(FONT_NORMAL, gText_BattleStyleSet, 198), y, styles[1], active);
}

static void BattleSpeed_DrawChoices(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_BATTLE_BATTLESPEED);
    u8 styles[4] = {0};
    s32 width1x, width2x, width3x, width4x, xMid, gap;

    styles[selection] = 1;

    DrawOptionMenuChoice(gText_BattleSpeed1x, 104, y, styles[0], active);

    width1x = GetStringWidth(1, gText_BattleSpeed1x, 0);
    width2x = GetStringWidth(1, gText_BattleSpeed2x, 0);
    width3x = GetStringWidth(1, gText_BattleSpeed3x, 0);
    width4x = GetStringWidth(1, gText_BattleSpeed4x, 0);
    gap = ((198 - 104 - width1x - width2x - width3x - width4x) / 3) + 1;

    xMid = 104 + width1x + gap;
    DrawOptionMenuChoice(gText_BattleSpeed2x, xMid, y, styles[1], active);
    DrawOptionMenuChoice(gText_BattleSpeed3x, xMid + width2x + gap, y, styles[2], active);
    DrawOptionMenuChoice(gText_BattleSpeed4x, 198 - width4x, y, styles[3], active);
}

static void BagUse_DrawChoices(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_BATTLE_BAGUSE);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_BattleSceneOn, 104, y, styles[0], active);
    DrawOptionMenuChoice(gText_BattleSceneOff, GetStringRightAlignXOffset(FONT_NORMAL, gText_BattleSceneOff, 198), y, styles[1], active);
}

static void QuickRun_DrawChoices(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_BATTLE_QUICKRUN);
    u8 styles[3] = {0};
    int xMid = GetMiddleX(gText_QuickRunOptionR, gText_QuickRunOptionBA, gText_BattleSceneOff);
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_QuickRunOptionR, 104, y, styles[0], active);
    DrawOptionMenuChoice(gText_QuickRunOptionBA, xMid, y, styles[1], active);
    DrawOptionMenuChoice(gText_BattleSceneOff, GetStringRightAlignXOffset(1, gText_BattleSceneOff, 198), y, styles[2], active);
}

static void DoubleBattle_DrawChoices(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_BATTLE_DOUBLEBATTLE);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_BattleSceneOn, 104, y, styles[0], active);
    DrawOptionMenuChoice(gText_BattleSceneOff, GetStringRightAlignXOffset(FONT_NORMAL, gText_BattleSceneOff, 198), y, styles[1], active);
}

static void MoveInfo_DrawChoices(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_BATTLE_MOVEINFO);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_BattleSceneOn, 104, y, styles[0], active);
    DrawOptionMenuChoice(gText_BattleSceneOff, GetStringRightAlignXOffset(FONT_NORMAL, gText_BattleSceneOff, 198), y, styles[1], active);
}

static void SoundMode_DrawChoices(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_SOUND_SOUNDMODE);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_SoundMono, 104, y, styles[0], active);
    DrawOptionMenuChoice(gText_SoundStereo, GetStringRightAlignXOffset(FONT_NORMAL, gText_SoundStereo, 198), y, styles[1], active);
}

static void BikeMusic_DrawChoices(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_SOUND_BIKEMUSIC);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_BattleSceneOn, 104, y, styles[0], active);
    DrawOptionMenuChoice(gText_BattleSceneOff, GetStringRightAlignXOffset(FONT_NORMAL, gText_BattleSceneOff, 198), y, styles[1], active);
}

static void SurfMusic_DrawChoices(int selection, int y)
{
    bool8 active = CheckConditions(MENUITEM_SOUND_SURFMUSIC);
    u8 styles[2] = {0};
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_BattleSceneOn, 104, y, styles[0], active);
    DrawOptionMenuChoice(gText_BattleSceneOff, GetStringRightAlignXOffset(FONT_NORMAL, gText_BattleSceneOff, 198), y, styles[1], active);
}

// Background tilemap
#define TILE_TOP_CORNER_L 0x1A2 // 418
#define TILE_TOP_EDGE     0x1A3 // 419
#define TILE_TOP_CORNER_R 0x1A4 // 420
#define TILE_LEFT_EDGE    0x1A5 // 421
#define TILE_RIGHT_EDGE   0x1A7 // 423
#define TILE_BOT_CORNER_L 0x1A8 // 424
#define TILE_BOT_EDGE     0x1A9 // 425
#define TILE_BOT_CORNER_R 0x1AA // 426

static void DrawBgWindowFrames(void)
{
    //                     bg, tile,              x, y, width, height, palNum
    // Option Texts window
    //FillBgTilemapBufferRect(1, TILE_TOP_CORNER_L,  1,  2,  1,  1,  7);
    //FillBgTilemapBufferRect(1, TILE_TOP_EDGE,      2,  2, 26,  1,  7);
    //FillBgTilemapBufferRect(1, TILE_TOP_CORNER_R, 28,  2,  1,  1,  7);
    //FillBgTilemapBufferRect(1, TILE_LEFT_EDGE,     1,  3,  1, 16,  7);
    //FillBgTilemapBufferRect(1, TILE_RIGHT_EDGE,   28,  3,  1, 16,  7);
    //FillBgTilemapBufferRect(1, TILE_BOT_CORNER_L,  1, 13,  1,  1,  7);
    //FillBgTilemapBufferRect(1, TILE_BOT_EDGE,      2, 13, 26,  1,  7);
    //FillBgTilemapBufferRect(1, TILE_BOT_CORNER_R, 28, 13,  1,  1,  7);

    // Description window
    FillBgTilemapBufferRect(1, TILE_TOP_CORNER_L,  1, 14,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_TOP_EDGE,      2, 14, 27,  1,  7);
    FillBgTilemapBufferRect(1, TILE_TOP_CORNER_R, 28, 14,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_LEFT_EDGE,     1, 15,  1,  4,  7);
    FillBgTilemapBufferRect(1, TILE_RIGHT_EDGE,   28, 15,  1,  4,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_CORNER_L,  1, 19,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_EDGE,      2, 19, 27,  1,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_CORNER_R, 28, 19,  1,  1,  7);

    CopyBgTilemapBufferToVram(1);
}
