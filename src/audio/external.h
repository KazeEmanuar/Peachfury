#ifndef AUDIO_EXTERNAL_H
#define AUDIO_EXTERNAL_H

#include <PR/ultratypes.h>

#include "types.h"

// Sequence arguments, passed to play_sequence. seqId may be bit-OR'ed with
// SEQ_VARIATION; this will load the same sequence, but set a variation
// bit which may be read by the sequence script.
#define SEQUENCE_ARGS(priority, seqId) ((priority << 8) | seqId)

#define SOUND_MODE_STEREO           0
#define SOUND_MODE_MONO             3
#define SOUND_MODE_HEADSET          1

#define SEQ_PLAYER_LEVEL            0
#define SEQ_PLAYER_ENV              1
#define SEQ_PLAYER_SFX              2

extern s32 gAudioErrorFlags;
extern f32 gDefaultSoundArgs[3];

extern u8 gAudioSPTaskYieldBuffer[]; // ucode yield data ptr; only used in JP

struct SPTask *create_next_audio_frame_task(void);
void play_sound(s32 soundBits, f32 *pos);
void audio_signal_game_loop_tick(void);
void sequence_player_fade_out(s32 player, s32 fadeTimer);
void fade_volume_scale(s32 player, s32 targetScale, s32 fadeTimer);
void func_8031FFB4(s32 player, s32 fadeTimer, s32 arg2);
void sequence_player_unlower(s32 player, s32 fadeTimer);
void set_sound_disabled(s32 disabled);
void sound_init(void);
void killSound(u32 soundBits, f32 *vec);
void func_803206F8(f32 *arg0);
void muteCommonBanks(void);
void sound_banks_disable(s32 bankMask);
void sound_banks_enable(s32 bankMask);
void func_80320A4C(s32 bankIndex, s32 arg1);
void play_dialog_sound(s32 dialogID);
void play_music(s32 player, s32 seqArgs, s32 fadeTimer);
void stop_background_music(s32 seqId);
void fadeout_background_music(s32 arg0, s32 fadeOut);
void drop_queued_background_music(void);
s32 get_current_background_music(void);
void play_secondary_music(s32 seqId, s32 bgMusicVolume, s32 volume, s32 fadeTimer);
void func_80321080(s32 fadeTimer);
void func_803210D4(s32 fadeOutTime);
void play_course_clear(void);
void play_peachs_jingle(void);
void play_puzzle_jingle(void);
void play_star_fanfare(void);
void play_power_star_jingle(s32 arg0);
void play_race_fanfare(void);
void play_toads_jingle(void);
void sound_reset(s32 presetId);
void audio_set_sound_mode(s32 arg0);

void audio_init(void); // in load.c

#ifdef VERSION_EU
struct SPTask *unused_80321460(void);
#endif

#endif // AUDIO_EXTERNAL_H
