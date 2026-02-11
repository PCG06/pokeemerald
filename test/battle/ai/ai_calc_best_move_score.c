#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"

AI_SINGLE_BATTLE_TEST("AI will not further increase Attack / Sp. Atk stat if it knows it faints to target: AI faster")
{
    u16 move;

    PARAMETRIZE { move = MOVE_HOWL; }
    PARAMETRIZE { move = MOVE_CALM_MIND; }

    GIVEN {
        ASSUME(gMovesInfo[MOVE_SKY_UPPERCUT].power == 85);
        ASSUME(gMovesInfo[MOVE_HOWL].effect == EFFECT_ATTACK_UP_USER_ALLY);
        ASSUME(gMovesInfo[MOVE_CALM_MIND].effect == EFFECT_CALM_MIND);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_COMBUSKEN) { Speed(15); Moves(MOVE_SKY_UPPERCUT, MOVE_CELEBRATE); };
        OPPONENT(SPECIES_KANGASKHAN) { Speed(20); Moves(MOVE_CHIP_AWAY, MOVE_SWIFT, move); }
    } WHEN {
        TURN { MOVE(player, MOVE_SKY_UPPERCUT); EXPECT_MOVE(opponent, move); }
        TURN { EXPECT_MOVE(opponent, MOVE_CHIP_AWAY); MOVE(player, MOVE_SKY_UPPERCUT); }
    }
}

AI_SINGLE_BATTLE_TEST("AI will not further increase Attack / Sp. Atk stat if it knows it faints to target: AI slower")
{
    u16 move;

    PARAMETRIZE { move = MOVE_HOWL; }
    PARAMETRIZE { move = MOVE_CALM_MIND; }

    GIVEN {
        ASSUME(gMovesInfo[MOVE_SKY_UPPERCUT].power == 85);
        ASSUME(gMovesInfo[MOVE_HOWL].effect == EFFECT_ATTACK_UP_USER_ALLY);
        ASSUME(gMovesInfo[MOVE_CALM_MIND].effect == EFFECT_CALM_MIND);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_COMBUSKEN) { Speed(20); Moves(MOVE_DOUBLE_KICK, MOVE_CELEBRATE); };
        OPPONENT(SPECIES_KANGASKHAN) { Speed(15); Moves(MOVE_CHIP_AWAY, MOVE_SWIFT, move); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_KICK); EXPECT_MOVE(opponent, move); }
        TURN { EXPECT_MOVE(opponent, MOVE_CHIP_AWAY); MOVE(player, MOVE_DOUBLE_KICK); }
    }
}

AI_SINGLE_BATTLE_TEST("AI will increase speed if it is slower")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_COMBUSKEN) { Speed(20); Moves(MOVE_DOUBLE_KICK, MOVE_CELEBRATE); };
        OPPONENT(SPECIES_KANGASKHAN) { Speed(15); Moves(MOVE_CHIP_AWAY, MOVE_AGILITY); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_KICK); EXPECT_MOVE(opponent, MOVE_AGILITY); }
        TURN { EXPECT_MOVE(opponent, MOVE_CHIP_AWAY); MOVE(player, MOVE_DOUBLE_KICK); }
    }
}

AI_SINGLE_BATTLE_TEST("AI will correctly predict what move foe mon going to use")
{
    u16 move;

    PARAMETRIZE { move = MOVE_HOWL; }
    PARAMETRIZE { move = MOVE_CALM_MIND; }

    GIVEN {
        ASSUME(gMovesInfo[MOVE_SKY_UPPERCUT].power == 85);
        ASSUME(gMovesInfo[MOVE_HOWL].effect == EFFECT_ATTACK_UP_USER_ALLY);
        ASSUME(gMovesInfo[MOVE_CALM_MIND].effect == EFFECT_CALM_MIND);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_COMBUSKEN) { Speed(15); Moves(MOVE_SKY_UPPERCUT, MOVE_DOUBLE_KICK, MOVE_FLAME_WHEEL, MOVE_CELEBRATE); };
        OPPONENT(SPECIES_KANGASKHAN) { Speed(20); Moves(MOVE_CHIP_AWAY, MOVE_SWIFT, move); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_KICK); EXPECT_MOVE(opponent, move); }
        TURN { EXPECT_MOVE(opponent, MOVE_CHIP_AWAY); MOVE(player, MOVE_SKY_UPPERCUT); }
    }
}

AI_SINGLE_BATTLE_TEST("AI will not use Throat Chop if opposing mon has a better move")
{
    GIVEN {
        ASSUME(gMovesInfo[MOVE_PSYCHIC_FANGS].power == 85);
        ASSUME(gMovesInfo[MOVE_THROAT_CHOP].power == 80);
        ASSUME(gMovesInfo[MOVE_DISARMING_VOICE].power == 40);
        ASSUME(gMovesInfo[MOVE_FLAME_BURST].power == 70);
        ASSUME(MoveHasAdditionalEffect(MOVE_THROAT_CHOP, MOVE_EFFECT_THROAT_CHOP) == TRUE);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_REGIROCK) { Speed(15); Moves(MOVE_DISARMING_VOICE, MOVE_FLAME_BURST); };
        OPPONENT(SPECIES_WOBBUFFET) { Speed(20); Moves(MOVE_THROAT_CHOP, MOVE_PSYCHIC_FANGS); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_PSYCHIC_FANGS); MOVE(player, MOVE_FLAME_BURST); }
        TURN { EXPECT_MOVE(opponent, MOVE_PSYCHIC_FANGS); MOVE(player, MOVE_DISARMING_VOICE); }
        TURN { EXPECT_MOVE(opponent, MOVE_PSYCHIC_FANGS); MOVE(player, MOVE_FLAME_BURST);}
    }
}

AI_SINGLE_BATTLE_TEST("AI will select Throat Chop if the sound move is the best damaging move from opposing mon")
{
    GIVEN {
        ASSUME(MoveHasAdditionalEffect(MOVE_THROAT_CHOP, MOVE_EFFECT_THROAT_CHOP) == TRUE);
        ASSUME(gMovesInfo[MOVE_PSYCHIC_FANGS].power == 85);
        ASSUME(gMovesInfo[MOVE_THROAT_CHOP].power == 80);
        ASSUME(gMovesInfo[MOVE_FLAME_BURST].power == 70);
        ASSUME(gMovesInfo[MOVE_HYPER_VOICE].power == 90);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_REGIROCK) { Speed(15); Moves(MOVE_HYPER_VOICE, MOVE_FLAME_BURST); };
        OPPONENT(SPECIES_WOBBUFFET) { Speed(20); Moves(MOVE_THROAT_CHOP, MOVE_PSYCHIC_FANGS); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_PSYCHIC_FANGS); MOVE(player, MOVE_FLAME_BURST); }
        TURN { EXPECT_MOVE(opponent, MOVE_PSYCHIC_FANGS); MOVE(player, MOVE_HYPER_VOICE); }
        TURN { EXPECT_MOVE(opponent, MOVE_THROAT_CHOP); MOVE(player, MOVE_HYPER_VOICE);}
    }
}

AI_SINGLE_BATTLE_TEST("Explosion interaction - glalie should correctly score crunch over EQ when high enough HP, or pick explosion when it's viable"){

    u32 hpVal;
    PARAMETRIZE { hpVal = 1; }
    PARAMETRIZE { hpVal = 138; }

    GIVEN{
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_CLOYSTER) {
            Level(44);
            HP(68); 
            Item(ITEM_SITRUS_BERRY); 
            Nature(NATURE_ADAMANT);
            Ability(ABILITY_SHELL_ARMOR);
            Moves(MOVE_DETECT, MOVE_RAZOR_SHELL, MOVE_ICICLE_SPEAR, MOVE_ICE_SHARD);
        }
        OPPONENT(SPECIES_GLALIE_MEGA) {
            Level(44);
            HP(hpVal);
            Nature(NATURE_JOLLY);
            Ability(ABILITY_REFRIGERATE);
            Moves(MOVE_RETURN, MOVE_EARTHQUAKE, MOVE_EXPLOSION, MOVE_CRUNCH);
        }
    } WHEN {
        TURN{
            MOVE(player, MOVE_RAZOR_SHELL);
            EXPECT_MOVE(opponent, hpVal == 1 ? MOVE_EXPLOSION : MOVE_EARTHQUAKE);
        }
    }
}

AI_SINGLE_BATTLE_TEST("Explosion interaction - glalie should correctly score crunch over EQ when high enough HP, or pick low sweep over explosion due to explosion negative effect"){

    u32 hpVal;
    PARAMETRIZE { hpVal = 1; }
    PARAMETRIZE { hpVal = 138; }

    GIVEN{
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_CLOYSTER) {
            Level(44);
            HP(68); 
            Item(ITEM_SITRUS_BERRY); 
            Nature(NATURE_ADAMANT);
            Ability(ABILITY_SHELL_ARMOR);
            Moves(MOVE_DETECT, MOVE_RAZOR_SHELL, MOVE_ICICLE_SPEAR, MOVE_ICE_SHARD);
        }
        OPPONENT(SPECIES_GLALIE_MEGA) {
            Level(44);
            HP(hpVal);
            Nature(NATURE_JOLLY);
            Ability(ABILITY_REFRIGERATE);
            Moves(MOVE_LOW_SWEEP, MOVE_EARTHQUAKE, MOVE_EXPLOSION, MOVE_CRUNCH);
        }
    } WHEN {
        TURN{
            MOVE(player, MOVE_RAZOR_SHELL);
            EXPECT_MOVE(opponent, hpVal == 1 ? MOVE_LOW_SWEEP : MOVE_LOW_SWEEP);
        }
    }
}

AI_SINGLE_BATTLE_TEST("Guaranteed secondary effect - AI should care about player's moveset for attack and special attack drops (spatk drop vs atk only set)"){
    GIVEN{
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_RUNERIGUS){
            Level(59);
            Speed(5);
            Nature(NATURE_RELAXED);
            Ability(ABILITY_SHADOW_SHIELD);
            Moves(MOVE_EARTHQUAKE, MOVE_SCARY_FACE);
        }
        OPPONENT(SPECIES_IRON_VALIANT){
            Level(59);
            Speed(10);
            Item(ITEM_BOOSTER_ENERGY);
            Nature(NATURE_JOLLY);
            Moves(MOVE_CLOSE_COMBAT, MOVE_SPIRIT_BREAK, MOVE_BULK_UP, MOVE_KNOCK_OFF);
        }
    } WHEN {
        TURN{
            MOVE(player, MOVE_SCARY_FACE);
            EXPECT_MOVE(opponent, MOVE_BULK_UP);
        }
    }
}

AI_SINGLE_BATTLE_TEST("Guaranteed secondary effect - AI should care about player's moveset for attack and special attack drops (atk drop vs spatk only set)"){
    GIVEN{
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_RUNERIGUS){
            Level(59);
            Speed(5);
            Nature(NATURE_RELAXED);
            Ability(ABILITY_SHADOW_SHIELD);
            Moves(MOVE_SHADOW_BALL, MOVE_SCARY_FACE);
        }
        OPPONENT(SPECIES_IRON_VALIANT){
            Level(59);
            Speed(10);
            Item(ITEM_BOOSTER_ENERGY);
            Nature(NATURE_JOLLY);
            Moves(MOVE_CLOSE_COMBAT, MOVE_LUNGE, MOVE_BULK_UP, MOVE_KNOCK_OFF);
        }
    } WHEN {
        TURN{
            MOVE(player, MOVE_SCARY_FACE);
            EXPECT_MOVE(opponent, MOVE_BULK_UP);
        }
    }
}

AI_SINGLE_BATTLE_TEST("MoveEffectInPlus - AI prefers stat drop to neutral move in 2+HKO cases"){
    GIVEN{
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_RUNERIGUS){
            Level(59);
            Speed(5);
            Nature(NATURE_RELAXED);
            Ability(ABILITY_SHADOW_SHIELD);
            Moves(MOVE_SHADOW_BALL, MOVE_SCARY_FACE);
        }
        OPPONENT(SPECIES_IRON_VALIANT){
            Level(59);
            Speed(10);
            Item(ITEM_BOOSTER_ENERGY);
            Nature(NATURE_JOLLY);
            Moves(MOVE_LIQUIDATION, MOVE_AQUA_TAIL);
        }
    } WHEN {
        TURN{
            MOVE(player, MOVE_SCARY_FACE);
            EXPECT_MOVE(opponent, MOVE_LIQUIDATION);
        }
    }
}

AI_SINGLE_BATTLE_TEST("HasMoveToStopSetup - AI should not see self-targeted speed drops as preventing setup moves in 2hko cases"){
    u16 move;
    PARAMETRIZE { move = MOVE_EARTHQUAKE; }
    PARAMETRIZE { move = MOVE_BULLDOZE; }

    GIVEN{
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_RHYDON){
            Level(100);
            Nature(NATURE_ADAMANT);
            Item(ITEM_EVIOLITE);
            Speed(1);
            Ability(ABILITY_LIGHTNING_ROD);
            Moves(MOVE_HAMMER_ARM, move);
        }
        OPPONENT(SPECIES_GRIMMSNARL){
            Level(100);
            Nature(NATURE_JOLLY);
            Ability(ABILITY_INFILTRATOR);
            Speed(2);
            HP(331);
            Moves(MOVE_NASTY_PLOT, MOVE_AURA_SPHERE);
        }
    } WHEN {
        TURN{
            MOVE(player, MOVE_HAMMER_ARM);
            EXPECT_MOVE(opponent, move == MOVE_EARTHQUAKE ? MOVE_NASTY_PLOT : MOVE_AURA_SPHERE);
        }
    }
}

AI_SINGLE_BATTLE_TEST("Rapid Spin should prevent secondary hazard effect moves from getting a setup score boost")
{
    ASSUME(MoveHasAdditionalEffect(MOVE_STONE_AXE, MOVE_EFFECT_STEALTH_ROCK) == TRUE);
    ASSUME(MoveHasAdditionalEffectSelf(MOVE_RAPID_SPIN, MOVE_EFFECT_RAPID_SPIN) == TRUE);
    ASSUME(gMovesInfo[MOVE_X_SCISSOR].power == 80);
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_BLASTOISE){ Level(56); Nature(NATURE_ADAMANT); Ability(ABILITY_TORRENT); Item(ITEM_MYSTIC_WATER); Speed(109); Moves(MOVE_FAKE_OUT, MOVE_FLIP_TURN, MOVE_SCALD, MOVE_RAPID_SPIN); }
        OPPONENT(SPECIES_KLEAVOR){ Level(54); Nature(NATURE_JOLLY); Ability(ABILITY_SHARPNESS); Item(ITEM_FOCUS_SASH); Speed(124); Moves(MOVE_STONE_AXE, MOVE_X_SCISSOR, MOVE_ACCELEROCK, MOVE_NIGHT_SLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLIP_TURN); EXPECT_MOVE(opponent, MOVE_X_SCISSOR); }
    }
}

AI_SINGLE_BATTLE_TEST("Clangorous Soul - gets best move boost when player does under 67 pct damage")
{
    u16 move;
    PARAMETRIZE { move = MOVE_SOLAR_BEAM; }
    PARAMETRIZE { move = MOVE_AIR_SLASH; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_CHARIZARD){ Level(80); Nature(NATURE_TIMID); Ability(ABILITY_BLAZE); Item(ITEM_CHARIZARDITE_Y); Speed(221); Moves(MOVE_FLAMETHROWER, move); }
        OPPONENT(SPECIES_KOMMO_O){ Level(80); Nature(NATURE_TIMID); Ability(ABILITY_OVERCOAT); Item(ITEM_THROAT_SPRAY); Speed(181); Moves(MOVE_CLANGOROUS_SOUL, MOVE_CLANGING_SCALES, MOVE_AURA_SPHERE, MOVE_FLASH_CANNON); }
    } WHEN {
        TURN { MOVE(player, move); EXPECT_MOVE(opponent, move == MOVE_AIR_SLASH ? MOVE_CLANGING_SCALES : MOVE_CLANGOROUS_SOUL); }
    }
}

AI_SINGLE_BATTLE_TEST("Clangorous Soul - unusable under 33 pct HP")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_CHARIZARD){ Level(80); Nature(NATURE_TIMID); Ability(ABILITY_BLAZE); Item(ITEM_CHARIZARDITE_Y); Speed(221); Moves(MOVE_FLAMETHROWER); }
        OPPONENT(SPECIES_KOMMO_O){ Level(80); MaxHP(234); HP(76); Nature(NATURE_TIMID); Ability(ABILITY_OVERCOAT); Item(ITEM_THROAT_SPRAY); Speed(181); Moves(MOVE_CLANGOROUS_SOUL, MOVE_CLANGING_SCALES, MOVE_AURA_SPHERE, MOVE_FLASH_CANNON); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLAMETHROWER); EXPECT_MOVE(opponent, MOVE_CLANGING_SCALES); }
    }
}

AI_SINGLE_BATTLE_TEST("Clangorous Soul - unaware prevents score boost")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_CLODSIRE){ Level(80); Nature(NATURE_IMPISH); Ability(ABILITY_UNAWARE); Item(ITEM_BLACK_SLUDGE); Speed(61); Moves(MOVE_POISON_JAB); }
        OPPONENT(SPECIES_KOMMO_O){ Level(80); MaxHP(234); Nature(NATURE_TIMID); Ability(ABILITY_OVERCOAT); Item(ITEM_THROAT_SPRAY); Speed(181); Moves(MOVE_CLANGOROUS_SOUL, MOVE_CLANGING_SCALES, MOVE_AURA_SPHERE, MOVE_FLASH_CANNON); }
    } WHEN {
        TURN { MOVE(player, MOVE_POISON_JAB); EXPECT_MOVE(opponent, MOVE_CLANGING_SCALES); }
    }
}

AI_SINGLE_BATTLE_TEST("Belly Drum - physical move >50 pct damage gets blocked by ice face, allowing setup")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_EMBOAR){ Level(44); Nature(NATURE_BRAVE); Ability(ABILITY_BLAZE); Item(ITEM_IRON_BALL); HP(164); MaxHP(164); Speed(33); Moves(MOVE_FLAME_WHEEL); }
        OPPONENT(SPECIES_EISCUE){ Level(42); Nature(NATURE_JOLLY); Ability(ABILITY_ICE_FACE); Item(ITEM_SALAC_BERRY); HP(128); MaxHP(128); Speed(66); Moves(MOVE_BELLY_DRUM, MOVE_ICICLE_CRASH, MOVE_LIQUIDATION); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLAME_WHEEL); EXPECT_MOVE(opponent, MOVE_BELLY_DRUM); }
    }
}

AI_SINGLE_BATTLE_TEST("Belly Drum - special move vs ice face active correctly handled")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_EMBOAR){ Level(44); Nature(NATURE_ADAMANT); Ability(ABILITY_BLAZE); Item(ITEM_MUSCLE_BAND); HP(164); MaxHP(164); Speed(75); Moves(MOVE_FLAMETHROWER); }
        OPPONENT(SPECIES_EISCUE){ Level(42); Nature(NATURE_JOLLY); Ability(ABILITY_ICE_FACE); Item(ITEM_SALAC_BERRY); HP(128); MaxHP(128); Speed(66); Moves(MOVE_BELLY_DRUM, MOVE_ICICLE_CRASH, MOVE_LIQUIDATION); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLAMETHROWER); EXPECT_MOVE(opponent, MOVE_LIQUIDATION); }
    }
}

AI_SINGLE_BATTLE_TEST("Belly Drum - physical move >50pct damage vs ice face already used")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_EMBOAR){ Level(44); Nature(NATURE_ADAMANT); Ability(ABILITY_BLAZE); Item(ITEM_MUSCLE_BAND); HP(164); MaxHP(164); Speed(75); Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_EISCUE_NOICE){ Level(42); Nature(NATURE_JOLLY); Ability(ABILITY_ICE_FACE); Item(ITEM_SALAC_BERRY); HP(128); MaxHP(128); Speed(139); Moves(MOVE_BELLY_DRUM, MOVE_ICICLE_CRASH, MOVE_LIQUIDATION); }
    } WHEN {
        TURN { MOVE(player, MOVE_EARTHQUAKE); EXPECT_MOVE(opponent, MOVE_LIQUIDATION); }
    }
}

AI_SINGLE_BATTLE_TEST("Fillet Away AI handling")
{
    u16 move;
    PARAMETRIZE { move = MOVE_SCALD; }
    PARAMETRIZE { move = MOVE_THUNDERBOLT; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SLOWBRO){ Level(100); Nature(NATURE_BOLD); Ability(ABILITY_REGENERATOR); Speed(96); Moves(move); }
        OPPONENT(SPECIES_VELUZA){ Level(100); Nature(NATURE_ADAMANT); Ability(ABILITY_SHARPNESS); Speed(176); Moves(MOVE_FILLET_AWAY, MOVE_AQUA_CUTTER); }
    } WHEN {
        TURN { MOVE(player, move); EXPECT_MOVE(opponent, move == MOVE_SCALD ? MOVE_FILLET_AWAY : MOVE_AQUA_CUTTER); }
    }
}

AI_SINGLE_BATTLE_TEST("Solar beam should get ignored at AI_CompareDamagingMoves and correctly select least hits to KO otherwise instead of randomly between all moves")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_PINCURCHIN){ Level(85); Nature(NATURE_SASSY); Ability(ABILITY_LIGHTNING_ROD); Item(ITEM_ASSAULT_VEST); Speed(50); Moves(MOVE_VOLT_SWITCH, MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_CHARIZARD){ Level(85); Nature(NATURE_TIMID); Ability(ABILITY_BLAZE); Item(ITEM_CHARIZARDITE_Y); Speed(221); Moves(MOVE_FLAMETHROWER, MOVE_AIR_SLASH, MOVE_SOLAR_BEAM, MOVE_DRAGON_PULSE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDERBOLT); EXPECT_MOVE(opponent, MOVE_DRAGON_PULSE); }
    }
}

AI_SINGLE_BATTLE_TEST("CompareMoveSpeeds should ignore fake out and status moves for AI_CompareDamagingMoves BEST_DAMAGE_MOVE scoring")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_BLAZIKEN){ Level(44); Nature(NATURE_ADAMANT); Ability(ABILITY_STRIKER); Speed(89); HP(20); Moves(MOVE_DETECT, MOVE_TRIPLE_ARROWS); }
        OPPONENT(SPECIES_KLEAVOR){ Level(43); Nature(NATURE_ADAMANT); Ability(ABILITY_SOLID_ROCK); Speed(80); Moves(MOVE_ICE_PUNCH, MOVE_ACCELEROCK); }
    } WHEN {
        TURN { MOVE(player, MOVE_DETECT); EXPECT_MOVE(opponent, MOVE_ACCELEROCK); }
        TURN { 
            MOVE(player, MOVE_TRIPLE_ARROWS);
            SCORE_EQ_VAL(opponent, MOVE_ACCELEROCK, (AI_SCORE_DEFAULT + BEST_DAMAGE_MOVE + FAST_KILL)); 
            SCORE_EQ_VAL(opponent, MOVE_ICE_PUNCH, (AI_SCORE_DEFAULT + SLOW_KILL)); 
        }
    }
}

AI_SINGLE_BATTLE_TEST("sheer force moveeffectinplus")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_VOLCANION){ Level(85); Nature(NATURE_TIMID); Ability(ABILITY_WATER_ABSORB); Speed(165); Moves(MOVE_FLAMETHROWER); }
        OPPONENT(SPECIES_GRIMMSNARL_MEGA){ Level(85); Nature(NATURE_QUIET); Ability(ABILITY_SHEER_FORCE); Item(ITEM_GRIMMITE); Speed(96); Moves(MOVE_ACID_SPRAY, MOVE_MOONBLAST, MOVE_AURA_SPHERE); }
    } WHEN {
        TURN { 
            MOVE(player, MOVE_FLAMETHROWER);
            SCORE_EQ_VAL(opponent, MOVE_ACID_SPRAY, 100);
            SCORE_EQ_VAL(opponent, MOVE_MOONBLAST, 101);
            SCORE_EQ_VAL(opponent, MOVE_AURA_SPHERE, 101);
        }
    }
}


AI_SINGLE_BATTLE_TEST("status chance are not positive effect on kills")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_VOLCANION){ Level(85); HP(1); Nature(NATURE_TIMID); Ability(ABILITY_WATER_ABSORB); Speed(165); Moves(MOVE_FLAMETHROWER); }
        OPPONENT(SPECIES_GRIMMSNARL){ Level(85); Nature(NATURE_QUIET); Ability(ABILITY_PRANKSTER); Speed(96); Moves(MOVE_THUNDERBOLT, MOVE_MOONBLAST, MOVE_POWER_UP_PUNCH); }
    } WHEN {
        TURN { 
            MOVE(player, MOVE_FLAMETHROWER);
            SCORE_EQ_VAL(opponent, MOVE_THUNDERBOLT, 104);
            SCORE_EQ_VAL(opponent, MOVE_MOONBLAST, 104);
            SCORE_EQ_VAL(opponent, MOVE_POWER_UP_PUNCH, 105);
        }
    }
}

AI_SINGLE_BATTLE_TEST("surf is positive effect for cramorant")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_BLAZIKEN){ Level(44); Nature(NATURE_ADAMANT); Ability(ABILITY_STRIKER); Speed(89); HP(20); Moves(MOVE_DETECT, MOVE_TRIPLE_ARROWS); }
        OPPONENT(SPECIES_CRAMORANT){ Level(85); Nature(NATURE_QUIET); Ability(ABILITY_GULP_MISSILE); Speed(96); Moves(MOVE_SURF, MOVE_SCALD); }
    } WHEN {
        TURN { 
            MOVE(player, MOVE_TRIPLE_ARROWS);
            SCORE_EQ_VAL(opponent, MOVE_SURF, 107);
            SCORE_EQ_VAL(opponent, MOVE_SCALD, 106);
        }
    }
}

AI_SINGLE_BATTLE_TEST("Player EFFECT_EXPLOSION ignored for last chance calculations")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ELECTRODE){ Level(100); Moves(MOVE_EXPLOSION, MOVE_THUNDER_SHOCK); }
        OPPONENT(SPECIES_ZIGZAGOON){ Level(100); Moves(MOVE_SUCKER_PUNCH, MOVE_HEADBUTT); }
    } WHEN {
        TURN { 
            MOVE(player, MOVE_THUNDER_SHOCK);
            SCORE_EQ_VAL(opponent, MOVE_SUCKER_PUNCH, 100);
            SCORE_EQ_VAL(opponent, MOVE_HEADBUTT, 101);
        }
    }
}

AI_SINGLE_BATTLE_TEST("Player EFFECT_EXPLOSION ignored for destiny bond calculations")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ELECTRODE){ Level(100); Moves(MOVE_EXPLOSION, MOVE_THUNDER_SHOCK); Speed(100); }
        OPPONENT(SPECIES_ZIGZAGOON){ Level(100); Moves(MOVE_HEADBUTT, MOVE_DESTINY_BOND); Speed(200); }
    } WHEN {
        TURN { 
            MOVE(player, MOVE_THUNDER_SHOCK);
            SCORE_EQ_VAL(opponent, MOVE_HEADBUTT, 101);
            SCORE_EQ_VAL(opponent, MOVE_DESTINY_BOND, 100);
        }
    }
}

AI_SINGLE_BATTLE_TEST("Player EFFECT_EXPLOSION ignored for ShouldTryToFlinch cases")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ELECTRODE){ Level(100); Moves(MOVE_EXPLOSION, MOVE_THUNDER_SHOCK); Speed(100); }
        OPPONENT(SPECIES_ZIGZAGOON){ Level(100); Moves(MOVE_DIZZY_PUNCH, MOVE_HEADBUTT); Speed(200); }
    } WHEN {
        TURN { 
            MOVE(player, MOVE_THUNDER_SHOCK);
            SCORE_EQ_VAL(opponent, MOVE_HEADBUTT, 100);
            SCORE_EQ_VAL(opponent, MOVE_DIZZY_PUNCH, 101);
        }
    }
}

AI_SINGLE_BATTLE_TEST("Player EFFECT_EXPLOSION ignored for setup calculations")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ELECTRODE){ Level(100); Moves(MOVE_EXPLOSION, MOVE_TACKLE); }
        OPPONENT(SPECIES_ZIGZAGOON){ Level(100); Moves(MOVE_SWORDS_DANCE, MOVE_HEADBUTT); }
    } WHEN {
        TURN { 
            MOVE(player, MOVE_TACKLE);
            SCORE_EQ_VAL(opponent, MOVE_SWORDS_DANCE, 103);
        }
    }
}
