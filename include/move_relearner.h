#ifndef GUARD_MOVE_RELEARNER_H
#define GUARD_MOVE_RELEARNER_H

void TeachMoveRelearnerMove(void);
void MoveRelearnerShowHideHearts(s32);
void MoveRelearnerShowHideCategoryIcon(s32);

#define MAX_RELEARNER_MOVES max(MAX_LEVEL_UP_MOVES, 100)

#endif //GUARD_MOVE_RELEARNER_H
