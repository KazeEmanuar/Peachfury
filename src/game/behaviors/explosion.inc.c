// explosion.c.inc

void bhv_explosion_init(void) {
    create_sound_spawner(SOUND_GENERAL2_BOBOMB_EXPLOSION);
    set_environmental_camera_shake(SHAKE_ENV_EXPLOSION);

    o->oOpacity = 255;
}

void bhv_explosion_loop(void) {
    struct Object *explosion;
    s32 i;
    if (o->behavior == segmented_to_virtual(bhvExplosion2)) {
        o->oPosX = approach_f32_asymptotic(o->oPosX, gMarioObject->oPosX, 0.95f);
        o->oPosY = approach_f32_asymptotic(o->oPosY, gMarioObject->oPosY, 0.95f);
        o->oPosZ = approach_f32_asymptotic(o->oPosZ, gMarioObject->oPosZ, 0.95f);
    }
    if (o->oTimer == 9) {
        if (find_water_level(o->oPosX, o->oPosZ) > o->oPosY) {
            for (i = 0; i < 40; i++)
                spawn_object(o, MODEL_WHITE_PARTICLE_SMALL, bhvBobombExplosionBubble);
        } else {
            explosion = spawn_object(o, MODEL_SMOKE, bhvBobombBullyDeathSmoke);
            explosion->oPosX = o->header.gfx.pos[0];
            explosion->oPosY = o->header.gfx.pos[1];
            explosion->oPosZ = o->header.gfx.pos[2];
            explosion->oPosX = o->header.gfx.pos[0];
            explosion->oPosY = o->header.gfx.pos[1];
            explosion->oPosZ = o->header.gfx.pos[2];
        }

        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    }

    o->oOpacity -= 14;

    cur_obj_scale((f32)(o->oTimer / 9.0f + 1.0f) * (1.f + o->oBehParams2ndByte * 0.1f));
}