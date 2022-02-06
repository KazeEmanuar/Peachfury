// spawn_default_star.c.inc

static struct ObjectHitbox sCollectStarHitbox = {
    /* interactType:      */ INTERACT_STAR_OR_KEY,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 0,
    /* health:            */ 0,
    /* numLootCoins:      */ 0,
    /* radius:            */ 80,
    /* height:            */ 50,
    /* hurtboxRadius:     */ 0,
    /* hurtboxHeight:     */ 0,
};
u8 isYoshiStar() {
    return (o->header.gfx.sharedChild == gLoadedGraphNodes[MODEL_YOSHI_STAR]);
}
extern s8 missionActive;
void bhv_collect_star_init(void) {
    s8 starId;
    u8 currentLevelStarFlags;

    starId = (o->oBehParams >> 24) & 3;
    currentLevelStarFlags = save_file_get_star_flags(gCurrSaveFileNum - 1, gCurrCourseNum - 1 + (o->oBehParams >> 24)/4);
    if (!isYoshiStar()) {
        if (currentLevelStarFlags & (1 << starId)) {
            o->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_TRANSPARENT_STAR];
        } else {
            o->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_STAR];
        }
    } else {
        missionActive = -1;
    }
    obj_set_hitbox(o, &sCollectStarHitbox);
}

void bhv_collect_star_loop(void) {
    o->oFaceAngleYaw += 0x800;

    if (o->oInteractStatus & INT_STATUS_INTERACTED) {
        mark_obj_for_deletion(o);
        o->oInteractStatus = 0;
    }
    if (o->header.gfx.sharedChild != gLoadedGraphNodes[MODEL_TRANSPARENT_STAR]) {
        play_sound(SOUND_AIR_PEACH_TWINKLE, gCurrentObject->header.gfx.cameraToObject);
    }
}

void bhv_star_spawn_init(void) {
    o->oMoveAngleYaw = atan2s(o->oHomeZ - o->oPosZ, o->oHomeX - o->oPosX);
    o->oStarSpawnDisFromHome = sqrtf(sqr(o->oHomeX - o->oPosX) + sqr(o->oHomeZ - o->oPosZ));
    o->oVelY = (o->oHomeY - o->oPosY) / 30.0f;
    o->oForwardVel = o->oStarSpawnDisFromHome / 30.0f;
    o->oStarSpawnUnkFC = o->oPosY;
    if (o->oBehParams2ndByte == 0)
        cutscene_object(CUTSCENE_STAR_SPAWN, o);
    else
        cutscene_object(CUTSCENE_RED_COIN_STAR_SPAWN, o);

    set_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_MARIO_AND_DOORS);
    o->activeFlags |= ACTIVE_FLAG_INITIATED_TIME_STOP;
    cur_obj_become_intangible();
}

void bhv_star_spawn_loop(void) {
    switch (o->oAction) {
        case 0:
            o->oFaceAngleYaw += 0x1000;
            if (o->oTimer > 20)
                o->oAction = 1;
            break;

        case 1:
            obj_move_xyz_using_fvel_and_yaw();
            o->oStarSpawnUnkFC += o->oVelY;
            o->oPosY = o->oStarSpawnUnkFC + sins((o->oTimer * 0x8000) / 30) * 400.0f;
            o->oFaceAngleYaw += 0x1000;
            spawn_object(o, MODEL_NONE, bhvSparkleSpawn);
            cur_obj_play_sound_1(SOUND_ENV_STAR);
            if (o->oTimer == 29) {
                o->oAction = 2;
                o->oForwardVel = 0;
                play_power_star_jingle(TRUE);
            }
            break;

        case 2:
            if (o->oTimer < 20)
                o->oVelY = 20 - o->oTimer;
            else
                o->oVelY = -10.0f;

            spawn_object(o, MODEL_NONE, bhvSparkleSpawn);
            obj_move_xyz_using_fvel_and_yaw();
            o->oFaceAngleYaw = o->oFaceAngleYaw - o->oTimer * 0x10 + 0x1000;
            cur_obj_play_sound_1(SOUND_ENV_STAR);

            if (o->oPosY < o->oHomeY) {
                cur_obj_play_sound_2(SOUND_GENERAL_STAR_APPEARS);
                cur_obj_become_tangible();
                o->oPosY = o->oHomeY;
                o->oAction = 3;
            }
            break;
        case 3:
            o->oFaceAngleYaw += 0x800;
            if (o->oTimer == 20) {
                gObjCutsceneDone = TRUE;
                clear_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_MARIO_AND_DOORS);
                o->activeFlags &= ~ACTIVE_FLAG_INITIATED_TIME_STOP;
            }
            if (o->oInteractStatus & INT_STATUS_INTERACTED) {
                mark_obj_for_deletion(o);
                o->oInteractStatus = 0;
            }
            break;
    }
    play_sound(SOUND_AIR_PEACH_TWINKLE, gCurrentObject->header.gfx.cameraToObject);
}

struct Object *spawn_star(struct Object *sp30, f32 sp34, f32 sp38, f32 sp3C) {
    sp30 = spawn_object_abs_with_rot(o, 0, MODEL_STAR, bhvStarSpawnCoordinates, o->oPosX, o->oPosY,
                                     o->oPosZ, 0, 0, 0);
    sp30->oBehParams = o->oBehParams;
    sp30->oHomeX = sp34;
    sp30->oHomeY = sp38;
    sp30->oHomeZ = sp3C;
    sp30->oFaceAnglePitch = 0;
    sp30->oFaceAngleRoll = 0;
    return sp30;
}

struct Object *spawn_yoshi_star(struct Object *sp30, f32 sp34, f32 sp38, f32 sp3C) {
    sp30 = spawn_object_abs_with_rot(o, 0, MODEL_YOSHI_STAR, bhvStarSpawnCoordinates, o->oPosX,
                                     o->oPosY, o->oPosZ, 0, 0, 0);
    sp30->oBehParams = o->oBehParams;
    sp30->oHomeX = sp34;
    sp30->oHomeY = sp38;
    sp30->oHomeZ = sp3C;
    sp30->oFaceAnglePitch = 0;
    sp30->oFaceAngleRoll = 0;
    return sp30;
}

void spawn_default_star(f32 sp20, f32 sp24, f32 sp28) {
    struct Object *sp1C;
    sp1C = spawn_star(sp1C, sp20, sp24, sp28);
    sp1C->oBehParams2ndByte = 0;
}

extern s8 orangeYoshApressCount;
void spawn_red_coin_cutscene_star(f32 sp20, f32 sp24, f32 sp28) {
    struct Object *sp1C;
    if ((gCurrLevelNum == LEVEL_BOB) && (orangeYoshApressCount >= 0)) {
        sp1C = spawn_yoshi_star(sp1C, sp20, sp24, sp28);
        sp1C->oBehParams2ndByte = 1;
    } else {
        sp1C = spawn_star(sp1C, sp20, sp24, sp28);
        sp1C->oBehParams2ndByte = 1;
    }
}

void spawn_no_exit_star(f32 sp20, f32 sp24, f32 sp28) {
    struct Object *sp1C;
    sp1C = spawn_star(sp1C, sp20, sp24, sp28);
    sp1C->oBehParams2ndByte = 1;
    sp1C->oInteractionSubtype |= INT_SUBTYPE_NO_EXIT;
}

void bhv_hidden_red_coin_star_init(void) {
    s16 sp36;
    struct Object *sp30;

    if (gCurrCourseNum != COURSE_JRB)
        spawn_object(o, MODEL_TRANSPARENT_STAR, bhvRedCoinStarMarker);

    sp36 = count_objects_with_behavior(bhvRedCoin) + count_objects_with_behavior(bhvRedBird);
    if (sp36 == 0) {
        sp30 =
            spawn_object_abs_with_rot(o, 0, MODEL_STAR, bhvStar, o->oPosX, o->oPosY, o->oPosZ, 0, 0, 0);
        sp30->oBehParams = o->oBehParams;
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    }

    o->oHiddenStarTriggerCounter = 8 - sp36;
}

void bhv_hidden_red_coin_star_loop(void) {
    gRedCoinsCollected = o->oHiddenStarTriggerCounter;
    switch (o->oAction) {
        case 0:
            if (o->oHiddenStarTriggerCounter == 8)
                o->oAction = 1;
            break;

        case 1:
            if (o->oTimer > 2) {
                spawn_red_coin_cutscene_star(o->oPosX, o->oPosY, o->oPosZ);
                spawn_mist_particles();
                o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
            }
            break;
    }
}
