#include "sm64.h"
#include "types.h"
//#include "game/game.h"
#include "game/level_update.h"
#include "dialog_ids.h"
//#include "game/kaze_library.h"
#include "seq_ids.h"
#include "game/camera.h"
#include "engine/math_util.h"
#include "game/obj_behaviors_2.h"
#include "game/object_helpers.h"
#include "levels/wf/header.h"

#define m gMarioState
#define o gCurrentObject

void donutcode(void) {
    if (!o->oOpacity) {
        cur_obj_update_floor_height();
        o->oOpacity = 1;
    }
    switch (o->oAction) {
        case 0:
            if (cur_obj_is_mario_on_platform()) {
                o->oAction++;
            }
            break;
        case 1:
            o->oGraphYOffset = (o->oTimer & 1) * -5.f;
            if (o->oTimer > 16) {
                o->oAction++;
                o->oGraphYOffset = 0;
            }
            break;
        case 2:
            o->oVelY -= 0.5f;
            if (o->oPosY < (o->oFloorHeight - 50.f)) {
                o->oAction++;
                spawn_object(o, MODEL_EXPLOSION, bhvExplosion);
                cur_obj_hide();
                o->oVelY = 0.f;
            }
            break;
        case 3:
            if (o->oTimer > 90) {
                cur_obj_unhide();
                cur_obj_set_pos_to_home();
                o->oAction = 0;
            }
            break;
    }
    cur_obj_move_y_with_terminal_vel();
    if (o->oAction != 3) {
        load_object_collision_model();
    }
}