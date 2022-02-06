// tower_door.c.inc
#include "game/level_update.h"
s32 check_attack() {
    if (obj_check_if_collided_with_object(o, gMarioObject)) {
        if (abs_angle_diff(mario_obj_angle_to_object(gMarioState, o), gMarioObject->oMoveAngleYaw-0x8000) > 0x6000) {
            if (gMarioStates->action == ACT_SLIDE_KICK)
                return 1;
            if (gMarioStates->action == ACT_PUNCHING)
                return 1;
            if (gMarioStates->action == ACT_MOVE_PUNCHING)
                return 1;
            if (gMarioStates->action == ACT_SLIDE_KICK_SLIDE)
                return 1;
            if (gMarioStates->action == ACT_JUMP_KICK)
                return 2;
            if (gMarioStates->action == ACT_WALL_KICK_AIR)
                return 2;
        }
    }
    return 0;
}

void bhv_tower_door_loop(void) {
    struct Object *coin;
    if (check_attack()) {
        spawn_mist_particles_variable(0, 0, 30.f);
        spawn_triangle_break_particles(15, 138, 1.5f, 4);
        obj_mark_for_deletion(o);
        coin = spawn_object(o, MODEL_YELLOW_COIN, bhvSingleCoinGetsSpawned);
        coin->oCoinUnk110 = 20.f;
        coin = spawn_object(o, MODEL_YELLOW_COIN, bhvSingleCoinGetsSpawned);
        coin->oCoinUnk110 = 20.f;
        create_sound_spawner(SOUND_OBJ_BULLY_METAL);
    }
}
