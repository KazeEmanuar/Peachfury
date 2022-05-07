#include <ultra64.h>

#include "sm64.h"
#include "gfx_dimensions.h"
#include "audio/external.h"
#include "buffers/buffers.h"
#include "buffers/gfx_output_buffer.h"
#include "buffers/framebuffers.h"
#include "buffers/zbuffer.h"
#include "engine/level_script.h"
#include "game_init.h"
#include "main.h"
#include "memory.h"
#include "profiler.h"
#include "save_file.h"
#include "seq_ids.h"
#include "sound_init.h"
#include "print.h"
#include "segment2.h"
#include "segment_symbols.h"
#include "thread6.h"
#include <prevent_bss_reordering.h>
#include "../../enhancements/puppycam.h"
#include "hud.h"
#include "ingame_menu.h"
#include "OPT_FOR_SIZE.h"

u32 gGameTime = 0;
OSMesgQueue gGraphicsVblankQueue;
OSMesg gGraphicsMesg;
OSMesg gGraphicsInit[2];
struct VblankHandler gGraphicsVblankHandler;
u32 gMoveSpeed = 1;

// FIXME: I'm not sure all of these variables belong in this file, but I don't
// know of a good way to split them
extern u8 finalBuild;
struct Controller gControllers[3];
struct SPTask *gGfxSPTask;
Gfx *gDisplayListHead;
u8 *gGfxPoolEnd;
struct GfxPool *gGfxPool;
OSContStatus gControllerStatuses[4];
OSContPad gControllerPads[4];
u8 gControllerBits;
s8 gEepromProbe;
OSMesgQueue gGameVblankQueue;
OSMesgQueue D_80339CB8;
OSMesg D_80339CD0[2];
OSMesg D_80339CD4;
struct VblankHandler gGameVblankHandler;
uintptr_t gPhysicalFrameBuffers[3];
uintptr_t gPhysicalZBuffer;
void *mAnimData;
#ifdef ANIMSWAP
void *mAnimDataSwap;
#endif
// void *D_80339CF4;
struct MarioAnimation gmAnim;
#ifdef ANIMSWAP
struct MarioAnimation gmAnimSwap;
#endif
struct MarioAnimation gDemo;
u32 gGlobalTimer = 0;

static u16 sCurrFBNum = 0;
u16 frameBufferIndex = 0;
struct Controller *gPlayer1Controller = &gControllers[0];
struct Controller *gPlayer2Controller = &gControllers[1];
// probably debug only, see note below
struct Controller *gPlayer3Controller = &gControllers[2];

#define FPS_COUNTER_X_POS 220
#define FPS_COUNTER_Y_POS 10

OSTime gLastOSTime = 0;
u16 gFPS = 0;

#define MAXFPS 61
#define ONESECOND (1562744 * 30)
u32 FPSEntries[MAXFPS];
u8 FPSEntryID = 0;
u8 getFPS() {
    u8 i;
    OSTime addedTime = 0;
    for (i = 0; i < MAXFPS; i++) {
        addedTime += FPSEntries[FPSEntryID - i + (MAXFPS * ((FPSEntryID - i) < 0))];
        if (addedTime > ONESECOND) {
            return (i + 1);
        }
    }
    return (MAXFPS - 1);
}
void render_fps(void) {
    OSTime newTime2 = osGetTime();
    FPSEntries[FPSEntryID] = (newTime2 - gLastOSTime);
    gLastOSTime = newTime2;
    FPSEntryID = (FPSEntryID + 1) % MAXFPS;
    gFPS = getFPS();
    print_text_fmt_int(FPS_COUNTER_X_POS, FPS_COUNTER_Y_POS, "FPS %d", gFPS);
}

struct NprimitiveAreas {
    u8 levelID;
    u8 area;
};
// putting this on will make textboxes crash
struct NprimitiveAreas Nprimitives[] = {
    { LEVEL_WF, 1 },
    { LEVEL_JRB, 6 },
};
int NprimitivesLenght = sizeof(Nprimitives) / 2;
/*u8 isException() {
    int i;// returning 1 crashes if a textbox is active?
    for (i = 0; i < NprimitivesLenght; i++) {
        if ((gCurrLevelNum == Nprimitives[i].levelID) && (gCurrAreaIndex == Nprimitives[i].area)) {
            return 1;
        }
    }
    return 0;
}*/
extern s16 SCREEN_WIDTH_MATH;
extern s16 SCREEN_HEIGHT_MATH;
extern s16 BORDER_HEIGHT_MATH;
extern s16 BORDER_WIDTH_MATH;
void my_rdp_init(void) {
    gDPPipeSync(gDisplayListHead++);
    // gDPPipelineMode(gDisplayListHead++, G_PM_1PRIMITIVE);
    gSPClipRatio(gDisplayListHead++, FRUSTRATIO_2);

    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    gDPSetCombineMode(gDisplayListHead++, G_CC_SHADE, G_CC_SHADE);

    gSPSetOtherMode(gDisplayListHead++, G_SETOTHERMODE_H, 4, 20,
                    G_PM_1PRIMITIVE | G_TL_TILE | G_TT_NONE | G_TD_CLAMP | G_TP_PERSP | G_TF_BILERP
                        | G_TC_FILT | G_CK_NONE | G_CD_MAGICSQ | G_CYC_FILL);


    gSPSetOtherMode(gDisplayListHead++, G_SETOTHERMODE_L, 0, 32, G_AC_NONE | G_RM_OPA_SURF | G_RM_OPA_SURF2);
    gDPPipeSync(gDisplayListHead++);
}

/**
 * Initializes the RSP's built-in geometry and lighting engines.
 * Most of these (with the notable exception of gSPNumLights), are
 * almost immediately overwritten.
 */
void my_rsp_init(void) {
    gSPClearGeometryMode(gDisplayListHead++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH | G_FOG
                                                 | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR
                                                 | G_LOD);

    gSPSetGeometryMode(gDisplayListHead++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK | G_LIGHTING);

    gSPNumLights(gDisplayListHead++, NUMLIGHTS_1);
    gSPTexture(gDisplayListHead++, 0, 0, 0, G_TX_RENDERTILE, G_OFF);

    // @bug Nintendo did not explicitly define the clipping ratio.
    // For Fast3DEX2, this causes the dreaded warped vertices issue
    // unless the clipping ratio is changed back to the intended value,
    // as Fast3DEX2 uses a different initial value than Fast3D(EX).
#ifdef F3DEX_GBI_2
    gSPClipRatio(gDisplayListHead++, FRUSTRATIO_1);
#endif
}

/** Clear the Z buffer. */
void clear_z_buffer(void) {
    gDPPipeSync(gDisplayListHead++);

    gDPSetDepthSource(gDisplayListHead++, G_ZS_PIXEL);
    gDPSetDepthImage(gDisplayListHead++, gPhysicalZBuffer);

    gDPSetColorImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH, gPhysicalZBuffer);
    gDPSetFillColor(gDisplayListHead++, GPACK_ZDZ(G_MAXFBZ, 0) << 16 | GPACK_ZDZ(G_MAXFBZ, 0));

    gDPFillRectangle(gDisplayListHead++, BORDER_WIDTH_MATH, BORDER_HEIGHT_MATH, SCREEN_WIDTH_MATH - 1,
                     SCREEN_HEIGHT_MATH - 1);
}

/** Sets up the final framebuffer image. */
void display_frame_buffer(void) {
    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetColorImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH,
                     gPhysicalFrameBuffers[frameBufferIndex]);
}

extern s16 SCREEN_WIDTH_MATH;
extern s16 SCREEN_HEIGHT_MATH;
extern s16 BORDER_HEIGHT_MATH;
extern s16 BORDER_WIDTH_MATH;
/** Clears the framebuffer, allowing it to be overwritten. */
void clear_frame_buffer(s32 color) {
    gDPPipeSync(gDisplayListHead++);

    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetCycleType(gDisplayListHead++, G_CYC_FILL);

    gDPSetFillColor(gDisplayListHead++, color);

    gDPFillRectangle(gDisplayListHead++, 0, 0, SCREEN_WIDTH - 1,
                     SCREEN_HEIGHT - 1); // ONLY HAPPENS ONCE SO THIS IS ALLOWED

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
}

/** Clears and initializes the viewport. */
void clear_viewport(Vp *viewport, s32 color) {
    s16 vpUlx = (viewport->vp.vtrans[0] - viewport->vp.vscale[0]) / 4 + 1;
    s16 vpUly = (viewport->vp.vtrans[1] - viewport->vp.vscale[1]) / 4 + 1;
    s16 vpLrx = (viewport->vp.vtrans[0] + viewport->vp.vscale[0]) / 4 - 2;
    s16 vpLry = (viewport->vp.vtrans[1] + viewport->vp.vscale[1]) / 4 - 2;

#ifdef WIDESCREEN
    vpUlx = GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(vpUlx);
    vpLrx = GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(SCREEN_WIDTH - vpLrx);
#endif

    gDPPipeSync(gDisplayListHead++);

    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetCycleType(gDisplayListHead++, G_CYC_FILL);

    gDPSetFillColor(gDisplayListHead++, color);
    gDPFillRectangle(gDisplayListHead++, vpUlx, vpUly, vpLrx, vpLry);

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
}

/** Draws the horizontal screen borders */
void draw_screen_borders(void) {
    gDPPipeSync(gDisplayListHead++);

    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetCycleType(gDisplayListHead++, G_CYC_FILL);

    gDPSetFillColor(gDisplayListHead++, GPACK_RGBA5551(0, 0, 0, 0) << 16 | GPACK_RGBA5551(0, 0, 0, 0));

    if (BORDER_HEIGHT_MATH > 1) {
        gDPFillRectangle(gDisplayListHead++, GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(0), 0,
                         GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(0) - 1, BORDER_HEIGHT_MATH - 1);
        gDPFillRectangle(gDisplayListHead++, GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(0), SCREEN_HEIGHT_MATH,
                         GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(0) - 1, SCREEN_HEIGHT - 1);

        if (BORDER_WIDTH_MATH > 1) {
            gDPFillRectangle(gDisplayListHead++, 0, 0, BORDER_WIDTH_MATH - 1, SCREEN_HEIGHT - 1);
            gDPFillRectangle(gDisplayListHead++, SCREEN_WIDTH_MATH, 0, SCREEN_WIDTH - 1,
                             SCREEN_HEIGHT - 1);
        }
    }
}

void make_viewport_clip_rect(Vp *viewport) {
    s16 vpUlx = (viewport->vp.vtrans[0] - viewport->vp.vscale[0]) / 4;
    s16 vpPly = (viewport->vp.vtrans[1] - viewport->vp.vscale[1]) / 4;
    s16 vpLrx = (viewport->vp.vtrans[0] + viewport->vp.vscale[0]) / 4 - 1;
    s16 vpLry = (viewport->vp.vtrans[1] + viewport->vp.vscale[1]) / 4 - 1;

    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, vpUlx, vpPly, vpLrx, vpLry);
}

/**
 * Loads the F3D microcodes.
 * Refer to this function if you would like to load
 * other microcodes (i.e. S2DEX).
 */
void create_task_structure(void) {
    s32 entries = gDisplayListHead - gGfxPool->buffer;

    gGfxSPTask->msgqueue = &D_80339CB8;
    gGfxSPTask->msg = (OSMesg) 2;
    gGfxSPTask->task.t.type = M_GFXTASK;
    gGfxSPTask->task.t.ucode_boot = rspbootTextStart;
    gGfxSPTask->task.t.ucode_boot_size = ((u8 *) rspbootTextEnd - (u8 *) rspbootTextStart);
    gGfxSPTask->task.t.flags = 0;
#ifdef F3DZEX_GBI_2
    gGfxSPTask->task.t.ucode = gspF3DZEX2_PosLight_fifoTextStart;
    gGfxSPTask->task.t.ucode_data = gspF3DZEX2_PosLight_fifoDataStart;
#elif F3DEX_GBI_2
    gGfxSPTask->task.t.ucode = gspF3DEX2_fifoTextStart;
    gGfxSPTask->task.t.ucode_data = gspF3DEX2_fifoDataStart;
#elif F3DEX_GBI
    gGfxSPTask->task.t.ucode = gspF3DEX_fifoTextStart;
    gGfxSPTask->task.t.ucode_data = gspF3DEX_fifoDataStart;
#else
    gGfxSPTask->task.t.ucode = gspFast3D_fifoTextStart;
    gGfxSPTask->task.t.ucode_data = gspFast3D_fifoDataStart;
#endif
    gGfxSPTask->task.t.ucode_size = SP_UCODE_SIZE; // (this size is ignored)
    gGfxSPTask->task.t.ucode_data_size = SP_UCODE_DATA_SIZE;
    gGfxSPTask->task.t.dram_stack = (u64 *) gGfxSPTaskStack;
    gGfxSPTask->task.t.dram_stack_size = SP_DRAM_STACK_SIZE8;
    gGfxSPTask->task.t.output_buff = gGfxSPTaskOutputBuffer;
    gGfxSPTask->task.t.output_buff_size =
        (u64 *) ((u8 *) gGfxSPTaskOutputBuffer + sizeof(gGfxSPTaskOutputBuffer));
    gGfxSPTask->task.t.data_ptr = (u64 *) &gGfxPool->buffer;
    gGfxSPTask->task.t.data_size = entries * sizeof(Gfx);
    gGfxSPTask->task.t.yield_data_ptr = (u64 *) gGfxSPTaskYieldBuffer;
    gGfxSPTask->task.t.yield_data_size = OS_YIELD_DATA_SIZE;
}

/** Starts rendering the scene. */
void init_render_image(void) {
    move_segment_table_to_dmem();
    my_rdp_init();
    my_rsp_init();
    clear_z_buffer();
    display_frame_buffer();
}

/** Ends the master display list. */
void end_master_display_list(void) {
    draw_screen_borders();

    gDPFullSync(gDisplayListHead++);
    gSPEndDisplayList(gDisplayListHead++);

    create_task_structure();
}

void draw_reset_bars(void) {
    s32 sp24;
    s32 sp20;
    s32 fbNum;
    u64 *sp18;

    if (gResetTimer != 0 && D_8032C648 < 15) {
        if (sCurrFBNum == 0) {
            fbNum = 2;
        } else {
            fbNum = sCurrFBNum - 1;
        }

        sp18 = (u64 *) PHYSICAL_TO_VIRTUAL(gPhysicalFrameBuffers[fbNum]);
        sp18 += D_8032C648++ * (SCREEN_WIDTH / 4);

        for (sp24 = 0; sp24 < ((SCREEN_HEIGHT / 16) + 1); sp24++) {
            // Must be on one line to match -O2
            for (sp20 = 0; sp20 < (SCREEN_WIDTH / 4); sp20++)
                *sp18++ = 0;
            sp18 += ((SCREEN_WIDTH / 4) * 14);
        }
    }

    osWritebackDCacheAll();
    osRecvMesg(&gGameVblankQueue, &D_80339BEC, OS_MESG_BLOCK);
    osRecvMesg(&gGameVblankQueue, &D_80339BEC, OS_MESG_BLOCK);
}

#define UNCACHEDRAM 0x20000000
extern u8 isEmu;
void rendering_init(void) {
    gGfxPool = &gGfxPools[0];
    set_segment_base_addr(1, gGfxPool->buffer);
    gGfxSPTask = &gGfxPool->spTask;
    gDisplayListHead = ((u32) gGfxPool->buffer) | UNCACHEDRAM;
    gGfxPoolEnd = (u8 *) (gGfxPool->buffer + GFX_POOL_SIZE);
    // gGfxPoolEnd = gDisplayListHead + GFX_POOL_SIZE + UNCACHEDRAM;
    init_render_image();
    clear_frame_buffer(0);
    end_master_display_list();
    send_display_list(&gGfxPool->spTask);
    if (!isEmu) {
        frameBufferIndex++;
    }
    gGameTime++;
}

void config_gfx_pool(void) {
    gGfxPool = &gGfxPools[gGameTime & 1];
    set_segment_base_addr(1, gGfxPool->buffer);
    gGfxSPTask = &gGfxPool->spTask;
    gDisplayListHead = (u32) gGfxPool->buffer | UNCACHEDRAM;
    gGfxPoolEnd = (u8 *) (gGfxPool->buffer + GFX_POOL_SIZE);
    // gGfxPoolEnd = gDisplayListHead + GFX_POOL_SIZE + UNCACHEDRAM;
}

/** Handles vsync. */
void display_and_vsync(void) {
    osRecvMesg(&D_80339CB8, &D_80339BEC, OS_MESG_BLOCK);
    send_display_list(&gGfxPool->spTask);
    osViSwapBuffer((void *) PHYSICAL_TO_VIRTUAL(gPhysicalFrameBuffers[sCurrFBNum]));
    if (!isEmu) {
        if (++sCurrFBNum == 3) {
            sCurrFBNum = 0;
        }
        if (++frameBufferIndex == 3) {
            frameBufferIndex = 0;
        }
    }
    gGameTime++;
}

// take the updated controller struct and calculate
// the new x, y, and distance floats.
void adjust_analog_stick(struct Controller *controller) {
    // reset the controller's x and y floats.

    // modulate the rawStickX and rawStickY to be the new f32 values by adding/subtracting 6.
    if (controller->rawStickX <= -8) {
        controller->stickX = controller->rawStickX + 6;
    } else if (controller->rawStickX >= 8) {
        controller->stickX = controller->rawStickX - 6;
    } else {
        controller->stickX = 0;
    }

    if (controller->rawStickY <= -8) {
        controller->stickY = controller->rawStickY + 6;
    } else if (controller->rawStickY >= 8) {
        controller->stickY = controller->rawStickY - 6;
    } else {
        controller->stickY = 0;
    }

    // calculate f32 magnitude from the center by vector length.
    controller->stickMag =
        sqrtf(controller->stickX * controller->stickX + controller->stickY * controller->stickY);

    // magnitude cannot exceed 64.0f: if it does, modify the values appropriately to
    // flatten the values down to the allowed maximum value.
    if (controller->stickMag > 64.f) {
        controller->stickX *= 64.f / controller->stickMag;
        controller->stickY *= 64.f / controller->stickMag;
        controller->stickMag = 64.f;
    }
}

// update the controller struct with available inputs if present.
extern s8 orangeYoshApressCount;

extern struct CutsceneJump *currentScene;
void read_controller_inputs(void) {
    s32 i;

    // if any controllers are plugged in, update the
    // controller information.
    if (gControllerBits) {
       // osRecvMesg(&gSIEventMesgQueue, &D_80339BEC, OS_MESG_BLOCK);
        osContGetReadData(&gControllerPads[0]);
#ifdef VERSION_SH
        release_rumble_pak_control();
#endif
    }

    for (i = 0; i < 2; i++) {
        struct Controller *controller = &gControllers[i];

        // if we're receiving inputs, update the controller struct
        // with the new button info.
        if (controller->controllerData != NULL) {
            if (currentScene) {
                controller->controllerData->button &= (A_BUTTON | B_BUTTON);
                controller->controllerData->stick_x = 0;
                controller->controllerData->stick_y = 0;
            }
            controller->rawStickX = controller->controllerData->stick_x;
            controller->rawStickY = controller->controllerData->stick_y;
            controller->buttonPressed = controller->controllerData->button
                                        & (controller->controllerData->button ^ controller->buttonDown);
            // 0.5x A presses are a good meme
            controller->buttonDown = controller->controllerData->button;
            if (orangeYoshApressCount >= 0) {
                if (controller->buttonPressed & A_BUTTON) {
                    orangeYoshApressCount--;
                }
            }
            adjust_analog_stick(controller);
        } else // otherwise, if the controllerData is NULL, 0 out all of the inputs.
        {
            controller->rawStickX = 0;
            controller->rawStickY = 0;
            controller->buttonPressed = 0;
            controller->buttonDown = 0;
            controller->stickX = 0;
            controller->stickY = 0;
            controller->stickMag = 0;
        }
    }
}

// initialize the controller structs to point at the OSCont information.
void init_controllers(void) {
    s16 port, cont;

    // set controller 1 to point to the set of status/pads for input 1 and
    // init the controllers.
    gControllers[0].statusData = &gControllerStatuses[0];
    gControllers[0].controllerData = &gControllerPads[0];
    osContInit(&gSIEventMesgQueue, &gControllerBits, &gControllerStatuses[0]);

    // strangely enough, the EEPROM probe for save data is done in this function.
    // save pak detection?
    gEepromProbe = osEepromProbe(&gSIEventMesgQueue);

    // loop over the 4 ports and link the controller structs to the appropriate
    // status and pad. Interestingly, although there are pointers to 3 controllers,
    // only 2 are connected here. The third seems to have been reserved for debug
    // purposes and was never connected in the retail ROM, thus gPlayer3Controller
    // cannot be used, despite being referenced in various code.
    for (cont = 0, port = 0; port < 2 && cont < 2; port++) {
        // is controller plugged in?
        if (gControllerBits & (1 << port)) {
            // the game allows you to have just 1 controller plugged
            // into any port in order to play the game. this was probably
            // so if any of the ports didn't work, you can have controllers
            // plugged into any of them and it will work.
#ifdef VERSION_SH
            gControllers[cont].port = port;
#endif
            gControllers[cont].statusData = &gControllerStatuses[port];
            gControllers[cont++].controllerData = &gControllerPads[port];
        }
    }
}

void setup_game_memory(void) {
    // UNUSED u8 pad[8];

    set_segment_base_addr(0, (void *) 0x80000000);
    osCreateMesgQueue(&D_80339CB8, &D_80339CD4, 1);
    osCreateMesgQueue(&gGameVblankQueue, &D_80339CD0, 2);
    osCreateMesgQueue(&gGraphicsVblankQueue, &gGraphicsInit, 2);
    gPhysicalZBuffer = VIRTUAL_TO_PHYSICAL(gZBuffer);
    gPhysicalFrameBuffers[0] = VIRTUAL_TO_PHYSICAL(gFrameBuffer0);
    gPhysicalFrameBuffers[1] = VIRTUAL_TO_PHYSICAL(gFrameBuffer1);
    gPhysicalFrameBuffers[2] = VIRTUAL_TO_PHYSICAL(gFrameBuffer2);

    mAnimData = main_pool_alloc(0x4000, MEMORY_POOL_LEFT);
    set_segment_base_addr(17, (void *) mAnimData);
    func_80278A78(&gmAnim, gMarioAnims, mAnimData);
#ifdef ANIMSWAP
    mAnimDataSwap = main_pool_alloc(0x4000, MEMORY_POOL_LEFT);
    set_segment_base_addr(17, (void *) mAnimDataSwap);
    func_80278A78(&gmAnimSwap, gMarioAnims, mAnimDataSwap);
#endif
    /* D_80339CF4 = main_pool_alloc(2048, MEMORY_POOL_LEFT);
     set_segment_base_addr(24, (void *) D_80339CF4);*/
    load_segment(0x10, _entrySegmentRomStart, _entrySegmentRomEnd, MEMORY_POOL_LEFT);
    load_segment_decompress(2, _segment2_mio0SegmentRomStart, _segment2_mio0SegmentRomEnd);
}

#include "ultra64.h"
#include "rcp.h"

// main game loop thread. runs forever as long as the game
// continues.

f32 __floatundisf(u32 c) {
    return (f32) c;
}

f64 __floatundidf(u32 c) {
    return (f64) c;
}

u32 last10Timers[8][10] = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

u32 avg10U32s(u32 *start) {
    s32 i;
    u32 ret = 0;
    for (i = 0; i < 10; i++) {
        ret += start[i];
    }
    return ret / 10;
}
extern u8 unlimitFPS;
u8 last10TimersID = 0;
u32 lvlscriptDuration = 0;
u32 thread9scriptDuration = 0;
u32 thread9scriptMinus = 0;
u8 thread9LogStarted = 0;
u32 thread4scriptDuration = 0;
u32 thread4scriptMinus = 0;
u8 thread4LogStarted = 0;
extern u32 sThreadTime[2];
void process_profiler(void) {
    u32 clockTime;
    u32 cmdTime;
    u32 PipeTime;
    u32 TmemTime;
    if (unlimitFPS && !isEmu) {
        render_fps();
    }
}

/*s16 approach_yaw(s16 curYaw, s16 target, f32 speed) {
    return (s16) (target - approach_f32_asymptotic(
        (s16) (target - curYaw),
        0,
        speed
    ));
}*/
#define APPROACHMAX 0x1F00
extern s16 abs_angle_diff(s16 x0, s16 x1);
s32 approach_angle(s32 current, s32 target) {
    register s32 diff1;
    register s32 ret;
    if ((diff1 = abs_angle_diff(current, target)) >= APPROACHMAX) {
        return target;
    }
    ret = (target + current) / 2;
    if ((diff1 < (absi(target - current + 0x10000))) && (diff1 < (absi(target - current - 0x10000)))) {
        return ret;
    } else {
        return ret + 0x8000;
    }
}
s32 approach_angleF(s32 current, s32 target) {
    register s32 diff1;
    register s32 ret;
    if ((diff1 = abs_angle_diff(current, target)) >= APPROACHMAX) {
        return target;
    }
    ret = current - (target - current) / 2;
    if ((diff1 < (absi(target - current + 0x10000))) && (diff1 < (absi(target - current - 0x10000)))) {
        return ret;
    } else {
        return ret + 0x8000;
    }
}
extern f32 absf(f32 x);
f32 approach_pos(f32 current, f32 target) {
    if (absf(current - target) > 400.f) {
        return target;
    }
    return current + (target - current) * .5f;
}
f32 approach_posF(f32 current, f32 target) {
    if (absf(current - target) > 400.f) {
        return target;
    }
    return current - (target - current) * .5f;
}

void warp_node(struct Object *node) {
    register s32 i;
    for (i = 0; i < 3; i++) {
        node->header.gfx.deltaCurrentPos[i] = node->header.gfx.pos[i];
        node->header.gfx.deltaCurrentRot[i] = node->header.gfx.angle[i];
        node->header.gfx.deltaCurrentScale[i] = node->header.gfx.scale[i];
    }
}

#define SECONDS_PER_CYCLE 0.00000002133f
u8 startThread = FALSE;
u64 lastRenderedFrame = 255;
extern s32 sGameLoopTicked;

void thread5_game_loop(UNUSED void *arg) {
    struct LevelCommand *addr;
    OSTime prevtime = 0;
    OSTime posttime = 0;

    setup_game_memory();
#ifdef VERSION_SH
    init_rumble_pak_scheduler_queue();
#endif
    init_controllers();
#ifdef VERSION_SH
    create_thread_6();
#endif
    save_file_load_all();

    set_vblank_handler(2, &gGameVblankHandler, &gGameVblankQueue, (OSMesg) 1);

    // point addr to the entry point into the level script data.
    addr = segmented_to_virtual(level_script_entry);

    play_music(SEQ_PLAYER_SFX, SEQUENCE_ARGS(0, SEQ_SOUND_PLAYER), 0);

    while (1) {
        if (!startThread) {
            osStartThread(&gGraphicsLoopThread);
            startThread = TRUE;
        }
        isEmulator();

        prevtime = osGetTime();

        // if any controllers are plugged in, start read the data for when
        // read_controller_inputs is called later.
        if (gControllerBits) {
#ifdef VERSION_SH
            block_until_rumble_pak_free();
#endif
            osContStartReadData(&gSIEventMesgQueue);
        }

        if (gPlayer1Controller->buttonPressed & R_JPAD) {
            gShowFPS ^= 1;
            gLastOSTime = osGetTime();
        }

        hud_logic();
        screen_transition_logic();
        gPauseScreenMode = ingame_menu_logic();

        sGameLoopTicked = 1;
        //read_controller_inputs();
        addr = level_script_execute(addr);
        gGlobalTimer++;
        posttime = osGetTime();
        lvlscriptDuration = posttime - prevtime;
        if (thread9LogStarted) {
            thread9scriptMinus += lvlscriptDuration;
        }
        if (thread4LogStarted) {
            thread4scriptMinus += lvlscriptDuration;
        }
        osRecvMesg(&gGameVblankQueue, &D_80339BEC, OS_MESG_BLOCK);
        osRecvMesg(&gGameVblankQueue, &D_80339BEC, OS_MESG_BLOCK);
    }
}

u8 unlimitFPS = 1;
u8 wisdeScreenMode = 0;
extern u8 isEmu;
void thread9_graphics_loop() {
    OSTime prevtime = 0;
    s32 deltatime = 0;
    OSTime posttime = 0;

    set_vblank_handler(4, &gGraphicsVblankHandler, &gGraphicsVblankQueue, (OSMesg) 1);
    rendering_init();
    while (1) {
        if (gResetTimer) {
            draw_reset_bars();
            continue;
        }

        deltatime = osGetTime() - prevtime;
        prevtime = osGetTime();
        thread9LogStarted = 1;
        thread9scriptMinus = 0;
        // What does gMoveSpeed do?
        // case 0: render with no interpolation
        // case 1: render while interpolating by half (this is for bonus VIs above 30fps)
        // case 2: advance the whole frame and then an additional half frame. this exists to smooth out
        // the visuals should we go below 30fps.
        if (isEmu) {
            gMoveSpeed = 1; // interpolate by half since this is an extra rendered frame
            if (lastRenderedFrame == gGlobalTimer) {
                gMoveSpeed = 0; // immidiately snap to the current frame since the interpolated
                                // frame was already rendered
            }
        } else {
#define CYCLEPERSCPU 46875.f
            #define TIMEPASSED(x) (x / CYCLEPERSCPU)
            if (TIMEPASSED(deltatime) < 16.72666666f) {
                gMoveSpeed = 1; // interpolate by half since this is an extra rendered frame
                if (lastRenderedFrame == gGlobalTimer) {
                    gMoveSpeed = 0; // immidiately snap to the current frame since the interpolated
                                    // frame was already rendered
                }
            } else if (TIMEPASSED(deltatime) > 47.95f) { // game assumes this frame takes as long to render as
                                             // the next one will
                switch (gGlobalTimer
                        - lastRenderedFrame) { // L stands for "last rendered VI". A stands for
                                               // "physics advanced that VI". T stands for "current
                                               // VI". N stands for "predicted next rendered VI"
                    case 1:
                        gMoveSpeed = 2; // LA, 0, A, T, A, 0, NA: move and rotate by distance +
                                        // distance/2, so that over 6 VIs, both rendered frames
                                        // are 1.5 frames apart visually.
                        break;
                    case 2:
                        gMoveSpeed = 0; // A, L, A, 0, TA: opposite scenario to case 1. 2 frames
                                        // advanced meaning it is not time to render the actual visuals.
                        break;
                    default:
                        gMoveSpeed = 0; // failsave mechanic.
                        break;
                }
            } else {
                gMoveSpeed = 0; // the default 2 VIs have passed and we simply render the current frame.
            }
        }
        lastRenderedFrame = gGlobalTimer;
        config_gfx_pool();
        init_render_image();
        render_game();
        end_master_display_list();
        alloc_display_list(0);
        posttime = osGetTime();
        thread9scriptDuration = posttime - prevtime - thread9scriptMinus;
        if (thread4LogStarted) {
            thread4scriptMinus += thread9scriptDuration;
        }
        thread9LogStarted = 0;
        display_and_vsync();
        if (!unlimitFPS){
            
        osRecvMesg(&gGraphicsVblankQueue, &gGraphicsMesg, OS_MESG_BLOCK);
        }

        osRecvMesg(&gGraphicsVblankQueue, &gGraphicsMesg, OS_MESG_BLOCK);
        process_profiler();
    }
}
