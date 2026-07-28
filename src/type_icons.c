#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "battle_gimmick.h"
#include "decompress.h"
#include "graphics.h"
#include "pokedex.h"
#include "sprite.h"
#include "type_icons.h"

static void LoadTypeSpritesAndPalettes(void);
static void LoadTypeIconsPerBattler(enum BattlerId, u32);

static bool32 UseDoubleBattleCoords(u32);

static enum Type GetMonPublicType(enum BattlerId, u32);
static bool32 ShouldHideUncaughtType(enum Species species);
static bool32 ShouldHideUnseenType(enum Species species);
static enum Type GetMonDefensiveTeraType(struct Pokemon *, struct Pokemon *, enum BattlerId, u32, enum Species, enum Species);
static bool32 IsIllusionActiveAndTypeUnchanged(struct Pokemon *, enum Species, enum BattlerId);

static void CreateSpriteFromType(u32, bool32, enum Type[], u32, enum BattlerId);
static bool32 ShouldSkipSecondType(enum Type[], u32);
static void SetTypeIconXY(s32 *, s32 *, u32, bool32, enum Type[], u32);

static void CreateSpriteAndSetTypeSpriteAttributes(enum Type, u32 x, u32 y, u32, enum BattlerId, bool32);
static bool32 ShouldFlipTypeIcon(bool32, u32, enum Type);

static void SpriteCB_TypeIcon(struct Sprite*);
static void DestroyTypeIcon(struct Sprite*);
static void DestroyAllTypeIcons(void);
static void FreeAllTypeIconResources(void);
static bool32 ShouldHideTypeIcon(enum BattlerId);
static s32 GetTypeIconHiddenY(struct Sprite *);
static s32 GetTypeIconSlideMovement(s32, s32);

// Player singles stack a vertical column past the right end of the level, flush
// with the edge of the box, and slide it in from off the right of the screen.
// The lower badge shares the level's line and the column grows upward from
// there, since the healthbox draws over anything below that line.
// Doubles keep a horizontal pair so four healthboxes remain easy to scan.
#define TYPE_ICON_PLAYER_SINGLE_X_OFFSET  79
#define TYPE_ICON_PLAYER_SINGLE_Y_OFFSET (-1)
#define TYPE_ICON_PLAYER_DOUBLE_X_OFFSET (-34)
#define TYPE_ICON_OPPONENT_X_OFFSET      78
#define TYPE_ICON_PAIR_SPACING           10
#define TYPE_ICON_RESTING_Y_OFFSET       (-5)
#define TYPE_ICON_VERTICAL_Y_OFFSET      (-11)
#define TYPE_ICON_VERTICAL_Y_SPACING     11
#define TYPE_ICON_REVEAL_DISTANCE        10
#define TYPE_ICON_SLIDE_SPEED            2

enum TypeIconTransitionAxis
{
    TYPE_ICON_TRANSITION_VERTICAL,
    TYPE_ICON_TRANSITION_HORIZONTAL,
};

const union AnimCmd sSpriteAnim_TypeIcon_Normal[] =
{
    ANIMCMD_FRAME(TYPE_ICON_1_FRAME(TYPE_NORMAL), 0),
    ANIMCMD_END
};
const union AnimCmd sSpriteAnim_TypeIcon_Fighting[] =
{
    ANIMCMD_FRAME(TYPE_ICON_1_FRAME(TYPE_FIGHTING), 0),
    ANIMCMD_END
};
const union AnimCmd sSpriteAnim_TypeIcon_Flying[] =
{
    ANIMCMD_FRAME(TYPE_ICON_1_FRAME(TYPE_FLYING), 0),
    ANIMCMD_END
};
const union AnimCmd sSpriteAnim_TypeIcon_Poison[] =
{
    ANIMCMD_FRAME(TYPE_ICON_1_FRAME(TYPE_POISON), 0),
    ANIMCMD_END
};
const union AnimCmd sSpriteAnim_TypeIcon_Ground[] =
{
    ANIMCMD_FRAME(TYPE_ICON_1_FRAME(TYPE_GROUND), 0),
    ANIMCMD_END
};
const union AnimCmd sSpriteAnim_TypeIcon_Rock[] =
{
    ANIMCMD_FRAME(TYPE_ICON_1_FRAME(TYPE_ROCK), 0),
    ANIMCMD_END
};
const union AnimCmd sSpriteAnim_TypeIcon_Bug[] =
{
    ANIMCMD_FRAME(TYPE_ICON_1_FRAME(TYPE_BUG), 0),
    ANIMCMD_END
};
const union AnimCmd sSpriteAnim_TypeIcon_Ghost[] =
{
    ANIMCMD_FRAME(TYPE_ICON_1_FRAME(TYPE_GHOST), 0),
    ANIMCMD_END
};
const union AnimCmd sSpriteAnim_TypeIcon_Steel[] =
{
    ANIMCMD_FRAME(TYPE_ICON_1_FRAME(TYPE_STEEL), 0),
    ANIMCMD_END
};
const union AnimCmd sSpriteAnim_TypeIcon_Mystery[] =
{
    ANIMCMD_FRAME(TYPE_ICON_1_FRAME(TYPE_MYSTERY), 0),
    ANIMCMD_END
};

const union AnimCmd sSpriteAnim_TypeIcon_Fire[] =
{
    ANIMCMD_FRAME(TYPE_ICON_2_FRAME(TYPE_FIRE), 0),
    ANIMCMD_END
};
const union AnimCmd sSpriteAnim_TypeIcon_Water[] =
{
    ANIMCMD_FRAME(TYPE_ICON_2_FRAME(TYPE_WATER), 0),
    ANIMCMD_END
};
const union AnimCmd sSpriteAnim_TypeIcon_Grass[] =
{
    ANIMCMD_FRAME(TYPE_ICON_2_FRAME(TYPE_GRASS), 0),
    ANIMCMD_END
};
const union AnimCmd sSpriteAnim_TypeIcon_Electric[] =
{
    ANIMCMD_FRAME(TYPE_ICON_2_FRAME(TYPE_ELECTRIC), 0),
    ANIMCMD_END
};
const union AnimCmd sSpriteAnim_TypeIcon_Psychic[] =
{
    ANIMCMD_FRAME(TYPE_ICON_2_FRAME(TYPE_PSYCHIC), 0),
    ANIMCMD_END
};
const union AnimCmd sSpriteAnim_TypeIcon_Ice[] =
{
    ANIMCMD_FRAME(TYPE_ICON_2_FRAME(TYPE_ICE), 0),
    ANIMCMD_END
};
const union AnimCmd sSpriteAnim_TypeIcon_Dragon[] =
{
    ANIMCMD_FRAME(TYPE_ICON_2_FRAME(TYPE_DRAGON), 0),
    ANIMCMD_END
};
const union AnimCmd sSpriteAnim_TypeIcon_Dark[] =
{
    ANIMCMD_FRAME(TYPE_ICON_2_FRAME(TYPE_DARK), 0),
    ANIMCMD_END
};
const union AnimCmd sSpriteAnim_TypeIcon_Fairy[] =
{
    ANIMCMD_FRAME(TYPE_ICON_2_FRAME(TYPE_FAIRY), 0),
    ANIMCMD_END
};

const union AnimCmd *const sSpriteAnimTable_TypeIcons[] =
{
    [TYPE_NONE] =       sSpriteAnim_TypeIcon_Mystery,
    [TYPE_NORMAL] =     sSpriteAnim_TypeIcon_Normal,
    [TYPE_FIGHTING] =   sSpriteAnim_TypeIcon_Fighting,
    [TYPE_FLYING] =     sSpriteAnim_TypeIcon_Flying,
    [TYPE_POISON] =     sSpriteAnim_TypeIcon_Poison,
    [TYPE_GROUND] =     sSpriteAnim_TypeIcon_Ground,
    [TYPE_ROCK] =       sSpriteAnim_TypeIcon_Rock,
    [TYPE_BUG] =        sSpriteAnim_TypeIcon_Bug,
    [TYPE_GHOST] =      sSpriteAnim_TypeIcon_Ghost,
    [TYPE_STEEL] =      sSpriteAnim_TypeIcon_Steel,
    [TYPE_MYSTERY] =    sSpriteAnim_TypeIcon_Mystery,
    [TYPE_FIRE] =       sSpriteAnim_TypeIcon_Fire,
    [TYPE_WATER] =      sSpriteAnim_TypeIcon_Water,
    [TYPE_GRASS] =      sSpriteAnim_TypeIcon_Grass,
    [TYPE_ELECTRIC] =   sSpriteAnim_TypeIcon_Electric,
    [TYPE_PSYCHIC] =    sSpriteAnim_TypeIcon_Psychic,
    [TYPE_ICE] =        sSpriteAnim_TypeIcon_Ice,
    [TYPE_DRAGON] =     sSpriteAnim_TypeIcon_Dragon,
    [TYPE_DARK] =       sSpriteAnim_TypeIcon_Dark,
    [TYPE_FAIRY] =      sSpriteAnim_TypeIcon_Fairy,
    [TYPE_STELLAR] =    sSpriteAnim_TypeIcon_Mystery,
};

const struct SpritePalette sTypeIconPal1 =
{
    .data = gBattleIcons_Pal1,
    .tag = TYPE_ICON_TAG
};

const struct SpritePalette sTypeIconPal2 =
{
    .data = gBattleIcons_Pal2,
    .tag = TYPE_ICON_TAG_2
};

const struct OamData sOamData_TypeIcons =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .shape = SPRITE_SHAPE(8x16),
    .size = SPRITE_SIZE(8x16),
    // One priority ahead of the battler sprites (priority 2) so a Pokémon never
    // covers the badges. This ties the healthbox priority, but the badges carry
    // the largest possible subpriority, so the healthbox and its HP bar still
    // draw over them and cleanly mask the slide into and out of the name row.
    .priority = 1,
};

const struct CompressedSpriteSheet sSpriteSheet_TypeIcons2 =
{
    .data = gBattleIcons_Gfx2,
    .size = (8*16) * 9,
    .tag = TYPE_ICON_TAG_2,
};

const struct CompressedSpriteSheet sSpriteSheet_TypeIcons1 =
{
    .data = gBattleIcons_Gfx1,
    .size = (8*16) * 10,
    .tag = TYPE_ICON_TAG,
};

const struct SpriteTemplate sSpriteTemplate_TypeIcons1 =
{
    .tileTag = TYPE_ICON_TAG,
    .paletteTag = TYPE_ICON_TAG,
    .oam = &sOamData_TypeIcons,
    .anims = sSpriteAnimTable_TypeIcons,
    .callback = SpriteCB_TypeIcon
};

const struct SpriteTemplate sSpriteTemplate_TypeIcons2 =
{
    .tileTag = TYPE_ICON_TAG_2,
    .paletteTag = TYPE_ICON_TAG_2,
    .oam = &sOamData_TypeIcons,
    .anims = sSpriteAnimTable_TypeIcons,
    .callback = SpriteCB_TypeIcon
};

void LoadTypeIcons(enum BattlerId battler)
{
    u32 position;

    struct Pokemon* mon = GetBattlerMon(battler);
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);

    // A fast transition between battlers in doubles can begin before the old
    // badges finish leaving. Remove that generation before loading the next so
    // two sprite sets never overlap or fight over their shared sheets.
    DestroyAllTypeIcons();

    if (B_SHOW_TYPES == SHOW_TYPES_NEVER
        || (B_SHOW_TYPES == SHOW_TYPES_SEEN && !GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_GET_SEEN)))
        return;

    LoadTypeSpritesAndPalettes();

    for (position = 0; position < gBattlersCount; ++position)
        LoadTypeIconsPerBattler(battler, position);
}

static void LoadTypeSpritesAndPalettes(void)
{
    if (IndexOfSpritePaletteTag(TYPE_ICON_TAG) != UCHAR_MAX)
        return;

    LoadCompressedSpriteSheet(&sSpriteSheet_TypeIcons1);
    LoadCompressedSpriteSheet(&sSpriteSheet_TypeIcons2);
    LoadSpritePalette(&sTypeIconPal1);
    LoadSpritePalette(&sTypeIconPal2);
}

static void LoadTypeIconsPerBattler(enum BattlerId battler, u32 position)
{
    u32 typeNum;
    enum Type types[2];
    enum BattlerId battlerId = GetBattlerAtPosition(position);
    bool32 useDoubleBattleCoords = UseDoubleBattleCoords(battlerId);

    if (!IsBattlerAlive(battlerId))
        return;

    for (typeNum = 0; typeNum < 2; ++typeNum)
        types[typeNum] = GetMonPublicType(battlerId, typeNum);

    for (typeNum = 0; typeNum < 2; ++typeNum)
        CreateSpriteFromType(position, useDoubleBattleCoords, types, typeNum, battler);
}

static bool32 UseDoubleBattleCoords(u32 position)
{
    if (!IsDoubleBattle())
        return FALSE;

    if ((position == B_POSITION_PLAYER_LEFT) && (gBattleMons[B_POSITION_PLAYER_RIGHT].species == SPECIES_NONE))
        return FALSE;

    if ((position == B_POSITION_OPPONENT_LEFT) && (gBattleMons[B_POSITION_OPPONENT_RIGHT].species == SPECIES_NONE))
        return FALSE;

    return TRUE;
}

static enum Type GetMonPublicType(enum BattlerId battlerId, u32 typeNum)
{
    struct Pokemon *mon = GetBattlerMon(battlerId);
    enum Species monSpecies = GetMonData(mon,MON_DATA_SPECIES,NULL);
    struct Pokemon *monIllusion;
    enum Species illusionSpecies;

    if (ShouldHideUncaughtType(monSpecies) || ShouldHideUnseenType(monSpecies))
        return TYPE_MYSTERY;

    monIllusion = GetIllusionMonPtr(battlerId);
    illusionSpecies = GetMonData(monIllusion,MON_DATA_SPECIES,NULL);

    if (GetActiveGimmick(battlerId) == GIMMICK_TERA)
        return GetMonDefensiveTeraType(mon,monIllusion,battlerId,typeNum,illusionSpecies,monSpecies);

    if (IsIllusionActiveAndTypeUnchanged(monIllusion,monSpecies, battlerId))
        return GetSpeciesType(illusionSpecies, typeNum);

    return gBattleMons[battlerId].types[typeNum];
}

static bool32 ShouldHideUncaughtType(enum Species species)
{
    if (B_SHOW_TYPES != SHOW_TYPES_CAUGHT)
        return FALSE;

    if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_GET_CAUGHT))
        return FALSE;

    return TRUE;
}

static bool32 ShouldHideUnseenType(enum Species species)
{
    if (B_SHOW_TYPES != SHOW_TYPES_SEEN)
        return FALSE;

    if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_GET_SEEN))
        return FALSE;

    return TRUE;
}

static enum Type GetMonDefensiveTeraType(struct Pokemon *mon, struct Pokemon *monIllusion, enum BattlerId battlerId, u32 typeNum, enum Species illusionSpecies, enum Species monSpecies)
{
    enum Type teraType = GetBattlerTeraType(battlerId);
    enum Species targetSpecies;

    if (teraType != TYPE_STELLAR)
        return teraType;

    targetSpecies = (monIllusion != NULL) ? illusionSpecies : monSpecies;

    return GetSpeciesType(targetSpecies, typeNum);
}

static bool32 IsIllusionActiveAndTypeUnchanged(struct Pokemon *monIllusion, enum Species monSpecies, enum BattlerId battlerId)
{
    u32 typeNum;

    if (monIllusion == NULL)
        return FALSE;

    for (typeNum = 0; typeNum < 2; typeNum++)
        if (GetSpeciesType(monSpecies, typeNum) != gBattleMons[battlerId].types[typeNum])
        return FALSE;

    return TRUE;
}

static void CreateSpriteFromType(u32 position, bool32 useDoubleBattleCoords, enum Type types[], u32 typeNum, enum BattlerId battler)
{
    s32 x = 0, y = 0;

    if (ShouldSkipSecondType(types, typeNum))
        return;

    SetTypeIconXY(&x, &y, position, useDoubleBattleCoords, types, typeNum);

    CreateSpriteAndSetTypeSpriteAttributes(types[typeNum], x, y, position, battler, useDoubleBattleCoords);
}

static bool32 ShouldSkipSecondType(enum Type types[], u32 typeNum)
{
    if (!typeNum)
        return FALSE;

    if (types[0] != types[1])
        return FALSE;

    return TRUE;
}

static void SetTypeIconXY(s32 *x, s32 *y, u32 position, bool32 useDoubleBattleCoords, enum Type types[], u32 typeNum)
{
    enum BattlerId battler = GetBattlerAtPosition(position);
    struct Sprite *healthbox = &gSprites[gHealthboxSpriteIds[battler]];
    bool32 hasTwoTypes = types[0] != types[1];

    if (IsOnPlayerSide(battler) && !useDoubleBattleCoords)
    {
        *x = healthbox->x + TYPE_ICON_PLAYER_SINGLE_X_OFFSET;
        *y = healthbox->y + TYPE_ICON_PLAYER_SINGLE_Y_OFFSET;

        if (hasTwoTypes)
            *y += TYPE_ICON_VERTICAL_Y_OFFSET + TYPE_ICON_VERTICAL_Y_SPACING * typeNum;
        return;
    }

    *x = healthbox->x + (IsOnPlayerSide(battler) ? TYPE_ICON_PLAYER_DOUBLE_X_OFFSET : TYPE_ICON_OPPONENT_X_OFFSET);
    *y = healthbox->y + TYPE_ICON_RESTING_Y_OFFSET;

    // A monotype stays centered in the reserved slot. Dual types expand toward
    // the outside edge, keeping both clear of the name and level.
    if (hasTwoTypes && typeNum == 0)
        *x -= TYPE_ICON_PAIR_SPACING;
}

static void CreateSpriteAndSetTypeSpriteAttributes(enum Type type, u32 x, u32 y, u32 position, enum BattlerId battler, bool32 useDoubleBattleCoords)
{
    struct Sprite* sprite;
    enum BattlerId displayedBattler = GetBattlerAtPosition(position);
    bool32 useHorizontalTransition = IsOnPlayerSide(displayedBattler) && !useDoubleBattleCoords;
    const struct SpriteTemplate* spriteTemplate = gTypesInfo[type].useSecondTypeIconPalette ? &sSpriteTemplate_TypeIcons2 : &sSpriteTemplate_TypeIcons1;
    u32 spriteId = CreateSpriteAtEnd(spriteTemplate,
                                     x + (useHorizontalTransition ? TYPE_ICON_REVEAL_DISTANCE : 0),
                                     y + (useHorizontalTransition ? 0 : TYPE_ICON_REVEAL_DISTANCE),
                                     UCHAR_MAX);

    if (spriteId == MAX_SPRITES)
        return;

    sprite = &gSprites[spriteId];
    sprite->tMonPosition = position;
    sprite->tBattlerId = battler;
    sprite->tTransitionAxis = useHorizontalTransition ? TYPE_ICON_TRANSITION_HORIZONTAL : TYPE_ICON_TRANSITION_VERTICAL;
    sprite->tVerticalPosition = y;
    sprite->tHorizontalPosition = x;

    sprite->hFlip = ShouldFlipTypeIcon(useDoubleBattleCoords, position, type);

    StartSpriteAnim(sprite, type);
}

static bool32 ShouldFlipTypeIcon(bool32 useDoubleBattleCoords, u32 position, enum Type typeId)
{
    enum BattleSide side = (useDoubleBattleCoords) ? B_SIDE_OPPONENT : B_SIDE_PLAYER;

    if (GetBattlerSide(GetBattlerAtPosition(position)) != side)
        return FALSE;

    return !gTypesInfo[typeId].isSpecialCaseType;
}

static void SpriteCB_TypeIcon(struct Sprite *sprite)
{
    u32 position = sprite->tMonPosition;
    enum BattlerId battlerId = sprite->tBattlerId;
    s32 targetPosition;
    bool32 shouldHide = ShouldHideTypeIcon(battlerId);

    if (sprite->tTransitionAxis == TYPE_ICON_TRANSITION_HORIZONTAL)
    {
        targetPosition = sprite->tHorizontalPosition + (shouldHide ? TYPE_ICON_REVEAL_DISTANCE : 0);
        sprite->x += GetTypeIconSlideMovement(sprite->x, targetPosition);
    }
    else
    {
        targetPosition = shouldHide ? GetTypeIconHiddenY(sprite) : sprite->tVerticalPosition;
        sprite->y += GetTypeIconSlideMovement(sprite->y, targetPosition);
    }

    if (shouldHide
     && ((sprite->tTransitionAxis == TYPE_ICON_TRANSITION_HORIZONTAL && sprite->x == targetPosition)
      || (sprite->tTransitionAxis == TYPE_ICON_TRANSITION_VERTICAL && sprite->y == targetPosition)))
    {
        DestroyTypeIcon(sprite);
        return;
    }

    sprite->y2 = gSprites[gHealthboxSpriteIds[GetBattlerAtPosition(position)]].y2;
}

static const u32 typeIconTags[] =
{
    TYPE_ICON_TAG,
    TYPE_ICON_TAG_2
};

static void DestroyTypeIcon(struct Sprite* sprite)
{
    u32 spriteId, tag;

    // All type badges share these two sheets and palettes. Freeing resources
    // with the first destroyed sprite corrupts its still-visible siblings for a
    // frame, so destroy only this sprite and release the sheets after the last.
    DestroySprite(sprite);

    for (spriteId = 0; spriteId < MAX_SPRITES; ++spriteId)
    {
        if (!gSprites[spriteId].inUse)
            continue;

        for (tag = 0; tag < 2; tag++)
        {
            if (gSprites[spriteId].template->paletteTag == typeIconTags[tag])
                return;

            if (gSprites[spriteId].template->tileTag == typeIconTags[tag])
                return;
        }
    }

    FreeAllTypeIconResources();
}

static void DestroyAllTypeIcons(void)
{
    u32 spriteId, tag;

    for (spriteId = 0; spriteId < MAX_SPRITES; ++spriteId)
    {
        if (!gSprites[spriteId].inUse)
            continue;

        for (tag = 0; tag < ARRAY_COUNT(typeIconTags); ++tag)
        {
            if (gSprites[spriteId].template->tileTag == typeIconTags[tag]
             || gSprites[spriteId].template->paletteTag == typeIconTags[tag])
            {
                DestroySprite(&gSprites[spriteId]);
                break;
            }
        }
    }

    FreeAllTypeIconResources();
}

static void FreeAllTypeIconResources(void)
{
    u32 tag;

    for (tag = 0; tag < 2; tag++)
    {
        FreeSpriteTilesByTag(typeIconTags[tag]);
        FreeSpritePaletteByTag(typeIconTags[tag]);
    }
}

static void (*const sShowTypesControllerFuncs[])(enum BattlerId battler) =
{
    PlayerHandleChooseMove,
    HandleChooseMoveAfterDma3,
    HandleInputChooseTarget,
    HandleInputShowTargets,
    HandleInputShowEntireFieldTargets,
    HandleMoveSwitching,
    HandleInputChooseMove,
};


static bool32 ShouldHideTypeIcon(enum BattlerId battlerId)
{
    u32 funcIndex;

    for (funcIndex = 0; funcIndex < ARRAY_COUNT(sShowTypesControllerFuncs); funcIndex++)
        if (gBattlerControllerFuncs[battlerId] == sShowTypesControllerFuncs[funcIndex])
            return FALSE;

    return TRUE;
}

static s32 GetTypeIconHiddenY(struct Sprite *sprite)
{
    return sprite->tVerticalPosition + TYPE_ICON_REVEAL_DISTANCE;
}

static s32 GetTypeIconSlideMovement(s32 currentPos, s32 targetPos)
{
    if (currentPos < targetPos)
        return min(TYPE_ICON_SLIDE_SPEED, targetPos - currentPos);
    if (currentPos > targetPos)
        return -min(TYPE_ICON_SLIDE_SPEED, currentPos - targetPos);
    return 0;
}
