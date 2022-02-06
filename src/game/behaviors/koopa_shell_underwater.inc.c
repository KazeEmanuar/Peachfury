// koopa_shell_underwater.c.inc

struct ObjectHitbox sKoopaShellUnderwaterHitbox = {
    /* interactType: */ INTERACT_GRABBABLE,
    /* downOffset: */ 0,
    /* damageOrCoinValue: */ 0,
    /* health: */ 1,
    /* numLootCoins: */ 0,
    /* radius: */ 80,
    /* height: */ 50,
    /* hurtboxRadius: */ 0,
    /* hurtboxHeight: */ 0,
};

void koopa_shellu_spawn_sparkles(f32 a) {
    struct Object *sp1C;
    if (o->oTimer & 0x01) {
        sp1C = spawn_object(o, MODEL_NONE, bhvSparkleSpawn);
        sp1C->oPosY += a;
    }
}

void set_koopa_shell_underwater_hitbox(void) {
    obj_set_hitbox(o, &sKoopaShellUnderwaterHitbox);
    o->oFaceAngleYaw += 0x310;
    if (o->oDistanceToMario < 3000.f) {
        koopa_shellu_spawn_sparkles(10.0f);
    }
}

void bhv_koopa_shell_underwater_loop(void) {
    switch (o->oHeldState) {
        case HELD_FREE:
            set_koopa_shell_underwater_hitbox();
            break;
        case HELD_HELD:
            cur_obj_unrender_and_reset_state(-1, 0);
            break;
        case HELD_THROWN:
        case HELD_DROPPED:
            spawn_mist_particles();
            o->oHeldState = HELD_FREE;
            o->oPosX = o->oHomeX;
            o->oPosY = o->oHomeY;
            o->oPosZ = o->oHomeZ;
            o->oIntangibleTimer = 0;
            break;
    }
    if (o->oInteractStatus & INT_STATUS_STOP_RIDING) {
        obj_mark_for_deletion(o);
        spawn_mist_particles();
    }
    o->oInteractStatus = 0;
}
