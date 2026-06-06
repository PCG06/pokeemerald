#include "global.h"
#include "main.h"
#include "speedup.h"
#include "palette.h"
#include "battle.h"

EWRAM_DATA u32 sSkipCounter = 0;
EWRAM_DATA bool32 sDoSpeedup = FALSE;
EWRAM_DATA u32 sNumSkips = 0;

static u8 sBattleSpeeds[] = {
    [OPTIONS_BATTLE_SPEED_1X] = 1,
    [OPTIONS_BATTLE_SPEED_2X] = 2,
    [OPTIONS_BATTLE_SPEED_3X] = 3,
    [OPTIONS_BATTLE_SPEED_4X] = 4,
    [OPTIONS_BATTLE_SPEED_6X] = 6,
    [OPTIONS_BATTLE_SPEED_8X] = 8
};

void StartSpeedup(void)
{
    if (gSaveBlock2Ptr->optionsBattleSpeed != 0 && !sDoSpeedup)
    {
        sDoSpeedup = TRUE;
        if (gBattleStruct->currentSpeedup != 0)
            sNumSkips = gBattleStruct->currentSpeedup;
        else
            sNumSkips = sBattleSpeeds[gSaveBlock2Ptr->optionsBattleSpeed];
    }
}

void StopSpeedup(void)
{
    gBattleStruct->currentSpeedup = sNumSkips;
    sDoSpeedup = FALSE;
    sNumSkips = 0;
}

bool32 SpeedupShouldSkip(void)
{
    if (!sDoSpeedup)
        return FALSE;

    if (sSkipCounter + 1 >= sNumSkips)
    {
        sSkipCounter = 0;
        return FALSE;
    }
    else
    {
        UpdatePaletteFade();
        sSkipCounter++;
        return TRUE;
    }
}
