#ifndef GUARD_CONSTANTS_BW_BATTLE_UI_H
#define GUARD_CONSTANTS_BW_BATTLE_UI_H

// constants
#ifndef TILE_TO_PIXELS
#define TILE_TO_PIXELS(t)  ((t) ? (t * 8) : 0)
#define PIXELS_TO_TILES(p) ((p) ? (p / 8) : 0)
#endif

#define BUI_ACTION_BOX_ENTRY_COUNT  (4)

// The action box and move box sheets are copied into their windows as raw 4bpp
// tiles, so these are the tile counts of each piece rather than pixel sizes.
#define BUI_ACTION_BOX_TILES        (17 * 6)
#define BUI_MOVE_PANEL_TILE_WIDTH   (14)
#define BUI_MOVE_PANEL_TILE_HEIGHT  (3)
#define BUI_MOVE_PANEL_TILES        (BUI_MOVE_PANEL_TILE_WIDTH * BUI_MOVE_PANEL_TILE_HEIGHT)
#define BUI_MOVE_BOX_TILE_WIDTH     (28)
#define BUI_MOVE_BOX_TILES          (BUI_MOVE_BOX_TILE_WIDTH * 6)

// Distance between the two halves of each cursor frame. The cursor is one
// sprite built from four 16x16 corners, so these are the corner offsets.
#define BUI_ACTION_CURSOR_MAX_X     (TILE_TO_PIXELS( 7) - 3)
#define BUI_ACTION_CURSOR_MAX_Y     (TILE_TO_PIXELS( 2) - 4)

#define BUI_MOVE_CURSOR_MAX_X       (TILE_TO_PIXELS(13) - 2)
#define BUI_MOVE_CURSOR_MAX_Y       (TILE_TO_PIXELS( 2) - 3)

#define BUI_Z_MOVE_CURSOR_MAX_X     (TILE_TO_PIXELS(27) - 2)
#define BUI_Z_MOVE_CURSOR_MAX_Y     (TILE_TO_PIXELS( 5) - 3)

#define BUI_CURSOR_CONVERT_FLAG     (15)
#define sCursorMode                 data[0]
#define sBattler                    data[1]

enum BattleUISpriteTags
{
    TAG_CURSOR = 0x9999,
};

enum BattleUITextColors
{
    BUI_TXTCLR_MOVE_BOX,
    // action box (fight!, bag, etc)
    BUI_TXTCLR_ABOX_1, // red
    BUI_TXTCLR_ABOX_2, // yellow
    BUI_TXTCLR_ABOX_3, // green
    BUI_TXTCLR_ABOX_4, // blue

    NUM_BUI_TXTCLRS,
};

#endif // GUARD_CONSTANTS_BW_BATTLE_UI_H
