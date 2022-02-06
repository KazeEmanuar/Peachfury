// seaweed.c.inc

void bhv_seaweed_init(void) {
    o->header.gfx.unk38.animFrame = random_float() * 80.0f;
    o->oDrawingDistance = 20000.f;
}

#define OFFSET 0x8000
extern u8 isEmu;
void bhv_seaweed_bundle_init(void) {
    struct Object *seaweed;
    o->oPosY -= 200.f;
    // seaweed = spawn_object(o, MODEL_SEAWEED, bhvSeaweed);
    o->oFaceAngleYaw = 14523;
    o->oFaceAnglePitch = 5500 + OFFSET;
    o->oFaceAngleRoll = 9600;
    o->header.gfx.scale[0] = 1.0 + random_f32_around_zero(.1f);
    o->header.gfx.scale[1] = 1.0 + random_f32_around_zero(.1f);
    o->header.gfx.scale[2] = 1.0 + random_f32_around_zero(.1f);
    cur_obj_set_model(MODEL_SEAWEED);

    seaweed = spawn_object(o, MODEL_SEAWEED, bhvSeaweed);
    seaweed->oFaceAngleYaw = 40500;
    seaweed->oFaceAnglePitch = 8700 + OFFSET;
    seaweed->oFaceAngleRoll = 4100;
    seaweed->header.gfx.scale[0] = 0.8 + random_f32_around_zero(.1f);
    seaweed->header.gfx.scale[1] = 0.8 + random_f32_around_zero(.1f);
    seaweed->header.gfx.scale[2] = 0.8 + random_f32_around_zero(.1f);
    seaweed->header.gfx.unk38.animFrame = random_float() * 80.0f;
    if (isEmu) {
        seaweed = spawn_object(o, MODEL_SEAWEED, bhvSeaweed);
        seaweed->oFaceAngleYaw = 57236;
        seaweed->oFaceAnglePitch = 9500 + OFFSET;
        seaweed->oFaceAngleRoll = 0;
        seaweed->header.gfx.scale[0] = 1.1 + random_f32_around_zero(.1f);
        seaweed->header.gfx.scale[1] = 1.1 + random_f32_around_zero(.1f);
        seaweed->header.gfx.scale[2] = 1.1 + random_f32_around_zero(.1f);
        seaweed->header.gfx.unk38.animFrame = random_float() * 80.0f;
    }
}
