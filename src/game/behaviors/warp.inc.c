// warp.c.inc
extern const const Collision bitpipe_collision[];

void bhv_warp_loop(void) {
    u32 *collisionData = segmented_to_virtual(bitpipe_collision);
    u16 sp6;
    if (o->oBehParams & 0x0001) {
        gCurrentObject->collisionData = collisionData;
    }
    if (o->oBehParams & 0X0002) {
        gCurrentObject->header.gfx.scale[0] = 2.0f;
        gCurrentObject->header.gfx.scale[2] = 2.0f;
    }
    if (o->oTimer == 0) {
        if (!gCurrentObject->collisionData) {
            sp6 = (o->oBehParams >> 24) & 0xFF;
            if (sp6 == 0)
                o->hitboxRadius = 50.0f;
            else if (sp6 == 0xFF)
                o->hitboxRadius = 10000.0f;
            else
                o->hitboxRadius = sp6 * 10.0;
            o->hitboxHeight = 50.0f + ((o->oBehParams >> 8) & 0xFF)*50.f;
        }
    }
    o->oInteractStatus = 0;
}

void bhv_fading_warp_loop() // identical to the above function except for o->hitboxRadius
{
    u16 sp6;
    if (o->oTimer == 0) {
        sp6 = (o->oBehParams >> 24) & 0xFF;
        if (sp6 == 0)
            o->hitboxRadius = 85.0f;
        else if (sp6 == 0xFF)
            o->hitboxRadius = 10000.0f;
        else
            o->hitboxRadius = sp6 * 10.0;
        o->hitboxHeight = 50.0f;
    }
    o->oInteractStatus = 0;
}
