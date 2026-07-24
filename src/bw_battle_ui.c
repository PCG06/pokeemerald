#include "global.h"
#include "bg.h"
#include "palette.h"
#include "text.h"
#include "window.h"
#include "sprite.h"
#include "string_util.h"
#include "international_string_util.h"
#include "menu.h"
#include "graphics.h"
#include "decompress.h"
#include "battle.h"
#include "battle_controllers.h"
#include "battle_gimmick.h"
#include "battle_message.h"
#include "battle_z_move.h"
#include "safari_zone.h"
#include "battle_interface.h"
#include "task.h"
#include "sound.h"
#include "malloc.h"
#include "bw_battle_ui.h"
#include "constants/songs.h"
#include "constants/rgb.h"
#include "constants/bw_battle_ui.h"
#include "config/bw_battle_ui.h"

static EWRAM_INIT struct {
    u8 cursorSpriteId;
} sBWBattleUI_Resources =
{
    .cursorSpriteId = SPRITE_NONE,
};

// declarations
static void SpriteCB_BattleUICursor(struct Sprite *);

static void BattleUI_SetCursorBattler(enum BattlerId);
static u32 BattleUI_GetCursorBattler(void);

static void BattleUI_DisplayMoveBoxGraphics(enum BattlerId, u32);
static void BattleUI_DisplayNormalMoveBox(enum BattlerId, struct ChooseMoveStruct *);
static void BattleUI_DisplayZMoveBox(enum BattlerId, struct ChooseMoveStruct *);

static void BattleUI_AddTextPrinter(u32, u32, u32, u32, enum BattleUITextColors, const u8 *);

#include "data/bw_battle_ui.h"

// code
bool32 BattleUI_UsesInputBox(void)
{
    return (BW_BATTLE_UI && BW_BATTLE_UI_TEXTBOX && BW_BATTLE_UI_INPUTBOX
         && gBattleScripting.windowsType == B_WIN_TYPE_NORMAL);
}

const u32 *BattleUI_GetTextboxTiles(void)
{
    if (BW_BATTLE_UI && BW_BATTLE_UI_TEXTBOX)
        return sBWBattleUI_TextboxTiles;

    return gBattleTextboxTiles;
}

const u16 *BattleUI_GetTextboxPalette(void)
{
    if (BW_BATTLE_UI && BW_BATTLE_UI_TEXTBOX)
        return sBWBattleUI_TextboxPalette;

    return gBattleTextboxPalette;
}

const u32 *BattleUI_GetTextboxTilemap(void)
{
    if (BW_BATTLE_UI && BW_BATTLE_UI_TEXTBOX)
        return sBWBattleUI_TextboxTilemap;

    return gBattleTextboxTilemap;
}

void BattleUI_PopulateActionBox(void)
{
    u32 windowId = B_WIN_ACTION_MENU;

    // The sheet is 4bpp tiles in the same order the window stores them, so it
    // copies straight in. Blitting it walked all 6528 pixels one at a time.
    CopyToWindowPixelBuffer(windowId, sBWBattleUI_ActionBox, BUI_ACTION_BOX_TILES * TILE_SIZE_4BPP, 0);

    bool32 safari = !!(gBattleTypeFlags & BATTLE_TYPE_SAFARI);
    u32 fontId = FONT_BATTLE_UI_ELEMENTS;

    for (u32 i = 0; i < BUI_ACTION_BOX_ENTRY_COUNT; i++)
    {
        const u8 *action = sBWBattleUI_ActionBoxFields[safari][i];
        u32 x = GetStringCenterAlignXOffset(fontId, action, TILE_TO_PIXELS(6));
        x += (i & 1) ? TILE_TO_PIXELS(10) : (TILE_TO_PIXELS(2) - 2);
        u32 y = ((i & 2) ? TILE_TO_PIXELS(4) : TILE_TO_PIXELS(1)) - 1;

        BattleUI_AddTextPrinter(windowId, fontId, x, y, BUI_TXTCLR_ABOX_1 + i, action);
    }

    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, COPYWIN_GFX);
}

void BattleUI_CreateCursorSprite(enum BattlerId battler)
{
    u32 spriteId = BattleUI_GetCursorSpriteId();

    if (!BattleUI_UsesInputBox())
        return;

    // A battle can end through a path that resets sprite data without going
    // through BattleUI_SetCursorMode, which leaves a stale id behind. Only
    // reuse the sprite if it is still the cursor we created.
    if (spriteId != SPRITE_NONE
     && (!gSprites[spriteId].inUse || gSprites[spriteId].template->tileTag != TAG_CURSOR))
    {
        spriteId = SPRITE_NONE;
        BattleUI_SetCursorSpriteId(SPRITE_NONE);
    }

    if (spriteId != SPRITE_NONE)
    {
        BattleUI_SetCursorBattler(battler);
        return;
    }

    if (IndexOfSpriteTileTag(TAG_CURSOR) == 0xFF)
    {
        LoadCompressedSpriteSheet(&(const struct CompressedSpriteSheet){
            .data = sBWBattleUI_CursorGfx,
            .size = GetDecompressedDataSize(sBWBattleUI_CursorGfx),
            .tag = TAG_CURSOR
        });
    }
    LoadSpritePalette(&(const struct SpritePalette){
        .data = sBWBattleUI_CursorPalette,
        .tag = TAG_CURSOR
    });

    spriteId = CreateSprite(&sBWBattleUI_CursorTemplate, 0, 0, 0);
    AGB_WARNING(spriteId != SPRITE_NONE);
    if (spriteId == SPRITE_NONE)
    {
        FreeSpriteTilesByTag(TAG_CURSOR);
        FreeSpritePaletteByTag(TAG_CURSOR);
        return;
    }

    BattleUI_SetCursorSpriteId(spriteId);
    BattleUI_SetCursorMode(BUI_CURSOR_MODE_HIDDEN);
    BattleUI_SetCursorBattler(battler);
}

void BattleUI_DestroyCursorSprite(void)
{
    u32 spriteId = BattleUI_GetCursorSpriteId();

    if (spriteId == SPRITE_NONE)
        return;

    DestroySpriteAndFreeResources(&gSprites[spriteId]);
    BattleUI_SetCursorSpriteId(SPRITE_NONE);
}

u32 BattleUI_GetCursorSpriteId(void)
{
    return sBWBattleUI_Resources.cursorSpriteId;
}

void BattleUI_SetCursorSpriteId(u32 spriteId)
{
    sBWBattleUI_Resources.cursorSpriteId = spriteId;
}

void BattleUI_SetCursorMode(enum BWBattleUICursorMode mode)
{
    if (BattleUI_GetCursorSpriteId() == SPRITE_NONE)
    {
        return;
    }
    else if (mode == NUM_BUI_CURSOR_MODES)
    {
        BattleUI_DestroyCursorSprite();
        return;
    }

    struct Sprite *sprite = &gSprites[BattleUI_GetCursorSpriteId()];

    sprite->sCursorMode = mode;
}

enum BWBattleUICursorMode BattleUI_GetCursorMode(void)
{
    // filter out BUI_CURSOR_CONVERT_FLAG
    return (gSprites[BattleUI_GetCursorSpriteId()].sCursorMode & 0xFF);
}

void BattleUI_DisplayMoveBox(enum BattlerId battler)
{
    u32 *tilemap = malloc_and_decompress(BattleUI_GetTextboxTilemap(), NULL);
    CopyRectToBgTilemapBufferRect(0, tilemap,
        0, 0,
        32, DISPLAY_TILE_HEIGHT, // we only need top-most part
        0, 40,
        DISPLAY_TILE_WIDTH, DISPLAY_TILE_HEIGHT,
        0, 0, 0);
    Free(tilemap);

    struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)(&gBattleResources->bufferA[battler][4]);
    bool32 hasZMove = (gBattleStruct->zmove.viable && IsGimmickSelected(battler, GIMMICK_Z_MOVE));

    if (hasZMove)
    {
        BattleUI_DisplayZMoveBox(battler, moveInfo);
        BattleUI_SetCursorMode(BUI_CURSOR_MODE_Z_MOVE);
    }
    else
    {
        gNumberOfMovesToChoose = 0;
        BattleUI_DisplayNormalMoveBox(battler, moveInfo);
        BattleUI_SetCursorMode(BUI_CURSOR_MODE_MOVES);
    }

    CopyBgTilemapBufferToVram(0);
}

// local
static void SpriteCB_BattleUICursor(struct Sprite *sprite)
{
    enum BWBattleUICursorMode mode = BattleUI_GetCursorMode();
    enum BattlerId battler = BattleUI_GetCursorBattler();
    bool32 hasSubsprite = (sprite->sCursorMode >> BUI_CURSOR_CONVERT_FLAG);

    // BG0 is scrolled a full screen per section, which is the cheapest reliable
    // signal for which box the player is currently looking at.
    switch (gBattle_BG0_Y)
    {
    case 0: // textbox
    default:
        mode = BUI_CURSOR_MODE_HIDDEN;
        break;
    case DISPLAY_HEIGHT: // action menu
        mode = BUI_CURSOR_MODE_ACTION;
        break;
    case DISPLAY_HEIGHT * 2:
        if (gBattleStruct->zmove.viable && IsGimmickSelected(battler, GIMMICK_Z_MOVE))
            mode = BUI_CURSOR_MODE_Z_MOVE;
        else
            mode = BUI_CURSOR_MODE_MOVES;
        break;
    }

    if (mode != BattleUI_GetCursorMode())
    {
        BattleUI_SetCursorMode(mode);
        hasSubsprite = FALSE;
    }

    u32 cursor = 0;

    switch (mode)
    {
    default:
    case BUI_CURSOR_MODE_HIDDEN:
        sprite->invisible = TRUE;
        break;
    case BUI_CURSOR_MODE_ACTION:
        cursor = gActionSelectionCursor[battler];
        sprite->x = TILE_TO_PIXELS(13) + 3;
        sprite->y = TILE_TO_PIXELS(14) - 1;
        sprite->x2 = (cursor & 1) ? (TILE_TO_PIXELS(8) + 2) : 0;
        sprite->y2 = (cursor & 2) ? (TILE_TO_PIXELS(3) + 0) : 0;
        goto SET_SUBSPRITE;
    case BUI_CURSOR_MODE_MOVES:
        cursor = gMoveSelectionCursor[battler];
        sprite->x = 5;
        sprite->y = TILE_TO_PIXELS(14) - 2;
        sprite->x2 = (cursor & 1) ? (TILE_TO_PIXELS(14) + 0) : 0;
        sprite->y2 = (cursor & 2) ? (TILE_TO_PIXELS( 3) - 1) : 0;
        goto SET_SUBSPRITE;
    case BUI_CURSOR_MODE_Z_MOVE:
        sprite->x = 5;
        sprite->y = TILE_TO_PIXELS(14) - 2;
        sprite->x2 = 0;
        sprite->y2 = 0;
    SET_SUBSPRITE:
        if (!hasSubsprite)
        {
            sprite->invisible = FALSE;
            SetSubspriteTables(sprite, &sBWBattleUI_CursorSubspritesTable[mode - 1]);
        }
        break;
    }

    if (!hasSubsprite)
        sprite->sCursorMode |= (TRUE << BUI_CURSOR_CONVERT_FLAG);
}

static void BattleUI_SetCursorBattler(enum BattlerId battler)
{
    gSprites[BattleUI_GetCursorSpriteId()].sBattler = battler;
}

static u32 BattleUI_GetCursorBattler(void)
{
    return gSprites[BattleUI_GetCursorSpriteId()].sBattler;
}

static void BattleUI_DisplayMoveBoxGraphics(enum BattlerId battler, u32 windowId)
{
    // Both sheets are 4bpp tiles laid out to match the windows they fill, so
    // they copy straight in rather than being blitted a pixel at a time.
    if (windowId == B_WIN_MOVE_NAME_1 && IsGimmickSelected(battler, GIMMICK_Z_MOVE))
    {
        CopyToWindowPixelBuffer(windowId, sBWBattleUI_MoveBoxGraphicsZ,
                                BUI_MOVE_BOX_TILES * TILE_SIZE_4BPP, 0);
        return;
    }

    u32 slot = windowId - B_WIN_MOVE_NAME_1;
    const u8 *panel = sBWBattleUI_MoveBoxGraphics + slot * BUI_MOVE_PANEL_TILES * TILE_SIZE_4BPP;

    if (windowId != B_WIN_MOVE_NAME_1)
    {
        CopyToWindowPixelBuffer(windowId, panel, BUI_MOVE_PANEL_TILES * TILE_SIZE_4BPP, 0);
        return;
    }

    // The first window spans the whole box so it can also hold the Z move
    // layout, so its panel is the left half of each of the top three rows.
    for (u32 row = 0; row < BUI_MOVE_PANEL_TILE_HEIGHT; row++)
    {
        CopyToWindowPixelBuffer(windowId,
                                panel + row * BUI_MOVE_PANEL_TILE_WIDTH * TILE_SIZE_4BPP,
                                BUI_MOVE_PANEL_TILE_WIDTH * TILE_SIZE_4BPP,
                                row * BUI_MOVE_BOX_TILE_WIDTH);
    }
}

static void BattleUI_DisplayNormalMoveBox(enum BattlerId battler, struct ChooseMoveStruct *moveInfo)
{
    gBattleStruct->zmove.viewing = FALSE;
    bool32 hasDynamax = (IsGimmickSelected(battler, GIMMICK_DYNAMAX) || GetActiveGimmick(battler) == GIMMICK_DYNAMAX);

    for (u32 i = 0; i < MAX_MON_MOVES; i++)
    {
        u32 windowId = B_WIN_MOVE_NAME_1 + i;
        u32 moveId = (hasDynamax) ? GetMaxMove(battler, moveInfo->moves[i]) : moveInfo->moves[i];

        // template
        BattleUI_DisplayMoveBoxGraphics(battler, windowId);

        // type
        enum Type type = (moveId == MOVE_NONE) ? TYPE_MYSTERY : GetMoveType(moveId);
        LoadPalette(sBWBattleUI_MoveBoxPalette, BG_PLTT_ID(10 + i), PLTT_SIZE_4BPP);
        LoadPalette(&sBWBattleUI_MoveBoxTypePalettes[type * 3], BG_PLTT_ID(10 + i) + 10, PLTT_SIZEOF(3));

        if (moveId != MOVE_NONE)
        {
            // name
            StringCopy(gDisplayedStringBattle, GetMoveName(moveId));
            if (B_SHOW_EFFECTIVENESS)
                StringAppend(gDisplayedStringBattle, BattleUI_GetTypeEffectivenessSymbol(battler, moveId));

            u32 x = TILE_TO_PIXELS(1);
            u32 fontId = GetFontIdToFit(gDisplayedStringBattle, FONT_SMALL, 0, TILE_TO_PIXELS(9));

            BattleUI_AddTextPrinter(windowId, fontId, x, 4, BUI_TXTCLR_MOVE_BOX, gDisplayedStringBattle);

            // pp
            if (gBattleResources->bufferA[battler][2] != TRUE)
            {
                u8 *txtPtr = ConvertIntToDecimalStringN(gDisplayedStringBattle, moveInfo->currentPp[i],
                                                        STR_CONV_MODE_RIGHT_ALIGN, 2);
                *(txtPtr)++ = CHAR_SLASH;
                ConvertIntToDecimalStringN(txtPtr, moveInfo->maxPp[i], STR_CONV_MODE_LEFT_ALIGN, 2);

                u32 state = GetCurrentPpToMaxPpState(moveInfo->currentPp[i], moveInfo->maxPp[i]);
                x += GetStringRightAlignXOffset(FONT_SMALL, gDisplayedStringBattle, TILE_TO_PIXELS(12));

                union TextColor clr = sBWBattleUI_TextColors[BUI_TXTCLR_MOVE_BOX];
                clr.foreground = state + 1;

                // can't use BattleUI_AddTextPrinter directly
                AddTextPrinterParameterized6(windowId, FONT_SMALL,
                    x, 4,
                    0, 0,
                    clr,
                    TEXT_SKIP_DRAW, gDisplayedStringBattle);
            }
        }

        if (windowId != B_WIN_MOVE_NAME_1)
        {
            PutWindowTilemap(windowId);
            CopyWindowToVram(windowId, COPYWIN_GFX);
        }

        if (moveId != MOVE_NONE)
            gNumberOfMovesToChoose++;
    }

    // must be copied separately, otherwise you can see this gets shown on screen before the z move box gets cleared.
    PutWindowRectTilemap(B_WIN_MOVE_NAME_1, 0, 0, 14, 3);
    CopyWindowRectToVram(B_WIN_MOVE_NAME_1, COPYWIN_GFX, 0, 0, 14, 3);
}

static void BattleUI_DisplayZMoveBox(enum BattlerId battler, struct ChooseMoveStruct *moveInfo)
{
    u32 windowId = B_WIN_MOVE_NAME_1;
    u32 move = moveInfo->moves[gMoveSelectionCursor[battler]];
    u32 zMove = GetUsableZMove(battler, move);

    gBattleStruct->zmove.viewing = TRUE;

    bool32 isStatusMove = IsBattleMoveStatus(move);

    // template
    BattleUI_DisplayMoveBoxGraphics(battler, windowId);

    // type
    enum Type type = (zMove == MOVE_NONE) ? TYPE_MYSTERY : GetMoveType(zMove);
    LoadPalette(sBWBattleUI_MoveBoxPalette, BG_PLTT_ID(10), PLTT_SIZE_4BPP);
    LoadPalette(&sBWBattleUI_MoveBoxTypePalettes[type * 3], BG_PLTT_ID(10) + 10, PLTT_SIZEOF(3));

    // name
    if (isStatusMove)
    {
        gDisplayedStringBattle[0] = CHAR_Z;
        gDisplayedStringBattle[1] = CHAR_HYPHEN;
        StringCopy(gDisplayedStringBattle + 2, GetMoveName(move));
    }
    else
    {
        StringCopy(gDisplayedStringBattle, GetMoveName(zMove));
    }

    if (B_SHOW_EFFECTIVENESS && !isStatusMove)
        StringAppend(gDisplayedStringBattle, BattleUI_GetTypeEffectivenessSymbol(battler, zMove));

    u32 x = GetStringCenterAlignXOffset(FONT_SMALL, gDisplayedStringBattle, TILE_TO_PIXELS(28));

    BattleUI_AddTextPrinter(windowId, FONT_SMALL, x, 6, BUI_TXTCLR_MOVE_BOX, gDisplayedStringBattle);

    // description
    u8 *txtPtr = NULL;

    if (isStatusMove)
    {
        u8 zEffect = GetMoveZEffect(move);

        gDisplayedStringBattle[0] = EOS;

        if (zEffect == Z_EFFECT_CURSE)
        {
            if (moveInfo->monTypes[0] == TYPE_GHOST
             || moveInfo->monTypes[1] == TYPE_GHOST
             || moveInfo->monTypes[2] == TYPE_GHOST)
                zEffect = Z_EFFECT_RECOVER_HP;
            else
                zEffect = Z_EFFECT_ATK_UP_1;
        }

        switch (zEffect)
        {
        case Z_EFFECT_RESET_STATS:
            StringCopy(gDisplayedStringBattle, COMPOUND_STRING("Reset Lowered Stats"));
            break;
        case Z_EFFECT_ALL_STATS_UP_1:
            StringCopy(gDisplayedStringBattle, COMPOUND_STRING("+ All Stats"));
            break;
        case Z_EFFECT_BOOST_CRITS:
            StringCopy(gDisplayedStringBattle, COMPOUND_STRING("+ Critical Hit Chance"));
            break;
        case Z_EFFECT_FOLLOW_ME:
            StringCopy(gDisplayedStringBattle, COMPOUND_STRING("Follow Me"));
            break;
        case Z_EFFECT_RECOVER_HP:
            StringCopy(gDisplayedStringBattle, COMPOUND_STRING("Recover HP"));
            break;
        case Z_EFFECT_RESTORE_REPLACEMENT_HP:
            StringCopy(gDisplayedStringBattle, COMPOUND_STRING("Heal Replacement HP"));
            break;
        case Z_EFFECT_ATK_UP_1:
        case Z_EFFECT_DEF_UP_1:
        case Z_EFFECT_SPD_UP_1:
        case Z_EFFECT_SPATK_UP_1:
        case Z_EFFECT_SPDEF_UP_1:
        case Z_EFFECT_ACC_UP_1:
        case Z_EFFECT_EVSN_UP_1:
            gDisplayedStringBattle[0] = CHAR_PLUS;
            gDisplayedStringBattle[1] = 0;
            gDisplayedStringBattle[2] = EOS;
            PREPARE_STAT_BUFFER(gBattleTextBuff1, zEffect - Z_EFFECT_ATK_UP_1 + 1);
            ExpandBattleTextBuffPlaceholders(gBattleTextBuff1, gDisplayedStringBattle + 2);
            break;
        case Z_EFFECT_ATK_UP_2:
        case Z_EFFECT_DEF_UP_2:
        case Z_EFFECT_SPD_UP_2:
        case Z_EFFECT_SPATK_UP_2:
        case Z_EFFECT_SPDEF_UP_2:
        case Z_EFFECT_ACC_UP_2:
        case Z_EFFECT_EVSN_UP_2:
            gDisplayedStringBattle[0] = CHAR_PLUS;
            gDisplayedStringBattle[1] = CHAR_PLUS;
            gDisplayedStringBattle[2] = 0;
            gDisplayedStringBattle[3] = EOS;
            PREPARE_STAT_BUFFER(gBattleTextBuff1, zEffect - Z_EFFECT_ATK_UP_2 + 1);
            ExpandBattleTextBuffPlaceholders(gBattleTextBuff1, gDisplayedStringBattle + 3);
            break;
        case Z_EFFECT_ATK_UP_3:
        case Z_EFFECT_DEF_UP_3:
        case Z_EFFECT_SPD_UP_3:
        case Z_EFFECT_SPATK_UP_3:
        case Z_EFFECT_SPDEF_UP_3:
        case Z_EFFECT_ACC_UP_3:
        case Z_EFFECT_EVSN_UP_3:
            gDisplayedStringBattle[0] = CHAR_PLUS;
            gDisplayedStringBattle[1] = CHAR_PLUS;
            gDisplayedStringBattle[2] = CHAR_PLUS;
            gDisplayedStringBattle[3] = 0;
            gDisplayedStringBattle[4] = EOS;
            PREPARE_STAT_BUFFER(gBattleTextBuff1, zEffect - Z_EFFECT_ATK_UP_3 + 1);
            ExpandBattleTextBuffPlaceholders(gBattleTextBuff1, gDisplayedStringBattle + 4);
            break;
        default:
            if (B_SHOW_USELESS_Z_MOVE_INFO == TRUE)
                StringCopy(gDisplayedStringBattle, COMPOUND_STRING("No Additional Effect"));
            break;
        }
    }
    else if (GetMoveEffect(zMove) == EFFECT_EXTREME_EVOBOOST) // Damaging move -> status z move
    {
        StringCopy(gDisplayedStringBattle, COMPOUND_STRING("++ All Stats"));
    }
    else
    {
        u32 power = GetZMovePower(move);

        if (zMove >= MOVE_CATASTROPIKA)
            power = GetMovePower(zMove);

        txtPtr = StringCopy(gDisplayedStringBattle, COMPOUND_STRING("Power: "));
        txtPtr = ConvertIntToDecimalStringN(txtPtr, power, STR_CONV_MODE_LEFT_ALIGN, 3);
    }

    bool32 showMovePp = gBattleResources->bufferA[battler][2] != TRUE || move != MOVE_NONE;

    u32 tile = showMovePp ? 14 : 28;
    x = GetStringCenterAlignXOffset(FONT_SMALL, gDisplayedStringBattle, TILE_TO_PIXELS(tile));
    BattleUI_AddTextPrinter(windowId, FONT_SMALL, x, TILE_TO_PIXELS(3) + 2, BUI_TXTCLR_MOVE_BOX, gDisplayedStringBattle);

    if (showMovePp)
    {
        txtPtr = StringCopy(gDisplayedStringBattle, COMPOUND_STRING("PP: "));
        txtPtr = ConvertIntToDecimalStringN(txtPtr, 1, STR_CONV_MODE_RIGHT_ALIGN, 2);
        *(txtPtr)++ = CHAR_SLASH;
        ConvertIntToDecimalStringN(txtPtr, 1, STR_CONV_MODE_RIGHT_ALIGN, 1);

        x = TILE_TO_PIXELS(14) + GetStringCenterAlignXOffset(FONT_SMALL, gDisplayedStringBattle, TILE_TO_PIXELS(14));

        BattleUI_AddTextPrinter(windowId, FONT_SMALL, x, TILE_TO_PIXELS(3) + 2, BUI_TXTCLR_MOVE_BOX, gDisplayedStringBattle);
    }

    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, COPYWIN_GFX);
}

static void BattleUI_AddTextPrinter(u32 windowId, u32 fontId, u32 x, u32 y, enum BattleUITextColors color, const u8 *str)
{
    AddTextPrinterParameterized6(windowId, fontId, x, y, 0, 0, sBWBattleUI_TextColors[color], TEXT_SKIP_DRAW, str);
}
