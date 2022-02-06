#include <ultra64.h>
#include <stdio.h>

#include "sm64.h"
#include "audio/external.h"
#include "game_init.h"
#include "memory.h"
#include "sound_init.h"
#include "profiler.h"
#include "buffers/buffers.h"
#include "segments.h"
#include "main.h"
#include "thread6.h"
#include "mem_error_screen.h"
//#include "OPT_FOR_SIZE.h"

OSThread gGraphicsLoopThread;
struct VblankHandler *gVblankHandler4 = NULL;

// Message IDs
#define MESG_SP_COMPLETE 100
#define MESG_DP_COMPLETE 101
#define MESG_VI_VBLANK 102
#define MESG_START_GFX_SPTASK 103
#define MESG_NMI_REQUEST 104

OSThread gIdleThread;
OSThread gMainThread;
OSThread gGameLoopThread;
OSThread gSoundThread;
#ifdef VERSION_SH
OSThread gRumblePakThread;

s32 gRumblePakPfs; // Actually an OSPfs but we don't have that header yet
#endif

OSIoMesg gDmaIoMesg;
OSMesg D_80339BEC;
OSMesgQueue gDmaMesgQueue;
OSMesgQueue gSIEventMesgQueue;
OSMesgQueue gPIMesgQueue;
OSMesgQueue gIntrMesgQueue;
OSMesgQueue gSPTaskMesgQueue;
#ifdef VERSION_SH
OSMesgQueue gRumblePakSchedulerMesgQueue;
OSMesgQueue gRumbleThreadVIMesgQueue;
#endif
OSMesg gDmaMesgBuf[1];
OSMesg gPIMesgBuf[32];
OSMesg gSIEventMesgBuf[1];
OSMesg gIntrMesgBuf[16];
OSMesg gUnknownMesgBuf[16];
#ifdef VERSION_SH
OSMesg gRumblePakSchedulerMesgBuf[1];
OSMesg gRumbleThreadVIMesgBuf[1];

struct RumbleData gRumbleDataQueue[3];
struct StructSH8031D9B0 gCurrRumbleSettings;
#endif

struct VblankHandler *gVblankHandler1 = NULL;
struct VblankHandler *gVblankHandler2 = NULL;
struct SPTask *gActiveSPTask = NULL;
struct SPTask *sCurrentAudioSPTask = NULL;
struct SPTask *sCurrentDisplaySPTask = NULL;
struct SPTask *sNextAudioSPTask = NULL;
struct SPTask *sNextDisplaySPTask = NULL;
u32 sNumVblanks = 0;
s8 gResetTimer = 0;
s8 D_8032C648 = 0;

s8 gShowProfiler = 0; // debug
s8 gShowFPS = 0;
u8 finalBuild = 0;

void setup_mesg_queues(void) {
    osCreateMesgQueue(&gDmaMesgQueue, gDmaMesgBuf, ARRAY_COUNT(gDmaMesgBuf));
    osCreateMesgQueue(&gSIEventMesgQueue, gSIEventMesgBuf, ARRAY_COUNT(gSIEventMesgBuf));
    osSetEventMesg(OS_EVENT_SI, &gSIEventMesgQueue, NULL);

    osCreateMesgQueue(&gSPTaskMesgQueue, gUnknownMesgBuf, ARRAY_COUNT(gUnknownMesgBuf));
    osCreateMesgQueue(&gIntrMesgQueue, gIntrMesgBuf, ARRAY_COUNT(gIntrMesgBuf));
    osViSetEvent(&gIntrMesgQueue, (OSMesg) MESG_VI_VBLANK, 1);

    osSetEventMesg(OS_EVENT_SP, &gIntrMesgQueue, (OSMesg) MESG_SP_COMPLETE);
    osSetEventMesg(OS_EVENT_DP, &gIntrMesgQueue, (OSMesg) MESG_DP_COMPLETE);
    osSetEventMesg(OS_EVENT_PRENMI, &gIntrMesgQueue, (OSMesg) MESG_NMI_REQUEST);
}

void alloc_pool(void) {
    void *start = (void *) SEG_POOL_START;
    void *end = (void *) RAM_END;

    // Detect memory size
    if (does_pool_end_lie_out_of_bounds(end))
        end = (void *) RAM_END_4MB;

    main_pool_init(start, end);
    gEffectsMemoryPool = mem_pool_init(0x4000, MEMORY_POOL_LEFT);
}

void create_thread(OSThread *thread, OSId id, void (*entry)(void *), void *arg, void *sp, OSPri pri) {
    thread->next = NULL;
    thread->queue = NULL;
    osCreateThread(thread, id, entry, arg, sp, pri);
}

#ifdef VERSION_SH
extern void func_sh_802F69CC(void);
#endif

void handle_nmi_request(void) {
    gResetTimer = 1;
    D_8032C648 = 0;
    muteCommonBanks();
    sound_banks_disable(0x037A);
    fadeout_music(90);
#ifdef VERSION_SH
    func_sh_802F69CC();
#endif
}

void receive_new_tasks(void) {
    struct SPTask *spTask;

    while (osRecvMesg(&gSPTaskMesgQueue, (OSMesg *) &spTask, OS_MESG_NOBLOCK) != -1) {
        spTask->state = SPTASK_STATE_NOT_STARTED;
        switch (spTask->task.t.type) {
            case 2:
                sNextAudioSPTask = spTask;
                break;
            case 1:
                sNextDisplaySPTask = spTask;
                break;
        }
    }

    if (sCurrentAudioSPTask == NULL && sNextAudioSPTask != NULL) {
        sCurrentAudioSPTask = sNextAudioSPTask;
        sNextAudioSPTask = NULL;
    }

    if (sCurrentDisplaySPTask == NULL && sNextDisplaySPTask != NULL) {
        sCurrentDisplaySPTask = sNextDisplaySPTask;
        sNextDisplaySPTask = NULL;
    }
}

void start_sptask(s32 taskType) {
    UNUSED s32 pad; // needed to pad the stack

    if (taskType == M_AUDTASK) {
        gActiveSPTask = sCurrentAudioSPTask;
    } else {
        gActiveSPTask = sCurrentDisplaySPTask;
    }

    osSpTaskLoad(&gActiveSPTask->task);
    osSpTaskStartGo(&gActiveSPTask->task);
    gActiveSPTask->state = SPTASK_STATE_RUNNING;
}

void interrupt_gfx_sptask(void) {
    if (gActiveSPTask->task.t.type == M_GFXTASK) {
        gActiveSPTask->state = SPTASK_STATE_INTERRUPTED;
        osSpTaskYield();
    }
}

void start_gfx_sptask(void) {
    if (gActiveSPTask == NULL && sCurrentDisplaySPTask != NULL
        && sCurrentDisplaySPTask->state == SPTASK_STATE_NOT_STARTED) {
        start_sptask(M_GFXTASK);
    }
}

void handle_vblank(void) {
    sNumVblanks++;
    if (gResetTimer > 0) {
        gResetTimer++;
    }

    receive_new_tasks();

    // First try to kick off an audio task. If the gfx task is currently
    // running, we need to asynchronously interrupt it -- handle_sp_complete
    // will pick up on what we're doing and start the audio task for us.
    // If there is already an audio task running, there is nothing to do.
    // If there is no audio task available, try a gfx task instead.
    if (sCurrentAudioSPTask != NULL) {
        if (gActiveSPTask != NULL) {
            interrupt_gfx_sptask();
        } else {
            start_sptask(M_AUDTASK);
        }
    } else {
        if (gActiveSPTask == NULL && sCurrentDisplaySPTask != NULL
            && sCurrentDisplaySPTask->state != SPTASK_STATE_FINISHED) {
            start_sptask(M_GFXTASK);
        }
    }
#ifdef VERSION_SH
    rumble_thread_update_vi();
#endif

    // Notify the game loop about the vblank.
    if (gVblankHandler1 != NULL) {
        osSendMesg(gVblankHandler1->queue, gVblankHandler1->msg, OS_MESG_NOBLOCK);
    }
    if (gVblankHandler2 != NULL) {
        osSendMesg(gVblankHandler2->queue, gVblankHandler2->msg, OS_MESG_NOBLOCK);
    }
    if (gVblankHandler4 != NULL) {
        osSendMesg(gVblankHandler4->queue, gVblankHandler4->msg, OS_MESG_NOBLOCK);
    }
}

void handle_sp_complete(void) {
    struct SPTask *curSPTask = gActiveSPTask;

    gActiveSPTask = NULL;

    if (curSPTask->state == SPTASK_STATE_INTERRUPTED) {
        // handle_vblank tried to start an audio task while there was already a
        // gfx task running, so it had to interrupt the gfx task. That interruption
        // just finished.
        if (osSpTaskYielded(&curSPTask->task) == 0) {
            // The gfx task completed before we had time to interrupt it.
            // Mark it finished, just like below.
            curSPTask->state = SPTASK_STATE_FINISHED;
        }

        // Start the audio task, as expected by handle_vblank.
        start_sptask(M_AUDTASK);
    } else {
        curSPTask->state = SPTASK_STATE_FINISHED;
        if (curSPTask->task.t.type == M_AUDTASK) {
            // After audio tasks come gfx tasks.
            if (sCurrentDisplaySPTask != NULL
                && sCurrentDisplaySPTask->state != SPTASK_STATE_FINISHED) {
                start_sptask(M_GFXTASK);
            }
            sCurrentAudioSPTask = NULL;
            if (curSPTask->msgqueue != NULL) {
                osSendMesg(curSPTask->msgqueue, curSPTask->msg, OS_MESG_NOBLOCK);
            }
        } else {
        }
    }
}

void handle_dp_complete(void) {
    // Gfx SP task is completely done.
    if (sCurrentDisplaySPTask->msgqueue != NULL) {
        osSendMesg(sCurrentDisplaySPTask->msgqueue, sCurrentDisplaySPTask->msg, OS_MESG_NOBLOCK);
    }
    sCurrentDisplaySPTask->state = SPTASK_STATE_FINISHED_DP;
    sCurrentDisplaySPTask = NULL;
}
#include "src/game/object_list_processor.h"
extern u8 isEmu;
extern u8 isEmulator();





#define REG_EDID 0x0005
#define REG_VER 11
typedef struct
{
  uint32_t          dram_addr;                /* 0x0000 */
  uint32_t          cart_addr;                /* 0x0004 */
  uint32_t          rd_len;                   /* 0x0008 */
  uint32_t          wr_len;                   /* 0x000C */
  uint32_t          status;                   /* 0x0010 */
  uint32_t          dom1_lat;                 /* 0x0014 */
  uint32_t          dom1_pwd;                 /* 0x0018 */
  uint32_t          dom1_pgs;                 /* 0x001C */
  uint32_t          dom1_rls;                 /* 0x0020 */
  uint32_t          dom2_lat;                 /* 0x0024 */
  uint32_t          dom2_pwd;                 /* 0x0028 */
  uint32_t          dom2_pgs;                 /* 0x002C */
  uint32_t          dom2_rls;                 /* 0x0030 */
                                              /* 0x0034 */
} pi_regs_t;
#define REG_BASE 0xBF800000
#define REG_BASE2 0xA8040000
#define REGS_PTR ((volatile uint32_t *) REG_BASE)
#define REGS_PTR2 ((volatile uint32_t *) REG_BASE2)
#define pi_regs                   (*(volatile pi_regs_t*)0xA4600000)
static inline void __pi_wait(void)
{
  while (pi_regs.status & (PI_STATUS_DMA_BUSY | PI_STATUS_IO_BUSY));
}
static inline uint32_t __pi_read_raw(uint32_t dev_addr)
{
  __pi_wait();
  return *(volatile uint32_t *)dev_addr;
}
static inline uint32_t reg_rd(int reg) {
    return __pi_read_raw((uint32_t) &REGS_PTR[reg]);
}
static inline uint32_t reg_rd2(int reg) {
    return __pi_read_raw((uint32_t) &REGS_PTR2[reg]);
}
static inline uint32_t reg_rd3() {
  __pi_wait();
  return *(volatile uint16_t *)0x18001006;
}
u8 isEverdrive;
#define REG_KEY2           8
#define REG_KEY           0x2001
u8 isED() {
        return 1;
    if (reg_rd2(REG_KEY2) == 0) {
        return 1;
    }
    if ((reg_rd(REG_KEY)) == 0) {
        return 1;
    }
    if ((reg_rd3()) == 0) {
        return 1;
    }
    return 0;
}

void thread3_main(UNUSED void *arg) {
    setup_mesg_queues();
    alloc_pool();
    load_engine_code_segment();
    bzero(gObjectPool, sizeof(gObjectPool[OBJECT_POOL_CAPACITY]));
    create_thread(&gSoundThread, 4, thread4_sound, NULL, gThread4Stack + 0x2000, 20);
    osStartThread(&gSoundThread);
    isEverdrive = isED();
    if (!gNotEnoughMemory)
        create_thread(&gGameLoopThread, 5, thread5_game_loop, NULL, gThread5Stack + 0x2000, 10);
    else
        create_thread(&gGameLoopThread, 5, thread5_mem_error_message_loop, NULL, gThread5Stack + 0x2000,
                      10);
    osStartThread(&gGameLoopThread);

    create_thread(&gGraphicsLoopThread, 9, thread9_graphics_loop, NULL, gThread9Stack + 0x2000, 9);

    while (1) {
        OSMesg msg;

        osRecvMesg(&gIntrMesgQueue, &msg, OS_MESG_BLOCK);
        switch ((uintptr_t) msg) {
            case MESG_VI_VBLANK:
                handle_vblank();
                break;
            case MESG_SP_COMPLETE:
                handle_sp_complete();
                break;
            case MESG_DP_COMPLETE:
                handle_dp_complete();
                break;
            case MESG_START_GFX_SPTASK:
                start_gfx_sptask();
                break;
            case MESG_NMI_REQUEST:
                handle_nmi_request();
                break;
        }
    }
}

void set_vblank_handler(s32 index, struct VblankHandler *handler, OSMesgQueue *queue, OSMesg *msg) {
    handler->queue = queue;
    handler->msg = msg;

    switch (index) {
        case 1:
            gVblankHandler1 = handler;
            break;
        case 2:
            gVblankHandler2 = handler;
            break;
        case 4:
            gVblankHandler4 = handler;
            break;
    }
}

void dispatch_audio_sptask(struct SPTask *spTask) {
    if (spTask != NULL) {
        osWritebackDCacheAll();
        osSendMesg(&gSPTaskMesgQueue, spTask, OS_MESG_NOBLOCK);
    }
}

void send_display_list(struct SPTask *spTask) {
    if (spTask != NULL) {
        osWritebackDCacheAll();
        spTask->state = SPTASK_STATE_NOT_STARTED;
        if (sCurrentDisplaySPTask == NULL) {
            sCurrentDisplaySPTask = spTask;
            sNextDisplaySPTask = NULL;
            osSendMesg(&gIntrMesgQueue, (OSMesg) MESG_START_GFX_SPTASK, OS_MESG_NOBLOCK);
        } else {
            sNextDisplaySPTask = spTask;
        }
    }
}

extern crash_screen_init();

/**
 * Initialize hardware, start main thread, then idle.
 */
void thread1_idle(UNUSED void *arg) {
    osCreateViManager(OS_PRIORITY_VIMGR);
    switch (osTvType) {
        case OS_TV_NTSC:
            // NTSC
            osViSetMode(&osViModeTable[OS_VI_NTSC_LAN1]);
            break;
        case OS_TV_MPAL:
            // MPAL
            osViSetMode(&osViModeTable[OS_VI_MPAL_LAN1]);
            break;
        case OS_TV_PAL:
            // PAL
            osViSetMode(&osViModeTable[OS_VI_PAL_LAN1]);
            break;
    }
    osViBlack(TRUE);
    osViSetSpecialFeatures(OS_VI_DITHER_FILTER_ON);
    osViSetSpecialFeatures(OS_VI_GAMMA_OFF);
    osCreatePiManager(OS_PRIORITY_PIMGR, &gPIMesgQueue, gPIMesgBuf, ARRAY_COUNT(gPIMesgBuf));
        create_thread(&gMainThread, 3, thread3_main, NULL, gThread3Stack + 0x2000, 100);
        osStartThread(&gMainThread);
    osSetThreadPri(NULL, 0);
    crash_screen_init();

    // halt
    while (1) {
        ;
    }
}

void main_func(void) {
    UNUSED u8 pad[64]; // needed to pad the stack

    __osInitialize_common();
    create_thread(&gIdleThread, 1, thread1_idle, NULL, gIdleThreadStack + 0x800, 100);
    osStartThread(&gIdleThread);
}
