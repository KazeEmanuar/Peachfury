#include <ultra64.h>

#include "area.h"
#include "audio/external.h"
#include "engine/graph_node.h"
#include "engine/math_util.h"
#include "level_table.h"
#include "level_update.h"
#include "main.h"
#include "paintings.h"
#include "print.h"
#include "profiler.h"
#include "save_file.h"
#include "seq_ids.h"
#include "sm64.h"
#include "sound_init.h"
#include "thread6.h"
#include "OPT_FOR_SIZE.h"

#define MUSIC_NONE 0xFFFF

static OSMesgQueue sSoundMesgQueue;
static OSMesg sSoundMesgBuf[1];
static struct VblankHandler sSoundVblankHandler;

u8 volGoalMaybe = 0;
static u8 D_8032C6C4 = 0;
u16 sCurrentMusic = MUSIC_NONE;
static u16 sCurrentCapMusic = MUSIC_NONE;
static u8 sPlayingInfiniteStairs = FALSE;
static s16 sSoundMenuModeToSoundMode[] = { SOUND_MODE_STEREO, SOUND_MODE_MONO, SOUND_MODE_HEADSET };
// Only the 20th array element is used.
static s8 paintingEjectSoundPlayed = FALSE;

void lower_background_noise(s32 a) // Soften volume
{
    switch (a) {
        case 1:
            set_sound_disabled(TRUE);
            break;
        case 2:
            func_8031FFB4(SEQ_PLAYER_LEVEL, 60, 40); // soften music
            break;
    }
    volGoalMaybe |= a;
}

void raise_background_noise(s32 a) // harden volume
{
    switch (a) {
        case 1:
            set_sound_disabled(FALSE);
            break;
        case 2:
            sequence_player_unlower(SEQ_PLAYER_LEVEL, 60);
            break;
    }
    volGoalMaybe &= ~a;
}

void disable_background_sound(void) {
    if (!D_8032C6C4) {
        D_8032C6C4 = 1;
        sound_banks_disable(0x037A);
    }
}

void enable_background_sound(void) {
    if (D_8032C6C4) {
        D_8032C6C4 = 0;
        sound_banks_enable(0x037A);
    }
}


void set_background_music(u16 a, u16 seqArgs) {
    if (gResetTimer == 0 && seqArgs != sCurrentMusic) {
            sound_reset(a);
            play_music(SEQ_PLAYER_LEVEL, seqArgs, 0);
            sCurrentMusic = seqArgs;
    }
}

void fadeout_music(s16 fadeOutTime) {
    func_803210D4(fadeOutTime);
    sCurrentMusic = MUSIC_NONE;
    sCurrentCapMusic = MUSIC_NONE;
}

void fadeout_level_music(s16 fadeTimer) {
    sequence_player_fade_out(0, fadeTimer);
    sCurrentMusic = MUSIC_NONE;
    sCurrentCapMusic = MUSIC_NONE;
}

void play_cap_music(u16 seqArgs) {
    play_music(SEQ_PLAYER_LEVEL, seqArgs, 0);
    if (sCurrentCapMusic != MUSIC_NONE && sCurrentCapMusic != seqArgs) {
        stop_background_music(sCurrentCapMusic);
    }
    sCurrentCapMusic = seqArgs;
}

extern void func_8031D6E4(s32 player, s32 fadeTimer, u8 fadePercentage);
extern void sequence_player_fade_out_internal(s32 player, s32 fadeOutTime);
extern u8 func_803200E4(u16 fadeTimer);/*
void play_cap_music(u16 seqArgs) {
    play_music(SEQ_PLAYER_ENV, seqArgs, 0);
    func_8031D6E4(SEQ_PLAYER_LEVEL, 120, 25);
    //             disable_background_sound();
    sequence_player_fade_out_internal(0, 120);
    sCurrentCapMusic = seqArgs;
}*/ 
 void fadeout_cap_music(void) {
     if (sCurrentCapMusic != MUSIC_NONE) {
         fadeout_background_music(sCurrentCapMusic, 600);
     }
 }/*
void fadeout_cap_music(void) {
    if (sCurrentCapMusic != MUSIC_NONE) {
        sequence_player_fade_out_internal(1, 120);
        func_803200E4(50);
        sequence_player_unlower(SEQ_PLAYER_LEVEL, 60);
    }
}*/

void stop_cap_music(void) {
    if (sCurrentCapMusic != MUSIC_NONE) {
        stop_background_music(sCurrentCapMusic);
        sCurrentCapMusic = MUSIC_NONE;
    }
}
/*
void stop_cap_music(void) {
    if (sCurrentCapMusic != MUSIC_NONE) {
        // enable_background_sound();
        play_sequence(1, 0, 1);
        sCurrentCapMusic = MUSIC_NONE;
    }
}*/
extern u32 thread9scriptDuration;
extern u32 thread9scriptMinus;
extern u8 thread9LogStarted;
extern u32 thread4scriptDuration;
extern u32 thread4scriptMinus;
extern u8 thread4LogStarted;
/**
 * Sound processing thread. Runs at 60 FPS.
 */
u32 sThreadTime[2] = {0,0};
u8 soundthreadNum = 0;
void thread4_sound(UNUSED void *arg) {
    OSTime prevtime = 0;
    audio_init();
    sound_init();


    osCreateMesgQueue(&sSoundMesgQueue, sSoundMesgBuf, ARRAY_COUNT(sSoundMesgBuf));
    set_vblank_handler(1, &sSoundVblankHandler, &sSoundMesgQueue, (OSMesg) 512);

    while (TRUE) {
        OSMesg msg;

        osRecvMesg(&sSoundMesgQueue, &msg, OS_MESG_BLOCK);
        prevtime = osGetTime();
        thread4LogStarted = 1;
        thread4scriptMinus = 0;

        if (gResetTimer < 25) {
            struct SPTask *spTask;

            spTask = create_next_audio_frame_task();
            if (spTask != NULL) {
                dispatch_audio_sptask(spTask);
            }

        }
        
        sThreadTime[soundthreadNum] = osGetTime() - prevtime - thread4scriptMinus;
        soundthreadNum = soundthreadNum^1;
        thread4LogStarted = 0;
    }
}
