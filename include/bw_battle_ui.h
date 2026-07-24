#ifndef GUARD_BW_BATTLE_UI_H
#define GUARD_BW_BATTLE_UI_H

enum BWBattleUICursorMode
{
    BUI_CURSOR_MODE_HIDDEN = 0,
    BUI_CURSOR_MODE_ACTION,
    BUI_CURSOR_MODE_MOVES,
    BUI_CURSOR_MODE_Z_MOVE,

    NUM_BUI_CURSOR_MODES
};

// TRUE when the BW action/move boxes should replace the gen3 menus. The Kanto
// tutorial and Battle Arena keep window sets too small for the BW artwork.
bool32 BattleUI_UsesInputBox(void);

const u32 *BattleUI_GetTextboxTiles(void);
const u16 *BattleUI_GetTextboxPalette(void);
const u32 *BattleUI_GetTextboxTilemap(void);

void BattleUI_PopulateActionBox(void);

void BattleUI_CreateCursorSprite(enum BattlerId);
void BattleUI_DestroyCursorSprite(void);
u32 BattleUI_GetCursorSpriteId(void);
void BattleUI_SetCursorSpriteId(u32);
void BattleUI_SetCursorMode(enum BWBattleUICursorMode);
enum BWBattleUICursorMode BattleUI_GetCursorMode(void);

void BattleUI_DisplayMoveBox(enum BattlerId);
const u8 *BattleUI_GetTypeEffectivenessSymbol(enum BattlerId, enum Move); // defined in src/battle_controller_player.c

#endif // GUARD_BW_BATTLE_UI_H
