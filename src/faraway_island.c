#include "global.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_weather.h"
#include "fieldmap.h"
#include "metatile_behavior.h"
#include "sprite.h"
#include "constants/event_objects.h"
#include "constants/field_effects.h"
#include "constants/metatile_behaviors.h"

extern const struct SpritePalette gSpritePalette_GeneralFieldEffect1;
extern const struct SpriteTemplate *const gFieldEffectObjectTemplatePointers[];

static const s16 sFarawayIslandRockCoords[4][2] =
{
    {14 + MAP_OFFSET,  9 + MAP_OFFSET},
    {18 + MAP_OFFSET,  9 + MAP_OFFSET},
    { 9 + MAP_OFFSET, 10 + MAP_OFFSET},
    {13 + MAP_OFFSET, 13 + MAP_OFFSET},
};
