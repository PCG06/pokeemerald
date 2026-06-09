#include "global.h"
#include "event_data.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Electro Boost grants a Charge boost and +1 Sp. Def to Electric-types")
{
    s16 damage[2] = {0};
    SetStartingStatus(STARTING_STATUS_ELECTRO_BOOST);

    GIVEN {
        PLAYER(SPECIES_PIKACHU);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_SHOCK); }
        TURN { MOVE(player, MOVE_THUNDER_SHOCK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_SHOCK, player);
        HP_BAR(opponent, captureDamage: &damage[0]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_SHOCK, player);
        HP_BAR(opponent, captureDamage: &damage[1]);
    } THEN {
        EXPECT_EQ(player->volatiles.chargeTimer > 0, TRUE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        EXPECT_MUL_EQ(damage[0], Q_4_12(2.0), damage[1]);
        ResetStartingStatuses();
    }
}

SINGLE_BATTLE_TEST("Electro Boost doesn't charge the user if it has used Charge the same turn")
{
    SetStartingStatus(STARTING_STATUS_ELECTRO_BOOST);

    GIVEN {
        PLAYER(SPECIES_PIKACHU) { Speed(5); }
        OPPONENT(SPECIES_PIKACHU) { Speed(4); }
    } WHEN {
        TURN { MOVE(player, MOVE_CHARGE); }
        TURN { MOVE(player, MOVE_THUNDER_SHOCK); MOVE(opponent, MOVE_THUNDER_SHOCK);}
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
        NOT MESSAGE("Pikachu was charged up!");
        MESSAGE("The opposing Pikachu was charged up!");

        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_SHOCK, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_SHOCK, opponent);

        MESSAGE("Pikachu was charged up!");
        MESSAGE("The opposing Pikachu was charged up!");
    } THEN {
        ResetStartingStatuses();
    }
}
