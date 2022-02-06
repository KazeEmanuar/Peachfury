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
#include "levels/bbh/header.h"

#define m gMarioState
#define o gCurrentObject

extern int buttontimer;
extern Gfx mat_bob_dl_buttonanimationbob[];
extern u8 bob_dl_button1bob_rgba16[];
extern const u8 button2bob[];
extern Vtx jrb_dl_nippercrowd_mesh_vtx_0[84];
struct SToffset {
    s16 x;
    s16 y;
};
/*
struct SToffset nipperanim[] = {
    { 0x400, 0x400 },   { 0xfc00, 0xfc00 }, { 0x0000, 0x0400 },
    { 0xfc00, 0xfc00 }, { 0x0000, 0x0400 }, { 0x0400, 0xfc00 },
};*/

struct SToffset nipperanim[] = {
    { 0x0000, 0x400 }, { 0xfc00, 0 }, { 0x0400, 0 }, { 0x0000, 0xfc00 }, { 0xfc00, 0 }, { 0x0400, 0 },
};

void animateNips() {
    int i;
    Vtx *a = segmented_to_virtual(jrb_dl_nippercrowd_mesh_vtx_0);
    for (i = 0; i < 84; i++) {
        a[i].n.tc[0] += nipperanim[o->oOpacity].y;
        a[i].n.tc[1] += nipperanim[o->oOpacity].x;
    }
    o->oOpacity++;
    o->oOpacity = o->oOpacity % 6;
}

extern Gfx mat_N64logo_N64logo1[];
extern Gfx mat_N64logo_N64logo2[];
extern Gfx mat_N64logo_N64logo3[];
extern Gfx mat_N64logo_N64logo4[];
extern Gfx mat_N64logo_N64logo5[];
extern Gfx mat_N64logo_N64logo6[];
extern Gfx mat_N64logo_N64logo7[];
extern Gfx mat_N64logo_N64logo8[];
extern Gfx mat_N64logo_N64logo9[];
extern Gfx mat_N64logo_N64logo10[];
extern Gfx mat_N64logo_N64logo11[];
extern Gfx mat_N64logo_N64logo12[];
extern Gfx mat_N64logo_N64logo13[];
extern Gfx mat_N64logo_N64logo14[];
extern Gfx mat_N64logo_N64logo15[];
extern Gfx mat_N64logo_N64logo16[];

/*Gfx *N64LogoFrames[16] = {
    &mat_N64logo_N64logo1,  &mat_N64logo_N64logo2,  &mat_N64logo_N64logo3,  &mat_N64logo_N64logo4,
    &mat_N64logo_N64logo5,  &mat_N64logo_N64logo6,  &mat_N64logo_N64logo7,  &mat_N64logo_N64logo8,
    &mat_N64logo_N64logo9,  &mat_N64logo_N64logo10, &mat_N64logo_N64logo11, &mat_N64logo_N64logo12,
    &mat_N64logo_N64logo13, &mat_N64logo_N64logo14, &mat_N64logo_N64logo15, &mat_N64logo_N64logo16,
};*/

Gfx N64LogoFrames[16] = {
    gsSPDisplayList(mat_N64logo_N64logo1),  gsSPDisplayList(mat_N64logo_N64logo2),
    gsSPDisplayList(mat_N64logo_N64logo3),  gsSPDisplayList(mat_N64logo_N64logo4),
    gsSPDisplayList(mat_N64logo_N64logo5),  gsSPDisplayList(mat_N64logo_N64logo6),
    gsSPDisplayList(mat_N64logo_N64logo7),  gsSPDisplayList(mat_N64logo_N64logo8),
    gsSPDisplayList(mat_N64logo_N64logo9),  gsSPDisplayList(mat_N64logo_N64logo10),
    gsSPDisplayList(mat_N64logo_N64logo11), gsSPDisplayList(mat_N64logo_N64logo12),
    gsSPDisplayList(mat_N64logo_N64logo13), gsSPDisplayList(mat_N64logo_N64logo14),
    gsSPDisplayList(mat_N64logo_N64logo15), gsSPDisplayList(mat_N64logo_N64logo16),
};
extern Gfx castle_grounds_dl_N64Logo_mesh[];
extern Gfx mat_ccm_dl_ZOOWATER[];
extern Vtx ccm_dl_WaterAnimation_mesh_layer_5_vtx_0[17];
u16 watervertcount = sizeof(ccm_dl_WaterAnimation_mesh_layer_5_vtx_0) / 0x10;

Gfx kill = gsSPEndDisplayList();

#define DIVISOR 255.f
extern Lights1 *lightsIn[];
extern Lights1 *lightsOut[];
void setMarioLight(u8 R, u8 G, u8 B) {
    u8 RGBMult[3] = { 0, 0, 0 };
    int k;
    f32 multiplicators[3];
    Lights1 *i;
    Lights1 *j;
    RGBMult[0] = R;
    RGBMult[1] = G;
    RGBMult[2] = B;

    multiplicators[0] = (((float) RGBMult[0]) / DIVISOR);
    multiplicators[1] = (((float) RGBMult[1]) / DIVISOR);
    multiplicators[2] = (((float) RGBMult[2]) / DIVISOR);
    for (k = 0; k < 15; k++) {
        i = (Lights1 *) (lightsIn[k]);
        j = (Lights1 *) segmented_to_virtual((int) lightsOut[k] & 0x7fFFFFFF);
        j->a.l.col[0] = i->a.l.col[0] * multiplicators[0];
        j->a.l.col[1] = i->a.l.col[1] * multiplicators[1];
        j->a.l.col[2] = i->a.l.col[2] * multiplicators[2];
        j->a.l.colc[0] = j->a.l.col[0];
        j->a.l.colc[1] = j->a.l.col[1];
        j->a.l.colc[2] = j->a.l.col[2];

        j->l->l.col[0] = i->l->l.col[0] * multiplicators[0];
        j->l->l.col[1] = i->l->l.col[1] * multiplicators[1];
        j->l->l.col[2] = i->l->l.col[2] * multiplicators[2];
        j->l->l.colc[0] = j->l->l.col[0];
        j->l->l.colc[1] = j->l->l.col[1];
        j->l->l.colc[2] = j->l->l.col[2];
    }
};
void setLightDirection(u8 x, u8 y, u8 z) {
    u8 RGBMult[3] = { 0, 0, 0 };
    int k;
    Lights1 *i;
    Lights1 *j;
    for (k = 0; k < 15; k++) {
        j = (Lights1 *) segmented_to_virtual((int) lightsOut[k] & 0x7fFFFFFF);
        j->l->l.dir[0] = x;
        j->l->l.dir[1] = y;
        j->l->l.dir[2] = z;
    }
}
extern u8 mario_mario_button_ci4_ci4_pal_rgba16[];
extern u8 mario_mario_button_ci4_ci4[];
extern u8 dick_mario_button_ci4_ci4_pal_rgba16[];
extern u8 dick_mario_button_ci4_ci4[];
extern Gfx mat_mario_body[];

Gfx swappied[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_mario_button_ci4_ci4_pal_rgba16),
    gsDPSetTextureImage(G_IM_FMT_CI, G_IM_SIZ_8b, 32, mario_mario_button_ci4_ci4),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dick_mario_button_ci4_ci4_pal_rgba16),
    gsDPSetTextureImage(G_IM_FMT_CI, G_IM_SIZ_8b, 32, dick_mario_button_ci4_ci4),
};

void makeMarioGREY(u8 brightness) {
    u16 grey;
    int k;
    Lights1 *i;
    Lights1 *j;
    Gfx *a = segmented_to_virtual(mat_mario_body);

    for (k = 0; k < 15; k++) {
        if (k == 3) {
            continue;
        }
        i = (Lights1 *) (lightsIn[k]);
        j = (Lights1 *) segmented_to_virtual((int) lightsOut[k] & 0x7fFFFFFF);
        grey = (i->a.l.col[0] + i->a.l.col[1] + i->a.l.col[2]) * 0.33f * (brightness / 255.f);
        if ((k == 2) || (k == 4) || (k == 6) || (k == 7) || (k == 8)) {
            grey *= 1.5f;
        }
        if (grey > 0xff) {
            grey = 0xff;
        }
        j->a.l.col[0] = grey;
        j->a.l.col[1] = grey;
        j->a.l.col[2] = grey;
        j->a.l.colc[0] = j->a.l.col[0];
        j->a.l.colc[1] = j->a.l.col[1];
        j->a.l.colc[2] = j->a.l.col[2];

        j->l->l.col[0] = grey;
        j->l->l.col[1] = grey;
        j->l->l.col[2] = grey;
        j->l->l.colc[0] = j->l->l.col[0];
        j->l->l.colc[1] = j->l->l.col[1];
        j->l->l.colc[2] = j->l->l.col[2];
    }
    a[4] = swappied[2];  // palete
    a[11] = swappied[3]; // texture
};

extern Vtx ccm_dl_AnimtedMice_mesh_layer_1_vtx_0[4];
void animateMice() {
    int i;
    Vtx *a = segmented_to_virtual(ccm_dl_AnimtedMice_mesh_layer_1_vtx_0);
    if (o->oTimer == 7) {
        o->oTimer = 0;
        o->oOpacity++;
        if (o->oOpacity == 4) {
            o->oOpacity = 0;
            for (i = 0; i < 12; i++) {
                a[i].n.tc[0] -= 4096;
            }
        }
        for (i = 0; i < 12; i++) {
            a[i].n.tc[0] += 1024;
            // a[i].n.tc[1] += nipperanim[o->oOpacity].x;
        }
    }
}
extern Gfx mat_bbh_dl_InsideWater[];
extern Gfx mat_ccm_dl_WATERRIPPLE_layer6[];
extern Gfx mat_ccm_dl_f3d_material_016[];
extern Gfx mat_castle_grounds_dl_OWWATER[];
extern Gfx ccm_dl_ZZZZPlantStart_mesh_layer_4[];
extern Vtx castle_grounds_dl_WaterAnimationOW_mesh_layer_5_vtx_0[27];
extern Vtx ccm_dl_Lava_mesh_layer_1_vtx_0[259];
u16 lavavertcount = sizeof(ccm_dl_Lava_mesh_layer_1_vtx_0) / 0x10;
extern Vtx bbh_dl_OCEAN_mesh_layer_1_vtx_0[286];
u16 oceanvertcout = sizeof(bbh_dl_OCEAN_mesh_layer_1_vtx_0) / 0x10;
extern Gfx mat_bbh_dl_OCEANWATER_layer1_area1[];

u16 pixelIndex[sizeof(bbh_dl_OCEAN_mesh_layer_1_vtx_0) / 0x10];

#define XMIN (-24453)
#define ZMIN (-26471)
#define SCALE (1764.61f)
u16 getVertIndex(s16 Xcord, s16 Zcord) {
    u16 x, y;
    // map X and Z cord onto different pixels
    x = ((u16) ((Xcord - XMIN) / SCALE)) % 64;
    y = ((u16) ((Zcord - ZMIN) / SCALE)) % 64;
    return y * 16 + x;
}

extern Gfx mat_booguy_f3d_material[];
u8 pulsateTimer = 0;
void pulsateBooGuy(void) {
    Gfx *a = segmented_to_virtual(mat_booguy_f3d_material);
    u8 brightness = (pulsateTimer > 0x3F) ? (pulsateTimer / 2) : ((0x80 - pulsateTimer) / 2);
    pulsateTimer = (pulsateTimer + 2) & 0x7F;
    a[19].words.w1 = (brightness << 16) + (brightness << 8) + 0xC0;
}

f32 beamSpeeds[128];

f32 lastFrameCount = 100.f;
f32 beamcount;
#define FRICTION 0.995f
#define DECEL 10.5f
#define CARRYOVER 0.1f
#define LIGHTFACTOR (2048.f / beamcount)
#define LIGHTFACTOR2 (65536.f / beamcount)
void generateLightBeamTexture(u8 *texture) {
    u8 x, y;
    s16 brightness;
    s16 brightnessY;
    u16 randomCall;
    beamcount = lastFrameCount;
    lastFrameCount = 0;
    for (x = 0; x < 128; x++) {
        beamSpeeds[x] *= FRICTION;
        beamSpeeds[x] -= DECEL;
        if (!(random_u16() & 0x3F)) {
            beamSpeeds[x] += ((random_u16() & 0x3F) + 8) * LIGHTFACTOR;
        }
        brightness = texture[x];
        brightness += texture[((x - 1) & 0x7F)] * CARRYOVER * LIGHTFACTOR;
        brightness += texture[((x + 1) & 0x7F)] * CARRYOVER * LIGHTFACTOR;
        brightness += beamSpeeds[x];
        if (brightness > 255) {
            brightness = 255;
            beamSpeeds[x] -= -20.f;
        } else if (brightness < 0) {
            brightness = 0;
            beamSpeeds[x] = 0;
        }
        lastFrameCount += brightness;
        for (y = 0; y < 16; y++) {
            brightnessY = brightness + 180 - 12 * ((y - 1) & 0x0F);
            if (brightnessY > 255) {
                brightnessY = 255;
            }
            texture[y * 128 + x] = brightnessY;
        }
    }
}

// 6x96 I4 sheet of animframes
u8 animFrames[6 * 3 * 16] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x11, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x31, 0x00, 0x01,
    0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x44,
    0x00, 0x01, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x45, 0x00, 0x01, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x10, 0x00, 0x57, 0x00, 0x00, 0x45, 0x10, 0x01, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x10, 0x00, 0x75, 0x00, 0x00, 0x75, 0x00, 0x01, 0x00, 0x10, 0x00, 0x00, 0x00,
    0x10, 0x00, 0x10, 0x02, 0x32, 0x10, 0x03, 0x97, 0x00, 0x02, 0x86, 0x00, 0x11, 0x00, 0x10, 0x00,
    0x00, 0x00, 0x21, 0x11, 0x20, 0x04, 0x64, 0x10, 0x05, 0xb9, 0x10, 0x04, 0x86, 0x10, 0x11, 0x01,
    0x20, 0x00, 0x00, 0x00, 0x31, 0x11, 0x40, 0x17, 0xb8, 0x20, 0x09, 0xfc, 0x20, 0x07, 0xa8, 0x20,
    0x31, 0x11, 0x30, 0x00, 0x00, 0x00, 0x23, 0x11, 0x31, 0x05, 0xa9, 0x40, 0x05, 0xee, 0x40, 0x05,
    0x99, 0x40, 0x22, 0x11, 0x21, 0x00, 0x00, 0x00, 0x23, 0x11, 0x32, 0x03, 0x77, 0x30, 0x04, 0xba,
    0x30, 0x03, 0x76, 0x30, 0x22, 0x11, 0x22, 0x00, 0x00, 0x00, 0x33, 0x11, 0x23, 0x12, 0x44, 0x21,
    0x02, 0x76, 0x20, 0x02, 0x44, 0x20, 0x32, 0x00, 0x23, 0x00, 0x00, 0x00, 0x43, 0x00, 0x14, 0x11,
    0x11, 0x11, 0x01, 0x22, 0x10, 0x01, 0x11, 0x10, 0x41, 0x00, 0x14, 0x00, 0x00, 0x00, 0x21, 0x00,
    0x12, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x00, 0x12, 0x00, 0x00, 0x00,

};

#define STARCOUNTMAX 16
s16 starInfo[3][STARCOUNTMAX];
void generateNightSky(u8 *texture) {
    u32 i;
    u32 j, k;
    for (i = 0; i < STARCOUNTMAX; i++) {
        if (starInfo[2][i] == -1) {
            if (!(random_u16() & 7)) {
                starInfo[0][i] = random_u16() & 0x3F;
                starInfo[1][i] = random_u16() & 0x3F;
                starInfo[2][i] = 0;
            }
        } else {
            starInfo[2][i]++;
            if (starInfo[2][i] == 32) {
                starInfo[2][i] = -1;
            }
        }
    }
    for (i = 0; i < 128 * 64 / 8; i++) {
        ((u32 *) (texture))[i] = 0;
    }
    for (i = 0; i < STARCOUNTMAX; i++) {
        if (starInfo[2][i] != -1) {
            for (k = 0; k < 6; k++) {
                texture[(starInfo[0][i] + (starInfo[1][i] + k) * 64) % 4096] =
                    animFrames[(starInfo[2][i] >> 1) * 18 + k * 3];
                texture[(starInfo[0][i] + (starInfo[1][i] + k) * 64 + 1) % 4096] =
                    animFrames[(starInfo[2][i] >> 1) * 18 + k * 3 + 1];
                texture[(starInfo[0][i] + (starInfo[1][i] + k) * 64 + 2) % 4096] =
                    animFrames[(starInfo[2][i] >> 1) * 18 + k * 3 + 2];
            }
        }
    }
}
extern Gfx bbh_dl_Room1Layer1_mesh_layer_1[];
extern Gfx bbh_dl_Room1Layer4_mesh_layer_4[];
extern Gfx bbh_dl_Room2Layer1_mesh_layer_1[];
extern Gfx bbh_dl_Room2Layer2_mesh_layer_2[];
extern Gfx bbh_dl_Room2Layer4_mesh_layer_4[];
extern Gfx bbh_dl_Room2Layer5_mesh_layer_5[];
extern Gfx bbh_dl_Room2Layer6_mesh_layer_6[];
extern Gfx bbh_dl_Room3Layer1_mesh_layer_1[];
extern Gfx bbh_dl_Room3Layer2_mesh_layer_2[];
extern Gfx bbh_dl_Room3Layer4_mesh_layer_4[];
extern Gfx bbh_dl_Room3Layer6_mesh_layer_6[];
extern Gfx bbh_dl_Room4Layer1_mesh_layer_1[];
extern Gfx bbh_dl_Room4Layer4_mesh_layer_4[];
extern Gfx bbh_dl_Room5Layer1_mesh_layer_1[];
extern Gfx bbh_dl_Room5Layer4_mesh_layer_4[];
extern Gfx bbh_dl_Room5Layer6_mesh_layer_6[];
extern Gfx bbh_dl_Room6Layer1_mesh_layer_1[];
extern Gfx bbh_dl_Room6Layer4_mesh_layer_4[];
extern Gfx bbh_dl_Room7Layer1_mesh_layer_1[];
extern Gfx bbh_dl_Room7Layer4_mesh_layer_4[];
extern Gfx bbh_dl_Room8Layer1_mesh_layer_1[];
extern Gfx bbh_dl_Room8Layer4_mesh_layer_4[];
extern Gfx bbh_dl_Room9Layer1_mesh_layer_1[];
extern Gfx bbh_dl_Room9Layer4_mesh_layer_4[];
extern u8 prevRoom;
extern u8 curRoom;

#define room1DLs 2
#define room2DLs 5
#define room3DLs 4
#define room4DLs 2
#define room5DLs 3
#define room6DLs 2
#define room7DLs 2
#define room8DLs 2
#define room9DLs 2
Gfx *room1[room1DLs] = { &bbh_dl_Room1Layer1_mesh_layer_1, &bbh_dl_Room1Layer4_mesh_layer_4 };
Gfx *room2[room2DLs] = { &bbh_dl_Room2Layer1_mesh_layer_1, &bbh_dl_Room2Layer2_mesh_layer_2,
                         &bbh_dl_Room2Layer4_mesh_layer_4, &bbh_dl_Room2Layer5_mesh_layer_5,
                         &bbh_dl_Room2Layer6_mesh_layer_6 };
Gfx *room3[room3DLs] = { &bbh_dl_Room3Layer1_mesh_layer_1, &bbh_dl_Room3Layer2_mesh_layer_2,
                         &bbh_dl_Room3Layer4_mesh_layer_4, &bbh_dl_Room3Layer6_mesh_layer_6 };
Gfx *room4[room4DLs] = { &bbh_dl_Room4Layer1_mesh_layer_1, &bbh_dl_Room4Layer4_mesh_layer_4 };
Gfx *room5[room5DLs] = { &bbh_dl_Room5Layer1_mesh_layer_1, &bbh_dl_Room5Layer4_mesh_layer_4,
                         &bbh_dl_Room5Layer6_mesh_layer_6 };
Gfx *room6[room6DLs] = { &bbh_dl_Room6Layer1_mesh_layer_1, &bbh_dl_Room6Layer4_mesh_layer_4 };
Gfx *room7[room7DLs] = { &bbh_dl_Room7Layer1_mesh_layer_1, &bbh_dl_Room7Layer4_mesh_layer_4 };
Gfx *room8[room8DLs] = { &bbh_dl_Room8Layer1_mesh_layer_1, &bbh_dl_Room8Layer4_mesh_layer_4 };
Gfx *room9[room9DLs] = { &bbh_dl_Room9Layer1_mesh_layer_1, &bbh_dl_Room9Layer4_mesh_layer_4 };

Gfx **roomList[9] = { room1, room2, room3, room4, room5, room6, room7, room8, room9 };
u8 roomSizes[9] = { room1DLs, room2DLs, room3DLs, room4DLs, room5DLs,
                    room6DLs, room7DLs, room8DLs, room9DLs };
Gfx backupRooms[9][6];
s16 gearRots[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
#define ROOMCOUNT 9
void cullBBHRooms() {
    u8 i, j, k;
    Gfx *manip;
    if (curRoom > 0) {
        if (!backupRooms[0][0].words.w0) {
            for (i = 0; i < ROOMCOUNT; i++) {
                for (j = 0; j < roomSizes[i]; j++) {
                    backupRooms[i][j] = *((Gfx *) segmented_to_virtual(*(roomList[i] + j)));
                }
            }
        }
        for (i = 0; i < ROOMCOUNT; i++) {
            for (j = 0; j < roomSizes[i]; j++) {
                manip = segmented_to_virtual(*(roomList[i] + j));
                *manip = kill;
            }
        }
        for (j = 0; j < roomSizes[curRoom - 1]; j++) {
            manip = segmented_to_virtual(*(roomList[curRoom - 1] + j));
            *manip = backupRooms[curRoom - 1][j];
        }
        if ((gMarioState->action == ACT_PULLING_DOOR) || (gMarioState->action == ACT_PUSHING_DOOR)) {
            for (j = 0; j < roomSizes[prevRoom - 1]; j++) {
                manip = segmented_to_virtual(*(roomList[prevRoom - 1] + j));
                *manip = backupRooms[prevRoom - 1][j];
            }
        }
    }
}
f32 remainderf(f32 big, f32 small) {
    int div = (int) (big / small);
    return big - div * small;
}
void HSVtoRGB(u8 *h, u8 *s, u8 *v) {
    // return RGB in HSV
    f32 s2 = *s / 255.f;
    f32 v2 = *v / 255.f;
    f32 c = v2 * s2;
    f32 X = c * (1.f - absf((remainderf((*h / 42.5f), 2) - 1)));
    f32 M = v2 - c;
    f32 r2, g2, b2;
    if (*h < 43) {
        r2 = c;
        g2 = X;
        b2 = 0;
    } else if (*h < 85) {
        r2 = X;
        g2 = c;
        b2 = 0;
    } else if (*h < 128) {
        r2 = 0;
        g2 = c;
        b2 = X;
    } else if (*h < 170) {
        r2 = 0;
        g2 = X;
        b2 = c;
    } else if (*h < 213) {
        r2 = X;
        g2 = 0;
        b2 = c;
    } else {
        r2 = c;
        g2 = 0;
        b2 = X;
    }
    *h = (r2 + M) * 255.f;
    *s = (g2 + M) * 255.f;
    *v = (b2 + M) * 255.f;
}

void RGBtoHSV(u8 *r, u8 *g, u8 *b) {
    // return HSV in RGB
    f32 r2, g2, b2;
    f32 cmax, cmin, delta;
    r2 = *r / 255.f;
    g2 = *g / 255.f;
    b2 = *b / 255.f;
    cmax = max(max(r2, g2), max(g2, b2));
    cmin = min(min(r2, g2), min(g2, b2));
    delta = cmax - cmin;
    if (delta == 0) {
        *r = 0;
    } else if (cmax == r2) {
        *r = 42.5f * (g2 - b2) / delta;
    } else if (cmax == g2) {
        *r = 42.5f * (((b2 - r2) / delta) + 2.f);
    } else { // cmax == b2
        *r = 42.5f * (((r2 - g2) / delta) + 4.f);
    }
    if (cmax > 0) {
        *g = delta * 255.f / cmax;
    } else {
        *g = 0;
    }
    *b = cmax * 255.f;
}
u16 paletteScrollTimer = 0;
void RGBScrollPallete(s16 *palette, u8 paletteSize, s16 *base) {
    u8 i, r, g, b;
    paletteScrollTimer += 0x200;
    for (i = 0; i < paletteSize; i++) {
        r = (*base >> 11) * 8;
        g = ((*base >> 6) & 0x1F) * 8;
        b = ((*base >> 1) & 0x1F) * 8;
        RGBtoHSV(&r, &g, &b); // rewrite to only use one pointer
        r += (paletteScrollTimer >> 8);
        HSVtoRGB(&r, &g, &b); // rewrite to only use one pointer

        *palette = GPACK_RGBA5551(r, g, b, 1);
        palette++;
        base++;
    }
}

extern Gfx castle_grounds_dl_AANoCullLayer6_mesh_layer_6[];
extern Gfx BBsignBackup[10];
extern Gfx mat_castle_grounds_dl_BBB3[];
extern Gfx mat_castle_grounds_dl_BBB1[];
extern Gfx mat_castle_grounds_dl_BBB2[];

extern Vtx castle_grounds_dl_transparentwater_001_mesh_layer_5_vtx_0[22];
u16 watervertcount3 = sizeof(castle_grounds_dl_transparentwater_001_mesh_layer_5_vtx_0) / 0x10;
extern Gfx mat_castle_grounds_dl_CloudSky_layer5[];
extern Vtx castle_grounds_dl_CloudOcean_mesh_layer_5_vtx_0[48];
cloudvertcount = sizeof(castle_grounds_dl_CloudOcean_mesh_layer_5_vtx_0) / 0x10;
#include "levels/wf/header.h"
u16 wobblyvertCount = sizeof(wf_dl_Wobbly_mesh_layer_1_vtx_0) / 0x10;
extern Vtx castle_grounds_dl_CloudOcean_001_mesh_layer_5_vtx_0[48];
cloudvertcount2 = sizeof(castle_grounds_dl_CloudOcean_mesh_layer_5_vtx_0) / 0x10;
extern u8 bbh_dl_NightskyAnimpng_i4[];
extern void animateSails(void);
u16 watervertcount2 = sizeof(castle_grounds_dl_WaterAnimationOW_mesh_layer_5_vtx_0) / 0x10;
u32 waterphase = 0;
Gfx a = gsDPSetTextureImage(G_IM_FMT_I, G_IM_SIZ_8b, 64, 0);
extern Gfx mat_castle_grounds_dl_BOWSERLIGHTS[];
extern u8 castle_grounds_dl__1ebdd000_fmt0431_slice0_mip00_128x128_tm04_ci4_pal_rgba16[];
extern Gfx mat_whalespout_SpoutScroll_layer5[];
extern u8 paletteBackup[30];
extern u8 paletteBackup2[32];
extern u8 paletteBackup3[32];
extern Gfx mat_lavafloor_LAVAMAT_layer1[];
extern Vtx lavafloor_LAVA_mesh_layer_1_vtx_0[64];
u8 lavaVTXCnt = sizeof(lavafloor_LAVA_mesh_layer_1_vtx_0) / 0x10;
extern Gfx mat_bitdw_dl_NEONARROW[];
#include "levels/bitdw/header.h"
#include "levels/wf/header.h"
u16 gearVerts = sizeof(bitdw_dl_ZGEARS_mesh_layer_6_vtx_0) / 0x10;
extern Gfx mat_wf_dl_UNDERGROUNDWATER_layer5[];
extern u8 bitdw_dl_r_arrow03_ci4_pal_rgba16[];
extern s8 startWave;
void textureanim(void) {
    int i;
    int j;
    Gfx *b;
    s32 alpha;
    u8 brightness;
    Vtx *a = segmented_to_virtual(ccm_dl_WaterAnimation_mesh_layer_5_vtx_0);
    Gfx *scrollMaterial /* = segmented_to_virtual(mat_ccm_dl_ZOOWATER)*/;
    int old;
    u8 *watertexture;
    u8 *lightTexture = segmented_to_virtual(bbh_dl_LightBeam2_i8);
    u8 *nightSky = segmented_to_virtual(bbh_dl_NightskyAnimpng_i4);
    s16 *paletteIndex;
    s16 angle;
    switch (o->oBehParams2ndByte) {
        case 0:
            buttontimer++;
            if (!(buttontimer & 0x0F)) {
                Gfx *a = segmented_to_virtual(mat_bob_dl_buttonanimationbob);
                if (buttontimer & 0x10) {
                    a[5].words.w1 = bob_dl_button1bob_rgba16;
                } else {
                    a[5].words.w1 = button2bob;
                }
            }
            break;
        case 1:
            // move by 0x200 on x or 0x400 on y
            j = (o->oTimer % 50);
            if ((j == 7) || (j == 15) || (j == 23) || (j == 32) || (j == 41) || (j == 49)) {
                animateNips();
            }
            break;
        case 2:
            b = segmented_to_virtual(castle_grounds_dl_N64Logo_mesh);
            j = (o->oTimer / 4) & 0x0F;
            b[0] = N64LogoFrames[j];
            break;
        case 3:
            waterphase += 0x140;
#define SCROLLSIZE 12
#define WAVEHEIGHTMAX (15.f)
            for (i = 0; i < watervertcount; i++) {
                a[i].n.ob[1] = -347 + sins(a[i].n.ob[0] * SCROLLSIZE + waterphase) * WAVEHEIGHTMAX
                               + sins(a[i].n.ob[2] * SCROLLSIZE + waterphase + 0x2000) * WAVEHEIGHTMAX;
            }
            /* water_render();
             scrollMaterial[5].words.w1 = water_get_src() & 0x00FFFFFF;*/
            if (!o->oTimer) {
                setMarioLight(255, 200, 200);
            }
            break;
        case 4:
            // scrollMaterial = segmented_to_virtual(mat_castle_grounds_dl_OWWATER);
            a = segmented_to_virtual(castle_grounds_dl_WaterAnimationOW_mesh_layer_5_vtx_0);
            /*water_render();
            scrollMaterial[5].words.w1 = water_get_src() & 0x00FFFFFF;*/

            waterphase += 0x07;
            if (!a[0].n.flag) {
                for (i = 0; i < watervertcount2; i++) {
                    a[i].n.flag = a[i].n.ob[1] + 1;
                }
            }
#define SCROLLSIZE 12
#define WAVEHEIGHTMAX (25.f)
            for (i = 0; i < watervertcount2; i++) {
                a[i].n.ob[1] =
                    a[i].n.flag + sins(a[i].n.ob[0] * SCROLLSIZE + waterphase * 0x20) * WAVEHEIGHTMAX
                    + sins(a[i].n.ob[2] * SCROLLSIZE + waterphase * 0x20 + 0x2000) * WAVEHEIGHTMAX;
            }
            scrollMaterial = segmented_to_virtual(mat_castle_grounds_dl_CloudSky_layer5);
#define SCROLLBITFLAG _SHIFTL(0xFFF, 12, 12)
#define SCROLLBITFLAG2 _SHIFTL(0xFFF, 0, 12)
            // alpha value
            /*old = (scrollMaterial[21].words.w0 & SCROLLBITFLAG) >> 12;
            scrollMaterial[21].words.w0 =
                ((scrollMaterial[21].words.w0 & (~SCROLLBITFLAG)) | _SHIFTL(old + 1, 12, 12));*/
            // clouds
            old = (scrollMaterial[13].words.w0 & SCROLLBITFLAG) >> 12;
            scrollMaterial[13].words.w0 =
                ((scrollMaterial[13].words.w0 & (~SCROLLBITFLAG)) | _SHIFTL(old - 2, 12, 12));

            /*old = (scrollMaterial[13].words.w0 & SCROLLBITFLAG2) >> 0;
            scrollMaterial[13].words.w0 =
                ((scrollMaterial[13].words.w0 & (~SCROLLBITFLAG2)) | _SHIFTL(old + 1, 0, 12));*/

            a = segmented_to_virtual(castle_grounds_dl_CloudOcean_mesh_layer_5_vtx_0);
            if (!a[0].n.flag) {
                for (i = 0; i < cloudvertcount; i++) {
                    a[i].n.flag = a[i].n.ob[1] + 1;
                }
            }
#define SCROLLSIZE 5
#define WAVEHEIGHTMAX (1024)
            for (i = 0; i < cloudvertcount; i++) {
                a[i].n.ob[1] = a[i].n.flag
                               + sins(a[i].n.ob[2] * SCROLLSIZE + waterphase * 17) * WAVEHEIGHTMAX
                               + sins((a[i].n.ob[0] + a[i].n.ob[2] / 4) * SCROLLSIZE + waterphase * 21)
                                     * WAVEHEIGHTMAX;
                if ((s16) a[i].n.flag > -200) {
                    if (a[i].n.ob[1] > -WAVEHEIGHTMAX / 2) {
                        alpha = a[i].v.cn[3] + 1;
                    } else {
                        alpha = a[i].v.cn[3] - 1;
                    }
                    if (alpha < 0) {
                        alpha = 0;
                    }
                    if (alpha > 255) {
                        alpha = 255;
                    }
                    a[i].v.cn[3] = alpha;
                }
                brightness = (a[i].n.ob[1] * 2 + (WAVEHEIGHTMAX * 2)) / 64;
                a[i].v.cn[0] = 0x7F +brightness;
                a[i].v.cn[1] = 0x3F;
                a[i].v.cn[2] = 0x7F + brightness;
            }
            break;
        case 5: // CuckyDev: Water renderer
            /* water_render();
             scrollMaterial[5].words.w1 = water_get_src() & 0x00FFFFFF;*/
            break;
        case 6:
            animateMice();
            break;
        case 7:
            // zoo water refraction
            scrollMaterial = segmented_to_virtual(mat_ccm_dl_WATERRIPPLE_layer6);
#define SCROLLBITFLAG _SHIFTL(0xFFF, 12, 12)
#define SCROLLBITFLAG2 _SHIFTL(0xFFF, 0, 12)
            old = (scrollMaterial[21].words.w0 & SCROLLBITFLAG) >> 12;
            scrollMaterial[21].words.w0 =
                ((scrollMaterial[21].words.w0 & (~SCROLLBITFLAG)) | _SHIFTL(old + 2, 12, 12));

            old = (scrollMaterial[13].words.w0 & SCROLLBITFLAG) >> 12;
            scrollMaterial[13].words.w0 =
                ((scrollMaterial[13].words.w0 & (~SCROLLBITFLAG)) | _SHIFTL(old - 2, 12, 12));

            old = (scrollMaterial[13].words.w0 & SCROLLBITFLAG2) >> 0;
            scrollMaterial[13].words.w0 =
                ((scrollMaterial[13].words.w0 & (~SCROLLBITFLAG2)) | _SHIFTL(old + 2, 0, 12));
            break;
        case 8:
            makeMarioGREY(255);
            a = segmented_to_virtual(ccm_dl_Lava_mesh_layer_1_vtx_0);
            waterphase += 0x280;
            if (!o->oTimer) {
                for (i = 0; i < lavavertcount; i++) {
                    a[i].n.flag = a[i].n.ob[1];
                }
            }
#define SCROLLSIZE 150
#define WAVEHEIGHTMAX (15.f)
            for (i = 0; i < lavavertcount; i++) {
                a[i].n.ob[1] = a[i].n.flag
                               + sins(a[i].n.ob[0] * SCROLLSIZE + waterphase) * WAVEHEIGHTMAX
                               + sins(a[i].n.ob[2] * SCROLLSIZE + waterphase + 0x2000) * WAVEHEIGHTMAX;
            }
            break;
        case 9:
            animateSails();
            water_renderI4();
            a = segmented_to_virtual(bbh_dl_OCEAN_mesh_layer_1_vtx_0);
            setMarioLight(192 - LIGHTINGTIMER, 192, 192);
            waterphase += 0x10;
            if (!a[0].n.flag) {
                for (i = 0; i < oceanvertcout; i++) {
                    a[i].n.flag = a[i].n.ob[1] + 1;

                    pixelIndex[i] = getVertIndex(a[i].n.ob[0], a[i].n.ob[2]);
                }
            }
#define SCROLLSIZE 15
#define WAVEHEIGHTMAX (170)
            watertexture = water_get_srcI4();
            for (i = 0; i < oceanvertcout; i++) {
                a[i].n.ob[1] = /* a[i].n.flag
                                + */
                    sins(a[i].n.ob[2] * SCROLLSIZE + waterphase * 17) * WAVEHEIGHTMAX
                    + sins((a[i].n.ob[0] + a[i].n.ob[2] / 4) * SCROLLSIZE + waterphase * 16)
                          * WAVEHEIGHTMAX;
                if (a[i].n.ob[1] > 300) {
                    a[i].v.cn[3] += 3;
                    if (a[i].v.cn[3] > 192) {
                        a[i].v.cn[3] = 192;
                    }
                } else {
                    a[i].v.cn[3] -= 1;
                    if (a[i].v.cn[3] > 192) {
                        a[i].v.cn[3] = 0;
                    }
                }
            }

            scrollMaterial = segmented_to_virtual(mat_bbh_dl_OCEANWATER_layer1_area1);
            scrollMaterial[9].words.w1 = water_get_srcI4() & 0x00FFFFFF;
#define SCROLLBITFLAG _SHIFTL(0xFFF, 12, 12)
#define SCROLLBITFLAG2 _SHIFTL(0xFFF, 0, 12)
            old = (scrollMaterial[23].words.w0 & SCROLLBITFLAG) >> 12;
            scrollMaterial[23].words.w0 =
                ((scrollMaterial[23].words.w0 & (~SCROLLBITFLAG)) | _SHIFTL(old + 11, 12, 12));
            old = (scrollMaterial[23].words.w0 & SCROLLBITFLAG2) >> 0;
            scrollMaterial[23].words.w0 =
                ((scrollMaterial[23].words.w0 & (~SCROLLBITFLAG2)) | _SHIFTL(old - 13, 0, 12));
            /*if (gMarioState->pos[1] + gMarioState->vel[1] < -1223.f) {
                gMarioState->health = 0;
                set_mario_action(gMarioState, ACT_DROWNING, 0);
            }
            if (gMarioState->action == ACT_DROWNING) {
                gMarioState->pos[1] = approach_f32_asymptotic(
                    gMarioState->pos[1],
                    -1223.f + sins(gMarioState->pos[2] * SCROLLSIZE + waterphase * 17) *
            WAVEHEIGHTMAX
                        + sins((gMarioState->pos[0] + gMarioState->pos[2] / 4) * SCROLLSIZE
                               + waterphase * 16)
                              * WAVEHEIGHTMAX,
                    .5f);
                gMarioState->vel[1] = 0;
            }*/
            break;
        case 10:
            scrollMaterial = segmented_to_virtual(mat_bbh_dl_InsideWater);
            water_render();
            scrollMaterial[5].words.w1 = water_get_src() & 0x00FFFFFF;
            if (gMarioState->floor) {
                if (SURFACETYPE(gMarioState->floor) == SURFACE_DEEP_QUICKSAND) {
                    if (gMarioState->pos[1] == gMarioState->floorHeight) {
                        if (gMarioState->action != ACT_FEET_STUCK_IN_GROUND) {
                            play_sound(SOUND_MARIO_WAAAOOOW,
                                       gMarioState->marioObj->header.gfx.cameraToObject);
                        }
                        gMarioState->action = ACT_FEET_STUCK_IN_GROUND;
                        m->actionTimer = 0;
                        if (!o->oOpacity) {
                            play_transition(WARP_TRANSITION_FADE_INTO_COLOR, 0x0A, 0, 0, 0);
                        }
                        o->oOpacity++;
                        m->numKeys -= 5;
                        if (o->oOpacity == 10) {
                            o->oOpacity = 0;
                            gMarioState->action = ACT_BACKWARD_AIR_KB;
                            gMarioState->pos[0] = o->oPosX;
                            gMarioState->pos[1] = o->oPosY;
                            gMarioState->pos[2] = o->oPosZ;
                            play_transition(WARP_TRANSITION_FADE_FROM_COLOR, 10, 0, 0, 0);
                            m->numKeys = 0;
                            // m->hurtCounter = 4;
                        }
                    }
                }
            }
            break;
        case 11:
            pulsateBooGuy();
            cullBBHRooms();
            break;
        case 12:
            generateLightBeamTexture(lightTexture);
            if ((absf(gMarioState->pos[0] - o->oPosX) + absf(gMarioState->pos[1] - o->oPosY)
                 + absf(gMarioState->pos[2] - o->oPosZ))
                < 50.f) {
                curRoom = 2;
            }
            break;
        case 13:
            generateNightSky(nightSky);
            break;
        case 14:
            // scrollMaterial = segmented_to_virtual(mat_castle_grounds_dl_OWWATER);
            a = segmented_to_virtual(castle_grounds_dl_transparentwater_001_mesh_layer_5_vtx_0);
            /*water_render();
            scrollMaterial[5].words.w1 = water_get_src() & 0x00FFFFFF;*/

            waterphase += 0x07;
            if (!a[0].n.flag) {
                for (i = 0; i < watervertcount3; i++) {
                    a[i].n.flag = a[i].n.ob[1] + 1;
                }
            }
#define SCROLLSIZE 12
#define WAVEHEIGHTMAX (25.f)
            for (i = 0; i < watervertcount3; i++) {
                a[i].n.ob[1] =
                    a[i].n.flag + sins(a[i].n.ob[0] * SCROLLSIZE + waterphase * 0x20) * WAVEHEIGHTMAX
                    + sins(a[i].n.ob[2] * SCROLLSIZE + waterphase * 0x20 + 0x2000) * WAVEHEIGHTMAX;
            }

            scrollMaterial = segmented_to_virtual(mat_castle_grounds_dl_CloudSky_layer5);
#define SCROLLBITFLAG _SHIFTL(0xFFF, 12, 12)
#define SCROLLBITFLAG2 _SHIFTL(0xFFF, 0, 12)
            old = (scrollMaterial[13].words.w0 & SCROLLBITFLAG) >> 12;
            scrollMaterial[13].words.w0 =
                ((scrollMaterial[13].words.w0 & (~SCROLLBITFLAG)) | _SHIFTL(old - 2, 12, 12));

            a = segmented_to_virtual(castle_grounds_dl_CloudOcean_001_mesh_layer_5_vtx_0);
            if (!a[0].n.flag) {
                for (i = 0; i < cloudvertcount2; i++) {
                    a[i].n.flag = a[i].n.ob[1] + 1;
                }
            }
#define SCROLLSIZE 5
#define WAVEHEIGHTMAX (1024)
            for (i = 0; i < cloudvertcount2; i++) {
                a[i].n.ob[1] = a[i].n.flag
                               + sins(a[i].n.ob[2] * SCROLLSIZE + waterphase * 17) * WAVEHEIGHTMAX
                               + sins((a[i].n.ob[0] + a[i].n.ob[2] / 4) * SCROLLSIZE + waterphase * 21)
                                     * WAVEHEIGHTMAX;
                if ((s16) a[i].n.flag > -200) {
                    if (a[i].n.ob[1] > -WAVEHEIGHTMAX / 2) {
                        alpha = a[i].v.cn[3] + 1;
                    } else {
                        alpha = a[i].v.cn[3] - 1;
                    }
                    if (alpha < 0) {
                        alpha = 0;
                    }
                    if (alpha > 255) {
                        alpha = 255;
                    }
                    a[i].v.cn[3] = alpha;
                }
                brightness = (a[i].n.ob[1] * 2 + (WAVEHEIGHTMAX * 2)) / 64;
                a[i].v.cn[0] = 0x7F;
                +brightness;
                a[i].v.cn[1] = 0x3F;
                a[i].v.cn[2] = 0x7F + brightness;
            }
            if (!(o->oTimer & 7)) {
                scrollMaterial = segmented_to_virtual(mat_castle_grounds_dl_BOWSERLIGHTS);
#define SCROLLBITFLAG _SHIFTL(0xFFF, 12, 12)
#define SCROLLBITFLAG2 _SHIFTL(0xFFF, 0, 12)
                old = (scrollMaterial[18].words.w0 & SCROLLBITFLAG2) >> 0;
                if (!(o->oTimer & 8) && !(o->oTimer & 0x30)) {
                    scrollMaterial[18].words.w0 =
                        ((scrollMaterial[18].words.w0 & (~SCROLLBITFLAG2)) | _SHIFTL(old + 19, 0, 12));
                } else {
                    scrollMaterial[18].words.w0 =
                        ((scrollMaterial[18].words.w0 & (~SCROLLBITFLAG2)) | _SHIFTL(old + 18, 0, 12));
                }
                /*                paletteIndex = segmented_to_virtual(
                                    castle_grounds_dl__1ebdd000_fmt0431_slice0_mip00_128x128_tm04_ci4_pal_rgba16);*/
                RGBScrollPallete(
                    segmented_to_virtual(
                        castle_grounds_dl__1ebdd000_fmt0431_slice0_mip00_128x128_tm04_ci4_pal_rgba16),
                    15, segmented_to_virtual(paletteBackup));
            }
            if (!(((Gfx *) (segmented_to_virtual(BBsignBackup)))[0].words.w0)) {
                for (i = 0; i < 10; i++) {
                    ((Gfx *) (segmented_to_virtual(BBsignBackup)))[i] = ((Gfx *) (segmented_to_virtual(
                        castle_grounds_dl_AANoCullLayer6_mesh_layer_6)))[i];
                }
            }
            switch (o->oSubAction) {
                case 0:
                case 4:
                case 6:
                    ((Gfx *) (segmented_to_virtual(castle_grounds_dl_AANoCullLayer6_mesh_layer_6)))[0] =
                        ((Gfx *) (segmented_to_virtual(BBsignBackup)))[9];
                    break;
                case 1:
                    ((Gfx *) (segmented_to_virtual(castle_grounds_dl_AANoCullLayer6_mesh_layer_6)))[0] =
                        ((Gfx *) (segmented_to_virtual(BBsignBackup)))[0];
                    ((Gfx *) (segmented_to_virtual(castle_grounds_dl_AANoCullLayer6_mesh_layer_6)))[3] =
                        ((Gfx *) (segmented_to_virtual(BBsignBackup)))[9];
                    break;
                case 2:
                    ((Gfx *) (segmented_to_virtual(castle_grounds_dl_AANoCullLayer6_mesh_layer_6)))[3] =
                        ((Gfx *) (segmented_to_virtual(BBsignBackup)))[3];
                    ((Gfx *) (segmented_to_virtual(castle_grounds_dl_AANoCullLayer6_mesh_layer_6)))[6] =
                        ((Gfx *) (segmented_to_virtual(BBsignBackup)))[9];
                    break;
                case 3:
                case 5:
                case 7:
                    ((Gfx *) (segmented_to_virtual(castle_grounds_dl_AANoCullLayer6_mesh_layer_6)))[0] =
                        ((Gfx *) (segmented_to_virtual(BBsignBackup)))[0];
                    ((Gfx *) (segmented_to_virtual(castle_grounds_dl_AANoCullLayer6_mesh_layer_6)))[6] =
                        ((Gfx *) (segmented_to_virtual(BBsignBackup)))[6];
                    break;
            }
            if (o->oOpacity < 8) {
                cur_obj_play_sound_2(SOUND_AIR_AMP_BUZZ);
            }
            if (o->oOpacity > 20 /* + ((o->oSubAction + 1) & 4) * 8*/) {
                o->oSubAction = (o->oSubAction + 1) & 7;
                o->oOpacity = 0;
            }
            o->oOpacity++;
            break;
        case 15:
            scrollMaterial = segmented_to_virtual(mat_whalespout_SpoutScroll_layer5);
#define SCROLLBITFLAG _SHIFTL(0xFFF, 12, 12)
#define SCROLLBITFLAG2 _SHIFTL(0xFFF, 0, 12)
            old = (scrollMaterial[13].words.w0 & SCROLLBITFLAG2) >> 0;
            scrollMaterial[13].words.w0 =
                ((scrollMaterial[13].words.w0 & (~SCROLLBITFLAG2)) | _SHIFTL(old - 8, 0, 12));
            old = (scrollMaterial[21].words.w0 & SCROLLBITFLAG2) >> 0;
            scrollMaterial[21].words.w0 =
                ((scrollMaterial[21].words.w0 & (~SCROLLBITFLAG2)) | _SHIFTL(old + 1, 0, 12));
#define SCROLLBITFLAG _SHIFTL(0xFFF, 12, 12)
#define SCROLLBITFLAG2 _SHIFTL(0xFFF, 0, 12)
            old = (scrollMaterial[13].words.w0 & SCROLLBITFLAG2) >> 0;
            scrollMaterial[13].words.w0 =
                ((scrollMaterial[13].words.w0 & (~SCROLLBITFLAG2)) | _SHIFTL(old - 8, 0, 12));
            old = (scrollMaterial[21].words.w0 & SCROLLBITFLAG2) >> 0;
            scrollMaterial[21].words.w0 =
                ((scrollMaterial[21].words.w0 & (~SCROLLBITFLAG2)) | _SHIFTL(old + 1, 0, 12));
            if (((s8 *) o)[0xF0]++ % 4 == 0) {
                cur_obj_play_sound_2(SOUND_OBJ_WHALEFALL); // //SOUND_OBJ_DIVING_INTO_WATER
            }
            break;
        case 16:
            scrollMaterial = segmented_to_virtual(mat_lavafloor_LAVAMAT_layer1);
#define SCROLLBITFLAG _SHIFTL(0xFFF, 12, 12)
#define SCROLLBITFLAG2 _SHIFTL(0xFFF, 0, 12)
            /*old = (scrollMaterial[21].words.w0 & SCROLLBITFLAG) >> 12;
            scrollMaterial[21].words.w0 =
                ((scrollMaterial[21].words.w0 & (~SCROLLBITFLAG)) | _SHIFTL(old + 1, 12, 12));*/

            /*       old = (scrollMaterial[13].words.w0 & SCROLLBITFLAG) >> 12;
                   scrollMaterial[13].words.w0 =
                       ((scrollMaterial[13].words.w0 & (~SCROLLBITFLAG)) | _SHIFTL(old - 1, 12,
               12));*/

            old = (scrollMaterial[13].words.w0 & SCROLLBITFLAG2) >> 0;
            scrollMaterial[13].words.w0 =
                ((scrollMaterial[13].words.w0 & (~SCROLLBITFLAG2)) | _SHIFTL(old + 1, 0, 12));
            a = segmented_to_virtual(lavafloor_LAVA_mesh_layer_1_vtx_0);
#define LAVATIMER (((o->oOpacity & 0x3F) > 0x1F) ? (o->oOpacity & 0x3F) : (0x3F - (o->oOpacity & 0x3F)))
            o->oOpacity++;
            setMarioLight(128 + LAVATIMER * 2, 128 + LAVATIMER, 128);
            // put light from below
            setLightDirection(0, 0xff, 0);
            waterphase += 0x4;
            if (!a[0].n.flag) {
                for (i = 0; i < lavaVTXCnt; i++) {
                    a[i].n.flag = a[i].n.ob[1] + 1;
                }
            }
#define SCROLLSIZE 15
#define WAVEHEIGHTMAX (17)
            for (i = 0; i < lavaVTXCnt; i++) {
                a[i].n.ob[1] = a[i].n.flag
                               + sins(a[i].n.ob[2] * SCROLLSIZE + waterphase * 17) * WAVEHEIGHTMAX
                               + sins((a[i].n.ob[0] + a[i].n.ob[2] / 4) * SCROLLSIZE + waterphase * 16)
                                     * WAVEHEIGHTMAX;
                // maxcolor = B99527
                if (a[i].n.ob[1] > 5) {
                    alpha = a[i].v.cn[3] + 3;
                } else {
                    alpha = a[i].v.cn[3] - 5;
                }
                if (alpha < 0) {
                    alpha = 0;
                }
                if (alpha > 192) {
                    alpha = 192;
                }
                a[i].v.cn[3] = alpha;
                a[i].v.cn[0] = 0xB0 * (alpha / 255.f);
                a[i].v.cn[1] = 0x90 * (alpha / 255.f);
                a[i].v.cn[2] = 0x24 * (alpha / 255.f);
            }

            a = segmented_to_virtual(bitdw_dl_ZGEARS_mesh_layer_6_vtx_0);
#define UVMIDDLE 1008
            if (!gearRots[0]) {
                for (i = 0; i < gearVerts; i++) {
                    gearRots[i] = atan2s(a[i].v.tc[1] - UVMIDDLE, a[i].v.tc[0] - UVMIDDLE);
                }
            }
#define UZVSIZESQRT2 1448.f
            for (i = 0; i < gearVerts; i++) {
                a[i].v.tc[0] = UVMIDDLE + sins(gearRots[i] - 0xD0 * o->oTimer) * UZVSIZESQRT2;
                a[i].v.tc[1] = UVMIDDLE + coss(gearRots[i] - 0xD0 * o->oTimer) * UZVSIZESQRT2;
            }

            if (!(o->oTimer & 7)) {
                scrollMaterial = segmented_to_virtual(mat_bitdw_dl_NEONARROW);
#define SCROLLBITFLAG _SHIFTL(0xFFF, 12, 12)
#define SCROLLBITFLAG2 _SHIFTL(0xFFF, 0, 12)
                old = (scrollMaterial[18].words.w0 & SCROLLBITFLAG) >> 12;
                scrollMaterial[18].words.w0 =
                    ((scrollMaterial[18].words.w0 & (~SCROLLBITFLAG)) | _SHIFTL(old - 22, 12, 12));
                RGBScrollPallete(segmented_to_virtual(bitdw_dl_r_arrow03_ci4_pal_rgba16), 16,
                                 segmented_to_virtual(paletteBackup2));
                paletteScrollTimer += 0x800;
            }
            RGBScrollPallete(segmented_to_virtual(bitdw_dl_KKIIDDZZ_DAT_2599__p0044_ci4_pal_rgba16), 16,
                             segmented_to_virtual(paletteBackup3));

            break;
        case 17:
            scrollMaterial = segmented_to_virtual(mat_wf_dl_UNDERGROUNDWATER_layer5);
#define SCROLLBITFLAG _SHIFTL(0xFFF, 12, 12)
#define SCROLLBITFLAG2 _SHIFTL(0xFFF, 0, 12)
            // ALSO MAKE MARIO GREY LIKE CRYSTALLIGHTS
            old = (scrollMaterial[13].words.w0 & SCROLLBITFLAG) >> 12;
            scrollMaterial[13].words.w0 =
                ((scrollMaterial[13].words.w0 & (~SCROLLBITFLAG)) | _SHIFTL(old + 1, 12, 12));

            break;
        case 18:
#define SCROLLSIZE 3
#define WAVEHEIGHTMAX (100.f)
            a = segmented_to_virtual(wf_dl_Wobbly_mesh_layer_1_vtx_0);
            waterphase += 0x50;
            if (!a[0].n.flag) {
                for (i = 0; i < sizeof(wf_dl_Wobbly_mesh_layer_1_vtx_0) / 0x10; i++) {
                    a[i].n.flag = a[i].n.ob[1];
                }
            }
            for (i = 0; i < sizeof(wf_dl_Wobbly_mesh_layer_1_vtx_0) / 0x10; i++) {
                a[i].n.ob[1] = a[i].n.flag
                               + sins(a[i].n.ob[2] * SCROLLSIZE + waterphase * 5) * WAVEHEIGHTMAX
                               + sins((a[i].n.ob[0] + a[i].n.ob[2] / 4) * SCROLLSIZE + waterphase * 4)
                                     * WAVEHEIGHTMAX;
            }

            a = segmented_to_virtual(wf_dl_Wobbly_mesh_layer_1_vtx_1);
            if (!a[0].n.flag) {
                for (i = 0; i < sizeof(wf_dl_Wobbly_mesh_layer_1_vtx_1) / 0x10; i++) {
                    a[i].n.flag = a[i].n.ob[1];
                }
            }
            for (i = 0; i < sizeof(wf_dl_Wobbly_mesh_layer_1_vtx_1) / 0x10; i++) {
                a[i].n.ob[1] = a[i].n.flag
                               + sins(a[i].n.ob[2] * SCROLLSIZE + waterphase * 5) * WAVEHEIGHTMAX
                               + sins((a[i].n.ob[0] + a[i].n.ob[2] / 4) * SCROLLSIZE + waterphase * 4)
                                     * WAVEHEIGHTMAX;
            }

            a = segmented_to_virtual(wf_dl_Wobbly_mesh_layer_1_vtx_2);
            if (!a[0].n.flag) {
                for (i = 0; i < sizeof(wf_dl_Wobbly_mesh_layer_1_vtx_2) / 0x10; i++) {
                    a[i].n.flag = a[i].n.ob[1];
                }
            }
            for (i = 0; i < sizeof(wf_dl_Wobbly_mesh_layer_1_vtx_2) / 0x10; i++) {
                a[i].n.ob[1] = a[i].n.flag
                               + sins(a[i].n.ob[2] * SCROLLSIZE + waterphase * 5) * WAVEHEIGHTMAX
                               + sins((a[i].n.ob[0] + a[i].n.ob[2] / 4) * SCROLLSIZE + waterphase * 4)
                                     * WAVEHEIGHTMAX;
            }
            a = segmented_to_virtual(wf_dl_WAVEY_mesh_layer_5_vtx_0);
            if (a[2].v.flag == 0) {
                for (i = 0; i < sizeof(wf_dl_WAVEY_mesh_layer_5_vtx_0) / 0x10; i++) {
                    a[i].v.flag = a[i].v.cn[3];
                }
            }

            if (startWave) {
                o->oVelX = 10.f;
                startWave = 0;
            }
            o->oVelY += o->oVelX * .1f;
            o->oVelZ += o->oVelY * .09f;
            o->oVelX = approach_f32_symmetric(o->oVelX, 0.f, 0.3f);
            o->oVelY = approach_f32_symmetric(o->oVelY, 0.f, 0.35f);
            o->oVelZ = approach_f32_symmetric(o->oVelZ, 0.f, 0.4f);
            o->oPosX += o->oVelX;
            o->oPosY += o->oVelY;
            o->oPosZ += o->oVelZ;
            #define DECAY 2.f
            o->oPosX = approach_f32_symmetric(o->oPosX, 0.f, DECAY*2.5f);
            o->oPosY = approach_f32_symmetric(o->oPosY, 0.f, DECAY);
            o->oPosZ = approach_f32_symmetric(o->oPosZ, 0.f, DECAY);
            if (o->oPosX > 200.f){
                o->oPosX = 200.f;
            }
            if (o->oPosY > 200.f){
                o->oPosY = 200.f;
            }
            if (o->oPosZ > 200.f){
                o->oPosZ = 200.f;
            }
            for (i = 0; i < sizeof(wf_dl_WAVEY_mesh_layer_5_vtx_0) / 0x10; i++) {
               // a[i].n.tc[1] -= 0x0040;
                switch (a[i].v.flag) {
                    case 0xFF:
                        a[i].v.cn[3] = o->oPosX;
                        break;
                    case 0x60:
                        a[i].v.cn[3] = o->oPosY;
                        break;
                    case 0x50:
                        a[i].v.cn[3] = o->oPosZ;
                        break;
                }
            }
            break;
    }
}

Gfx envColor[] = {
    gsDPSetEnvColor(64, 64, 64, 64),
    gsSPEndDisplayList(),
};
u8 bolted = 0;
#define LIGHTINGTIMER (envColor[0].words.w1 & 0x000000FF)
Gfx *geo_cause_env_lightning(s32 callContext, struct GraphNode *b, Mat4 *mtx) {
    Gfx *gfx = NULL;
    u8 brightness;
    struct GraphNodeGenerated *asGenerated = (struct GraphNodeGenerated *) b;
    if (callContext == GEO_CONTEXT_RENDER) {
        gfx = &envColor[0];
        if (!asGenerated->parameter) {
            brightness = (envColor[0].words.w1 & 0x000000FF);
            if ((brightness > 30) && ((random_u16() % 175) == 0)) {
                brightness = 0;
                bolted = 1;
            } else {
                brightness = approach_s16_symmetric(brightness, 64, 1);
            }
            envColor[0].words.w1 =
                brightness + brightness * 256 + brightness * 256 * 256 + brightness * 256 * 256 * 256;
        }
        asGenerated->fnNode.node.flags =
            (asGenerated->fnNode.node.flags & 0xFF) | (asGenerated->parameter << 8);
    }
    return gfx;
}

extern const u8 mipshappy[];
extern Gfx mat_bunny_MIPSFACE[];
void setmipstexture(void) {
    int i;
    Gfx *a = segmented_to_virtual(mat_bunny_MIPSFACE);
    a[4].words.w1 = mipshappy;
}
