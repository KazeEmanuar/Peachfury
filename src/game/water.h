#ifndef WATER_H
#define WATER_H

#include <ultra64.h>

//Water interface
void water_scroll();
void water_render();
const u8 *water_get_src();
void water_renderI4();
const u8 *water_get_srcI4();

#endif
