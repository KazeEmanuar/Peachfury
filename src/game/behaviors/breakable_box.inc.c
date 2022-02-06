// breakable_box.c.inc
s32 cur_obj_was_hit(void) {
   /* if (m->ceil) {
        if (m->ceil->object == o) {
            if (m->vel[1] > 0) {
                if (m->ceilHeight < m->pos[1] + 200.f + m->vel[1]) {
                    m->vel[1] = 0;
                    return 1;
                }
            }
        }
    }
    if (gMarioState->wall) {
        if (gMarioState->wall->object == o) {
            if (gMarioState->flags & (MARIO_PUNCHING | MARIO_KICKING)) {
                // also make sure mario is facing it
                if (abs_angle_diff(gMarioState->faceAngle[1],
                                   obj_angle_to_object(gMarioState->marioObj, o)) < 0x2000) {
                    return 1;
                }
            }
        }
    }*/
    return cur_obj_is_mario_ground_pounding_platform();
}


void bhv_breakable_box_loop(void) {
    obj_set_hitbox(o, &sBreakableBoxHitbox);
    cur_obj_set_model(MODEL_BREAKABLE_BOX_SMALL);
    if (o->oTimer == 0)
        breakable_box_init();
    if (cur_obj_was_hit() != 0) {
        obj_explode_and_spawn_coins(46.0f, 1);
        create_sound_spawner(SOUND_GENERAL_BREAK_BOX);
    }
}
