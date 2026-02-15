#include "global.h"
#include "test/battle.h"


ASSUMPTIONS
{
    ASSUME(gMovesInfo[MOVE_QUICK_ATTACK].priority > 0);
}

DOUBLE_BATTLE_TEST("Dazzling, Queenly Majesty and Armor Tail protect the user from priority moves")
{
    u32 species, ability;

    PARAMETRIZE { species = SPECIES_BRUXISH; ability = ABILITY_DAZZLING; }
    PARAMETRIZE { species = SPECIES_FARIGIRAF; ability = ABILITY_ARMOR_TAIL; }
    PARAMETRIZE { species = SPECIES_TSAREENA; ability = ABILITY_QUEENLY_MAJESTY; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(species) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_QUICK_ATTACK, target: opponentLeft); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, opponentRight);
        ABILITY_POPUP(opponentLeft, ability);
        MESSAGE("Wobbuffet cannot use Quick Attack!");
    }
}

DOUBLE_BATTLE_TEST("Dazzling, Queenly Majesty and Armor Tail protect users partner from priority moves")
{
    u32 species, ability;

    PARAMETRIZE { species = SPECIES_BRUXISH; ability = ABILITY_DAZZLING; }
    PARAMETRIZE { species = SPECIES_FARIGIRAF; ability = ABILITY_ARMOR_TAIL; }
    PARAMETRIZE { species = SPECIES_TSAREENA; ability = ABILITY_QUEENLY_MAJESTY; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(species) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_QUICK_ATTACK, target: opponentRight); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, opponentRight);
        ABILITY_POPUP(opponentLeft, ability);
        MESSAGE("Wobbuffet cannot use Quick Attack!");
    }
}

AI_DOUBLE_BATTLE_TEST("Dazzling/Queenly Majesty/Armor Tail - switch-in calcs should no longer break on-field scoring due to aiCalcInProgress flip and Slither Wing stays in")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);

        PLAYER(SPECIES_RHYPERIOR){ Level(100); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_VESPIQUEN){ Level(100); Moves(MOVE_LUNGE); }

        OPPONENT(SPECIES_SLITHER_WING){ Level(100); Moves(MOVE_FIRST_IMPRESSION, MOVE_FLARE_BLITZ); Nature(NATURE_BRAVE); Item(ITEM_LIFE_ORB); }
        OPPONENT(SPECIES_SHIFTRY){ Level(100); Moves(MOVE_LEAF_STORM); }

        OPPONENT(SPECIES_CINDERACE) { Level(100); Moves(MOVE_PYRO_BALL); }
    } WHEN {
        TURN { 
            MOVE(playerLeft, MOVE_TACKLE, target:opponentLeft);
            MOVE(playerRight, MOVE_LUNGE, target:opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_FLARE_BLITZ, target:playerRight);
            EXPECT_MOVE(opponentRight, MOVE_LEAF_STORM, target:playerLeft);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("Dazzling/Queenly Majesty/Armor Tail - move slot order shouldn't matter")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);

        PLAYER(SPECIES_RHYPERIOR){ Level(100); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_VESPIQUEN){ Level(100); Moves(MOVE_LUNGE); }

        OPPONENT(SPECIES_SLITHER_WING){ Level(100); Moves(MOVE_FLARE_BLITZ, MOVE_FIRST_IMPRESSION); Nature(NATURE_BRAVE); Item(ITEM_LIFE_ORB); }
        OPPONENT(SPECIES_SHIFTRY){ Level(100); Moves(MOVE_LEAF_STORM); }

        OPPONENT(SPECIES_CINDERACE) { Level(100); Moves(MOVE_PYRO_BALL); }
    } WHEN {
        TURN { 
            MOVE(playerLeft, MOVE_TACKLE, target:opponentLeft);
            MOVE(playerRight, MOVE_LUNGE, target:opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_FLARE_BLITZ, target:playerRight);
            EXPECT_MOVE(opponentRight, MOVE_LEAF_STORM, target:playerLeft);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("Dazzling/Queenly Majesty/Armor Tail - with only first impression, Slither Wing should instead switch out")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);

        PLAYER(SPECIES_RHYPERIOR){ Level(100); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_VESPIQUEN){ Level(100); Moves(MOVE_LUNGE); }

        OPPONENT(SPECIES_SLITHER_WING){ Level(100); Moves(MOVE_FIRST_IMPRESSION); Nature(NATURE_BRAVE); Item(ITEM_LIFE_ORB); }
        OPPONENT(SPECIES_SHIFTRY){ Level(100); Moves(MOVE_LEAF_STORM); }

        OPPONENT(SPECIES_CINDERACE) { Level(100); Moves(MOVE_PYRO_BALL); }
    } WHEN {
        TURN { 
            MOVE(playerLeft, MOVE_TACKLE, target:opponentLeft);
            MOVE(playerRight, MOVE_LUNGE, target:opponentRight);
            EXPECT_SWITCH(opponentLeft, 2);
            EXPECT_MOVE(opponentRight, MOVE_LEAF_STORM, target:playerLeft);
        }
    }
}
