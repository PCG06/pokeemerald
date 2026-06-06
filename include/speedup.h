#ifndef GUARD_SPEEDUP_H
#define GUARD_SPEEDUP_H

// Hedara's speed-up for Pokemon Recordskeeper.
// Goes up til max speed of 10x.
// Limited to 8x in option menu.

void StartSpeedup(void);
void StopSpeedup(void);
bool32 SpeedupShouldSkip(void);

#endif // GUARD_SPEEDUP_H
