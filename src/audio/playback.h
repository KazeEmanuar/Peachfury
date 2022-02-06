#ifndef AUDIO_PLAYBACK_H
#define AUDIO_PLAYBACK_H

#include <PR/ultratypes.h>

#include "internal.h"

// Mask bits denoting where to allocate notes from, according to a channel's
// noteAllocPolicy. Despite being checked as bitmask bits, the bits are not
// orthogonal; rather, the smallest bit wins, except for NOTE_ALLOC_LAYER,
// which *is* orthogonal to the other. SEQ implicitly includes CHANNEL.
// If none of the CHANNEL/SEQ/GLOBAL_FREELIST bits are set, all three locations
// are tried.
#define NOTE_ALLOC_LAYER 1
#define NOTE_ALLOC_CHANNEL 2
#define NOTE_ALLOC_SEQ 4
#define NOTE_ALLOC_GLOBAL_FREELIST 8

#define seq_channel_layer_note_decay(seqLayer) (seq_channel_layer_decay_release_internal(seqLayer, ADSR_STATE_DECAY));
#define seq_channel_layer_note_release(seqLayer) (seq_channel_layer_decay_release_internal(seqLayer, ADSR_STATE_RELEASE));
void init_synthetic_wave(struct Note *note, struct SequenceChannelLayer *seqLayer);
void init_note_lists(struct NotePool *pool);
void note_pool_clear(struct NotePool *pool);
void note_pool_fill(struct NotePool *pool, s32 count);
void audio_list_push_front(struct AudioListItem *list, struct AudioListItem *item);
void audio_list_remove(struct AudioListItem *item);
struct Note *alloc_note(struct SequenceChannelLayer *seqLayer);
void note_init_all(void);

#ifdef VERSION_EU
struct AudioBankSound *instrument_get_audio_bank_sound(struct Instrument *instrument, s32 semitone);
struct Instrument *get_instrument_inner(s32 bankId, s32 instId);
struct Drum *get_drum(s32 bankId, s32 drumId);
void note_set_vel_pan_reverb(struct Note *note, f32 velocity, u8 pan, u8 reverb);
void note_enable(struct Note *note);
void note_disable(struct Note *note);
#endif


#endif // AUDIO_PLAYBACK_H
