#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "battle_interface.h"
#include "battle_gimmick.h"
#include "battle_z_move.h"
#include "battle_setup.h"
#include "battle_util.h"
#include "item.h"
#include "palette.h"
#include "pokemon.h"
#include "sprite.h"
#include "util.h"
#include "test_runner.h"

#include "data/gimmicks.h"

// Populates gBattleStruct->gimmick.usableGimmick for each battler.
void AssignUsableGimmicks(void)
{
    for (enum BattlerId battler = 0; battler < gBattlersCount; ++battler)
    {
        gBattleStruct->gimmick.usableGimmick[battler] = GIMMICK_NONE;
        for (enum Gimmick gimmick = 0; gimmick < GIMMICKS_COUNT; ++gimmick)
        {
            if (CanActivateGimmick(battler, gimmick))
            {
                gBattleStruct->gimmick.usableGimmick[battler] = gimmick;
                break;
            }
        }
    }
}

// Returns whether a battler is able to use a gimmick. Checks consumption and gimmick specific functions.
bool32 CanActivateGimmick(enum BattlerId battler, enum Gimmick gimmick)
{
    return gGimmicksInfo[gimmick].CanActivate != NULL && gGimmicksInfo[gimmick].CanActivate(battler);
}

// Returns whether the player has a gimmick selected while in the move selection menu.
bool32 IsGimmickSelected(enum BattlerId battler, enum Gimmick gimmick)
{
    // There's no player select in tests, but some gimmicks need to test choice before they are fully activated.
    #if TESTING
    return (gBattleStruct->gimmick.toActivate & (1u << battler)) && gBattleStruct->gimmick.usableGimmick[battler] == gimmick;
    #else
    return gBattleStruct->gimmick.usableGimmick[battler] == gimmick && gBattleStruct->gimmick.playerSelect;
    #endif
}

// Sets a battler as having a gimmick active using their party index.
void SetActiveGimmick(enum BattlerId battler, enum Gimmick gimmick)
{
    gBattleStruct->gimmick.activeGimmick[GetBattlerTrainer(battler)][gBattlerPartyIndexes[battler]] = gimmick;
}

// Returns a battler's active gimmick, if any.
enum Gimmick GetActiveGimmick(enum BattlerId battler)
{
    return gBattleStruct->gimmick.activeGimmick[GetBattlerTrainer(battler)][gBattlerPartyIndexes[battler]];
}

// Returns whether a trainer mon is intended to use an unrestrictive gimmick via .useGimmick (i.e Tera).
bool32 ShouldTrainerBattlerUseGimmick(enum BattlerId battler, enum Gimmick gimmick)
{
    // There are no trainer party settings in battles, but the AI needs to know which gimmick to use.
    #if TESTING
    return gimmick == TestRunner_Battle_GetChosenGimmick(GetBattlerTrainer(battler), gBattlerPartyIndexes[battler]);
    #else
    // The player can bypass these checks because they can choose through the controller.
    if (IsOnPlayerSide(battler) && !((gBattleTypeFlags & BATTLE_TYPE_MULTI) && GetBattlerPosition(battler) == B_POSITION_PLAYER_RIGHT))
    {
        return TRUE;
    }
    // Check the trainer party data to see if a gimmick is intended.
    else
    {
        if (gimmick == GIMMICK_TERA && gBattleStruct->opponentMonCanTera & 1 << gBattlerPartyIndexes[battler])
            return TRUE;
        if (gimmick == GIMMICK_DYNAMAX && gBattleStruct->opponentMonCanDynamax & 1 << gBattlerPartyIndexes[battler])
            return TRUE;
    }
    #endif

    return FALSE;
}

// Returns whether a trainer has used a gimmick during a battle.
bool32 HasTrainerUsedGimmick(enum BattlerId battler, enum Gimmick gimmick)
{
    if (IsDoubleBattle() && (IsPartnerMonFromSameTrainer(battler) || (gimmick == GIMMICK_DYNAMAX)))
    {
        enum BattlerId partner = BATTLE_PARTNER(battler);
        if (gBattleStruct->gimmick.activated[partner][gimmick]
         || ((gBattleStruct->gimmick.toActivate & (1u << partner)) && gBattleStruct->gimmick.usableGimmick[partner] == gimmick))
            return TRUE;
    }

    return gBattleStruct->gimmick.activated[battler][gimmick];
}

// Sets a gimmick as used by a trainer with checks for Multi Battles.
void SetGimmickAsActivated(enum BattlerId battler, enum Gimmick gimmick)
{
    gBattleStruct->gimmick.activated[battler][gimmick] = TRUE;
    if (IsDoubleBattle() && (IsPartnerMonFromSameTrainer(battler) || (gimmick == GIMMICK_DYNAMAX)))
        gBattleStruct->gimmick.activated[BATTLE_PARTNER(battler)][gimmick] = TRUE;
}

#define SINGLES_GIMMICK_TRIGGER_POS_X (76)
#define SINGLES_GIMMICK_TRIGGER_POS_Y (96)
#define SINGLES_GIMMICK_TRIGGER_POS_Y_SLIDE (16)

#define DOUBLES_GIMMICK_TRIGGER_POS_X (76)
#define DOUBLES_GIMMICK_TRIGGER_POS_Y (96)
#define DOUBLES_GIMMICK_TRIGGER_POS_Y_SLIDE (16)

#define GIMMICK_TRIGGER_SLIDE_SPEED 2

static const s8 sDoublesGimmickTriggerXOffset[] =
{
    [B_POSITION_PLAYER_LEFT] = -40,
    [B_POSITION_PLAYER_RIGHT] = 10,
};

#define tBattler      data[0]
#define tHide         data[1]

static const s8 sGimmickTriggerSelectBounce[] = {-1, -2, -2, -1, 0, 1, 0};

static s32 GetGimmickTriggerSlideMovement(s32 currentX, s32 targetX)
{
    if (currentX < targetX)
        return min(GIMMICK_TRIGGER_SLIDE_SPEED, targetX - currentX);
    if (currentX > targetX)
        return -min(GIMMICK_TRIGGER_SLIDE_SPEED, currentX - targetX);
    return 0;
}

void ChangeGimmickTriggerSprite(u32 spriteId, u32 animId)
{
    struct Sprite *sprite;

    if (spriteId >= MAX_SPRITES || !gSprites[spriteId].inUse)
        return;

    sprite = &gSprites[spriteId];
    if (sprite->animNum != animId)
        StartSpriteAnim(sprite, animId);
}

void CreateGimmickTriggerSprite(enum BattlerId battler)
{
    const struct GimmickInfo * gimmick = &gGimmicksInfo[gBattleStruct->gimmick.usableGimmick[battler]];
    const struct SpriteSheet *triggerSheet = gimmick->triggerSheet;
    u32 paletteNum;
    u16 tileStart;

    // Exit if there shouldn't be a sprite produced.
    if (!IsOnPlayerSide(battler)
     || gBattleStruct->gimmick.usableGimmick[battler] == GIMMICK_NONE
     || triggerSheet == NULL
     || HasTrainerUsedGimmick(battler, gBattleStruct->gimmick.usableGimmick[battler]))
    {
        return;
    }

    paletteNum = LoadSpritePalette(gimmick->triggerPal);
    if (paletteNum == 0xFF)
        return;

    // Every trigger uses the same tags so the existing allocation can be reused.
    // Refresh both resources when changing battlers, otherwise quickly moving
    // between two different gimmicks can show the previous icon or palette.
    LoadPalette(gimmick->triggerPal->data, OBJ_PLTT_ID(paletteNum), PLTT_SIZE_4BPP);
    tileStart = GetSpriteTileStartByTag(TAG_GIMMICK_TRIGGER_TILE);
    if (tileStart == 0xFFFF)
    {
        LoadSpriteSheet(triggerSheet);
        tileStart = GetSpriteTileStartByTag(TAG_GIMMICK_TRIGGER_TILE);
        if (tileStart == 0xFFFF)
            return;
    }
    else
    {
        CpuCopy32(triggerSheet->data,
                  (void *)(OBJ_VRAM0 + TILE_SIZE_4BPP * tileStart),
                  triggerSheet->size);
    }

    if (gBattleStruct->gimmick.triggerSpriteId == 0xFF)
    {
        if (GetBattlerCoordsIndex(battler) == BATTLE_COORDS_DOUBLES)
            gBattleStruct->gimmick.triggerSpriteId = CreateSprite(gimmick->triggerTemplate,
                                                                  DOUBLES_GIMMICK_TRIGGER_POS_X + sDoublesGimmickTriggerXOffset[GetBattlerPosition(battler)],
                                                                  DOUBLES_GIMMICK_TRIGGER_POS_Y + DOUBLES_GIMMICK_TRIGGER_POS_Y_SLIDE, 0);
        else
            gBattleStruct->gimmick.triggerSpriteId = CreateSprite(gimmick->triggerTemplate,
                                                                  SINGLES_GIMMICK_TRIGGER_POS_X,
                                                                  SINGLES_GIMMICK_TRIGGER_POS_Y + SINGLES_GIMMICK_TRIGGER_POS_Y_SLIDE, 0);

        if (gBattleStruct->gimmick.triggerSpriteId >= MAX_SPRITES)
        {
            gBattleStruct->gimmick.triggerSpriteId = 0xFF;
            return;
        }
    }

    gSprites[gBattleStruct->gimmick.triggerSpriteId].tBattler = battler;
    gSprites[gBattleStruct->gimmick.triggerSpriteId].tHide = FALSE;
    gSprites[gBattleStruct->gimmick.triggerSpriteId].oam.paletteNum = paletteNum;

    ChangeGimmickTriggerSprite(gBattleStruct->gimmick.triggerSpriteId, 0);
}

bool32 IsGimmickTriggerSpriteActive(void)
{
    if (GetSpriteTileStartByTag(TAG_GIMMICK_TRIGGER_TILE) == 0xFFFF)
        return FALSE;
    else if (IndexOfSpritePaletteTag(TAG_GIMMICK_TRIGGER_PAL) != 0xFF)
        return TRUE;
    else
        return FALSE;
}

bool32 IsGimmickTriggerSpriteMatchingBattler(enum BattlerId battler)
{
    if (battler == gSprites[gBattleStruct->gimmick.triggerSpriteId].tBattler)
        return TRUE;
    return FALSE;
}

void HideGimmickTriggerSprite(void)
{
    if (gBattleStruct->gimmick.triggerSpriteId != 0xFF)
    {
        ChangeGimmickTriggerSprite(gBattleStruct->gimmick.triggerSpriteId, 0);
        gSprites[gBattleStruct->gimmick.triggerSpriteId].tHide = TRUE;
    }
}

void DestroyGimmickTriggerSprite(void)
{
    if (gBattleStruct->gimmick.triggerSpriteId != 0xFF)
        DestroySprite(&gSprites[gBattleStruct->gimmick.triggerSpriteId]);
    gBattleStruct->gimmick.triggerSpriteId = 0xFF;
    FreeSpritePaletteByTag(TAG_GIMMICK_TRIGGER_PAL);
    FreeSpriteTilesByTag(TAG_GIMMICK_TRIGGER_TILE);
}

static void SpriteCb_GimmickTrigger(struct Sprite *sprite)
{
    s32 ySlide, xPos, yPos;
    s32 targetY;
    s32 bounceOffset = 0;

    if (GetBattlerCoordsIndex(sprite->tBattler) == BATTLE_COORDS_DOUBLES)
    {
        ySlide = DOUBLES_GIMMICK_TRIGGER_POS_Y_SLIDE;
        xPos = DOUBLES_GIMMICK_TRIGGER_POS_X + sDoublesGimmickTriggerXOffset[GetBattlerPosition(sprite->tBattler)];
        yPos = DOUBLES_GIMMICK_TRIGGER_POS_Y;
    }
    else
    {
        ySlide = SINGLES_GIMMICK_TRIGGER_POS_Y_SLIDE;
        xPos = SINGLES_GIMMICK_TRIGGER_POS_X;
        yPos = SINGLES_GIMMICK_TRIGGER_POS_Y;
    }

    if (sprite->tHide)
    {
        targetY = yPos + ySlide;
        sprite->y += GetGimmickTriggerSlideMovement(sprite->y, targetY);
        if (sprite->y == targetY)
        {
            DestroyGimmickTriggerSprite();
            return;
        }
    }
    else
    {
        // Edge case: in doubles, if selecting move and next mon's action too fast, the second battler's gimmick icon uses the y from the first battler's gimmick icon
        if (sprite->x != xPos)
            sprite->y = yPos + ySlide;

        targetY = yPos;
        sprite->y += GetGimmickTriggerSlideMovement(sprite->y, targetY);
    }

    // x no longer animates
    sprite->x = xPos;
    sprite->oam.priority = 2;

    // Fixed in the moves panel now, so y2 only carries the brief selection bounce.
    sprite->y2 = bounceOffset;
}

#undef tBattler
#undef tHide
#undef tSelectBounce

// for sprite data fields
#define tBattler        data[0]
#define tHidden         data[1]
#define tPosX           data[2]
#define tLevelXDelta    data[3] // X position depends whether level has 3, 2 or 1 digit
#define tIntroTimer     data[4]
#define tIntroPlayed    data[5]
#define tVisualKey      data[6]

// data fields for healthboxMain
// oam.affineParam holds healthboxRight spriteId
#define hMain_Battler               data[6]

void LoadIndicatorSpritesGfx(void)
{
    LoadSpritePalette(&sSpritePalette_MiscIndicator);
    LoadSpritePalette(&sSpritePalette_MegaIndicator);
    LoadSpritePalette(&sSpritePalette_TeraIndicator);
}

static const s8 sIndicatorIntroOffsets[] = {-5, -4, -3, -2, -1, 0, 1, 0};

static void SpriteCb_GimmickIndicator(struct Sprite *sprite)
{
    enum BattlerId battler = sprite->tBattler;

    sprite->x = gSprites[gHealthboxSpriteIds[battler]].x + sprite->tPosX + sprite->tLevelXDelta;
    sprite->x2 = gSprites[gHealthboxSpriteIds[battler]].x2;
    sprite->y2 = gSprites[gHealthboxSpriteIds[battler]].y2;

    if (sprite->tHidden)
    {
        sprite->invisible = TRUE;
        return;
    }

    sprite->invisible = FALSE;
    if (sprite->tIntroTimer > 0)
    {
        u32 frame = ARRAY_COUNT(sIndicatorIntroOffsets) - sprite->tIntroTimer;

        sprite->y2 += sIndicatorIntroOffsets[frame];
        sprite->tIntroTimer--;
    }
}

static inline u32 GetIndicatorSpriteId(u32 healthboxId)
{
    return gBattleStruct->gimmick.indicatorSpriteId[gSprites[healthboxId].hMain_Battler];
}

const u32 *GetIndicatorSpriteSrc(enum BattlerId battler)
{
    u32 gimmick = GetActiveGimmick(battler);

    if (IsBattlerPrimalReverted(battler))
    {
        if (gBattleMons[battler].species == SPECIES_GROUDON_PRIMAL)
            return (u32 *)&sOmegaIndicatorGfx;
        else
            return (u32 *)&sAlphaIndicatorGfx;
    }
    else if (gimmick == GIMMICK_TERA) // special case
    {
        u32 teraType = GetBattlerTeraType(battler);

        if (teraType < ARRAY_COUNT(sTeraIndicatorDataPtrs))
            return (u32 *)sTeraIndicatorDataPtrs[teraType];
        return NULL;
    }
    else if (gimmick < GIMMICKS_COUNT && gGimmicksInfo[gimmick].indicatorData != NULL)
    {
        return (u32 *)gGimmicksInfo[gimmick].indicatorData;
    }
    else
    {
        return NULL;
    }
}

u32 GetIndicatorPalTag(enum BattlerId battler)
{
    u32 gimmick = GetActiveGimmick(battler);
    if (IsBattlerPrimalReverted(battler))
        return TAG_MISC_INDICATOR_PAL;
    else if (gimmick < GIMMICKS_COUNT && gGimmicksInfo[gimmick].indicatorPalTag != 0)
        return gGimmicksInfo[gimmick].indicatorPalTag;
    else
        return TAG_NONE;
}

static u32 GetIndicatorVisualKey(enum BattlerId battler)
{
    u32 gimmick = GetActiveGimmick(battler);

    if (IsBattlerPrimalReverted(battler))
        return gBattleMons[battler].species == SPECIES_GROUDON_PRIMAL ? 0x40 : 0x41;
    if (gimmick == GIMMICK_TERA)
        return 0x20 + GetBattlerTeraType(battler);
    if (gimmick < GIMMICKS_COUNT)
        return gimmick;
    return GIMMICK_NONE;
}

#define INDICATOR_SIZE (8 * 16 / 2)

void UpdateIndicatorVisibilityAndType(u32 healthboxId, bool32 invisible)
{
    enum BattlerId battler = gSprites[healthboxId].hMain_Battler;
    u32 spriteId = GetIndicatorSpriteId(healthboxId);
    u32 palTag = GetIndicatorPalTag(battler);
    u32 palNum;
    u32 visualKey;
    u16 tileStart;
    const u32 *src;
    struct Sprite *sprite;
    bool32 shouldHide = invisible || gSprites[healthboxId].invisible;

    if (spriteId == 0 || spriteId >= MAX_SPRITES) // safari zone means the player doesn't have an indicator sprite id
        return;

    sprite = &gSprites[spriteId];
    src = GetIndicatorSpriteSrc(battler);
    palNum = IndexOfSpritePaletteTag(palTag);
    tileStart = GetSpriteTileStartByTag(BATTLER_INDICATOR_TAG + battler);

    if (palTag != TAG_NONE && palNum != 0xFF && src != NULL && tileStart != 0xFFFF)
    {
        visualKey = GetIndicatorVisualKey(battler);
        if (sprite->tVisualKey != visualKey)
        {
            sprite->tVisualKey = visualKey;
            sprite->tIntroPlayed = FALSE;
            sprite->tIntroTimer = 0;
        }

        sprite->oam.paletteNum = palNum;
        // Upload the complete 8x16 icon in one operation. All indicator art is
        // copied unchanged so Mega, Dynamax, Primal, and every Tera type retain
        // their own palette details and transparent pixels.
        CpuCopy32(src, (void *)(OBJ_VRAM0 + TILE_SIZE_4BPP * tileStart), INDICATOR_SIZE);

        sprite->tHidden = shouldHide;
        sprite->invisible = shouldHide;
        if (shouldHide)
        {
            // Do not resume halfway through a bounce after a healthbox hide.
            sprite->tIntroTimer = 0;
            return;
        }
        if (!shouldHide && !sprite->tIntroPlayed)
        {
            sprite->tIntroPlayed = TRUE;
            sprite->tIntroTimer = ARRAY_COUNT(sIndicatorIntroOffsets);
        }
    }
    else // in case of error
    {
        sprite->tHidden = TRUE;
        sprite->tIntroTimer = 0;
        sprite->tIntroPlayed = FALSE;
        sprite->tVisualKey = GIMMICK_NONE;
        sprite->invisible = TRUE;
    }
}

#undef INDICATOR_SIZE

void UpdateIndicatorOamPriority(u32 healthboxId, u32 oamPriority)
{
    u32 spriteId = GetIndicatorSpriteId(healthboxId);

    if (spriteId != 0 && spriteId < MAX_SPRITES)
        gSprites[spriteId].oam.priority = oamPriority;
}

void UpdateIndicatorLevelData(u32 healthboxId, u32 level)
{
    u32 spriteId = GetIndicatorSpriteId(healthboxId);
    s32 xDelta = 0;

    if (spriteId == 0 || spriteId >= MAX_SPRITES)
        return;

    if (level >= 100)
        xDelta -= 4;
    else if (level < 10)
        xDelta += 5;

    gSprites[spriteId].tLevelXDelta = xDelta;
}

static const s8 sIndicatorPositions[][2] =
{
    [B_POSITION_PLAYER_LEFT] = {49, -9},
    [B_POSITION_OPPONENT_LEFT] = {40, -9},
    [B_POSITION_PLAYER_RIGHT] = {48, -9},
    [B_POSITION_OPPONENT_RIGHT] = {40, -9},
};

void CreateIndicatorSprite(enum BattlerId battler)
{
    enum BattlerPosition position;
    u32 spriteId;
    s16 xHealthbox = 0, x = 0, y = 0;

    position = GetBattlerPosition(battler);
    GetBattlerHealthboxCoords(battler, &xHealthbox, &y);

    x = sIndicatorPositions[position][0];
    y += sIndicatorPositions[position][1];

    LoadSpriteSheet(&sBattler_GimmickSpritesheets[battler]);
    if (GetSpriteTileStartByTag(BATTLER_INDICATOR_TAG + battler) == 0xFFFF)
        return;

    spriteId = CreateSprite(&(sSpriteTemplate_BattlerIndicators[battler]), 0, y, 0);
    if (spriteId >= MAX_SPRITES)
        return;

    gBattleStruct->gimmick.indicatorSpriteId[battler] = spriteId;
    gSprites[spriteId].tBattler = battler;
    gSprites[spriteId].tPosX = x;
    gSprites[spriteId].tHidden = TRUE;
    gSprites[spriteId].invisible = TRUE;
}

#undef tBattler
#undef tHidden
#undef tPosX
#undef tLevelXDelta
#undef tIntroTimer
#undef tIntroPlayed
#undef tVisualKey

#undef hMain_Battler
