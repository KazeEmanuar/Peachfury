#ifndef PLATFORM_DISPLACEMENT_H
#define PLATFORM_DISPLACEMENT_H

#include <PR/ultratypes.h>

#include "types.h"

void update_mario_platform(void);
void apply_platform_displacement(u32 isMario, struct Object *platform);
void apply_mario_platform_displacement(void);
#ifndef VERSION_JP
void clear_mario_platform(void);
#endif

#endif // PLATFORM_DISPLACEMENT_H
