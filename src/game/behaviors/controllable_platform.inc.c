// controllable_platform.c.inc

static s8 D_80331694 = 0;

void controllable_platform_act_1(void) {
    o->oParentRelativePosY -= 4.0f;
    if (o->oParentRelativePosY < 41.0f) {
        o->oParentRelativePosY = 41.0f;
        o->oAction = 2;
    }
}

void controllable_platform_act_2(void) {
    if (o->oBehParams2ndByte == D_80331694)
        return;

    o->oParentRelativePosY += 4.0f;
    if (o->oParentRelativePosY > 51.0f) {
        o->oParentRelativePosY = 51.0f;
        o->oAction = 0;
    }
}

void bhv_controllable_platform_sub_loop(void) {
    switch (o->oAction) {
        case 0:
            if (o->oTimer < 30)
                break;

            if (gMarioObject->platform == o) {
                D_80331694 = o->oBehParams2ndByte;
                o->oAction = 1;
                cur_obj_play_sound_2(SOUND_GENERAL_MOVING_PLATFORM_SWITCH);
            }
            break;

        case 1:
            controllable_platform_act_1();
            break;

        case 2:
            controllable_platform_act_2();
            break;
    }

    o->oVelX = o->parentObj->oVelX;
    o->oVelZ = o->parentObj->oVelZ;

    if (o->parentObj->activeFlags == ACTIVE_FLAG_DEACTIVATED)
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
}

void bhv_controllable_platform_init(void) {
    struct Object *sp34;
    sp34 = spawn_object_rel_with_rot(o, MODEL_HMC_METAL_ARROW_PLATFORM, bhvControllablePlatformSub, 0,
                                     51, 204, 0, 0, 0);
    sp34->oBehParams2ndByte = 1;
    sp34 = spawn_object_rel_with_rot(o, MODEL_HMC_METAL_ARROW_PLATFORM, bhvControllablePlatformSub, 0,
                                     51, -204, 0, -0x8000, 0);
    sp34->oBehParams2ndByte = 2;
    sp34 = spawn_object_rel_with_rot(o, MODEL_HMC_METAL_ARROW_PLATFORM, bhvControllablePlatformSub, 204,
                                     51, 0, 0, 0x4000, 0);
    sp34->oBehParams2ndByte = 3;
    sp34 = spawn_object_rel_with_rot(o, MODEL_HMC_METAL_ARROW_PLATFORM, bhvControllablePlatformSub,
                                     -204, 51, 0, 0, -0x4000, 0);
    sp34->oBehParams2ndByte = 4;

    D_80331694 = 0;

    o->oControllablePlatformUnkFC = o->oPosY;
}

void controllable_platform_hit_wall(s8 sp1B) {
    o->oControllablePlatformUnkF8 = sp1B;
    o->oTimer = 0;
    D_80331694 = 5;

    cur_obj_play_sound_2(SOUND_GENERAL_QUIET_POUND1);
}

void controllable_platform_check_walls(s8 sp1B, s8 sp1C[3], Vec3f sp20, UNUSED Vec3f sp24, Vec3f sp28) {
    if (sp1C[1] == 1 || (sp1C[0] == 1 && sp1C[2] == 1))
        controllable_platform_hit_wall(sp1B);
    else {
        if (sp1C[0] == 1) {
            if (((sp1B == 1 || sp1B == 2) && (s32) sp20[2] != 0)
                || ((sp1B == 3 || sp1B == 4) && (s32) sp20[0] != 0)) {
                controllable_platform_hit_wall(sp1B);
            } else {
                o->oPosX += sp20[0];
                o->oPosZ += sp20[2];
            }
        }

        if (sp1C[2] == 1) {
            if (((sp1B == 1 || sp1B == 2) && (s32) sp28[2] != 0)
                || ((sp1B == 3 || sp1B == 4) && (s32) sp28[0] != 0)) {
                controllable_platform_hit_wall(sp1B);
            } else {
                o->oPosX += sp28[0];
                o->oPosZ += sp28[2];
            }
        }
    }

    if (!is_point_within_radius_of_mario(o->oPosX, o->oPosY, o->oPosZ, 400)) {
        D_80331694 = 6;
        o->oControllablePlatformUnk100 = 1;
        o->oTimer = 0;
    }
}

void controllable_platform_shake_on_wall_hit(void) {
    if (o->oControllablePlatformUnkF8 == 1 || o->oControllablePlatformUnkF8 == 2) {
        o->oFaceAnglePitch = sins(o->oTimer * 0x1000) * 182.04444 * 10.0;
        o->oPosY = o->oControllablePlatformUnkFC + sins(o->oTimer * 0x2000) * 20.0f;
    } else {
        o->oFaceAngleRoll = sins(o->oTimer * 0x1000) * 182.04444 * 10.0;
        o->oPosY = o->oControllablePlatformUnkFC + sins(o->oTimer * 0x2000) * 20.0f;
    }

    if (o->oTimer == 32) {
        D_80331694 = o->oControllablePlatformUnkF8;
        o->oFaceAnglePitch = 0;
        o->oFaceAngleRoll = 0;
        o->oPosY = o->oControllablePlatformUnkFC;
    }
}

void controllable_platform_tilt_from_mario(void) {
    s16 sp1E = gMarioObject->header.gfx.pos[0] - o->oPosX;
    s16 sp1C = gMarioObject->header.gfx.pos[2] - o->oPosZ;

    if (gMarioObject->platform == o
        || gMarioObject->platform == cur_obj_nearest_object_with_behavior(bhvControllablePlatformSub)) {
        o->oFaceAnglePitch = sp1C * 4;
        o->oFaceAngleRoll = -sp1E * 4;
        if (D_80331694 == 6) {
            D_80331694 = 0;
            o->oTimer = 0;
            o->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
        }
    } else {
    }
}

void bhv_controllable_platform_loop(void) {
}
