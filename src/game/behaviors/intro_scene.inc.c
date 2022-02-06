// intro_scene.inc.c

void spawn_child_obj_relative(struct Object *parent, s16 xOffset, s16 yOffset, s16 zOffset, s16 pitchOffset,
                   s16 yawOffset, s16 rollOffset, s16 forwardVel,
                   s32 model, const BehaviorScript *behavior) {
    struct Object *sp1C = spawn_object(parent, model, behavior);

    sp1C->header.gfx.unk38.animFrame = random_float() * 6.f;
    sp1C->oEndBirdUnk104 = sCutsceneVars[9].point[0];
    sCutsceneVars[9].point[0] += 1.f;
    sp1C->oPosX += xOffset;
    sp1C->oPosY += yOffset;
    if (gCutsceneTimer > 700)
        sp1C->oPosY += -150.f;
    sp1C->oPosZ += zOffset;
    sp1C->oMoveAnglePitch += pitchOffset;
    sp1C->oMoveAngleYaw += yawOffset;
    sp1C->oMoveAngleRoll += rollOffset;
    sp1C->oForwardVel = forwardVel;
}

void bhv_intro_scene_loop(void) {
}
