#include <PR/ultratypes.h>

#include "sm64.h"
#include "actors/common1.h"
#include "gfx_dimensions.h"
#include "game_init.h"
#include "level_update.h"
#include "camera.h"
#include "print.h"
#include "ingame_menu.h"
#include "hud.h"
#include "segment2.h"
#include "area.h"
#include "save_file.h"
#include "print.h"
#include "../../enhancements/puppycam.h"
#include "behavior_data.h"
#include "OPT_FOR_SIZE.h"

int standStillTimer = 0;
#define HUDWAITTIME 40
#define HUDTIME 70
/* @file hud.c
 * This file implements HUD rendering and power meter animations.
 * That includes stars, lives, coins, camera status, power meter, timer
 * cannon reticle, and the unused keys.
 **/

struct PowerMeterHUD {
    s8 animation;
    s16 x;
    s16 y;
    f32 unused;
};

struct UnusedHUDStruct {
    u32 unused1;
    u16 unused2;
    u16 unused3;
};

struct CameraHUD {
    s16 status;
};

// Stores health segmented value defined by numHealthWedges
// When the HUD is rendered this value is 8, full health.
static s16 sPowerMeterStoredHealth;

static struct PowerMeterHUD sPowerMeterHUD = {
    POWER_METER_HIDDEN,
    140,
    166,
    1.0,
};

// Power Meter timer that keeps counting when it's visible.
// Gets reset when the health is filled and stops counting
// when the power meter is hidden.
s32 sPowerMeterVisibleTimer = 0;

static struct UnusedHUDStruct sUnusedHUDValues = { 0x00, 0x0A, 0x00 };

static struct CameraHUD sCameraHUD = { CAM_STATUS_NONE };

/**
 * Renders a rgba16 16x16 glyph texture from a table list.
 */
void render_hud_tex_lut(s32 x, s32 y, u8 *texture) {
    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture);
    gSPDisplayList(gDisplayListHead++, &dl_hud_img_load_tex_block);
    gSPTextureRectangle(gDisplayListHead++, x << 2, y << 2, (x + 15) << 2, (y + 15) << 2,
                        G_TX_RENDERTILE, 0, 0, 4 << 10, 1 << 10);
}

/**
 * Renders a rgba16 8x8 glyph texture from a table list.
 */
__attribute__((always_inline)) inline void render_hud_small_tex_lut(s32 x, s32 y, u8 *texture) {
    gDPSetTile(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0,
               G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR,
               G_TX_NOMASK, G_TX_NOLOD);
    gDPTileSync(gDisplayListHead++);
    gDPSetTile(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 2, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 3,
               G_TX_NOLOD, G_TX_CLAMP, 3, G_TX_NOLOD);
    gDPSetTileSize(gDisplayListHead++, G_TX_RENDERTILE, 0, 0, (8 - 1) << G_TEXTURE_IMAGE_FRAC,
                   (8 - 1) << G_TEXTURE_IMAGE_FRAC);
    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture);
    gDPLoadSync(gDisplayListHead++);
    gDPLoadBlock(gDisplayListHead++, G_TX_LOADTILE, 0, 0, 8 * 8 - 1, CALC_DXT(8, G_IM_SIZ_16b_BYTES));
    gSPTextureRectangle(gDisplayListHead++, x << 2, y << 2, (x + 7) << 2, (y + 7) << 2, G_TX_RENDERTILE,
                        0, 0, 4 << 10, 1 << 10);
}

/**
 * Renders power meter health segment texture using a table list.
 */
__attribute__((always_inline)) inline void render_power_meter_health_segment(s16 numHealthWedges) {
    u8 *(*healthLUT)[];

    healthLUT = segmented_to_virtual(&power_meter_health_segments_lut);

    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1,
                       (*healthLUT)[numHealthWedges - 1]);
    gDPLoadSync(gDisplayListHead++);
    gDPLoadBlock(gDisplayListHead++, G_TX_LOADTILE, 0, 0, 32 * 32 - 1,
                 CALC_DXT(32, G_IM_SIZ_16b_BYTES));
    gSP1Triangle(gDisplayListHead++, 0, 1, 2, 0);
    gSP1Triangle(gDisplayListHead++, 0, 2, 3, 0);
}

/**
 * Renders power meter display lists.
 * That includes the "POWER" base and the colored health segment textures.
 */
__attribute__((always_inline)) inline void render_dl_power_meter(s16 numHealthWedges) {
    Mtx *mtx;
    mtx = alloc_display_list(sizeof(Mtx));
    guTranslate(mtx, (f32) sPowerMeterHUD.x - 50.f, (f32) sPowerMeterHUD.y + 16.f, 0);

    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx++), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, &dl_power_meter_base);

    if (numHealthWedges != 0) {
        gSPDisplayList(gDisplayListHead++, &dl_power_meter_health_segments_begin);
        render_power_meter_health_segment(numHealthWedges);
        gSPDisplayList(gDisplayListHead++, &dl_power_meter_health_segments_end);
    }

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/**
 * Power meter animation called when there's less than 8 health segments
 * Checks its timer to later change into deemphasizing mode.
 */
__attribute__((always_inline)) inline void animate_power_meter_emphasized(void) {

    if (!(gHudDisplay.flags & HUD_DISPLAY_FLAG_EMPHASIZE_POWER)) {
        if (sPowerMeterVisibleTimer == 45.0) {
            sPowerMeterHUD.animation = POWER_METER_DEEMPHASIZING;
        }
    } else {
        sPowerMeterVisibleTimer = 0;
    }
}

/**
 * Power meter animation called after emphasized mode.
 * Moves power meter y pos speed until it's at 200 to be visible.
 */
__attribute__((always_inline)) inline void animate_power_meter_deemphasizing(void) {
    s16 speed = 5;

    if (sPowerMeterHUD.y >= 181) {
        speed = 3;
    }

    if (sPowerMeterHUD.y >= 191) {
        speed = 2;
    }

    if (sPowerMeterHUD.y >= 196) {
        speed = 1;
    }

    sPowerMeterHUD.y += speed;

    if (sPowerMeterHUD.y >= 201) {
        sPowerMeterHUD.y = 200;
        sPowerMeterHUD.animation = POWER_METER_VISIBLE;
    }
}

/**
 * Power meter animation called when there's 8 health segments.
 * Moves power meter y pos quickly until it's at 301 to be hidden.
 */
__attribute__((always_inline)) inline void animate_power_meter_hiding(void) {
    sPowerMeterHUD.y += 20;
    if (sPowerMeterHUD.y >= 301) {
        sPowerMeterHUD.animation = POWER_METER_HIDDEN;
        sPowerMeterVisibleTimer = 0;
    }
}

/**
 * Handles power meter actions depending of the health segments values.
 */
__attribute__((always_inline)) inline void handle_power_meter_actions(s16 numHealthWedges) {
    // Show power meter if health is not full, less than 8
    if (numHealthWedges < 8 && sPowerMeterStoredHealth == 8
        && sPowerMeterHUD.animation == POWER_METER_HIDDEN) {
        sPowerMeterHUD.animation = POWER_METER_EMPHASIZED;
        sPowerMeterHUD.y = 166;
    }

    // Show power meter if health is full, has 8
    if (numHealthWedges == 8 && sPowerMeterStoredHealth == 7) {
        sPowerMeterVisibleTimer = 0;
    }

    // After health is full, hide power meter
    if (numHealthWedges == 8 && sPowerMeterVisibleTimer > 45.0) {
        sPowerMeterHUD.animation = POWER_METER_HIDING;
    }

    // Update to match health value
    sPowerMeterStoredHealth = numHealthWedges;

    // If Mario is swimming, keep power meter visible
    if (gMarioState->action & ACT_FLAG_SWIMMING) {
        if (sPowerMeterHUD.animation == POWER_METER_HIDDEN
            || sPowerMeterHUD.animation == POWER_METER_EMPHASIZED) {
            sPowerMeterHUD.animation = POWER_METER_DEEMPHASIZING;
            sPowerMeterHUD.y = 166;
        }
        sPowerMeterVisibleTimer = 0;
    }
}

/**
 * Renders the power meter that shows when Mario is in underwater
 * or has taken damage and has less than 8 health segments.
 * And calls a power meter animation function depending of the value defined.
 */
__attribute__((always_inline)) inline void render_hud_power_meter(void) {
    s16 shownHealthWedges = gHudDisplay.wedges;

    if (sPowerMeterHUD.animation != POWER_METER_HIDING) {
        handle_power_meter_actions(shownHealthWedges); // may set state to hidden and stop render
    }

    if (sPowerMeterHUD.animation == POWER_METER_HIDDEN) {
        return;
    }

    render_dl_power_meter(shownHealthWedges);
}

#ifdef VERSION_JP
#define HUD_TOP_Y 210
#else
#define HUD_TOP_Y 209
#endif

extern s8 missionActive;
extern s8 orangeYoshApressCount;
__attribute__((always_inline)) inline void render_yoshi_missions() {
    switch (missionActive) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            if (orangeYoshApressCount >= 0) {
                print_text_fmt_int(12, 14, "%d A PRESSES LEFT", orangeYoshApressCount); // 'Coin' glyph
            } else {
                print_text_fmt_int(12, 14, "MISSION FAILED", orangeYoshApressCount); // 'Coin' glyph
            }
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            break;
        case 10:
            break;
        default:
            break;
    }
}

int coinsaction = 0;
int coinsactionTimer = 0;
int priorcoins;
s16 coinOffset = 0;
#define HUD_STARS_X 78

__attribute__((always_inline)) inline void render_hud_coins(void) {
    if (sCurrPlayMode == 2) {
        print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X), HUD_TOP_Y, "+"); // 'Coin' glyph
        print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X) + 16, HUD_TOP_Y, "*"); // 'X' glyph
        print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X) + 30, HUD_TOP_Y, "%d",
                           gHudDisplay.coins);
        return;
    }
    switch (coinsaction) {
        case 1:
            print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X),
                       HUD_TOP_Y + (8 - coinsactionTimer) * 2,
                       "+"); // 'Coin' glyph
            print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X) + 16,
                       HUD_TOP_Y + (8 - coinsactionTimer) * 2,
                       "*"); // 'X' glyph
            print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X) + 30,
                               HUD_TOP_Y + (8 - coinsactionTimer) * 2, "%d", gHudDisplay.coins);
            break;
        case 2:
            // stay
            if ((coinOffset = (priorcoins != gHudDisplay.coins)) || (standStillTimer > 0)) {
                coinsactionTimer = 0;
            }
            print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X), HUD_TOP_Y + coinOffset,
                       "+"); // 'Coin' glyph
            print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X) + 16, HUD_TOP_Y + coinOffset,
                       "*"); // 'X' glyph
            print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X) + 30,
                               HUD_TOP_Y + coinOffset, "%d", gHudDisplay.coins);
            break;
        case 3:
            // disappear;
            print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X),
                       HUD_TOP_Y + coinsactionTimer * 2, "+"); // 'Coin' glyph
            print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X) + 16,
                       HUD_TOP_Y + coinsactionTimer * 2, "*"); // 'X' glyph
            print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X) + 30,
                               HUD_TOP_Y + coinsactionTimer * 2, "%d", gHudDisplay.coins);
            break;
    }
}

int starsaction = 0;
int starsactionTimer = 0;
int priorstars;
/**
 * Renders the amount of stars collected.
 * Disables "X" glyph when Mario has 100 stars or more.
 */
__attribute__((always_inline)) inline void render_hud_stars(void) {
    if (sCurrPlayMode == 2) {
        print_text(12, HUD_TOP_Y,
                   "-"); // 'Coin' glyph
        print_text(12 + 16, HUD_TOP_Y,
                   "*"); // 'X' glyph
        print_text_fmt_int(12 + 30, HUD_TOP_Y, "%d", gMarioState->numStars);
        return;
    } else if (gHudFlash == 1 && gGlobalTimer & 0x08) {
        return;
    } else {
        switch (starsaction) {
            case 1:
                print_text(12, HUD_TOP_Y + (8 - starsactionTimer) * 2,
                           "-"); // 'Coin' glyph
                print_text(12 + 16, HUD_TOP_Y + (8 - starsactionTimer) * 2,
                           "*"); // 'X' glyph
                print_text_fmt_int(12 + 30, HUD_TOP_Y + (8 - starsactionTimer) * 2, "%d",
                                   gMarioState->numStars);
                break;
            case 2:
                // stay
                print_text(12, HUD_TOP_Y,
                           "-"); // 'Coin' glyph
                print_text(12 + 16, HUD_TOP_Y,
                           "*"); // 'X' glyph
                print_text_fmt_int(12 + 30, HUD_TOP_Y, "%d", gMarioState->numStars);
                break;
            case 3:
                // disappear;
                print_text(12, HUD_TOP_Y + starsactionTimer * 2, "-");      // 'Coin' glyph
                print_text(12 + 16, HUD_TOP_Y + starsactionTimer * 2, "*"); // 'X' glyph
                print_text_fmt_int(12 + 30, HUD_TOP_Y + starsactionTimer * 2, "%d",
                                   gMarioState->numStars);
                break;
        }
        priorstars = gMarioState->numStars;
    }
}

/**
 * Unused function that renders the amount of keys collected.
 * Leftover function from the beta version of the game.
 */
u32 timeSinceNoMash;
__attribute__((always_inline)) inline void
render_hud_keys(void) { // actually renders B mash overlay bmashoverlay
    u8 *(*cameraLUT)[6];
    if ((gMarioState->action == ACT_GRINDING)
        || ((gMarioState->action == ACT_RIDE_MINECART) && (gCurrLevelNum == LEVEL_JRB))) {
        timeSinceNoMash++;
        if (gMarioState->controller->buttonPressed & B_BUTTON) {
            timeSinceNoMash = 0;
        }
        if (timeSinceNoMash > 120) {
            cameraLUT = segmented_to_virtual(&main_hud_camera_lut);
            gSPDisplayList(gDisplayListHead++, dl_hud_img_begin);
            if (gGlobalTimer & 0x10) {
                render_hud_tex_lut(154, 170, (*cameraLUT)[GLYPH_CAM_LAKITU_HEAD]);
            } else {
                render_hud_tex_lut(154, 170, (*cameraLUT)[GLYPH_CAM_CAMERA]);
            }
            gSPDisplayList(gDisplayListHead++, dl_hud_img_end);
        }
    } else {
        timeSinceNoMash = 0;
    }
}

u8 sceneTimer = 0;
extern struct CutsceneJump *currentScene;
void hud_logic(void) {
    if (gMarioState->action
        & ACT_FLAG_STATIONARY) { // todo: hanging on zoo vine thingies shouldnt trigger this
        standStillTimer++;
    } else {
        standStillTimer = 0;
    }

    if (sPowerMeterHUD.animation != POWER_METER_HIDDEN) {
        switch (sPowerMeterHUD.animation) {
            case POWER_METER_EMPHASIZED:
                animate_power_meter_emphasized();
                break;
            case POWER_METER_DEEMPHASIZING:
                animate_power_meter_deemphasizing();
                break;
            case POWER_METER_HIDING:
                animate_power_meter_hiding();
                break;
            default:
                break;
        }

        sPowerMeterVisibleTimer += 1;
    }

    if (gHudFlash && gGlobalTimer & 0x08) {
    } else {
        // Coins
        switch (coinsaction) {
            case 0:
                // dont display
                if ((priorcoins != gHudDisplay.coins) || (standStillTimer > HUDWAITTIME)) {
                    coinsaction = 1;
                    coinsactionTimer = 0;
                }
                break;
            case 1:
                if (coinsactionTimer == 8) {
                    coinsaction = 2;
                    coinsactionTimer = 0;
                }
                break;
            case 2:
                // stay
                if ((coinOffset = (priorcoins != gHudDisplay.coins)) || (standStillTimer > 0)) {
                    coinsactionTimer = 0;
                }
                if (coinsactionTimer == HUDTIME) {
                    coinsaction = 3;
                    coinsactionTimer = 0;
                }
                break;
            case 3:
                // Disappear
                if (priorcoins != gHudDisplay.coins) {
                    coinsaction = 1;
                    coinsactionTimer = 8 - coinsactionTimer;
                }
                if (coinsactionTimer == 7) {
                    coinsaction = 0;
                    coinsactionTimer = 0;
                }
                break;
        }
        coinsactionTimer++;
        priorcoins = gHudDisplay.coins;
        // Stars
        switch (starsaction) {
            case 0:
                // dont display
                if ((priorstars != gMarioState->numStars) || (standStillTimer > HUDWAITTIME)) {
                    starsaction = 1;
                    starsactionTimer = 0;
                }
                break;
            case 1:
                if (starsactionTimer == 8) {
                    starsaction = 2;
                    starsactionTimer = 0;
                }
                break;
            case 2:
                // stay
                if ((priorstars != gMarioState->numStars) || (standStillTimer > 0)) {
                    starsactionTimer = 0;
                }
                if (starsactionTimer == HUDTIME) {
                    starsaction = 3;
                    starsactionTimer = 0;
                }
                break;
            case 3:
                if (priorstars != gMarioState->numStars) {
                    starsaction = 1;
                    starsactionTimer = 8 - starsactionTimer;
                }
                if (starsactionTimer == 7) {
                    starsaction = 0;
                    starsactionTimer = 0;
                }
                break;
        }

        starsactionTimer++;
    }
}
extern u8 huntIsOn;
u8 goombacount = 0;
__attribute__((always_inline)) inline void print_special_strings(void) {
    if ((gCurrLevelNum == LEVEL_BOB) && (gCurrAreaIndex == 2) && (gCurrActNum == 2)) {
        print_text_fmt_int(12, 14, "?%02d", 10 - goombacount);
        print_text_fmt_int(48, 14, "!%02d", 10);
    }
    if ((gCurrLevelNum == LEVEL_CCM) && (gCurrActNum == 2) && huntIsOn) {
        print_text_fmt_int(12, 14, "#%02d", huntIsOn - 1);
        print_text_fmt_int(48, 14, "!%02d", 5);
    } else {
        huntIsOn = 0;
    }
}
u16 breath = MAXBREATH; // max 256 decrease by 1 a frame
#define breathPosY 120.f
#define breathPosZ 90.f
#include "engine/math_util.h"
extern s16 newcam_pitch;
extern u8 breathmeter_Airmeterempty_rgba16[];
extern u8 breathmeter_Airmeterfull_rgba16[];
extern u8 breathmeter_breathmeter_nothing_rgba16[];
__attribute__((always_inline)) inline void render_breath() {
    u16 i;
    u16 *fulltex = segmented_to_virtual(breathmeter_Airmeterfull_rgba16);
    u16 *emptytex = segmented_to_virtual(breathmeter_Airmeterempty_rgba16);
    u16 *rendertex = segmented_to_virtual(breathmeter_breathmeter_nothing_rgba16);
#define POSX 160
#define HALFWIDTH 32
#define CENTERX (POSX - HALFWIDTH)
#define CENTERY ((s16) breathPosZ - sins(gMarioState->faceAngle[0]) * 25.f)

    for (i = 0; (i < breath / 6) && (i < 125); i++) {
        rendertex[i + 128 * 0] = fulltex[i + 128 * 0];
        rendertex[i + 128 * 1] = fulltex[i + 128 * 1];
        rendertex[i + 128 * 2] = fulltex[i + 128 * 2];
        rendertex[i + 128 * 3] = fulltex[i + 128 * 3];
        rendertex[i + 128 * 4] = fulltex[i + 128 * 4];
        rendertex[i + 128 * 5] = fulltex[i + 128 * 5];
        rendertex[i + 128 * 6] = fulltex[i + 128 * 6];
        rendertex[i + 128 * 7] = fulltex[i + 128 * 7];
        rendertex[i + 128 * 8] = fulltex[i + 128 * 8];
        rendertex[i + 128 * 9] = fulltex[i + 128 * 9];
        rendertex[i + 128 * 10] = fulltex[i + 128 * 10];
        rendertex[i + 128 * 11] = fulltex[i + 128 * 11];
        rendertex[i + 128 * 12] = fulltex[i + 128 * 12];
        rendertex[i + 128 * 13] = fulltex[i + 128 * 13];
        rendertex[i + 128 * 14] = fulltex[i + 128 * 14];
        rendertex[i + 128 * 15] = fulltex[i + 128 * 15];
    }
    for (; i < 125; i++) {
        rendertex[i + 128 * 0] = emptytex[i + 128 * 0];
        rendertex[i + 128 * 1] = emptytex[i + 128 * 1];
        rendertex[i + 128 * 2] = emptytex[i + 128 * 2];
        rendertex[i + 128 * 3] = emptytex[i + 128 * 3];
        rendertex[i + 128 * 4] = emptytex[i + 128 * 4];
        rendertex[i + 128 * 5] = emptytex[i + 128 * 5];
        rendertex[i + 128 * 6] = emptytex[i + 128 * 6];
        rendertex[i + 128 * 7] = emptytex[i + 128 * 7];
        rendertex[i + 128 * 8] = emptytex[i + 128 * 8];
        rendertex[i + 128 * 9] = emptytex[i + 128 * 9];
        rendertex[i + 128 * 10] = emptytex[i + 128 * 10];
        rendertex[i + 128 * 11] = emptytex[i + 128 * 11];
        rendertex[i + 128 * 12] = emptytex[i + 128 * 12];
        rendertex[i + 128 * 13] = emptytex[i + 128 * 13];
        rendertex[i + 128 * 14] = emptytex[i + 128 * 14];
        rendertex[i + 128 * 15] = emptytex[i + 128 * 15];
    }
    gDPSetCycleType(gDisplayListHead++, G_CYC_COPY);
    gDPSetRenderMode(gDisplayListHead++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetAlphaCompare(gDisplayListHead++, G_AC_THRESHOLD);
    gDPLoadSync(gDisplayListHead++);
    gDPLoadTextureTile(gDisplayListHead++, breathmeter_breathmeter_nothing_rgba16, G_IM_FMT_RGBA,
                       G_IM_SIZ_16b, 128, 16, 0, 0, 127 + 0, 15 + 0, 0, G_TX_NOMIRROR | G_TX_WRAP,
                       G_TX_NOMIRROR | G_TX_WRAP, 7, 4, -1, -1);
    gSPScisTextureRectangle(gDisplayListHead++, CENTERX * 4, CENTERY * 4, (63 + CENTERX) * 4,
                            (7 + CENTERY) * 4, G_TX_RENDERTILE, 0, 0, 4 << 10, 1 << 10);

    gDPSetTexturePersp(gDisplayListHead++, G_TP_PERSP);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetAlphaCompare(gDisplayListHead++, G_AC_NONE);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    // gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
}
/**
 * Render HUD strings using hudDisplayFlags with it's render functions,
 * excluding the cannon reticle which detects a camera preset for it.
 */
extern Gfx logo_Plane_mesh[];
void render_radar() {
    Mtx *mtx;
    mtx = alloc_display_list(sizeof(Mtx));
    guTranslate(mtx, 160.f, 120.f, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx++), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, &logo_Plane_mesh);

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}
extern Gfx end_Plane_mesh[];
void render_radar2() {
    Mtx *mtx;
    mtx = alloc_display_list(sizeof(Mtx));
    guTranslate(mtx, 160.f, 120.f, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx++), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, &end_Plane_mesh);

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

#include "text_strings.h"
u8 curText = 0;
u8 curOpacity = 0;
u8 textAction = 0;
u8 credTexts[][25] = { { CRED1 }, { CRED3 }, { CRED1 }, { CRED2 } };
#include "buffers/buffers.h"
extern void print_generic_stringBIG(s16 x, s16 y, const u8 *str);
extern struct Object *gMarioObject;
void render_hud(void) {
    s16 hudDisplayFlags;
    u8 opacity;
    create_dl_ortho_matrix();
    if (currentScene) {
        opacity = sceneTimer * 25;
        render_radar();
    } else if (gCurrLevelNum == LEVEL_PSS) {
        render_radar2();
// print total time it took since game start
// print credits
#define HEIGHT 0x40
#define WIDTH 10
        if (gSaveBuffer.files[0][0].savestateTimer < 1800000) {
            print_text_fmt_int(WIDTH, HEIGHT, "%03d MIN",
                               gSaveBuffer.files[0][0].savestateTimer / 1800);
            print_text_fmt_int(WIDTH + 92, HEIGHT, "%02d SEC",
                               (gSaveBuffer.files[0][0].savestateTimer % 1800) / 30);
            print_text_fmt_int(WIDTH + 180, HEIGHT, "%02d FRAMES",
                               (gSaveBuffer.files[0][0].savestateTimer % 30));
        } else {
            print_text_fmt_int(WIDTH, HEIGHT, "%03d MIN", 999);
            print_text_fmt_int(WIDTH + 92, HEIGHT, "%02d SEC", 59);
            print_text_fmt_int(WIDTH + 180, HEIGHT, "%02d FRAMES", 29);
        }
        if (curOpacity == 0) {
            curText++;
            textAction = 1;
        } else if (curOpacity == 255) {
            textAction++;
        }
        if (curText > 3) {
            curOpacity = 2;
            return;
        }
        if (textAction == 1) {
            curOpacity = approach_s16_symmetric(curOpacity, 255, 5);
        } else if (textAction >= 255) {
            curOpacity = approach_s16_symmetric(curOpacity, 0, 5);
        }
#define STARTEXTY 40
#define MIDDLE 163
        gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
        gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, curOpacity);
        print_generic_stringBIG(get_str_x_pos_from_center(MIDDLE, credTexts[curText], 12.0f) + 1,
                                STARTEXTY - 9, credTexts[curText]);
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, curOpacity);
        print_generic_stringBIG(get_str_x_pos_from_center(MIDDLE, credTexts[curText], 12.0f),
                                STARTEXTY - 8, credTexts[curText]);

        gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
        return;
    }

    if (currentScene || (gCurrLevelNum == LEVEL_ENDING) || (!gMarioObject)) {
        return;
    }

    if (gCurrentArea != NULL && gCurrentArea->camera
        && gCurrentArea->camera->mode == CAMERA_MODE_INSIDE_CANNON) {
        render_hud_cannon_reticle();
    }

    print_special_strings();

    if (breath < BREATHMETERBAR) {
        render_breath();
    }
    if (gCurrCourseNum > 0) {
        render_hud_coins();
        render_yoshi_missions();
    }

    render_hud_stars();
    render_hud_keys();
    render_hud_power_meter();
}
void render_breath();