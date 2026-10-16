#include "global.h"
#include "battle.h"
#include "battle_factory.h"
#include "battle_tent.h"
#include "bg.h"
#include "daycare.h"
#include "decompress.h"
#include "event_data.h"
#include "field_effect.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "international_string_util.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "move_relearner.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "pokemon_summary_screen.h"
#include "scanline_effect.h"
#include "sound.h"
#include "stat_editor.h"
#include "string_util.h"
#include "task.h"
#include "trainer_pokemon_sprites.h"
#include "tv.h"
#include "constants/party_menu.h"
#include "constants/rgb.h"
#include "constants/songs.h"

#if P_STAT_EDITOR_ENABLE

//==========DEFINES==========//
struct StatEditorResources
{
    MainCallback savedCallback;     // determines callback to run when we exit. e.g. where do we want to go after closing the menu
    u8 gfxLoadState;
    u8 mode;
    u8 panel;
    u8 panelInputMode;
    u8 leftRow;
    u8 monIndex;
    u8 typeSpriteId;
    u8 natureUpSpriteId;
    u8 natureDownSpriteId;
    u8 monSpriteId;
    u8 monShadowSpriteId;
    u8 leftSelectorSpriteId;
    u8 rightSelectorSpriteId;
    u8 rightPanelColumn;
    u8 rightPanelRow;
    u8 rightPanelSelectedStat;
    u8 pressingArrow;
    bool8 hasRelearnableMoves;
    bool8 monAnimPlayed; // tracks if the mon's cry has been played at least once
    enum Type hpType;
    enum Species speciesID;
    u16 statEditingValue;
    u16 evTotal;
    u16 monAnimTimer;
    u16 selectorCycleTimer;
};

#define PANEL_LEFT  0
#define PANEL_RIGHT 1

#define LEFT_ROW_NICKNAME 0
#define LEFT_ROW_ABILITY  1
#define LEFT_ROW_NATURE   2
#define LEFT_ROW_COUNT    3

#define RIGHT_PANEL_ROW_HP_TYPE 6
#define RIGHT_PANEL_ROW_COUNT   ((P_STAT_EDITOR_HP_OR_TERA == P_STAT_EDITOR_NONE) ? 6 : 7)

#define PANEL_INPUT_SELECT 0
#define PANEL_INPUT_EDIT   1

#define RIGHT_PANEL_EVS 0
#define RIGHT_PANEL_IVS 1

#define PRESSING_NONE 0
#define PRESSING_LEFT 1
#define PRESSING_RIGHT 2

enum {
    EDIT_INPUT_INCREASE,
    EDIT_INPUT_INCREASE_BY_10,
    EDIT_INPUT_INCREASE_MAX,
    EDIT_INPUT_DECREASE,
    EDIT_INPUT_DECREASE_BY_10,
    EDIT_INPUT_DECREASE_MAX,
};

#define MIN_STAT 0

#define CHECK_IF_STAT_CANT_INCREASE (((sStatEditorDataPtr->statEditingValue == ((sStatEditorDataPtr->rightPanelColumn == RIGHT_PANEL_EVS) ? (MAX_PER_STAT_EVS) : (MAX_PER_STAT_IVS))) \
                                     || ((sStatEditorDataPtr->rightPanelColumn == RIGHT_PANEL_EVS) && (sStatEditorDataPtr->evTotal == MAX_TOTAL_EVS))))
/*
Breakdown of CHECK_IF_STAT_CANT_INCREASE
TLDR: Stat can't increase if you're either: at the maximum amount a stat can have (for both EVs and IVs), or for EVs, if you already hit the max total of EVs

 | (sStatEditorDataPtr->statEditingValue == ((sStatEditorDataPtr->rightPanelColumn == RIGHT_PANEL_EVS) ? (MAX_PER_STAT_EVS) : (MAX_PER_STAT_IVS))
  \> This part checks if the current stat being raised is already at max, whether it's an EV or IV

 | (sStatEditorDataPtr->rightPanelColumn == RIGHT_PANEL_EVS)
  \> This part checks if you're currently editing an EV

 | (sStatEditorDataPtr->evTotal == MAX_TOTAL_EVS)
  \> This part checks if the Pokémon already has the max amount of evs

 | ((sStatEditorDataPtr->rightPanelColumn == RIGHT_PANEL_EVS) && (sStatEditorDataPtr->evTotal == MAX_TOTAL_EVS))
  \> Together, these two check if you're editing an EV and already at the maximum amount of EVs
*/


#define TAG_LEFT_ARROW    30004
#define TAG_RIGHT_ARROW   30005
#define TAG_MISC          30006
#define TAG_MON_SHADOW    30007
#define TAG_MOVE_TYPES    30008
#define TAG_TERA_TYPES    30009
#define TAG_NATURE_ARROWS 30010

enum WindowIds
{
    WINDOW_MAIN_HEADER,
    WINDOW_STATS_HEADER,
    WINDOW_STATS_PANEL,
    WINDOW_NICKNAME,
    WINDOW_ABILITIES,
    WINDOW_NATURES,
};

//==========EWRAM==========//
static EWRAM_DATA struct StatEditorResources *sStatEditorDataPtr = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;
static EWRAM_DATA u16 sBg3TilemapBuffer[0x400] = {0};

//==========STATIC=DEFINES==========//
static void StatEditor_RunSetup(void);
static bool8 StatEditor_DoGfxSetup(void);
static bool8 StatEditor_InitBgs(void);
static void StatEditor_FadeAndBail(void);
static bool8 StatEditor_LoadGraphics(void);
static void StatEditor_InitWindows(void);
static void PrintTitleToWindowMainState(void);
static void Task_StatEditorWaitFadeIn(u8 taskId);
static void Task_StatEditorMain(u8 taskId);
static void CreateMonSprite(enum Species species);
static void DestroyMonSprite(void);
static void PlayMonCry(void);
static void RunMonAnimTimer(void);
static void PrintMonStats(void);
static bool32 IsEditingBoxMon(void);
static struct BoxPokemon *GetCurrentBoxMon(void);
static void UpdateMoveRelearnerState(void);
static void SetMonPosVars(void);
static void SelectorCallback(struct Sprite *sprite);
static u8 CreateSelectors(void);
static void DestroySelectors(void);
static void DestroyNatureArrows(void);

//==========CONST=DATA==========//
static const struct BgTemplate sStatEditorBgTemplates[] =
{
    {
        .bg = 0,    // windows, etc
        .charBaseIndex = 0,
        .mapBaseIndex = 30,
        .priority = 1
    },
    {
        .bg = 1,    // this bg loads the UI tilemap
        .charBaseIndex = 3,
        .mapBaseIndex = 28,
        .priority = 2
    },
    {
        .bg = 2,    // this bg loads the UI tilemap
        .charBaseIndex = 0,
        .mapBaseIndex = 26,
        .priority = 0
    },
    {
        .bg = 3,
        .charBaseIndex = 3,
        .mapBaseIndex = 31,
        .priority = 3,
    },
};

// Window Template Constants
#define STAT_EDITOR_BASE(prevBase,  prevWidth, prevHeight)   (prevBase + (prevWidth * prevHeight))

#define STAT_EDITOR_WIN_BASE    1

#define STAT_EDITOR_MAIN_HEADER_W               16
#define STAT_EDITOR_MAIN_HEADER_H               2
#define STAT_EDITOR_MAIN_HEADER_BASE            STAT_EDITOR_BASE(STAT_EDITOR_WIN_BASE, 0, 0)

#define STAT_EDITOR_STATS_HEADER_W              12
#define STAT_EDITOR_STATS_HEADER_H              2
#define STAT_EDITOR_STATS_HEADER_BASE           STAT_EDITOR_BASE(STAT_EDITOR_MAIN_HEADER_BASE, STAT_EDITOR_MAIN_HEADER_W, STAT_EDITOR_MAIN_HEADER_H)

#define STAT_EDITOR_STATS_PANEL_W               16
#define STAT_EDITOR_STATS_PANEL_H               14
#define STAT_EDITOR_STATS_PANEL_BASE            STAT_EDITOR_BASE(STAT_EDITOR_STATS_HEADER_BASE, STAT_EDITOR_STATS_HEADER_W, STAT_EDITOR_STATS_HEADER_H)

#define STAT_EDITOR_NICKNAME_W                  10
#define STAT_EDITOR_NICKNAME_H                  3
#define STAT_EDITOR_NICKNAME_BASE               STAT_EDITOR_BASE(STAT_EDITOR_STATS_PANEL_BASE, STAT_EDITOR_STATS_PANEL_W, STAT_EDITOR_STATS_PANEL_H)

#define STAT_EDITOR_ABILITIES_W                 14
#define STAT_EDITOR_ABILITIES_H                 2
#define STAT_EDITOR_ABILITIES_BASE              STAT_EDITOR_BASE(STAT_EDITOR_NICKNAME_BASE, STAT_EDITOR_NICKNAME_W, STAT_EDITOR_NICKNAME_H)

#define STAT_EDITOR_NATURES_W                   14
#define STAT_EDITOR_NATURES_H                   2
#define STAT_EDITOR_NATURES_BASE                STAT_EDITOR_BASE(STAT_EDITOR_ABILITIES_BASE, STAT_EDITOR_ABILITIES_W, STAT_EDITOR_ABILITIES_H)

static const struct WindowTemplate sMenuWindowTemplates[] =
{
    [WINDOW_MAIN_HEADER] = 
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 0,
        .width = STAT_EDITOR_MAIN_HEADER_W, 
        .height = STAT_EDITOR_MAIN_HEADER_H,
        .paletteNum = 2,
        .baseBlock = STAT_EDITOR_MAIN_HEADER_BASE,
    },
    [WINDOW_STATS_HEADER] =
    {
        .bg = 0,
        .tilemapLeft = 17,
        .tilemapTop = 1,
        .width = STAT_EDITOR_STATS_HEADER_W, 
        .height = STAT_EDITOR_STATS_HEADER_H,
        .paletteNum = 2,
        .baseBlock = STAT_EDITOR_STATS_HEADER_BASE,
    },
    [WINDOW_STATS_PANEL] = 
    {
        .bg = 0,
        .tilemapLeft = 14,
        .tilemapTop = 3,
        .width = STAT_EDITOR_STATS_PANEL_W,
        .height = STAT_EDITOR_STATS_PANEL_H,
        .paletteNum = 2,
        .baseBlock = STAT_EDITOR_STATS_PANEL_BASE,
    },
    [WINDOW_NICKNAME] = 
    {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 12,
        .width = STAT_EDITOR_NICKNAME_W,
        .height = STAT_EDITOR_NICKNAME_H,
        .paletteNum = 2,
        .baseBlock = STAT_EDITOR_NICKNAME_BASE,
    },
    [WINDOW_ABILITIES] = 
    {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 15,
        .width = STAT_EDITOR_ABILITIES_W,
        .height = STAT_EDITOR_ABILITIES_H,
        .paletteNum = 2,
        .baseBlock = STAT_EDITOR_ABILITIES_BASE,
    },
    [WINDOW_NATURES] = 
    {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 17,
        .width = STAT_EDITOR_NATURES_W,
        .height = STAT_EDITOR_NATURES_H,
        .paletteNum = 2,
        .baseBlock = STAT_EDITOR_NATURES_BASE,
    },
    DUMMY_WIN_TEMPLATE
};

static const u32 sStatEditorBgTiles[]       = INCGFX_U32("graphics/stat_editor/background_tileset.png", ".4bpp.smol");
static const u32 sStatEditorBgTilemap[]     = INCGFX_U32("graphics/stat_editor/background_tileset.bin", ".smolTM");
static const u32 sStatEditorScrollTilemap[] = INCGFX_U32("graphics/stat_editor/scroll_bg.bin", ".smolTM");
static const u16 sStatEditorBgPalette[]     = INCGFX_U16("graphics/stat_editor/background_pal.pal", ".gbapal");
static const u32 sLeftArrowGfx[]            = INCGFX_U32("graphics/stat_editor/left_arrow.png", ".4bpp.smol");
static const u32 sRightArrowGfx[]           = INCGFX_U32("graphics/stat_editor/right_arrow.png", ".4bpp.smol");
static const u32 sNatureArrowsGfx[]         = INCGFX_U32("graphics/stat_editor/nature_arrows.png", ".4bpp.smol");
static const u16 sMiscPalette[]             = INCGFX_U16("graphics/stat_editor/misc.pal", ".gbapal");
static const u16 sTextPalette[]             = INCGFX_U16("graphics/stat_editor/text.pal", ".gbapal");
static const u8 sA_ButtonGfx[]              = INCGFX_U8("graphics/stat_editor/a_button.png", ".4bpp");
static const u8 sB_ButtonGfx[]              = INCGFX_U8("graphics/stat_editor/b_button.png", ".4bpp");
static const u8 sLR_ButtonGfx[]             = INCGFX_U8("graphics/stat_editor/lr_button.png", ".4bpp");
static const u8 sStart_ButtonGfx[]          = INCGFX_U8("graphics/stat_editor/start_button.png", ".4bpp");
static const u8 sDPad_ButtonGfx[]           = INCGFX_U8("graphics/stat_editor/dpad_button.png", ".4bpp");
static const u8 sDPadLR_ButtonGfx[]         = INCGFX_U8("graphics/stat_editor/dpad_lr_button.png", ".4bpp");
static const u16 sMonShadowPalette[]        = INCGFX_U16("graphics/stat_editor/shadow.pal", ".gbapal");
static const u32 sMoveTypesGfx[]            = INCGFX_U32("graphics/stat_editor/move_types.png", ".4bpp.smol");
static const u16 sMoveTypesPalette[]        = INCGFX_U16("graphics/stat_editor/move_types.png", ".gbapal");
static const u32 sTeraTypesGfx[]            = INCGFX_U32("graphics/stat_editor/tera_types.png", ".4bpp.smol");
static const u16 sTeraTypesPalette[]        = INCGFX_U16("graphics/stat_editor/tera_types.png", ".gbapal");

static const struct SpritePalette sSpritePal_MonShadow =
{
    sMonShadowPalette, TAG_MON_SHADOW
};

static const struct OamData sOamData_HiddenPowerType =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x16),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_TypeNone[] = {
    ANIMCMD_FRAME(TYPE_NONE * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeNormal[] = {
    ANIMCMD_FRAME(TYPE_NORMAL * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_HPTypeFighting[] = {
    ANIMCMD_FRAME(TYPE_FIGHTING * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_HPTypeFlying[] = {
    ANIMCMD_FRAME(TYPE_FLYING * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_HPTypePoison[] = {
    ANIMCMD_FRAME(TYPE_POISON * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_HPTypeGround[] = {
    ANIMCMD_FRAME(TYPE_GROUND * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_HPTypeRock[] = {
    ANIMCMD_FRAME(TYPE_ROCK * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_HPTypeBug[] = {
    ANIMCMD_FRAME(TYPE_BUG * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_HPTypeGhost[] = {
    ANIMCMD_FRAME(TYPE_GHOST * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_HPTypeSteel[] = {
    ANIMCMD_FRAME(TYPE_STEEL * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_HPTypeMystery[] = {
    ANIMCMD_FRAME(TYPE_MYSTERY * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_HPTypeFire[] = {
    ANIMCMD_FRAME(TYPE_FIRE * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_HPTypeWater[] = {
    ANIMCMD_FRAME(TYPE_WATER * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_HPTypeGrass[] = {
    ANIMCMD_FRAME(TYPE_GRASS * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_HPTypeElectric[] = {
    ANIMCMD_FRAME(TYPE_ELECTRIC * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_HPTypePsychic[] = {
    ANIMCMD_FRAME(TYPE_PSYCHIC * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_HPTypeIce[] = {
    ANIMCMD_FRAME(TYPE_ICE * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_HPTypeDragon[] = {
    ANIMCMD_FRAME(TYPE_DRAGON * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_HPTypeDark[] = {
    ANIMCMD_FRAME(TYPE_DARK * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFairy[] = {
    ANIMCMD_FRAME(TYPE_FAIRY * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeStellar[] = {
    ANIMCMD_FRAME(TYPE_STELLAR * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_HiddenPowerType[NUMBER_OF_MON_TYPES] = {
    [TYPE_FIGHTING] = sSpriteAnim_HPTypeFighting,
    [TYPE_FLYING] = sSpriteAnim_HPTypeFlying,
    [TYPE_POISON] = sSpriteAnim_HPTypePoison,
    [TYPE_GROUND] = sSpriteAnim_HPTypeGround,
    [TYPE_ROCK] = sSpriteAnim_HPTypeRock,
    [TYPE_BUG] = sSpriteAnim_HPTypeBug,
    [TYPE_GHOST] = sSpriteAnim_HPTypeGhost,
    [TYPE_STEEL] = sSpriteAnim_HPTypeSteel,
    [TYPE_MYSTERY] = sSpriteAnim_HPTypeMystery,
    [TYPE_FIRE] = sSpriteAnim_HPTypeFire,
    [TYPE_WATER] = sSpriteAnim_HPTypeWater,
    [TYPE_GRASS] = sSpriteAnim_HPTypeGrass,
    [TYPE_ELECTRIC] = sSpriteAnim_HPTypeElectric,
    [TYPE_PSYCHIC] = sSpriteAnim_HPTypePsychic,
    [TYPE_ICE] = sSpriteAnim_HPTypeIce,
    [TYPE_DRAGON] = sSpriteAnim_HPTypeDragon,
    [TYPE_DARK] = sSpriteAnim_HPTypeDark,
    // Not HP Types (kept because yes)
    [TYPE_NONE] = sSpriteAnim_TypeNone,
    [TYPE_NORMAL] = sSpriteAnim_TypeNormal,
    [TYPE_FAIRY] = sSpriteAnim_TypeFairy,
    [TYPE_STELLAR] = sSpriteAnim_TypeStellar,
};

static const struct CompressedSpriteSheet sSpriteSheet_HiddenPowerType =
{
    .data = sMoveTypesGfx,
    .size = NUMBER_OF_MON_TYPES * 0x100,
    .tag = TAG_MOVE_TYPES
};

static const struct SpriteTemplate sSpriteTemplate_HiddenPowerType =
{
    .tileTag = TAG_MOVE_TYPES,
    .paletteTag = TAG_MOVE_TYPES,
    .oam = &sOamData_HiddenPowerType,
    .anims = sSpriteAnimTable_HiddenPowerType,
};

static const union AnimCmd sSpriteAnim_TeraTypeNone[] = {
    ANIMCMD_FRAME(TYPE_NONE * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeNormal[] = {
    ANIMCMD_FRAME(TYPE_NORMAL * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeFighting[] = {
    ANIMCMD_FRAME(TYPE_FIGHTING * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeFlying[] = {
    ANIMCMD_FRAME(TYPE_FLYING * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypePoison[] = {
    ANIMCMD_FRAME(TYPE_POISON * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeGround[] = {
    ANIMCMD_FRAME(TYPE_GROUND * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeRock[] = {
    ANIMCMD_FRAME(TYPE_ROCK * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeBug[] = {
    ANIMCMD_FRAME(TYPE_BUG * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeGhost[] = {
    ANIMCMD_FRAME(TYPE_GHOST * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeSteel[] = {
    ANIMCMD_FRAME(TYPE_STEEL * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeMystery[] = {
    ANIMCMD_FRAME(TYPE_MYSTERY * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeFire[] = {
    ANIMCMD_FRAME(TYPE_FIRE * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeWater[] = {
    ANIMCMD_FRAME(TYPE_WATER * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeGrass[] = {
    ANIMCMD_FRAME(TYPE_GRASS * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeElectric[] = {
    ANIMCMD_FRAME(TYPE_ELECTRIC * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypePsychic[] = {
    ANIMCMD_FRAME(TYPE_PSYCHIC * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeIce[] = {
    ANIMCMD_FRAME(TYPE_ICE * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeDragon[] = {
    ANIMCMD_FRAME(TYPE_DRAGON * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeDark[] = {
    ANIMCMD_FRAME(TYPE_DARK * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeFairy[] = {
    ANIMCMD_FRAME(TYPE_FAIRY * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeStellar[] = {
    ANIMCMD_FRAME(TYPE_STELLAR * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_TeraType[NUMBER_OF_MON_TYPES] = {
    [TYPE_NONE] = sSpriteAnim_TeraTypeNone,
    [TYPE_NORMAL] = sSpriteAnim_TeraTypeNormal,
    [TYPE_FIGHTING] = sSpriteAnim_TeraTypeFighting,
    [TYPE_FLYING] = sSpriteAnim_TeraTypeFlying,
    [TYPE_POISON] = sSpriteAnim_TeraTypePoison,
    [TYPE_GROUND] = sSpriteAnim_TeraTypeGround,
    [TYPE_ROCK] = sSpriteAnim_TeraTypeRock,
    [TYPE_BUG] = sSpriteAnim_TeraTypeBug,
    [TYPE_GHOST] = sSpriteAnim_TeraTypeGhost,
    [TYPE_STEEL] = sSpriteAnim_TeraTypeSteel,
    [TYPE_MYSTERY] = sSpriteAnim_TeraTypeMystery,
    [TYPE_FIRE] = sSpriteAnim_TeraTypeFire,
    [TYPE_WATER] = sSpriteAnim_TeraTypeWater,
    [TYPE_GRASS] = sSpriteAnim_TeraTypeGrass,
    [TYPE_ELECTRIC] = sSpriteAnim_TeraTypeElectric,
    [TYPE_PSYCHIC] = sSpriteAnim_TeraTypePsychic,
    [TYPE_ICE] = sSpriteAnim_TeraTypeIce,
    [TYPE_DRAGON] = sSpriteAnim_TeraTypeDragon,
    [TYPE_DARK] = sSpriteAnim_TeraTypeDark,
    [TYPE_FAIRY] = sSpriteAnim_TeraTypeFairy,
    [TYPE_STELLAR] = sSpriteAnim_TeraTypeStellar
};

static const struct OamData sOamData_TeraType =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(16x16),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_TeraType =
{
    .data = sTeraTypesGfx,
    .size = NUMBER_OF_MON_TYPES * (16 * 16),
    .tag = TAG_TERA_TYPES
};

static const struct SpriteTemplate sSpriteTemplate_TeraType =
{
    .tileTag = TAG_TERA_TYPES,
    .paletteTag = TAG_TERA_TYPES,
    .oam = &sOamData_TeraType,
    .anims = sSpriteAnimTable_TeraType,
};

enum FontColors
{
    FONT_BLACK,
    FONT_WHITE,
    FONT_WHITE_BLACK
};

static const u8 sMenuWindowFontColors[][3] =
{
    [FONT_BLACK]       = {0, 1, 2},
    [FONT_WHITE]       = {0, 3, 4},
    [FONT_WHITE_BLACK] = {0, 3, 1}
};

static const struct OamData sOamData_Selector =
{
    .size = SPRITE_SIZE(16x16),
    .shape = SPRITE_SHAPE(16x16),
    .priority = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_LeftArrow =
{
    .data = sLeftArrowGfx,
    .size = 16*16*3*4/2,
    .tag = TAG_LEFT_ARROW,
};

static const struct CompressedSpriteSheet sSpriteSheet_RightArrow =
{
    .data = sRightArrowGfx,
    .size = 16*16*3*4/2,
    .tag = TAG_RIGHT_ARROW,
};

static const struct SpritePalette sSpritePal_Misc =
{
    .data = sMiscPalette,
    .tag  = TAG_MISC,
};

static const union AnimCmd sSpriteAnim_ArrowIdle[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_ArrowCycle[] =
{
    ANIMCMD_FRAME(0, 12),
    ANIMCMD_FRAME(4, 12),
    ANIMCMD_FRAME(8, 12),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sSpriteAnim_ArrowActive[] =
{
    ANIMCMD_FRAME(8, 1),
    ANIMCMD_END,
};

enum {
    SELECTOR_ANIM_IDLE,
    SELECTOR_ANIM_CYCLE,
    SELECTOR_ANIM_ACTIVE,
};

static const union AnimCmd *const sSpriteAnimTable_Arrow[] =
{
    [SELECTOR_ANIM_IDLE]   = sSpriteAnim_ArrowIdle,
    [SELECTOR_ANIM_CYCLE]  = sSpriteAnim_ArrowCycle,
    [SELECTOR_ANIM_ACTIVE] = sSpriteAnim_ArrowActive,
};

static const struct SpriteTemplate sSpriteTemplate_LeftArrow =
{
    .tileTag = TAG_LEFT_ARROW,
    .paletteTag = TAG_MISC,
    .oam = &sOamData_Selector,
    .anims = sSpriteAnimTable_Arrow,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SelectorCallback,
};

static const struct SpriteTemplate sSpriteTemplate_RightArrow =
{
    .tileTag = TAG_RIGHT_ARROW,
    .paletteTag = TAG_MISC,
    .oam = &sOamData_Selector,
    .anims = sSpriteAnimTable_Arrow,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SelectorCallback,
};

static const struct CompressedSpriteSheet sSpriteSheet_NatureArrows =
{
    .data = sNatureArrowsGfx,
    .size = 16*16*2*4/2,
    .tag = TAG_NATURE_ARROWS,
};

static const union AnimCmd sSpriteAnim_NatureArrowDown[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_NatureArrowUp[] =
{
    ANIMCMD_FRAME(4, 1),
    ANIMCMD_END,
};

static const union AnimCmd *const sSpriteAnimTable_NatureArrows[] =
{
    sSpriteAnim_NatureArrowDown,
    sSpriteAnim_NatureArrowUp,
};

static const struct OamData sOamData_NatureArrow =
{
    .size = SPRITE_SIZE(16x16),
    .shape = SPRITE_SHAPE(16x16),
    .priority = 0,
};

static const struct SpriteTemplate sSpriteTemplate_NatureArrow =
{
    .tileTag    = TAG_NATURE_ARROWS,
    .paletteTag = TAG_MISC,
    .oam        = &sOamData_NatureArrow,
    .anims      = sSpriteAnimTable_NatureArrows,
    .images     = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback   = SpriteCallbackDummy,
};

#define NATURE_ARROW_X       (STARTING_X + 102)
#define NATURE_ARROW_BASE_Y  (STARTING_Y + 32)

#define STAT_ROW_HEIGHT  16
#define SECOND_COLUMN    (8 * 4)
#define THIRD_COLUMN     (8 * 8)
#define STARTING_X       28
#define STARTING_Y       1

#define LEFT_PANEL_SELECTOR_WIDTH          74
#define RIGHT_PANEL_SELECTOR_WIDTH         28
#define RIGHT_PANEL_HP_TYPE_SELECTOR_WIDTH 40

#define LEFT_NICKNAME_Y  12
#define LEFT_ABILITY_Y   34
#define LEFT_NATURE_Y    50

#define TYPE_ICON_X 216
#define TYPE_ICON_Y 129

#define WINDOW3_SCREEN_X 8
#define WINDOW3_SCREEN_Y 92

#define SELECTOR_LEFT_EDGE_X     (WINDOW3_SCREEN_X - 4)
#define SELECTOR_LEFT_NICKNAME_Y (WINDOW3_SCREEN_Y + LEFT_NICKNAME_Y + 4)
#define SELECTOR_LEFT_ABILITY_Y  (WINDOW3_SCREEN_Y + LEFT_ABILITY_Y + 4)
#define SELECTOR_LEFT_NATURE_Y   (WINDOW3_SCREEN_Y + LEFT_NATURE_Y + 4)

#define SELECTOR_RIGHT_EV_LEFT_EDGE_X  (54 + SECOND_COLUMN + 82)
#define SELECTOR_RIGHT_IV_LEFT_EDGE_X  (54 + THIRD_COLUMN + 82)
#define SELECTOR_RIGHT_BASE_Y          (8 + STARTING_Y + 25)

#define SELECTOR_RIGHT_HP_TYPE_LEFT_EDGE_X (TYPE_ICON_X - 20)
#define SELECTOR_RIGHT_HP_TYPE_Y            TYPE_ICON_Y

#define MON_ICON_X (32 + 6)
#define MON_ICON_Y (32 + 22)

#define BUTTON_Y 4

struct StatPrintCoords
{
    u16 x;
    u16 y;
};

static const struct StatPrintCoords sStatPrintData[] =
{
    [MON_DATA_MAX_HP]    = {STARTING_X,                     STARTING_Y},
    [MON_DATA_HP_EV]     = {STARTING_X + SECOND_COLUMN + 2, STARTING_Y},
    [MON_DATA_HP_IV]     = {STARTING_X + THIRD_COLUMN + 2,  STARTING_Y},

    [MON_DATA_ATK]       = {STARTING_X,                     STARTING_Y + STAT_ROW_HEIGHT},
    [MON_DATA_ATK_EV]    = {STARTING_X + SECOND_COLUMN + 2, STARTING_Y + STAT_ROW_HEIGHT},
    [MON_DATA_ATK_IV]    = {STARTING_X + THIRD_COLUMN + 2,  STARTING_Y + STAT_ROW_HEIGHT},

    [MON_DATA_DEF]       = {STARTING_X,                     STARTING_Y + (STAT_ROW_HEIGHT * 2)},
    [MON_DATA_DEF_EV]    = {STARTING_X + SECOND_COLUMN + 2, STARTING_Y + (STAT_ROW_HEIGHT * 2)},
    [MON_DATA_DEF_IV]    = {STARTING_X + THIRD_COLUMN + 2,  STARTING_Y + (STAT_ROW_HEIGHT * 2)},

    [MON_DATA_SPATK]     = {STARTING_X,                     STARTING_Y + (STAT_ROW_HEIGHT * 3)},
    [MON_DATA_SPATK_EV]  = {STARTING_X + SECOND_COLUMN + 2, STARTING_Y + (STAT_ROW_HEIGHT * 3)},
    [MON_DATA_SPATK_IV]  = {STARTING_X + THIRD_COLUMN + 2,  STARTING_Y + (STAT_ROW_HEIGHT * 3)},

    [MON_DATA_SPDEF]     = {STARTING_X,                     STARTING_Y + (STAT_ROW_HEIGHT * 4)},
    [MON_DATA_SPDEF_EV]  = {STARTING_X + SECOND_COLUMN + 2, STARTING_Y + (STAT_ROW_HEIGHT * 4)},
    [MON_DATA_SPDEF_IV]  = {STARTING_X + THIRD_COLUMN + 2,  STARTING_Y + (STAT_ROW_HEIGHT * 4)},

    [MON_DATA_SPEED]     = {STARTING_X,                     STARTING_Y + (STAT_ROW_HEIGHT * 5)},
    [MON_DATA_SPEED_EV]  = {STARTING_X + SECOND_COLUMN + 2, STARTING_Y + (STAT_ROW_HEIGHT * 5)},
    [MON_DATA_SPEED_IV]  = {STARTING_X + THIRD_COLUMN + 2,  STARTING_Y + (STAT_ROW_HEIGHT * 5)},
};

static const u16 sActualStatsMap[] = {
    MON_DATA_MAX_HP,
    MON_DATA_ATK,
    MON_DATA_DEF,
    MON_DATA_SPEED,
    MON_DATA_SPATK,
    MON_DATA_SPDEF,
};

static const u16 sEVStatsMap[] = {
    MON_DATA_HP_EV,
    MON_DATA_ATK_EV,
    MON_DATA_DEF_EV,
    MON_DATA_SPEED_EV,
    MON_DATA_SPATK_EV,
    MON_DATA_SPDEF_EV,
};

static const u16 sIVStatsMap[] = {
    MON_DATA_HP_IV,
    MON_DATA_ATK_IV,
    MON_DATA_DEF_IV,
    MON_DATA_SPEED_IV,
    MON_DATA_SPATK_IV,
    MON_DATA_SPDEF_IV,
};

static const u8 sStatsMap[] = {
    [STAT_HP] = 0,
    [STAT_ATK] = 1,
    [STAT_DEF] = 2,
    [STAT_SPATK] = 3,
    [STAT_SPDEF] = 4,
    [STAT_SPEED] = 5,
};

static const u8 sGenderColors[2][3] =
{
    {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_LIGHT_BLUE, TEXT_COLOR_BLUE},
    {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_LIGHT_RED,  TEXT_COLOR_RED}
};

static const u8 sText_MenuStat[]               = _("Stat");
static const u8 sText_MenuEV[]                 = _("EVs");
static const u8 sText_MenuIV[]                 = _("IVs");
static const u8 sText_MenuAbility[]            = _("Ability");
static const u8 sText_MenuNature[]             = _("Nature");
static const u8 sText_MenuLRButtonParty[]      = _("Party");
static const u8 sText_MenuLRButtonBox[]        = _("Box");
static const u8 sText_MenuStartButtonMoves[]   = _("Moves");
static const u8 sText_MenuABButtonSave[]       = _("Save");
static const u8 sText_MenuDPadChangeStat[]     = _("Stat");
static const u8 sText_MenuDPadChangeAbility[]  = _("Ability");
static const u8 sText_MenuDPadChangeNature[]   = _("Nature");
static const u8 sText_MenuDPadChangeHPType[]   = _("HP Type");
static const u8 sText_MenuDPadChangeTeraType[] = _("Tera Type");

// Begin Generic UI Initialization Code

void Task_OpenStatEditorFromStartMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        StatEditor_Init(CB2_ReturnToFieldWithOpenMenu);
        DestroyTask(taskId);
    }
}

void StatEditor_Init(MainCallback callback)
{
    if ((sStatEditorDataPtr = AllocZeroed(sizeof(struct StatEditorResources))) == NULL)
    {
        SetMainCallback2(callback);
        return;
    }

    sStatEditorDataPtr->gfxLoadState          = 0;
    sStatEditorDataPtr->savedCallback         = callback;
    sStatEditorDataPtr->leftSelectorSpriteId  = 0xFF;
    sStatEditorDataPtr->rightSelectorSpriteId = 0xFF;
    if (IsEditingBoxMon())
        sStatEditorDataPtr->monIndex          = gSpecialVar_MonBoxPos;
    else
        sStatEditorDataPtr->monIndex          = gSpecialVar_0x8004;
    sStatEditorDataPtr->panel                 = PANEL_LEFT;
    sStatEditorDataPtr->leftRow               = LEFT_ROW_NICKNAME;
    sStatEditorDataPtr->rightPanelColumn      = RIGHT_PANEL_EVS;
    sStatEditorDataPtr->rightPanelRow         = 0;
    sStatEditorDataPtr->monSpriteId           = MAX_SPRITES;
    sStatEditorDataPtr->monShadowSpriteId     = MAX_SPRITES;
    sStatEditorDataPtr->natureUpSpriteId      = MAX_SPRITES;
    sStatEditorDataPtr->natureDownSpriteId    = MAX_SPRITES;

    SetMainCallback2(StatEditor_RunSetup);
}

static void StatEditor_RunSetup(void)
{
    while (TRUE)
    {
        if (StatEditor_DoGfxSetup() == TRUE)
            break;
    }
}

static void StatEditor_MainCB(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void StatEditor_VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    if (P_STAT_EDITOR_SCROLLING_BG)
    {
        ChangeBgX(3, 64, BG_COORD_ADD);
        ChangeBgY(3, 64, BG_COORD_ADD);
    }
    if (P_STAT_EDITOR_MON_IDLE_ANIMS && sStatEditorDataPtr->monSpriteId != MAX_SPRITES)
        RunMonAnimTimer();
    if (sStatEditorDataPtr->panelInputMode == PANEL_INPUT_EDIT)
        sStatEditorDataPtr->selectorCycleTimer++;
    else
        sStatEditorDataPtr->selectorCycleTimer = 0;
}

static bool8 StatEditor_DoGfxSetup(void)
{
    switch (gMain.state)
    {
    case 0:
        DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000)
        SetVBlankHBlankCallbacksToNull();
        ResetVramOamAndBgCntRegs();
        ClearScheduledBgCopiesToVram();
        gMain.state++;
        break;
    case 1:
        ScanlineEffect_Stop();
        FreeAllSpritePalettes();
        ResetPaletteFade();
        ResetSpriteData();
        ResetTasks();
        gMain.state++;
        break;
    case 2:
        if (StatEditor_InitBgs())
        {
            sStatEditorDataPtr->gfxLoadState = 0;
            gMain.state++;
        }
        else
        {
            StatEditor_FadeAndBail();
            return TRUE;
        }
        break;
    case 3:
        if (StatEditor_LoadGraphics() == TRUE)
            gMain.state++;
        break;
    case 4:
        sStatEditorDataPtr->speciesID = GetBoxMonData(GetCurrentBoxMon(), MON_DATA_SPECIES_OR_EGG);
        UpdateMoveRelearnerState();
        LoadCompressedSpriteSheet(&sSpriteSheet_LeftArrow);
        LoadCompressedSpriteSheet(&sSpriteSheet_RightArrow);
        LoadCompressedSpriteSheet(&sSpriteSheet_NatureArrows);
        LoadSpritePalette(&sSpritePal_Misc);
        CreateMonSprite(sStatEditorDataPtr->speciesID);
        if (P_STAT_EDITOR_HP_OR_TERA == P_STAT_EDITOR_HIDDEN_POWER)
        {
            LoadCompressedSpriteSheet(&sSpriteSheet_HiddenPowerType);
            sStatEditorDataPtr->typeSpriteId = CreateSprite(&sSpriteTemplate_HiddenPowerType, 0, 0, 1);
        }
        else if (P_STAT_EDITOR_HP_OR_TERA == P_STAT_EDITOR_TERASTAL)
        {
            LoadCompressedSpriteSheet(&sSpriteSheet_TeraType);
            sStatEditorDataPtr->typeSpriteId = CreateSprite(&sSpriteTemplate_TeraType, 0, 0, 1);
        }
        sStatEditorDataPtr->natureUpSpriteId = CreateSprite(&sSpriteTemplate_NatureArrow, 0, 0, 1);
        sStatEditorDataPtr->natureDownSpriteId = CreateSprite(&sSpriteTemplate_NatureArrow, 0, 0, 1);
        gMain.state++;
        break;
    case 5:
        StatEditor_InitWindows();
        PrintTitleToWindowMainState();
        sStatEditorDataPtr->panelInputMode = PANEL_INPUT_SELECT;
        PrintMonStats();
        CreateSelectors();
        gMain.state++;
        break;
    case 6:
        CreateTask(Task_StatEditorWaitFadeIn, 0);
        BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
        gMain.state++;
        break;
    case 7:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    default:
        SetVBlankCallback(StatEditor_VBlankCB);
        SetMainCallback2(StatEditor_MainCB);
        return TRUE;
    }
    return FALSE;
}

static void StatEditor_FreeResources(void)
{
    DestroySelectors();
    DestroyNatureArrows();
    DestroyMonSprite();
    DestroyMonSpritesGfxManager(MON_SPR_GFX_MANAGER_A);
    DestroySprite(&gSprites[sStatEditorDataPtr->typeSpriteId]);
    FreeSpritePaletteByTag(TAG_MISC);
    FreeSpriteTilesByTag(TAG_MOVE_TYPES);
    FreeSpriteTilesByTag(TAG_TERA_TYPES);
    StopCryAndClearCrySongs();
    if (P_STAT_EDITOR_BG_BLEND || P_STAT_EDITOR_MON_SHADOWS)
    {
        // Clear alpha blending used for the mon shadow
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    }
    TRY_FREE_AND_SET_NULL(sStatEditorDataPtr);
    TRY_FREE_AND_SET_NULL(sBg1TilemapBuffer);
    FreeAllWindowBuffers();
}

static void Task_StatEditorWaitFadeAndBail(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sStatEditorDataPtr->savedCallback);
        StatEditor_FreeResources();
        DestroyTask(taskId);
    }
}

static void StatEditor_FadeAndBail(void)
{
    gLastViewedMonIndex = sStatEditorDataPtr->monIndex;
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_StatEditorWaitFadeAndBail, 0);
    SetVBlankCallback(StatEditor_VBlankCB);
    SetMainCallback2(StatEditor_MainCB);
}

static bool8 StatEditor_InitBgs(void)
{
    ResetAllBgsCoordinates();
    sBg1TilemapBuffer = Alloc(0x800);

    if (sBg1TilemapBuffer == NULL)
        return FALSE;

    memset(sBg1TilemapBuffer, 0, 0x800);
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sStatEditorBgTemplates, NELEMS(sStatEditorBgTemplates));
    SetBgTilemapBuffer(3, sBg3TilemapBuffer);
    SetBgTilemapBuffer(1, sBg1TilemapBuffer);
    ScheduleBgCopyTilemapToVram(3);
    ScheduleBgCopyTilemapToVram(1);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    if (P_STAT_EDITOR_BG_BLEND || P_STAT_EDITOR_MON_SHADOWS)
    {
        if (P_STAT_EDITOR_BG_BLEND)
            SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT2_BG3 | BLDCNT_TGT2_BG2 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT1_BG2 | BLDCNT_TGT1_BG1);
        else
            SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT2_BG3 | BLDCNT_TGT2_BG2 | BLDCNT_EFFECT_BLEND);

        SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(14, 6));
    }
    ShowBg(3);
    ShowBg(0);
    ShowBg(1);
    ShowBg(2);
    return TRUE;
}

static bool8 StatEditor_LoadGraphics(void)
{
    switch (sStatEditorDataPtr->gfxLoadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sStatEditorBgTiles, 0, 0, 0);
        sStatEditorDataPtr->gfxLoadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            DecompressDataWithHeaderWram(sStatEditorBgTilemap, sBg1TilemapBuffer);
            sStatEditorDataPtr->gfxLoadState++;
        }
        break;
    case 2:
        DecompressDataWithHeaderWram(sStatEditorScrollTilemap, sBg3TilemapBuffer);
        sStatEditorDataPtr->gfxLoadState++;
        break;
    case 3:
        LoadPalette(sStatEditorBgPalette, 0, PLTT_SIZE_4BPP);
        LoadPalette(sTextPalette, BG_PLTT_ID(2), PLTT_SIZE_4BPP);
        sStatEditorDataPtr->gfxLoadState++;
        break;
    case 4:
        if (P_STAT_EDITOR_HP_OR_TERA == P_STAT_EDITOR_HIDDEN_POWER)
            LoadPalette(sMoveTypesPalette, OBJ_PLTT_ID(13), 3 * PLTT_SIZE_4BPP);
        else if (P_STAT_EDITOR_HP_OR_TERA == P_STAT_EDITOR_TERASTAL)
            LoadPalette(sTeraTypesPalette, OBJ_PLTT_ID(13), 3 * PLTT_SIZE_4BPP);
        sStatEditorDataPtr->gfxLoadState++;
        break;
    default:
        sStatEditorDataPtr->gfxLoadState = 0;
        return TRUE;
    }
    return FALSE;
}

static void StatEditor_InitWindows(void)
{
    InitWindows(sMenuWindowTemplates);
    DeactivateAllTextPrinters();
    ScheduleBgCopyTilemapToVram(0);
    FillWindowPixelBuffer(WINDOW_MAIN_HEADER, PIXEL_FILL(0));
    PutWindowTilemap(WINDOW_MAIN_HEADER);
    CopyWindowToVram(WINDOW_MAIN_HEADER, COPYWIN_FULL);
    ScheduleBgCopyTilemapToVram(2);
}

static void Task_StatEditorWaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_StatEditorMain;
}

static void Task_StatEditorTurnOff(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sStatEditorDataPtr->savedCallback);
        StatEditor_FreeResources();
        DestroyTask(taskId);
    }
}

static void PrintTextOnWindowWithFont(u8 windowId, const u8 *string, u8 x, u8 y, u8 lineSpacing, u8 colorId, u32 fontId)
{
    AddTextPrinterParameterized4(windowId, fontId, x, y, 0, lineSpacing, sMenuWindowFontColors[colorId], TEXT_SKIP_DRAW , string);
}

static void PrintTextOnWindow(u8 windowId, const u8 *string, u8 x, u8 y, u8 lineSpacing, u8 colorId)
{
    PrintTextOnWindowWithFont(windowId, string, x, y, lineSpacing, colorId, FONT_SHORT_NARROW);
}

//
//       Stat Editor Code
//  End of UI setup code, beginning of stat editor specific code
//
static bool32 IsEditingBoxMon(void)
{
    return gSpecialVar_0x8004 == PC_MON_CHOSEN;
}

static struct BoxPokemon *GetCurrentBoxMon(void)
{
    if (IsEditingBoxMon())
        return GetBoxedMonPtr(gSpecialVar_MonBoxId, sStatEditorDataPtr->monIndex);
    return &gParties[B_TRAINER_PLAYER][sStatEditorDataPtr->monIndex].box;
}

static bool32 CanCycleBoxOrParty(void)
{
    if (!P_STAT_EDITOR_CYCLE_MODE)
        return FALSE;

    if (IsEditingBoxMon())
    {
        struct BoxPokemon *boxMons = GetBoxedMonPtr(gSpecialVar_MonBoxId, 0);

        return AdvanceStorageMonIndex(boxMons, sStatEditorDataPtr->monIndex, IN_BOX_COUNT - 1, 2) != -1 
            || AdvanceStorageMonIndex(boxMons, sStatEditorDataPtr->monIndex, IN_BOX_COUNT - 1, 0) != -1;
    }
    else
    {
        u32 count = gPartiesCount[B_TRAINER_PLAYER];
        u32 validMons = 0;

        for (u32 i = 0; i < count; i++)
        {
            if (!GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_IS_EGG))
                validMons++;
        }

        return validMons > 1;
    }
}

// Mon sprite data fields (copied from swsh_party_menu)
#define sSpecies    data[0]
#define sDontFlip   data[1]
#define sDelayAnim  data[2]
#define sIsShadow   data[3]
#define sIsEgg      data[4] // for passing into onFrame in PokemonSummaryDoMonAnimation

static void SpriteCB_StatEditorMonPokemon(struct Sprite *sprite)
{
    if (!gPaletteFade.active && sprite->sDelayAnim != 1)
    {
        sprite->sDontFlip = TRUE;

        if (!sStatEditorDataPtr->monAnimPlayed)
            PlayMonCry();

        PokemonSummaryDoMonAnimation(sprite, sprite->sSpecies, sprite->sIsEgg, sprite->sIsShadow);
        sStatEditorDataPtr->monAnimPlayed = TRUE;
    }
}

static u8 CreateStatEditorMonSprite(struct BoxPokemon *boxMon, bool32 isShadow)
{
    enum Species species = GetBoxMonData(boxMon, MON_DATA_SPECIES_OR_EGG);
    u8 shadowPalette = 0;
    u8 spriteId = CreateSprite(&gMultiuseSpriteTemplate, MON_ICON_X, MON_ICON_Y, 5);

    if (spriteId != MAX_SPRITES)
    {
        FreeSpriteOamMatrix(&gSprites[spriteId]);
        gSprites[spriteId].sSpecies = species;
        gSprites[spriteId].sDelayAnim = 0;
        gSprites[spriteId].sIsShadow = isShadow;
        gSprites[spriteId].sIsEgg = GetBoxMonData(boxMon, MON_DATA_IS_EGG);
        gSprites[spriteId].oam.priority = 0;
        if (isShadow)
        {
            gSprites[spriteId].subpriority = 2;
        }
        else
        {
            gSprites[spriteId].subpriority = 1;
        }
        gSprites[spriteId].callback = SpriteCB_StatEditorMonPokemon;
        if (P_STAT_EDITOR_MON_SHADOWS && isShadow)
        {
            FreeSpritePaletteByTag(TAG_MON_SHADOW);
            shadowPalette = LoadSpritePalette(&sSpritePal_MonShadow);
            gSprites[spriteId].oam.paletteNum = shadowPalette;
            gSprites[spriteId].oam.objMode = ST_OAM_OBJ_BLEND;
            gSprites[spriteId].x += 5;
            gSprites[spriteId].y += 2;
        }
    }

    return spriteId;
}

static bool32 HasAnyRelearnableMoves(enum MoveRelearnerStates state)
{
    struct BoxPokemon *boxMon = GetCurrentBoxMon();
    return CanBoxMonRelearnMoves(boxMon, state);
}

static void UpdateMoveRelearnerState(void)
{
    enum MoveRelearnerStates state;

    sStatEditorDataPtr->hasRelearnableMoves = FALSE;
    for (u32 i = 0; i < MOVE_RELEARNER_COUNT; i++)
    {
        state = (gMoveRelearnerState + i) % MOVE_RELEARNER_COUNT;
        if (HasAnyRelearnableMoves(state))
        {
            sStatEditorDataPtr->hasRelearnableMoves = TRUE;
            gMoveRelearnerState = state;
            break;
        }
    }
}

static inline bool32 ShouldShowMoveRelearner(void)
{
    return (P_STAT_EDITOR_MOVE_RELEARNER
         && sStatEditorDataPtr->hasRelearnableMoves
         && !InBattleFactory()
         && !InSlateportBattleTent());
}

static void PlayMonCry(void)
{
    struct BoxPokemon *boxMon = GetCurrentBoxMon();

    bool32 isEgg = GetBoxMonData(boxMon, MON_DATA_SANITY_IS_BAD_EGG) ? TRUE : GetBoxMonData(boxMon, MON_DATA_IS_EGG);

    if (!isEgg)
    {
        struct Pokemon mon;
        BoxMonToMon(boxMon, &mon);
        enum Species species = GetMonData(&mon, MON_DATA_SPECIES_OR_EGG);

        if (ShouldPlayNormalMonCry( &mon) == TRUE)
            PlayCry_ByMode(species, 0, CRY_MODE_NORMAL);
        else
            PlayCry_ByMode(species, 0, CRY_MODE_WEAK);
    }
}

#define HP_TYPE_BITS(boxMon) \
    (((GetBoxMonData(boxMon, MON_DATA_HP_IV)    & 1) << 0) \
   | ((GetBoxMonData(boxMon, MON_DATA_ATK_IV)   & 1) << 1) \
   | ((GetBoxMonData(boxMon, MON_DATA_DEF_IV)   & 1) << 2) \
   | ((GetBoxMonData(boxMon, MON_DATA_SPEED_IV) & 1) << 3) \
   | ((GetBoxMonData(boxMon, MON_DATA_SPATK_IV) & 1) << 4) \
   | ((GetBoxMonData(boxMon, MON_DATA_SPDEF_IV) & 1) << 5))

static enum Type GetBoxMonHiddenPowerTypeFromBits(u32 bits)
{
    enum Type hpTypes[NUMBER_OF_MON_TYPES];
    u32 hpTypeCount = 0;

    for (u32 i = 0; i < NUMBER_OF_MON_TYPES; i++)
    {
        if (gTypesInfo[i].isHiddenPowerType)
            hpTypes[hpTypeCount++] = i;
    }

    enum Type moveType = ((hpTypeCount - 1) * bits) / 63;
    return hpTypes[moveType];
}

static enum Type GetBoxMonHiddenPowerType(struct BoxPokemon *boxMon)
{
    u32 curBits = HP_TYPE_BITS(boxMon);
    return GetBoxMonHiddenPowerTypeFromBits(curBits);
}

static u32 GetBitDifferenceCount(u32 a, u32 b)
{
    u32 diff = a ^ b;
    u32 count = 0;

    while (diff)
    {
        count += diff & 1;
        diff >>= 1;
    }

    return count;
}

static u32 GetClosestHiddenPowerBits(u32 curBits, enum Type targetType)
{
    u32 bestBits = curBits;
    u32 bestCost = 7;

    for (u32 bits = 0; bits <= 63; bits++)
    {
        if (GetBoxMonHiddenPowerTypeFromBits(bits) != targetType)
            continue;

        u32 cost = GetBitDifferenceCount(curBits, bits);

        if (cost < bestCost)
        {
            bestCost = cost;
            bestBits = bits;
        }
    }

    return bestBits;
}

static void ApplyHiddenPowerBits(struct BoxPokemon *boxMon, u32 curBits, u32 bestBits)
{
    for (u32 i = STAT_HP; i < NUM_STATS; i++)
    {
        if (!((curBits ^ bestBits) & (1u << i)))
            continue;

        u32 iv = GetBoxMonData(boxMon, sIVStatsMap[i]);

        if (iv == 0)
            iv = 1;
        else if (iv == MAX_PER_STAT_IVS)
            iv = MAX_PER_STAT_IVS - 1;
        else if (iv & 1)
            iv--;
        else
            iv++;

        SetBoxMonData(boxMon, sIVStatsMap[i], &iv);
    }
}

static void SetBoxMonHiddenPowerType(struct BoxPokemon *boxMon, bool32 forward)
{
    u32 curBits = HP_TYPE_BITS(boxMon);
    enum Type targetType = GetBoxMonHiddenPowerTypeFromBits(curBits);

    do
    {
        targetType = forward ? (targetType + 1) : (targetType - 1);

        if (targetType > TYPE_DARK)
            targetType = TYPE_FIGHTING;
        if (targetType < TYPE_FIGHTING)
            targetType = TYPE_DARK;

    } while (targetType == TYPE_MYSTERY);

    u32 bestBits = GetClosestHiddenPowerBits(curBits, targetType);

    ApplyHiddenPowerBits(boxMon, curBits, bestBits);
}

static void UpdateHiddenPowerTypeIcon(void)
{
    struct BoxPokemon *boxMon = GetCurrentBoxMon();
    enum Type type = GetBoxMonHiddenPowerType(boxMon);
    u8 spriteId = sStatEditorDataPtr->typeSpriteId;
    struct Sprite *sprite = &gSprites[spriteId];

    StartSpriteAnim(sprite, type);
    sprite->oam.paletteNum = gTypesInfo[type].palette;
    sprite->x = TYPE_ICON_X;
    sprite->y = TYPE_ICON_Y;
    sprite->invisible = FALSE;
    sprite->subpriority = 1;
}

static void SetBoxMonTeraType(struct BoxPokemon *boxMon, bool32 forward)
{
    enum Type targetType = GetBoxMonData(boxMon, MON_DATA_TERA_TYPE);

    do
    {
        targetType = forward ? (targetType + 1) : (targetType - 1);

        if (targetType > TYPE_STELLAR)
            targetType = TYPE_NORMAL;
        if (targetType < TYPE_NORMAL)
            targetType = TYPE_STELLAR;

    } while (targetType == TYPE_MYSTERY);

    SetBoxMonData(boxMon, MON_DATA_TERA_TYPE, &targetType);
}

static void UpdateTeraTypeIcon(void)
{
    u8 spriteId = sStatEditorDataPtr->typeSpriteId;
    struct Sprite *sprite = &gSprites[spriteId];
    enum Type type = GetBoxMonData(GetCurrentBoxMon(), MON_DATA_TERA_TYPE);

    StartSpriteAnim(sprite, type);
    sprite->oam.paletteNum = gTypesInfo[type].palette;
    sprite->x = TYPE_ICON_X;
    sprite->y = TYPE_ICON_Y;
    sprite->invisible = FALSE;
    sprite->subpriority = 1;
}

static void CreateMonSprite(enum Species species)
{
    struct BoxPokemon *boxMon = GetCurrentBoxMon();
    u32 pid = GetBoxMonData(boxMon, MON_DATA_PERSONALITY);
    bool8 isShiny = GetBoxMonData(boxMon, MON_DATA_IS_SHINY);

    if (gMonSpritesGfxPtr == NULL)
        CreateMonSpritesGfxManager(MON_SPR_GFX_MANAGER_A, MON_SPR_GFX_MODE_NORMAL);

    HandleLoadSpecialPokePic(TRUE,
                              MonSpritesGfxManager_GetSpritePtr(MON_SPR_GFX_MANAGER_A, B_POSITION_OPPONENT_LEFT),
                              species,
                              pid);
    LoadSpritePaletteWithTag(GetMonSpritePalFromSpeciesAndPersonality(species, isShiny, pid), species);
    SetMultiuseSpriteTemplateToPokemon(species, B_POSITION_OPPONENT_LEFT);

    sStatEditorDataPtr->monSpriteId = CreateStatEditorMonSprite(boxMon, FALSE);
    if (P_STAT_EDITOR_MON_SHADOWS)
        sStatEditorDataPtr->monShadowSpriteId = CreateStatEditorMonSprite(boxMon, TRUE);
    else
        sStatEditorDataPtr->monShadowSpriteId = MAX_SPRITES;
    
    sStatEditorDataPtr->monAnimTimer = 0;
    sStatEditorDataPtr->monAnimPlayed = FALSE;
}

static void DestroyMonSprite(void)
{
    if (sStatEditorDataPtr->monSpriteId != MAX_SPRITES)
    {
        StopPokemonAnimationDelayTask();
        DestroySpriteAndFreeResources(&gSprites[sStatEditorDataPtr->monSpriteId]);
        sStatEditorDataPtr->monSpriteId = MAX_SPRITES;
    }
    if (sStatEditorDataPtr->monShadowSpriteId != MAX_SPRITES)
    {
        StopPokemonAnimationDelayTask();
        DestroySpriteAndFreeResources(&gSprites[sStatEditorDataPtr->monShadowSpriteId]);
        sStatEditorDataPtr->monShadowSpriteId = MAX_SPRITES;
    }
}

static void RunMonAnimTimer(void)
{
    u32 i;
    u8 monSpriteId = sStatEditorDataPtr->monSpriteId;
    u8 shadowSpriteId = sStatEditorDataPtr->monShadowSpriteId;

    if (monSpriteId != SPRITE_NONE && gSprites[monSpriteId].callback == SpriteCallbackDummy) // mon anim is finished
    {
        // Sanitize OAM bits to prevent the shared animation engine's flipping bug
        gSprites[monSpriteId].oam.matrixNum = (gSprites[monSpriteId].hFlip << 3) | (gSprites[monSpriteId].vFlip << 4);
        if (shadowSpriteId != SPRITE_NONE && shadowSpriteId != MAX_SPRITES)
            gSprites[shadowSpriteId].oam.matrixNum = (gSprites[shadowSpriteId].hFlip << 3) | (gSprites[shadowSpriteId].vFlip << 4);

        sStatEditorDataPtr->monAnimTimer++;
    }

    if (sStatEditorDataPtr->monAnimTimer > P_STAT_EDITOR_MON_IDLE_ANIMS_FRAMES && monSpriteId != SPRITE_NONE) // time to re-run the anim
    {
        struct BoxPokemon *boxMon = GetCurrentBoxMon();

        // Clear animation data for both sprites
        for (i = 1; i < 8; i++)
        {
            gSprites[monSpriteId].data[i] = 0;
            if (shadowSpriteId != SPRITE_NONE && shadowSpriteId != MAX_SPRITES)
                gSprites[shadowSpriteId].data[i] = 0;
        }

        // Restore species and shadow flags for both sprites
        gSprites[monSpriteId].sSpecies = GetBoxMonData(boxMon, MON_DATA_SPECIES_OR_EGG);
        gSprites[monSpriteId].sIsShadow = FALSE;
        gSprites[monSpriteId].sIsEgg = GetBoxMonData(boxMon, MON_DATA_IS_EGG);

        if (shadowSpriteId != SPRITE_NONE && shadowSpriteId != MAX_SPRITES)
        {
            gSprites[shadowSpriteId].sSpecies = GetBoxMonData(boxMon, MON_DATA_SPECIES_OR_EGG);
            gSprites[shadowSpriteId].sIsShadow = TRUE;
            gSprites[shadowSpriteId].sIsEgg = GetBoxMonData(boxMon, MON_DATA_IS_EGG);
        }

        // Restart animation for both sprites
        gSprites[monSpriteId].callback = SpriteCB_StatEditorMonPokemon;
        if (shadowSpriteId != SPRITE_NONE && shadowSpriteId != MAX_SPRITES)
            gSprites[shadowSpriteId].callback = SpriteCB_StatEditorMonPokemon;

        sStatEditorDataPtr->monAnimTimer = 0;
    }
}

#undef sSpecies
#undef sDontFlip
#undef sDelayAnim
#undef sIsShadow
#undef sIsEgg

static u8 CreateSelectors(void)
{
    if (sStatEditorDataPtr->leftSelectorSpriteId == 0xFF)
        sStatEditorDataPtr->leftSelectorSpriteId = CreateSprite(&sSpriteTemplate_LeftArrow,  SELECTOR_LEFT_EDGE_X, SELECTOR_LEFT_NICKNAME_Y, 0);

    if (sStatEditorDataPtr->rightSelectorSpriteId == 0xFF)
        sStatEditorDataPtr->rightSelectorSpriteId = CreateSprite(&sSpriteTemplate_RightArrow, SELECTOR_LEFT_EDGE_X + LEFT_PANEL_SELECTOR_WIDTH, SELECTOR_LEFT_NICKNAME_Y, 0);

    gSprites[sStatEditorDataPtr->leftSelectorSpriteId].invisible = FALSE;
    gSprites[sStatEditorDataPtr->rightSelectorSpriteId].invisible = FALSE;

    StartSpriteAnim(&gSprites[sStatEditorDataPtr->leftSelectorSpriteId],  SELECTOR_ANIM_IDLE);
    StartSpriteAnim(&gSprites[sStatEditorDataPtr->rightSelectorSpriteId], SELECTOR_ANIM_IDLE);
    return sStatEditorDataPtr->leftSelectorSpriteId;
}

static void DestroySelectors(void)
{
    if (sStatEditorDataPtr->leftSelectorSpriteId != 0xFF)
    {
        DestroySprite(&gSprites[sStatEditorDataPtr->leftSelectorSpriteId]);
        FreeSpriteTilesByTag(TAG_LEFT_ARROW);
    }
    if (sStatEditorDataPtr->rightSelectorSpriteId != 0xFF)
    {
        DestroySprite(&gSprites[sStatEditorDataPtr->rightSelectorSpriteId]);
        FreeSpriteTilesByTag(TAG_RIGHT_ARROW);
    }
    sStatEditorDataPtr->leftSelectorSpriteId  = 0xFF;
    sStatEditorDataPtr->rightSelectorSpriteId = 0xFF;
}

static void DestroyNatureArrows(void)
{
    if (sStatEditorDataPtr->natureUpSpriteId != MAX_SPRITES)
        DestroySprite(&gSprites[sStatEditorDataPtr->natureUpSpriteId]);
    if (sStatEditorDataPtr->natureDownSpriteId != MAX_SPRITES)
        DestroySprite(&gSprites[sStatEditorDataPtr->natureDownSpriteId]);
    FreeSpriteTilesByTag(TAG_NATURE_ARROWS);
    sStatEditorDataPtr->leftSelectorSpriteId  = MAX_SPRITES;
    sStatEditorDataPtr->rightSelectorSpriteId = MAX_SPRITES;
}

static void PrintTitleToWindowMainState(void)
{
    FillWindowPixelBuffer(WINDOW_MAIN_HEADER, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    if (CanCycleBoxOrParty())
    {
        BlitBitmapToWindow(WINDOW_MAIN_HEADER, sLR_ButtonGfx, 0, BUTTON_Y, 16, 8);
        if (IsEditingBoxMon())
            PrintTextOnWindowWithFont(WINDOW_MAIN_HEADER, sText_MenuLRButtonBox, 19, 0, 0, FONT_WHITE, FONT_NARROW);
        else
            PrintTextOnWindowWithFont(WINDOW_MAIN_HEADER, sText_MenuLRButtonParty, 19, 0, 0, FONT_WHITE, FONT_NARROW);
    }

    if (ShouldShowMoveRelearner())
    {
        BlitBitmapToWindow(WINDOW_MAIN_HEADER, sStart_ButtonGfx, 65, BUTTON_Y, 24, 8);
        PrintTextOnWindowWithFont(WINDOW_MAIN_HEADER, sText_MenuStartButtonMoves, 93, 0, 0, FONT_WHITE, FONT_NARROW);
    }
    PutWindowTilemap(WINDOW_MAIN_HEADER);
    CopyWindowToVram(WINDOW_MAIN_HEADER, COPYWIN_FULL);
}

void GetDPadText(void)
{
    if (sStatEditorDataPtr->panel == PANEL_LEFT)
    {
        BlitBitmapToWindow(WINDOW_MAIN_HEADER, sDPadLR_ButtonGfx, 0, BUTTON_Y, 16, 8);

        if (sStatEditorDataPtr->leftRow == LEFT_ROW_ABILITY)
            PrintTextOnWindowWithFont(WINDOW_MAIN_HEADER, sText_MenuDPadChangeAbility, 20, 0, 0, FONT_WHITE, FONT_NARROW);
        else if (sStatEditorDataPtr->leftRow == LEFT_ROW_NATURE)
            PrintTextOnWindowWithFont(WINDOW_MAIN_HEADER, sText_MenuDPadChangeNature, 20, 0, 0, FONT_WHITE, FONT_NARROW);
    }
    else if (sStatEditorDataPtr->panel == PANEL_RIGHT)
    {
        if (P_STAT_EDITOR_HP_OR_TERA != P_STAT_EDITOR_NONE && sStatEditorDataPtr->rightPanelRow == RIGHT_PANEL_ROW_HP_TYPE)
        {
            BlitBitmapToWindow(WINDOW_MAIN_HEADER, sDPadLR_ButtonGfx, 0, BUTTON_Y, 16, 8);
            if (P_STAT_EDITOR_HP_OR_TERA == P_STAT_EDITOR_HIDDEN_POWER)
                PrintTextOnWindowWithFont(WINDOW_MAIN_HEADER, sText_MenuDPadChangeHPType, 20, 0, 0, FONT_WHITE, FONT_NARROW);
            else if (P_STAT_EDITOR_HP_OR_TERA == P_STAT_EDITOR_TERASTAL)
                PrintTextOnWindowWithFont(WINDOW_MAIN_HEADER, sText_MenuDPadChangeTeraType, 20, 0, 0, FONT_WHITE, FONT_NARROW);
        }
        else
        {
            BlitBitmapToWindow(WINDOW_MAIN_HEADER, sDPad_ButtonGfx, 0, BUTTON_Y, 16, 8);
            PrintTextOnWindowWithFont(WINDOW_MAIN_HEADER, sText_MenuDPadChangeStat, 20, 0, 0, FONT_WHITE, FONT_NARROW);
        }
    }
}

static void PrintTitleToWindowEditState(void)
{
    FillWindowPixelBuffer(WINDOW_MAIN_HEADER, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    GetDPadText();
    BlitBitmapToWindow(WINDOW_MAIN_HEADER, sA_ButtonGfx, 75, BUTTON_Y, 8, 8);
    BlitBitmapToWindow(WINDOW_MAIN_HEADER, sB_ButtonGfx, 85, BUTTON_Y, 8, 8);
    PrintTextOnWindowWithFont(WINDOW_MAIN_HEADER, sText_MenuABButtonSave, 97, 0, 0, FONT_WHITE, FONT_NARROW);
    PutWindowTilemap(WINDOW_MAIN_HEADER);
    CopyWindowToVram(WINDOW_MAIN_HEADER, COPYWIN_FULL);
}

static void UpdateNatureArrowSprites(void)
{
    u32 nature = GetBoxMonData(GetCurrentBoxMon(), MON_DATA_HIDDEN_NATURE);
    u32 natureUpStat = gNaturesInfo[nature].statUp;
    u32 natureDownStat = gNaturesInfo[nature].statDown;
    struct Sprite *upSprite = &gSprites[sStatEditorDataPtr->natureUpSpriteId];
    struct Sprite *downSprite = &gSprites[sStatEditorDataPtr->natureDownSpriteId];

    StartSpriteAnim(upSprite,   1);
    StartSpriteAnim(downSprite, 0);

    if (natureUpStat == natureDownStat)
    {
        upSprite->invisible   = TRUE;
        downSprite->invisible = TRUE;
        return;
    }

    upSprite->x = NATURE_ARROW_X;
    upSprite->y = NATURE_ARROW_BASE_Y + (sStatsMap[natureUpStat] * STAT_ROW_HEIGHT);
    upSprite->invisible = FALSE;

    downSprite->x = NATURE_ARROW_X;
    downSprite->y = NATURE_ARROW_BASE_Y + (sStatsMap[natureDownStat] * STAT_ROW_HEIGHT);
    downSprite->invisible = FALSE;
}

static enum Ability GetBoxMonAbility(struct BoxPokemon *boxMon)
{
    enum Species species = GetBoxMonData(boxMon, MON_DATA_SPECIES_OR_EGG);
    u32 abilityNum = GetBoxMonData(boxMon, MON_DATA_ABILITY_NUM);
    return GetSpeciesAbility(species, abilityNum);
}

static void PrintMonStats(void)
{
    struct BoxPokemon *boxMon = GetCurrentBoxMon();
    struct Pokemon mon;
    BoxMonToMon(boxMon, &mon);

    u32 nature = GetBoxMonData(boxMon, MON_DATA_HIDDEN_NATURE);
    enum Ability ability = GetBoxMonAbility(boxMon);
    u32 currentStat, digits;
    s32 xPos;

    FillWindowPixelBuffer(WINDOW_STATS_HEADER, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WINDOW_STATS_PANEL, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WINDOW_NICKNAME, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WINDOW_ABILITIES, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WINDOW_NATURES, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    sStatEditorDataPtr->evTotal = 0;
    PrintTextOnWindow(WINDOW_STATS_HEADER, sText_MenuStat, 3,  0, 0, FONT_WHITE_BLACK);
    PrintTextOnWindow(WINDOW_STATS_HEADER, sText_MenuEV,   40, 0, 0, FONT_WHITE_BLACK);
    PrintTextOnWindow(WINDOW_STATS_HEADER, sText_MenuIV,   76, 0, 0, FONT_WHITE_BLACK);

    UpdateNatureArrowSprites();

    // Print Mon Stats
    for (u32 i = STAT_HP; i < NUM_STATS; i++)
    {
        currentStat = GetMonData(&mon, sActualStatsMap[i]);
        digits = P_STAT_EDITOR_CENTER_ALIGN_STATS ? (currentStat == 0 ? 1 : CountDigits(currentStat)) : 3;
        ConvertIntToDecimalStringN(gStringVar2, currentStat, STR_CONV_MODE_RIGHT_ALIGN, digits);
        xPos = GetStringCenterAlignXOffset(FONT_NORMAL, gStringVar2, 18) + sStatPrintData[sActualStatsMap[i]].x;
        PrintTextOnWindowWithFont(WINDOW_STATS_PANEL, gStringVar2, xPos, sStatPrintData[sActualStatsMap[i]].y, 0, FONT_BLACK, FONT_NORMAL);

        // EVs
        currentStat = GetBoxMonData(boxMon, sEVStatsMap[i]);
        digits = P_STAT_EDITOR_CENTER_ALIGN_STATS ? (currentStat == 0 ? 1 : CountDigits(currentStat)) : 3;
        sStatEditorDataPtr->evTotal += currentStat;
        ConvertIntToDecimalStringN(gStringVar2, currentStat, STR_CONV_MODE_RIGHT_ALIGN, digits);
        xPos = GetStringCenterAlignXOffset(FONT_NORMAL, gStringVar2, 18) + sStatPrintData[sEVStatsMap[i]].x;
        PrintTextOnWindowWithFont(WINDOW_STATS_PANEL, gStringVar2, xPos, sStatPrintData[sEVStatsMap[i]].y, 0, FONT_BLACK, FONT_NORMAL);

        // IVs
        currentStat = GetBoxMonData(boxMon, sIVStatsMap[i]);
        digits = P_STAT_EDITOR_CENTER_ALIGN_STATS ? (currentStat == 0 ? 1 : CountDigits(currentStat)) : 3;
        ConvertIntToDecimalStringN(gStringVar2, currentStat, STR_CONV_MODE_RIGHT_ALIGN, digits);
        xPos = GetStringCenterAlignXOffset(FONT_NORMAL, gStringVar2, 18) + sStatPrintData[sIVStatsMap[i]].x;
        PrintTextOnWindowWithFont(WINDOW_STATS_PANEL, gStringVar2, xPos, sStatPrintData[sIVStatsMap[i]].y, 0, FONT_BLACK, FONT_NORMAL);
    }

    digits = P_STAT_EDITOR_CENTER_ALIGN_STATS ? (currentStat == 0 ? 1 : CountDigits(currentStat)) : 3;
    ConvertIntToDecimalStringN(gStringVar2, sStatEditorDataPtr->evTotal, STR_CONV_MODE_RIGHT_ALIGN, digits);
    PrintTextOnWindowWithFont(WINDOW_STATS_PANEL, gStringVar2, STARTING_X + SECOND_COLUMN + 2, STARTING_Y + (STAT_ROW_HEIGHT * 6), 0, FONT_BLACK, FONT_NORMAL);

    if (P_STAT_EDITOR_HP_OR_TERA == P_STAT_EDITOR_HIDDEN_POWER)
        UpdateHiddenPowerTypeIcon();
    else if (P_STAT_EDITOR_HP_OR_TERA == P_STAT_EDITOR_TERASTAL)
        UpdateTeraTypeIcon();

    // Print ability / nature / name
    GetBoxMonNickname(boxMon, gStringVar2);
    xPos = P_STAT_EDITOR_CENTER_ALIGN_TEXT ? GetStringCenterAlignXOffset(FONT_SHORT_NARROW, gStringVar2, WindowWidthPx(WINDOW_NICKNAME)) + 1 : 12;
    PrintTextOnWindow(WINDOW_NICKNAME, gStringVar2, xPos, 4, 0, FONT_WHITE);

    StringCopy(gStringVar2, gAbilitiesInfo[ability].name);
    PrintTextOnWindowWithFont(WINDOW_ABILITIES, sText_MenuAbility, 3, 2, 0, FONT_BLACK, FONT_SMALL_NARROWER);
    u32 fontId = GetFontIdToFit(gStringVar2, FONT_SHORT_NARROW, 0, WindowWidthPx(WINDOW_ABILITIES) - 48);
    xPos = P_STAT_EDITOR_CENTER_ALIGN_TEXT ? GetStringCenterAlignXOffset(fontId, gStringVar2, WindowWidthPx(WINDOW_ABILITIES) + 28) : 32;
    PrintTextOnWindowWithFont(WINDOW_ABILITIES, gStringVar2, xPos, 2, 0, FONT_BLACK, fontId);

    StringCopy(gStringVar2, gNaturesInfo[nature].name);
    PrintTextOnWindowWithFont(WINDOW_NATURES, sText_MenuNature, 2, 2, 0, FONT_BLACK, FONT_SMALL_NARROWER);
    xPos = P_STAT_EDITOR_CENTER_ALIGN_TEXT ? GetStringCenterAlignXOffset(FONT_SHORT_NARROW, gStringVar2, WindowWidthPx(WINDOW_NATURES) + 28) : 32;
    PrintTextOnWindow(WINDOW_NATURES, gStringVar2, xPos, 2, 0, FONT_BLACK);

    PutWindowTilemap(WINDOW_STATS_HEADER);
    CopyWindowToVram(WINDOW_STATS_HEADER, COPYWIN_FULL);
    PutWindowTilemap(WINDOW_STATS_PANEL);
    CopyWindowToVram(WINDOW_STATS_PANEL, COPYWIN_FULL);
    PutWindowTilemap(WINDOW_NICKNAME);
    CopyWindowToVram(WINDOW_NICKNAME, COPYWIN_FULL);
    PutWindowTilemap(WINDOW_ABILITIES);
    CopyWindowToVram(WINDOW_ABILITIES, COPYWIN_FULL);
    PutWindowTilemap(WINDOW_NATURES);
    CopyWindowToVram(WINDOW_NATURES, COPYWIN_FULL);
}

struct SpriteCoords
{
    u8 x;
    u8 y;
};

static void SelectorCallback(struct Sprite *sprite)
{
    static const struct SpriteCoords sRightPanelCoords[RIGHT_PANEL_ROW_COUNT][2] = {
        {{SELECTOR_RIGHT_EV_LEFT_EDGE_X, SELECTOR_RIGHT_BASE_Y + (STAT_ROW_HEIGHT * 0)}, {SELECTOR_RIGHT_IV_LEFT_EDGE_X, SELECTOR_RIGHT_BASE_Y + (STAT_ROW_HEIGHT * 0)}},
        {{SELECTOR_RIGHT_EV_LEFT_EDGE_X, SELECTOR_RIGHT_BASE_Y + (STAT_ROW_HEIGHT * 1)}, {SELECTOR_RIGHT_IV_LEFT_EDGE_X, SELECTOR_RIGHT_BASE_Y + (STAT_ROW_HEIGHT * 1)}},
        {{SELECTOR_RIGHT_EV_LEFT_EDGE_X, SELECTOR_RIGHT_BASE_Y + (STAT_ROW_HEIGHT * 2)}, {SELECTOR_RIGHT_IV_LEFT_EDGE_X, SELECTOR_RIGHT_BASE_Y + (STAT_ROW_HEIGHT * 2)}},
        {{SELECTOR_RIGHT_EV_LEFT_EDGE_X, SELECTOR_RIGHT_BASE_Y + (STAT_ROW_HEIGHT * 3)}, {SELECTOR_RIGHT_IV_LEFT_EDGE_X, SELECTOR_RIGHT_BASE_Y + (STAT_ROW_HEIGHT * 3)}},
        {{SELECTOR_RIGHT_EV_LEFT_EDGE_X, SELECTOR_RIGHT_BASE_Y + (STAT_ROW_HEIGHT * 4)}, {SELECTOR_RIGHT_IV_LEFT_EDGE_X, SELECTOR_RIGHT_BASE_Y + (STAT_ROW_HEIGHT * 4)}},
        {{SELECTOR_RIGHT_EV_LEFT_EDGE_X, SELECTOR_RIGHT_BASE_Y + (STAT_ROW_HEIGHT * 5)}, {SELECTOR_RIGHT_IV_LEFT_EDGE_X, SELECTOR_RIGHT_BASE_Y + (STAT_ROW_HEIGHT * 5)}},
#if P_STAT_EDITOR_HP_OR_TERA != P_STAT_EDITOR_NONE
        {{SELECTOR_RIGHT_HP_TYPE_LEFT_EDGE_X, SELECTOR_RIGHT_HP_TYPE_Y},                 {SELECTOR_RIGHT_HP_TYPE_LEFT_EDGE_X, SELECTOR_RIGHT_HP_TYPE_Y}},
#endif // P_STAT_EDITOR_HP_OR_TERA
    };

    static const u8 sLeftPanelY[LEFT_ROW_COUNT] = {
        [LEFT_ROW_NICKNAME] = SELECTOR_LEFT_NICKNAME_Y,
        [LEFT_ROW_ABILITY]  = SELECTOR_LEFT_ABILITY_Y,
        [LEFT_ROW_NATURE]   = SELECTOR_LEFT_NATURE_Y,
    };

    bool32 isLeft = (sprite == &gSprites[sStatEditorDataPtr->leftSelectorSpriteId]);

    // Position
    if (sStatEditorDataPtr->panel == PANEL_LEFT)
    {
        u32 leftEdgeX;
        u32 rightEdgeX;

        if (sStatEditorDataPtr->leftRow == LEFT_ROW_NICKNAME)
        {
            leftEdgeX = SELECTOR_LEFT_EDGE_X;
            rightEdgeX = leftEdgeX + LEFT_PANEL_SELECTOR_WIDTH;
        }
        else
        {
            leftEdgeX = SELECTOR_LEFT_EDGE_X + 29;
            rightEdgeX = leftEdgeX + LEFT_PANEL_SELECTOR_WIDTH - 1;
        }

        sprite->x = isLeft ? leftEdgeX : rightEdgeX;
        sprite->y = sLeftPanelY[sStatEditorDataPtr->leftRow];
    }
    else
    {
        if (sStatEditorDataPtr->rightPanelRow != RIGHT_PANEL_ROW_HP_TYPE)
            sStatEditorDataPtr->rightPanelSelectedStat = sStatEditorDataPtr->rightPanelColumn + (sStatEditorDataPtr->rightPanelRow * 2);

        u32 leftEdgeX = sRightPanelCoords[sStatEditorDataPtr->rightPanelRow][sStatEditorDataPtr->rightPanelColumn].x;
        u32 rightEdgeX;

        if (P_STAT_EDITOR_HP_OR_TERA != P_STAT_EDITOR_NONE && sStatEditorDataPtr->rightPanelRow == RIGHT_PANEL_ROW_HP_TYPE)
            rightEdgeX = leftEdgeX + RIGHT_PANEL_HP_TYPE_SELECTOR_WIDTH;
        else
            rightEdgeX = leftEdgeX + RIGHT_PANEL_SELECTOR_WIDTH;

        sprite->x = isLeft ? leftEdgeX : rightEdgeX;
        sprite->y = sRightPanelCoords[sStatEditorDataPtr->rightPanelRow][sStatEditorDataPtr->rightPanelColumn].y;
    }

    // Visibility: hide left when at min, right when at max (edit mode, stats only)
    if (sStatEditorDataPtr->panelInputMode == PANEL_INPUT_EDIT
     && sStatEditorDataPtr->panel == PANEL_RIGHT
     && sStatEditorDataPtr->rightPanelRow != RIGHT_PANEL_ROW_HP_TYPE)
    {
        if (isLeft && sStatEditorDataPtr->statEditingValue == MIN_STAT)
        {
            sprite->invisible = TRUE;
            return;
        }
        if (!isLeft && CHECK_IF_STAT_CANT_INCREASE)
        {
            sprite->invisible = TRUE;
            return;
        }
    }

    sprite->invisible = FALSE;

    // Animation
    if (sStatEditorDataPtr->panelInputMode == PANEL_INPUT_SELECT)
    {
        if (sprite->animNum != SELECTOR_ANIM_IDLE)
            StartSpriteAnim(sprite, SELECTOR_ANIM_IDLE);
    }
    else
    {
        bool32 pressingIncrease = (sStatEditorDataPtr->pressingArrow == PRESSING_RIGHT);
        bool32 pressingDecrease = (sStatEditorDataPtr->pressingArrow == PRESSING_LEFT);
        bool32 pressing = isLeft ? pressingDecrease : pressingIncrease;

        if (pressing)
        {
            if (sprite->animNum != SELECTOR_ANIM_ACTIVE)
                StartSpriteAnim(sprite, SELECTOR_ANIM_ACTIVE);
        }
        else
        {
            u8 cycleFrame = (sStatEditorDataPtr->selectorCycleTimer / 12) % 3;

            if (sprite->animNum != SELECTOR_ANIM_CYCLE)
                StartSpriteAnim(sprite, SELECTOR_ANIM_CYCLE);

            sprite->animCmdIndex     = cycleFrame;
            sprite->animDelayCounter = sStatEditorDataPtr->selectorCycleTimer % 12;
        }
    }
}

static void Task_DelayedSpriteLoad(u8 taskId) // wait 4 frames after changing the mon you're editing so there are no palette problems
{
    if (gTasks[taskId].data[11] >= 4)
    {
        CreateMonSprite(sStatEditorDataPtr->speciesID);
        PrintMonStats();
        gTasks[taskId].func = Task_StatEditorMain;
    }
    else
    {
        gTasks[taskId].data[11]++;
    }
}

static void ReloadNewPokemon(u8 taskId)
{
    StopCryAndClearCrySongs();

    if (sStatEditorDataPtr->monSpriteId != MAX_SPRITES)
        gSprites[sStatEditorDataPtr->monSpriteId].invisible = TRUE;

    if (sStatEditorDataPtr->monShadowSpriteId != MAX_SPRITES)
        gSprites[sStatEditorDataPtr->monShadowSpriteId].invisible = TRUE;

    DestroyMonSprite();
    sStatEditorDataPtr->speciesID = GetBoxMonData(GetCurrentBoxMon(), MON_DATA_SPECIES_OR_EGG);
    SetMonPosVars();
    UpdateMoveRelearnerState();
    PrintTitleToWindowMainState();
    gTasks[taskId].func = Task_DelayedSpriteLoad;
    gTasks[taskId].data[11] = 0;
}

static u32 GetValidAbilitySlots(enum Species species, u8 outSlots[])
{
    u32 count = 0;
    enum Ability ab0 = GetSpeciesAbility(species, 0);
    enum Ability ab1 = GetSpeciesAbility(species, 1);
    enum Ability ab2 = GetSpeciesAbility(species, 2);

    outSlots[count++] = 0;

    if (ab1 != ABILITY_NONE && ab1 != ab0)
        outSlots[count++] = 1;

    if (ab2 != ABILITY_NONE && ab2 != ab0)
        outSlots[count++] = 2;

    return count;
}

static bool32 HasOnlyOneAbility(enum Species species)
{
    u8 outSlots[NUM_ABILITY_SLOTS];

    return GetValidAbilitySlots(species, outSlots) == 1;
}

static void CB2_ReturnToSummaryScreenFromNamingScreen(void)
{
    ShowPokemonSummaryScreen(SUMMARY_MODE_STAT_EDITOR, gParties[B_TRAINER_PLAYER], gSpecialVar_0x8004, gPartiesCount[B_TRAINER_PLAYER] - 1, gInitialSummaryScreenCallback);
}

static void CB2_ReturnToStatEditorFromNamingScreen(void)
{
    SetBoxMonData(GetSelectedBoxMonFromPcOrParty(), MON_DATA_NICKNAME, gStringVar2);

    if (P_PARTY_MENU_STAT_EDITOR)
        StatEditor_Init(CB2_ReturnToPartyMenuFromSummaryScreen);
    else if (P_SUMMARY_SCREEN_STAT_EDITOR)
        StatEditor_Init(CB2_ReturnToSummaryScreenFromNamingScreen);
}

static void CB2_StatEditorChangePokemonNickname(void)
{
    ChangePokemonNicknameWithCallback(CB2_ReturnToStatEditorFromNamingScreen);
}

static void SetMonPosVars(void)
{
    if (IsEditingBoxMon())
    {
        gSpecialVar_MonBoxPos = sStatEditorDataPtr->monIndex;
        gSpecialVar_MonBoxId = StorageGetCurrentBox();
    }
    else
    {
        gSpecialVar_0x8004 = sStatEditorDataPtr->monIndex;
    }
}

static void HandleLeftPanelValue(bool32 forward)
{
    struct BoxPokemon *boxMon = GetCurrentBoxMon();
    u32 val;

    switch (sStatEditorDataPtr->leftRow)
    {
    case LEFT_ROW_NICKNAME:
        break;

    case LEFT_ROW_ABILITY:
    {
        u8 slots[NUM_ABILITY_SLOTS];
        u32 count = GetValidAbilitySlots(sStatEditorDataPtr->speciesID, slots);

        if (count <= 1)
            return;

        u32 current = GetBoxMonData(boxMon, MON_DATA_ABILITY_NUM);
        u32 pos = 0;

        for (u32 i = 0; i < count; i++)
        {
            if (slots[i] == current)
            {
                pos = i;
                break;
            }
        }

        pos = forward ? ((pos + 1) % count) : (pos == 0 ? count - 1 : pos - 1);

        val = slots[pos];
        SetBoxMonData(boxMon, MON_DATA_ABILITY_NUM, &val);
        PrintMonStats();
        break;
    }

    case LEFT_ROW_NATURE:
        val = GetBoxMonData(boxMon, MON_DATA_HIDDEN_NATURE);

        val = forward ? ((val + 1) % NUM_NATURES) : ((val == 0 ? NUM_NATURES - 1 : val - 1));

        SetBoxMonData(boxMon, MON_DATA_HIDDEN_NATURE, &val);
        CalculateBoxMonStats(boxMon);
        PrintMonStats();
        break;
    }
}

static void Task_LeftPanelEditMode(u8 taskId)
{
    if (JOY_NEW(B_BUTTON) || JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].func = Task_StatEditorMain;
        PlaySE(SE_SELECT);
        sStatEditorDataPtr->panelInputMode = PANEL_INPUT_SELECT;
        sStatEditorDataPtr->pressingArrow = PRESSING_NONE;
        PrintTitleToWindowMainState();
        return;
    }

    switch (sStatEditorDataPtr->leftRow)
    {
    case LEFT_ROW_NICKNAME:
        PlaySE(SE_SELECT);
        sStatEditorDataPtr->savedCallback = CB2_StatEditorChangePokemonNickname;
        SetMonPosVars();
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_StatEditorTurnOff;
        break;

    case LEFT_ROW_ABILITY:
    case LEFT_ROW_NATURE:
        if (JOY_REPEAT(DPAD_RIGHT))
        {
            sStatEditorDataPtr->pressingArrow = PRESSING_RIGHT;
            PlaySE(SE_SELECT);
            HandleLeftPanelValue(TRUE);
        }
        else if (JOY_REPEAT(DPAD_LEFT))
        {
            sStatEditorDataPtr->pressingArrow = PRESSING_LEFT;
            PlaySE(SE_SELECT);
            HandleLeftPanelValue(FALSE);
        }
        else
        {
            sStatEditorDataPtr->pressingArrow = PRESSING_NONE;
        }
        break;
    }
}

static void Task_HPTypeEditMode(u8 taskId)
{
    if (JOY_NEW(B_BUTTON) || JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].func = Task_StatEditorMain;
        PlaySE(SE_SELECT);
        sStatEditorDataPtr->panelInputMode = PANEL_INPUT_SELECT;
        sStatEditorDataPtr->pressingArrow = PRESSING_NONE;
        PrintTitleToWindowMainState();
        return;
    }

    struct BoxPokemon *boxMon = GetCurrentBoxMon();
    u32 oldMaxHP  = GetBoxMonData(boxMon, MON_DATA_MAX_HP);
    u32 currentHP = GetBoxMonData(boxMon, MON_DATA_HP);
    s32 hpLost    = oldMaxHP - currentHP;

    if (JOY_REPEAT(DPAD_RIGHT))
    {
        sStatEditorDataPtr->pressingArrow = PRESSING_RIGHT;
        PlaySE(SE_SELECT);
        SetBoxMonHiddenPowerType(boxMon, TRUE);
        CalculateBoxMonStats(boxMon);
    }
    else if (JOY_REPEAT(DPAD_LEFT))
    {
        sStatEditorDataPtr->pressingArrow = PRESSING_LEFT;
        PlaySE(SE_SELECT);
        SetBoxMonHiddenPowerType(boxMon, FALSE);
        CalculateBoxMonStats(boxMon);
    }
    else
    {
        sStatEditorDataPtr->pressingArrow = PRESSING_NONE;
        return;
    }

    if (hpLost > 0 && currentHP != 0)
    {
        s32 diff = GetBoxMonData(boxMon, MON_DATA_MAX_HP) - hpLost;

        if (diff < 0)
            diff = 0;

        SetBoxMonData(boxMon, MON_DATA_HP, &diff);
    }

    PrintMonStats();
}

static void Task_TeraTypeEditMode(u8 taskId)
{
    if (JOY_NEW(B_BUTTON) || JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].func = Task_StatEditorMain;
        PlaySE(SE_SELECT);
        sStatEditorDataPtr->panelInputMode = PANEL_INPUT_SELECT;
        sStatEditorDataPtr->pressingArrow = PRESSING_NONE;
        PrintTitleToWindowMainState();
        return;
    }

    struct BoxPokemon *boxMon = GetCurrentBoxMon();

    if (JOY_REPEAT(DPAD_RIGHT))
    {
        sStatEditorDataPtr->pressingArrow = PRESSING_RIGHT;
        PlaySE(SE_SELECT);
        SetBoxMonTeraType(boxMon, TRUE);
        CalculateBoxMonStats(boxMon);
    }
    else if (JOY_REPEAT(DPAD_LEFT))
    {
        sStatEditorDataPtr->pressingArrow = PRESSING_LEFT;
        PlaySE(SE_SELECT);
        SetBoxMonTeraType(boxMon, FALSE);
        CalculateBoxMonStats(boxMon);
    }
    else
    {
        sStatEditorDataPtr->pressingArrow = PRESSING_NONE;
        return;
    }

    PrintMonStats();
}

static const u16 sSelectedStatToStatEnum[] = {
    MON_DATA_HP_EV,    MON_DATA_HP_IV,
    MON_DATA_ATK_EV,   MON_DATA_ATK_IV,
    MON_DATA_DEF_EV,   MON_DATA_DEF_IV,
    MON_DATA_SPATK_EV, MON_DATA_SPATK_IV,
    MON_DATA_SPDEF_EV, MON_DATA_SPDEF_IV,
    MON_DATA_SPEED_EV, MON_DATA_SPEED_IV,
};

static void ApplyRightPanelStatChange(void)
{
    struct BoxPokemon *boxMon = GetCurrentBoxMon();
    u32 currentStatEnum = sSelectedStatToStatEnum[sStatEditorDataPtr->rightPanelSelectedStat];
    s32 amountHPLost = 0;
    u32 currentHP    = 0;

    if (currentStatEnum == MON_DATA_HP_EV || currentStatEnum == MON_DATA_HP_IV)
    {
        u32 oldMaxHP = GetBoxMonData(boxMon, MON_DATA_MAX_HP);
        currentHP    = GetBoxMonData(boxMon, MON_DATA_HP);
        amountHPLost = oldMaxHP - currentHP;
    }

    SetBoxMonData(boxMon, currentStatEnum, &(sStatEditorDataPtr->statEditingValue));
    CalculateBoxMonStats(boxMon);

    if ((amountHPLost > 0) && (currentHP != 0))
    {
        s32 diff = GetBoxMonData(boxMon, MON_DATA_MAX_HP) - amountHPLost;

        if (diff < 0)
            diff = 0;

        SetBoxMonData(boxMon, MON_DATA_HP, &diff);
    }

    PrintMonStats();
}

static void HandleRightPanelEditInput(u32 input)
{
    if ((input <= EDIT_INPUT_INCREASE_MAX) && CHECK_IF_STAT_CANT_INCREASE)
        return;

    if ((input >= EDIT_INPUT_DECREASE) && (sStatEditorDataPtr->statEditingValue == MIN_STAT))
        return;

    switch (input)
    {
    case EDIT_INPUT_INCREASE:
        if (!CHECK_IF_STAT_CANT_INCREASE)
            sStatEditorDataPtr->statEditingValue++;
        break;
    case EDIT_INPUT_INCREASE_BY_10:
        if (sStatEditorDataPtr->rightPanelColumn == RIGHT_PANEL_EVS)
        {
            u32 remaining = MAX_TOTAL_EVS - sStatEditorDataPtr->evTotal;
            sStatEditorDataPtr->statEditingValue += (remaining < 10) ? remaining : 10;
            if (sStatEditorDataPtr->statEditingValue > MAX_PER_STAT_EVS)
                sStatEditorDataPtr->statEditingValue = MAX_PER_STAT_EVS;
        }
        else
        {
            sStatEditorDataPtr->statEditingValue += 10;
            if (sStatEditorDataPtr->statEditingValue > MAX_PER_STAT_IVS)
                sStatEditorDataPtr->statEditingValue = MAX_PER_STAT_IVS;
        }
        break;
    case EDIT_INPUT_INCREASE_MAX:
        if (sStatEditorDataPtr->rightPanelColumn == RIGHT_PANEL_EVS)
        {
            u32 remaining = MAX_TOTAL_EVS - sStatEditorDataPtr->evTotal;
            sStatEditorDataPtr->statEditingValue += (remaining < MAX_PER_STAT_EVS) ? remaining : MAX_PER_STAT_EVS;
            if (sStatEditorDataPtr->statEditingValue > MAX_PER_STAT_EVS)
                sStatEditorDataPtr->statEditingValue = MAX_PER_STAT_EVS;
        }
        else
        {
            sStatEditorDataPtr->statEditingValue = MAX_PER_STAT_IVS;
        }
        break;
    case EDIT_INPUT_DECREASE:
        if (sStatEditorDataPtr->statEditingValue != MIN_STAT)
            sStatEditorDataPtr->statEditingValue--;
        break;
    case EDIT_INPUT_DECREASE_BY_10:
        if (sStatEditorDataPtr->statEditingValue != MIN_STAT)
        {
            if (sStatEditorDataPtr->statEditingValue > 10)
                sStatEditorDataPtr->statEditingValue -= 10;
            else
                sStatEditorDataPtr->statEditingValue = MIN_STAT;
        }
        break;
    case EDIT_INPUT_DECREASE_MAX:
        sStatEditorDataPtr->statEditingValue = MIN_STAT;
        break;
    }

    ApplyRightPanelStatChange();
}

static void Task_RightPanelEditMode(u8 taskId)
{
    if (JOY_NEW(B_BUTTON) || JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].func = Task_StatEditorMain;
        PlaySE(SE_SELECT);
        sStatEditorDataPtr->panelInputMode = PANEL_INPUT_SELECT;
        sStatEditorDataPtr->pressingArrow = PRESSING_NONE;
        PrintTitleToWindowMainState();
        return;
    }

    if (JOY_REPEAT(DPAD_LEFT))
    {
        sStatEditorDataPtr->pressingArrow = PRESSING_LEFT;
        HandleRightPanelEditInput(EDIT_INPUT_DECREASE);
    }
    else if (JOY_REPEAT(DPAD_RIGHT))
    {
        sStatEditorDataPtr->pressingArrow = PRESSING_RIGHT;
        HandleRightPanelEditInput(EDIT_INPUT_INCREASE);
    }
    else if (JOY_REPEAT(DPAD_UP))
    {
        sStatEditorDataPtr->pressingArrow = PRESSING_RIGHT;
        HandleRightPanelEditInput(EDIT_INPUT_INCREASE_BY_10);
    }
    else if (JOY_REPEAT(DPAD_DOWN))
    {
        sStatEditorDataPtr->pressingArrow = PRESSING_LEFT;
        HandleRightPanelEditInput(EDIT_INPUT_DECREASE_BY_10);
    }
    else if (JOY_NEW(R_BUTTON))
    {
        sStatEditorDataPtr->pressingArrow = PRESSING_RIGHT;
        HandleRightPanelEditInput(EDIT_INPUT_INCREASE_MAX);
    }
    else if (JOY_NEW(L_BUTTON))
    {
        sStatEditorDataPtr->pressingArrow = PRESSING_LEFT;
        HandleRightPanelEditInput(EDIT_INPUT_DECREASE_MAX);
    }
    else
    {
        sStatEditorDataPtr->pressingArrow = PRESSING_NONE;
    }
}

static void Task_ChangeStatEditorPokemon(u8 taskId, s32 delta)
{
    if (IsEditingBoxMon())
    {
        u8 mode = (delta == 1) ? 0 : 2;
        s16 newPos = AdvanceStorageMonIndex(GetBoxedMonPtr(gSpecialVar_MonBoxId, 0), sStatEditorDataPtr->monIndex, IN_BOX_COUNT - 1, mode);
        if (newPos != -1)
        {
            sStatEditorDataPtr->monIndex = newPos;
            PlaySE(SE_SELECT);
            ReloadNewPokemon(taskId);
        }
        else
        {
            PlaySE(SE_FAILURE);
        }
    }
    else
    {
        u32 count = gPartiesCount[B_TRAINER_PLAYER];
        s16 newPos = -1;
        if (count > 1)
        {
            u32 validMons = 0;
            for (u32 i = 0; i < count; i++)
            {
                if (!GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_IS_EGG))
                    validMons++;
            }
            if (validMons > 1)
            {
                newPos = sStatEditorDataPtr->monIndex;

                do
                {
                    newPos = (newPos + delta + count) % count;
                } while (GetMonData(&gParties[B_TRAINER_PLAYER][newPos], MON_DATA_IS_EGG));
            }
        }
        if (newPos != -1)
        {
            sStatEditorDataPtr->monIndex = newPos;
            PlaySE(SE_SELECT);
            ReloadNewPokemon(taskId);
        }
        else
        {
            PlaySE(SE_FAILURE);
        }
    }
}

static void Task_StatEditorMain(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_PC_OFF);
        gLastViewedMonIndex = sStatEditorDataPtr->monIndex;
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_StatEditorTurnOff;
        return;
    }

    if (CanCycleBoxOrParty())
    {
        if (JOY_NEW(L_BUTTON))
        {
            Task_ChangeStatEditorPokemon(taskId, -1);
            return;
        }

        if (JOY_NEW(R_BUTTON))
        {
            Task_ChangeStatEditorPokemon(taskId, 1);
            return;
        }
    }

    if (JOY_NEW(START_BUTTON) && ShouldShowMoveRelearner())
    {
        PlaySE(SE_SELECT);
        gRelearnMode = RELEARN_MODE_STAT_EDITOR;
        sStatEditorDataPtr->savedCallback = CB2_InitLearnMove;
        SetMonPosVars();
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_StatEditorTurnOff;
        return;
    }

    if (sStatEditorDataPtr->panel == PANEL_LEFT)
    {
        if (JOY_NEW(DPAD_UP))
        {
            PlaySE(SE_SELECT);
            sStatEditorDataPtr->leftRow = (sStatEditorDataPtr->leftRow == 0) ? (LEFT_ROW_COUNT - 1) : (sStatEditorDataPtr->leftRow - 1);
            PrintMonStats();
            return;
        }

        if (JOY_NEW(DPAD_DOWN))
        {
            PlaySE(SE_SELECT);
            sStatEditorDataPtr->leftRow = (sStatEditorDataPtr->leftRow + 1) % LEFT_ROW_COUNT;
            PrintMonStats();
            return;
        }

        if (JOY_NEW(DPAD_LEFT))
        {
            PlaySE(SE_SELECT);
            sStatEditorDataPtr->panel = PANEL_RIGHT;
            sStatEditorDataPtr->rightPanelColumn = RIGHT_PANEL_IVS;
            sStatEditorDataPtr->rightPanelRow = 0;
            PrintMonStats();
            return;
        }

        if (JOY_NEW(DPAD_RIGHT))
        {
            PlaySE(SE_SELECT);
            sStatEditorDataPtr->panel = PANEL_RIGHT;
            sStatEditorDataPtr->rightPanelColumn = RIGHT_PANEL_EVS;
            sStatEditorDataPtr->rightPanelRow = 0;
            PrintMonStats();
            return;
        }

        if (JOY_NEW(A_BUTTON))
        {
            if (sStatEditorDataPtr->leftRow == LEFT_ROW_ABILITY
             && HasOnlyOneAbility(sStatEditorDataPtr->speciesID) == TRUE)
            {
                PlaySE(SE_FAILURE);
                sStatEditorDataPtr->panelInputMode = PANEL_INPUT_SELECT;
                return;
            }

            PlaySE(SE_SELECT);
            if (sStatEditorDataPtr->leftRow != LEFT_ROW_NICKNAME) // prevent title window from changing
            {
                sStatEditorDataPtr->panelInputMode = PANEL_INPUT_EDIT;
                PrintTitleToWindowEditState();
            }
            gTasks[taskId].func = Task_LeftPanelEditMode;
            return;
        }
    }
    else if (sStatEditorDataPtr->panel == PANEL_RIGHT)
    {
        if (JOY_NEW(A_BUTTON))
        {
            if (P_STAT_EDITOR_HP_OR_TERA != P_STAT_EDITOR_NONE && sStatEditorDataPtr->rightPanelRow == RIGHT_PANEL_ROW_HP_TYPE)
            {
                PlaySE(SE_SELECT);
                sStatEditorDataPtr->panelInputMode = PANEL_INPUT_EDIT;
                PrintTitleToWindowEditState();
                if (P_STAT_EDITOR_HP_OR_TERA == P_STAT_EDITOR_HIDDEN_POWER)
                    gTasks[taskId].func = Task_HPTypeEditMode;
                else if (P_STAT_EDITOR_HP_OR_TERA == P_STAT_EDITOR_TERASTAL)
                    gTasks[taskId].func = Task_TeraTypeEditMode;
                return;
            }

            sStatEditorDataPtr->rightPanelSelectedStat = sStatEditorDataPtr->rightPanelColumn + (sStatEditorDataPtr->rightPanelRow * 2);
            sStatEditorDataPtr->statEditingValue = GetBoxMonData(GetCurrentBoxMon(), sSelectedStatToStatEnum[sStatEditorDataPtr->rightPanelSelectedStat]);

            if (sStatEditorDataPtr->rightPanelColumn == RIGHT_PANEL_EVS
             && sStatEditorDataPtr->statEditingValue == MIN_STAT
             && sStatEditorDataPtr->evTotal == MAX_TOTAL_EVS)
            {
                PlaySE(SE_FAILURE);
                sStatEditorDataPtr->panelInputMode = PANEL_INPUT_SELECT;
                return;
            }

            PlaySE(SE_SELECT);
            sStatEditorDataPtr->panelInputMode = PANEL_INPUT_EDIT;
            PrintTitleToWindowEditState();
            gTasks[taskId].func = Task_RightPanelEditMode;
            return;
        }

        if (JOY_NEW(DPAD_UP))
        {
            PlaySE(SE_SELECT);
            sStatEditorDataPtr->rightPanelRow = (sStatEditorDataPtr->rightPanelRow == 0) ? (RIGHT_PANEL_ROW_COUNT - 1) : (sStatEditorDataPtr->rightPanelRow - 1);
            return;
        }

        if (JOY_NEW(DPAD_DOWN))
        {
            PlaySE(SE_SELECT);
            sStatEditorDataPtr->rightPanelRow = (sStatEditorDataPtr->rightPanelRow == (RIGHT_PANEL_ROW_COUNT - 1)) ? 0 : (sStatEditorDataPtr->rightPanelRow + 1);
            return;
        }

        if (JOY_NEW(DPAD_LEFT))
        {
            PlaySE(SE_SELECT);
            if (sStatEditorDataPtr->rightPanelRow == RIGHT_PANEL_ROW_HP_TYPE)
            {
                sStatEditorDataPtr->panel = PANEL_LEFT;
                sStatEditorDataPtr->leftRow = LEFT_ROW_NICKNAME;
            }
            else if (sStatEditorDataPtr->rightPanelColumn == RIGHT_PANEL_IVS)
            {    
                sStatEditorDataPtr->rightPanelColumn = RIGHT_PANEL_EVS;
            }
            else
            {
                sStatEditorDataPtr->panel = PANEL_LEFT;
                sStatEditorDataPtr->leftRow = LEFT_ROW_NICKNAME;
            }
            PrintMonStats();
            return;
        }

        if (JOY_NEW(DPAD_RIGHT))
        {
            PlaySE(SE_SELECT);
            if (P_STAT_EDITOR_HP_OR_TERA != P_STAT_EDITOR_NONE && sStatEditorDataPtr->rightPanelRow == RIGHT_PANEL_ROW_HP_TYPE)
            {
                sStatEditorDataPtr->panel = PANEL_LEFT;
                sStatEditorDataPtr->leftRow = LEFT_ROW_NICKNAME;
            }
            else if (sStatEditorDataPtr->rightPanelColumn == RIGHT_PANEL_EVS)
            {
                sStatEditorDataPtr->rightPanelColumn = RIGHT_PANEL_IVS;
            }
            else
            {
                sStatEditorDataPtr->panel = PANEL_LEFT;
                sStatEditorDataPtr->leftRow = LEFT_ROW_NICKNAME;
            }
            PrintMonStats();
            return;
        }
    }
}

#endif // P_STAT_EDITOR_ENABLE
