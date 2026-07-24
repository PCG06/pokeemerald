// const data

// textbox
static const u32 sBWBattleUI_TextboxTiles[] = INCGFX_U32("graphics/battle_interface/bw/textbox.png", ".4bpp.smol");
static const u16 sBWBattleUI_TextboxPalette[] = INCBIN_U16("graphics/battle_interface/bw/textbox.gbapal");
static const u32 sBWBattleUI_TextboxTilemap[] = INCBIN_U32("graphics/battle_interface/bw/textbox.bin.smolTM");

// inputbox->actionbox
static const u8 sBWBattleUI_ActionBox[] = INCGFX_U8("graphics/battle_interface/bw/actionbox.png", ".4bpp");
static const u8 *const sBWBattleUI_ActionBoxFields[][BUI_ACTION_BOX_ENTRY_COUNT] =
{
    // left-top -> right-bottom
    { // default
        COMPOUND_STRING("FIGHT!"),  COMPOUND_STRING("BAG"),
        COMPOUND_STRING("POKÉMON"), COMPOUND_STRING("RUN"),
    },
    { // safari
        COMPOUND_STRING("BALL!"),    COMPOUND_STRING("{POKEBLOCK}"),
        COMPOUND_STRING("GO NEAR"),  COMPOUND_STRING("RUN"),
    },
};

// inputbox->movebox
// Built as four 14x3 metatiles, one per panel, so each window can be filled
// with a single contiguous tile copy.
static const u8 sBWBattleUI_MoveBoxGraphics[] = INCGFX_U8("graphics/battle_interface/bw/movebox.png", ".4bpp", "-mwidth 14 -mheight 3");
static const u8 sBWBattleUI_MoveBoxGraphicsZ[] = INCGFX_U8("graphics/battle_interface/bw/movebox_z.png", ".4bpp");
static const u16 sBWBattleUI_MoveBoxPalette[] = INCGFX_U16("graphics/battle_interface/bw/movebox.png", ".gbapal");
static const u16 sBWBattleUI_MoveBoxTypePalettes[] = INCGFX_U16("graphics/battle_interface/bw/movebox_types.png", ".gbapal");

// inputbox->cursor
static const u32 sBWBattleUI_CursorGfx[] = INCGFX_U32("graphics/battle_interface/bw/cursor.png", ".4bpp.smol", "-mwidth 2 -mheight 2");
static const u16 sBWBattleUI_CursorPalette[] = INCGFX_U16("graphics/battle_interface/bw/cursor.png", ".gbapal");

// The cursor is a single sprite whose four corners are pulled apart far enough
// to frame whichever box it currently sits on.
static const struct Subsprite sBWBattleUI_ActionCursorSubsprite[] =
{
    {
        .x = 0,
        .y = 0,
        .shape = SPRITE_SHAPE(16x16),
        .size = SPRITE_SIZE(16x16),
        .tileOffset = 0,
    },
    {
        .x = BUI_ACTION_CURSOR_MAX_X,
        .y = 0,
        .shape = SPRITE_SHAPE(16x16),
        .size = SPRITE_SIZE(16x16),
        .tileOffset = 4,
    },
    {
        .x = 0,
        .y = BUI_ACTION_CURSOR_MAX_Y,
        .shape = SPRITE_SHAPE(16x16),
        .size = SPRITE_SIZE(16x16),
        .tileOffset = 8,
    },
    {
        .x = BUI_ACTION_CURSOR_MAX_X,
        .y = BUI_ACTION_CURSOR_MAX_Y,
        .shape = SPRITE_SHAPE(16x16),
        .size = SPRITE_SIZE(16x16),
        .tileOffset = 12,
    },
};

static const struct Subsprite sBWBattleUI_MoveCursorSubsprite[] =
{
    {
        .x = 0,
        .y = 0,
        .shape = SPRITE_SHAPE(16x16),
        .size = SPRITE_SIZE(16x16),
        .tileOffset = 0,
        .priority = 0
    },
    {
        .x = BUI_MOVE_CURSOR_MAX_X,
        .y = 0,
        .shape = SPRITE_SHAPE(16x16),
        .size = SPRITE_SIZE(16x16),
        .tileOffset = 4,
        .priority = 0
    },
    {
        .x = 0,
        .y = BUI_MOVE_CURSOR_MAX_Y,
        .shape = SPRITE_SHAPE(16x16),
        .size = SPRITE_SIZE(16x16),
        .tileOffset = 8,
        .priority = 0
    },
    {
        .x = BUI_MOVE_CURSOR_MAX_X,
        .y = BUI_MOVE_CURSOR_MAX_Y,
        .shape = SPRITE_SHAPE(16x16),
        .size = SPRITE_SIZE(16x16),
        .tileOffset = 12,
        .priority = 0
    },
};

static const struct Subsprite sBWBattleUI_ZMoveCursorSubsprite[] =
{
    {
        .x = 0,
        .y = 0,
        .shape = SPRITE_SHAPE(16x16),
        .size = SPRITE_SIZE(16x16),
        .tileOffset = 0,
        .priority = 0
    },
    {
        .x = BUI_Z_MOVE_CURSOR_MAX_X,
        .y = 0,
        .shape = SPRITE_SHAPE(16x16),
        .size = SPRITE_SIZE(16x16),
        .tileOffset = 4,
        .priority = 0
    },
    {
        .x = 0,
        .y = BUI_Z_MOVE_CURSOR_MAX_Y,
        .shape = SPRITE_SHAPE(16x16),
        .size = SPRITE_SIZE(16x16),
        .tileOffset = 8,
        .priority = 0
    },
    {
        .x = BUI_Z_MOVE_CURSOR_MAX_X,
        .y = BUI_Z_MOVE_CURSOR_MAX_Y,
        .shape = SPRITE_SHAPE(16x16),
        .size = SPRITE_SIZE(16x16),
        .tileOffset = 12,
        .priority = 0
    },
};

static const struct SubspriteTable sBWBattleUI_CursorSubspritesTable[] =
{
    { ARRAY_COUNT(sBWBattleUI_ActionCursorSubsprite), sBWBattleUI_ActionCursorSubsprite },
    { ARRAY_COUNT(sBWBattleUI_MoveCursorSubsprite),   sBWBattleUI_MoveCursorSubsprite },
    { ARRAY_COUNT(sBWBattleUI_ZMoveCursorSubsprite),  sBWBattleUI_ZMoveCursorSubsprite },
    { }
};

static const struct SpriteTemplate sBWBattleUI_CursorTemplate =
{
    .tileTag = TAG_CURSOR,
    .paletteTag = TAG_CURSOR,
    .oam = &(struct OamData){
        .shape = SPRITE_SHAPE(8x8),
        .size = SPRITE_SIZE(8x8),
    },
    .anims = (const union AnimCmd *const[]){
        [0] = (const union AnimCmd[]){
            ANIMCMD_FRAME( 0, 16),
            ANIMCMD_FRAME(16, 16),
            ANIMCMD_FRAME(32, 16),
            ANIMCMD_JUMP(0),
        },
    },
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_BattleUICursor,
};

static const union TextColor sBWBattleUI_TextColors[NUM_BUI_TXTCLRS] =
{
    [BUI_TXTCLR_MOVE_BOX] =
    {
        .foreground = 13,
        .shadow = 15,
    },
    [BUI_TXTCLR_ABOX_1] =
    {
        .foreground = 4,
        .accent = 2,
        .shadow = 9,
    },
    [BUI_TXTCLR_ABOX_2] =
    {
        .foreground = 14,
        .accent = 5,
        .shadow = 9,
    },
    [BUI_TXTCLR_ABOX_3] =
    {
        .foreground = 11,
        .accent = 10,
        .shadow = 9,
    },
    [BUI_TXTCLR_ABOX_4] =
    {
        .foreground = 12,
        .accent = 8,
        .shadow = 9,
    },
};
