#include <ultra64.h>

#include "synthesis.h"
#include "heap.h"
#include "data.h"
#include "load.h"
#include "seqplayer.h"
#include "external.h"
#include <PR/ultratypes.h>
#include "effects.h"
#include "external.h"
#include "playback.h"
#include "game/OPT_FOR_SIZE.h"

#define PORTAMENTO_IS_SPECIAL(x) ((x).mode & 0x80)
#define PORTAMENTO_MODE(x) ((x).mode & ~0x80)
#define PORTAMENTO_MODE_1 1
#define PORTAMENTO_MODE_2 2
#define PORTAMENTO_MODE_3 3
#define PORTAMENTO_MODE_4 4
#define PORTAMENTO_MODE_5 5

#pragma __attribute__((optimize("align-functions=32")))


#define DMEM_ADDR_TEMP 0x0
#define DMEM_ADDR_UNCOMPRESSED_NOTE 0x180
#define DMEM_ADDR_ADPCM_RESAMPLED 0x20
#define DMEM_ADDR_ADPCM_RESAMPLED2 0x160
#define DMEM_ADDR_NOTE_PAN_TEMP 0x200
#define DMEM_ADDR_STEREO_STRONG_TEMP_DRY 0x200
#define DMEM_ADDR_STEREO_STRONG_TEMP_WET 0x340
#define DMEM_ADDR_COMPRESSED_ADPCM_DATA 0x3f0
#define DMEM_ADDR_LEFT_CH 0x4c0
#define DMEM_ADDR_RIGHT_CH 0x600
#define DMEM_ADDR_WET_LEFT_CH 0x740
#define DMEM_ADDR_WET_RIGHT_CH 0x880

#define aSetLoadBufferPair(pkt, c, off)                                                                \
    aSetBuffer(pkt, 0, c + DMEM_ADDR_WET_LEFT_CH, 0, DEFAULT_LEN_1CH - c);                             \
    aLoadBuffer(pkt, VIRTUAL_TO_PHYSICAL2(gSynthesisReverb.ringBuffer.left + (off)));                  \
    aSetBuffer(pkt, 0, c + DMEM_ADDR_WET_RIGHT_CH, 0, DEFAULT_LEN_1CH - c);                            \
    aLoadBuffer(pkt, VIRTUAL_TO_PHYSICAL2(gSynthesisReverb.ringBuffer.right + (off)))

#define aSetSaveBufferPair(pkt, c, d, off)                                                             \
    aSetBuffer(pkt, 0, 0, c + DMEM_ADDR_WET_LEFT_CH, d);                                               \
    aSaveBuffer(pkt, VIRTUAL_TO_PHYSICAL2(gSynthesisReverb.ringBuffer.left +  (off)));                 \
    aSetBuffer(pkt, 0, 0, c + DMEM_ADDR_WET_RIGHT_CH, d);                                              \
    aSaveBuffer(pkt, VIRTUAL_TO_PHYSICAL2(gSynthesisReverb.ringBuffer.right + (off)));

#define ALIGN(val, amnt) (((val) + (1 << amnt) - 1) & ~((1 << amnt) - 1))

struct VolumeChange {
    u16 sourceLeft;
    u16 sourceRight;
    u16 targetLeft;
    u16 targetRight;
};

s32 note_init_for_layer(struct Note *note, struct SequenceChannelLayer *seqLayer);

void note_init(struct Note *note) {
    if (note->parentLayer->adsr.releaseRate == 0) {
        adsr_init(&note->adsr, note->parentLayer->seqChannel->adsr.envelope, &note->adsrVolScale);
    } else {
        adsr_init(&note->adsr, note->parentLayer->adsr.envelope, &note->adsrVolScale);
    }
    note->adsr.state = ADSR_STATE_INITIAL;
#ifdef VERSION_EU
    note->noteSubEu = gDefaultNoteSub;
#else
    note_init_volume(note);
    note_enable(note);
#endif
}

__attribute__((always_inline))  inline void process_notes(void) {
    f32 scale;
    f32 frequency;
    u8 reverb;
    f32 velocity;
    f32 pan;
    f32 cap;
    struct Note *note;
    struct NoteAttributes *attributes;
    struct AudioListItem *it;
    s32 i;

    // Macro versions of audio_list_push_front and audio_list_remove
    // (PREPEND does not actually need to be a macro, but it seems likely.)
#define PREPEND(item, head_arg)                                                                        \
    ((it = (item), it->prev != NULL)                                                                   \
         ? it                                                                                          \
         : (it->prev = (head_arg), it->next = (head_arg)->next, (head_arg)->next->prev = it,           \
            (head_arg)->next = it, (head_arg)->u.count++, it->pool = (head_arg)->pool, it))
#define POP(item)                                                                                      \
    ((it = (item), it->prev == NULL)                                                                   \
         ? it                                                                                          \
         : (it->prev->next = it->next, it->next->prev = it->prev, it->prev = NULL, it))

    for (i = 0; i < gMaxSimultaneousNotes; i++) {
        note = &gNotes[i];
#ifdef VERSION_EU
        playbackState = (struct NotePlaybackState *) &note->priority;
        if (note->parentLayer != NO_LAYER) {
#ifndef NO_SEGMENTED_MEMORY
            if ((uintptr_t) playbackState->parentLayer < 0x7fffffffU) {
                continue;
            }
#endif
            if (!playbackState->parentLayer->enabled && playbackState->priority >= NOTE_PRIORITY_MIN) {
                goto c;
            } else if (playbackState->parentLayer->seqChannel->seqPlayer == NULL) {
                sequence_channel_disable(playbackState->parentLayer->seqChannel);
                playbackState->priority = NOTE_PRIORITY_STOPPING;
                continue;
            } else if (playbackState->parentLayer->seqChannel->seqPlayer->muted) {
                if ((playbackState->parentLayer->seqChannel->muteBehavior
                    & (MUTE_BEHAVIOR_STOP_SCRIPT | MUTE_BEHAVIOR_STOP_NOTES))) {
                    goto c;
                }
            }
            goto d;
            if (1) {
                c:
                seq_channel_layer_note_release(playbackState->parentLayer);
                audio_list_remove(&note->listItem);
                audio_list_push_front(&note->listItem.pool->decaying, &note->listItem);
                playbackState->priority = NOTE_PRIORITY_STOPPING;
            }
        } else if (playbackState->priority >= NOTE_PRIORITY_MIN) {
            continue;
        }
        d:
        if (playbackState->priority != NOTE_PRIORITY_DISABLED) {
            noteSubEu = &note->noteSubEu;
            if (playbackState->priority == NOTE_PRIORITY_STOPPING || noteSubEu->finished) {
                if (playbackState->adsr.state == ADSR_STATE_DISABLED || noteSubEu->finished) {
                    if (playbackState->wantedParentLayer != NO_LAYER) {
                        note_disable(note);
                        if (playbackState->wantedParentLayer->seqChannel != NULL) {
                            note_init_for_layer(note, playbackState->wantedParentLayer);
                            note_vibrato_init(note);
                            audio_list_remove(&note->listItem);
                            audio_list_push_back(&note->listItem.pool->active, &note->listItem);
                            playbackState->wantedParentLayer = NO_LAYER;
                            // don't skip
                        } else {
                            note_disable(note);
                            audio_list_remove(&note->listItem);
                            audio_list_push_back(&note->listItem.pool->disabled, &note->listItem);
                            playbackState->wantedParentLayer = NO_LAYER;
                            goto skip;
                        }
                    } else {
                        note_disable(note);
                        audio_list_remove(&note->listItem);
                        audio_list_push_back(&note->listItem.pool->disabled, &note->listItem);
                        goto skip;
                    }
                }
                if (1) {
                }
            } else if (playbackState->adsr.state == ADSR_STATE_DISABLED) {
                note_disable(note);
                audio_list_remove(&note->listItem);
                audio_list_push_back(&note->listItem.pool->disabled, &note->listItem);
                goto skip;
            }

            scale = adsr_update(&playbackState->adsr);
            note_vibrato_update(note);
            attributes = &playbackState->attributes;
            if (playbackState->priority == NOTE_PRIORITY_STOPPING) {
                frequency = attributes->freqScale;
                velocity = attributes->velocity;
                pan = attributes->pan;
                reverb = attributes->reverb;
                if (1) {
                }
                bookOffset = noteSubEu->bookOffset;
            } else {
                frequency = playbackState->parentLayer->noteFreqScale;
                velocity = playbackState->parentLayer->noteVelocity;
                pan = playbackState->parentLayer->notePan;
                reverb = playbackState->parentLayer->seqChannel->reverb;
                bookOffset = playbackState->parentLayer->seqChannel->bookOffset & 0x7;
            }

            frequency *= playbackState->vibratoFreqScale * playbackState->portamentoFreqScale;
            frequency *= gAudioBufferParameters.resampleRate;
            velocity = velocity * scale * scale;
            note_set_resampling_rate(note, frequency);
            note_set_vel_pan_reverb(note, velocity, pan, reverb);
            noteSubEu->bookOffset = bookOffset;
            skip:;
        }
#else
        if (note->priority != NOTE_PRIORITY_DISABLED) {
            if (note->priority == NOTE_PRIORITY_STOPPING || note->finished) {
                if (note->adsrVolScale == 0 || note->finished) {
                    if (note->wantedParentLayer != NO_LAYER) {
                        note_disable(note);
                        if (note->wantedParentLayer->seqChannel != NULL) {
                            if (note_init_for_layer(note, note->wantedParentLayer) ) {
                                note_disable(note);
                                POP(&note->listItem);
                                PREPEND(&note->listItem, &gNoteFreeLists.disabled);
                            } else {
                                note_vibrato_init(note);
                                audio_list_push_back(&note->listItem.pool->active,
                                                     POP(&note->listItem));
                                note->wantedParentLayer = NO_LAYER;
                            }
                        } else {
                            note_disable(note);
                            audio_list_push_back(&note->listItem.pool->disabled, POP(&note->listItem));
                            note->wantedParentLayer = NO_LAYER;
                            continue;
                        }
                    } else {
                        note_disable(note);
                        audio_list_push_back(&note->listItem.pool->disabled, POP(&note->listItem));
                        continue;
                    }
                }
            } else {
                if (note->adsr.state == ADSR_STATE_DISABLED) {
                    note_disable(note);
                    audio_list_push_back(&note->listItem.pool->disabled, POP(&note->listItem));
                    continue;
                }
            }

            adsr_update(&note->adsr);
            note_vibrato_update(note);
            attributes = &note->attributes;
            if (note->priority == NOTE_PRIORITY_STOPPING) {
                frequency = attributes->freqScale;
                velocity = attributes->velocity;
                pan = attributes->pan;
                reverb = attributes->reverb;
            } else {
                frequency = note->parentLayer->noteFreqScale;
                velocity = note->parentLayer->noteVelocity;
                pan = note->parentLayer->notePan;
                reverb = note->parentLayer->seqChannel->reverb;
            }

            scale = note->adsrVolScale;
            frequency *= note->vibratoFreqScale * note->portamentoFreqScale;
            cap = 3.99992f;
            if (gAiFrequency != 32006) {
                frequency *= US_FLOAT(32000.0) / (f32) gAiFrequency;
            }
            frequency = (frequency < cap ? frequency : cap);
            scale *= 4.3498e-5f; // ~1 / 23000
            velocity = velocity * scale * scale;
            note_set_frequency(note, frequency);
            note_set_vel_pan_reverb(note, velocity, pan, reverb);
            continue;
        }
#endif
    }
#undef PREPEND
#undef POP
}

void seq_channel_layer_decay_release_internal(struct SequenceChannelLayer *seqLayer, s32 target) {
    struct Note *note;
    struct NoteAttributes *attributes;

    if (seqLayer == NO_LAYER || seqLayer->note == NULL) {
        return;
    }

    note = seqLayer->note;
    attributes = &note->attributes;

#ifndef VERSION_EU
    if (seqLayer->seqChannel != NULL && seqLayer->seqChannel->noteAllocPolicy == 0) {
        seqLayer->note = NULL;
    }
#endif

    if (note->wantedParentLayer == seqLayer) {
        note->wantedParentLayer = NO_LAYER;
    }

    if (note->parentLayer != seqLayer) {
#ifdef VERSION_EU
        if (note->parentLayer == NO_LAYER && note->wantedParentLayer == NO_LAYER && note->prevParentLayer == seqLayer && target != ADSR_STATE_DECAY) {
            note->adsr.fadeOutVel = gAudioBufferParameters.updatesPerFrameInv;
            note->adsr.action |= ADSR_ACTION_RELEASE;
        }
#endif
        return;
    }

    seqLayer->status = SOUND_LOAD_STATUS_NOT_LOADED;
    if (note->adsr.state != ADSR_STATE_DECAY) {
        attributes->freqScale = seqLayer->noteFreqScale;
        attributes->velocity = seqLayer->noteVelocity;
        attributes->pan = seqLayer->notePan;
        if (seqLayer->seqChannel != NULL) {
            attributes->reverb = seqLayer->seqChannel->reverb;
        }
        note->priority = NOTE_PRIORITY_STOPPING;
        note->prevParentLayer = note->parentLayer;
        note->parentLayer = NO_LAYER;
        if (target == ADSR_STATE_RELEASE) {
#ifdef VERSION_EU
            note->adsr.fadeOutVel = gAudioBufferParameters.updatesPerFrameInv;
#else
            note->adsr.fadeOutVel = 0x8000 / gAudioUpdatesPerFrame;
#endif
            note->adsr.action |= ADSR_ACTION_RELEASE;
        } else {
            note->adsr.action |= ADSR_ACTION_DECAY;
#ifdef VERSION_EU
            if (seqLayer->adsr.releaseRate == 0) {
                note->adsr.fadeOutVel = seqLayer->seqChannel->adsr.releaseRate * gAudioBufferParameters.unkUpdatesPerFrameScaled;
            } else {
                note->adsr.fadeOutVel = seqLayer->adsr.releaseRate * gAudioBufferParameters.unkUpdatesPerFrameScaled;
            }
            note->adsr.sustain = (FLOAT_CAST(seqLayer->seqChannel->adsr.sustain) * note->adsr.current) / 256.0f;
#else
            if (seqLayer->adsr.releaseRate == 0) {
                note->adsr.fadeOutVel = seqLayer->seqChannel->adsr.releaseRate * 24;
            } else {
                note->adsr.fadeOutVel = seqLayer->adsr.releaseRate * 24;
            }
            note->adsr.sustain = (note->adsr.current * seqLayer->seqChannel->adsr.sustain) / 0x10000;
#endif
        }
    }

    if (target == ADSR_STATE_DECAY) {
        audio_list_remove(&note->listItem);
        audio_list_push_front(&note->listItem.pool->decaying, &note->listItem);
    }
}


#ifdef VERSION_EU
s32 build_synthetic_wave(struct Note *note, struct SequenceChannelLayer *seqLayer, s32 waveId) {
    f32 freqScale;
    f32 ratio;
    u8 sampleCountIndex;

    if (waveId < 128) {
        waveId = 128;
    }

    freqScale = seqLayer->freqScale;
    if (seqLayer->portamento.mode != 0 && 0.0f < seqLayer->portamento.extent) {
        freqScale *= (seqLayer->portamento.extent + 1.0f);
    }
    if (freqScale < 1.0f) {
        sampleCountIndex = 0;
        ratio = 1.0465f;
    } else if (freqScale < 2.0f) {
        sampleCountIndex = 1;
        ratio = 0.52325f;
    } else if (freqScale < 4.0f) {
        sampleCountIndex = 2;
        ratio = 0.26263f;
    } else {
        sampleCountIndex = 3;
        ratio = 0.13081f;
    }
    seqLayer->freqScale *= ratio;
    note->waveId = waveId;
    note->sampleCountIndex = sampleCountIndex;

    note->noteSubEu.sound.samples = &gWaveSamples[waveId - 128][sampleCountIndex * 64];

    return sampleCountIndex;
}
#else
void build_synthetic_wave(struct Note *note, struct SequenceChannelLayer *seqLayer) {
    s32 i;
    s32 j;
    s32 pos;
    s32 stepSize;
    s32 offset;
    u8 lim;
    u8 origSampleCount = note->sampleCount;

    if (seqLayer->freqScale < US_FLOAT(1.0)) {
        note->sampleCount = 64;
        seqLayer->freqScale *= US_FLOAT(1.0465);
        stepSize = 1;
    } else if (seqLayer->freqScale < US_FLOAT(2.0)) {
        note->sampleCount = 32;
        seqLayer->freqScale *= US_FLOAT(0.52325);
        stepSize = 2;
    } else if (seqLayer->freqScale < US_FLOAT(4.0)) {
        note->sampleCount = 16;
        seqLayer->freqScale *= US_FLOAT(0.26263);
        stepSize = 4;
    } else {
        note->sampleCount = 8;
        seqLayer->freqScale *= US_FLOAT(0.13081);
        stepSize = 8;
    }

    if (note->sampleCount == origSampleCount && seqLayer->seqChannel->instOrWave == note->instOrWave) {
        return;
    }

    // Load wave sample
    note->instOrWave = (u8) seqLayer->seqChannel->instOrWave;
    for (i = -1, pos = 0; pos < 0x40; pos += stepSize) {
        i++;
        note->synthesisBuffers->samples[i] = gWaveSamples[seqLayer->seqChannel->instOrWave - 0x80][pos];
    }

    // Repeat sample
    for (offset = note->sampleCount; offset < 0x40; offset += note->sampleCount) {
        lim = note->sampleCount;
        if (offset < 0 || offset > 0) {
            for (j = 0; j < lim; j++) {
                note->synthesisBuffers->samples[offset + j] = note->synthesisBuffers->samples[j];
            }
        } else {
            for (j = 0; j < lim; j++) {
                note->synthesisBuffers->samples[offset + j] = note->synthesisBuffers->samples[j];
            }
        }
    }

    osWritebackDCache(note->synthesisBuffers->samples, sizeof(note->synthesisBuffers->samples));
}
#endif

__attribute__((always_inline))  inline void init_synthetic_wave(struct Note *note, struct SequenceChannelLayer *seqLayer) {
#ifdef VERSION_EU
    s32 sampleCountIndex;
    s32 waveSampleCountIndex;
    s32 waveId = seqLayer->instOrWave;
    if (waveId == 0xff) {
        waveId = seqLayer->seqChannel->instOrWave;
    }
    sampleCountIndex = note->sampleCountIndex;
    waveSampleCountIndex = build_synthetic_wave(note, seqLayer, waveId);
    note->synthesisState.samplePosInt = note->synthesisState.samplePosInt * euUnknownData_8030194c[waveSampleCountIndex] / euUnknownData_8030194c[sampleCountIndex];
#else
    s32 sampleCount = note->sampleCount;
    build_synthetic_wave(note, seqLayer);
    if (sampleCount != 0) {
        note->samplePosInt *= note->sampleCount / sampleCount;
    } else {
        note->samplePosInt = 0;
    }
#endif
}

    #define PORTAMENTO_TGT_NOTE portamentoTargetNote
    #define DRUM_INDEX          cmdSemitone
    #define SEMITONE            cmdSemitone
    #define USED_SEMITONE       usedSemitone
    #define M64_READ_U8(state, dst) \
{                               \
    u8 * _ptr_pc;               \
    u8  _pc;                    \
    _ptr_pc = (*state).pc;      \
    ((*state).pc)++;            \
    _pc = *_ptr_pc;             \
    dst = _pc;                  \
}
#define M64_READ_S16(state, dst)    \
{                                   \
    s16 _ret;                       \
    _ret = *(*state).pc << 8;       \
    ((*state).pc)++;                \
    _ret = *(*state).pc | _ret;     \
    ((*state).pc)++;                \
    dst = _ret;                     \
}
#define M64_READ_COMPRESSED_U16(state, dst) \
    dst = m64_read_compressed_u16(state);
#define M64_READ_COMPRESSED_U16(state, dst) \
{                                           \
    u16 ret = *(state->pc++);               \
    if (ret & 0x80) {                       \
        ret = (ret << 8) & 0x7f00;          \
        ret = *(state->pc++) | ret;         \
    }                                       \
    dst = ret;                              \
}

void seq_channel_layer_process_script(struct SequenceChannelLayer *layer) {
    struct SequencePlayer *seqPlayer; // sp5C, t4
    struct SequenceChannel *seqChannel; // sp58, t5
    struct M64ScriptState *state; // v0
    struct Portamento *portamento; // v0
    struct AudioBankSound *sound; // v0
    struct Instrument *instrument; // v1
    struct Drum *drum;
    UNUSED s32 pad[1];
    u8 sameSound; // sp3F
    UNUSED u8 allocNewNote; // sp3D, t0
    u8 cmd; // a0 sp3E, EU s2
    UNUSED u8 loBits;
    u16 sp3A; // t2, a0, a1
    UNUSED s32 pad2[1];
    s32 vel; // sp30, t3
    UNUSED s32 pad3[1];
    f32 freqScale; // sp28, f0
    f32 sp24;
    u8 temp8;
    UNUSED u8 semitone; // v0
    s32 usedSemitone; // a1
    f32 temp_f12;
    f32 temp_f2;
    s32 temp_a0_5;
    UNUSED u8 drumIndex; // t0
    UNUSED s32 cmdBase; // t1
    u8 temp_a0_6;
    u8 portamentoTargetNote; // t7
    UNUSED s32 bankId; // a3
    u8 instId; // v0
    u8 cmdSemitone; // v1
    f32 tuning; // f0

    // inlined copt var that gets pulled out to the rest of the function
    unsigned char _Kqi6;

//! Copt: manually __attribute__((always_inline))  inline these functions in the scope of this routine
#ifdef __sgi
#pragma __attribute__((always_inline))  inline routine(m64_read_u8)
#pragma __attribute__((always_inline))  inline routine(m64_read_compressed_u16)
#pragma __attribute__((always_inline))  inline routine(m64_read_s16)
#pragma __attribute__((always_inline))  inline routine(get_instrument)
#endif

    sameSound = TRUE;
    if (layer->enabled == FALSE) {
        return;
    }

    if (layer->delay > 1) {
        layer->delay--;
        if (!layer->stopSomething && layer->delay <= layer->duration) {
            seq_channel_layer_note_decay(layer);
            layer->stopSomething = TRUE;
        }
        return;
    }

    if (!layer->continuousNotes) {
        seq_channel_layer_note_decay(layer);
    }

    if (PORTAMENTO_MODE(layer->portamento) == PORTAMENTO_MODE_1 ||
        PORTAMENTO_MODE(layer->portamento) == PORTAMENTO_MODE_2) {
        layer->portamento.mode = 0;
    }

    seqChannel = (*(layer)).seqChannel;
    seqPlayer = (*(seqChannel)).seqPlayer;
    for (;;) {
        state = &layer->scriptState;
        //M64_READ_U8(state, cmd);
        // manually inlined because we need _Kqi6 :(
        {
            u8 *_ptr_pc;
            _ptr_pc = (*state).pc;
            ((*state).pc)++;
            //cmd = *_ptr_pc;

            _Kqi6 = *_ptr_pc;
            cmd = _Kqi6;
        }

        if (cmd <= 0xc0) {
            break;
        }

        switch (_Kqi6)
        {
            case 0xff: // layer_end; function return or end of script
                if (state->depth == 0) {
                    // N.B. this function call is *not* inlined even though it's
                    // within the same file, unlike in the rest of this function.
                    seq_channel_layer_disable(layer);
                    return;
                }
                state->depth--, state->pc = state->stack[state->depth];
                break;

            case 0xfc: // layer_call
                M64_READ_S16(state, sp3A);
                state->depth++, state->stack[state->depth - 1] = state->pc;
                state->pc = seqPlayer->seqData + sp3A;
                break;

            case 0xf8: // layer_loop; loop start, N iterations (or 256 if N = 0)
                M64_READ_U8(state, state->remLoopIters[state->depth])
                state->depth++, state->stack[state->depth - 1] = state->pc;
                break;

            case 0xf7: // layer_loopend
                if (--state->remLoopIters[state->depth - 1] != 0) {
                    state->pc = state->stack[state->depth - 1];
                } else {
                    state->depth--;
                }
                break;

            case 0xfb: // layer_jump
                M64_READ_S16(state, sp3A);
                state->pc = seqPlayer->seqData + sp3A;
                break;

            case 0xc1: // layer_setshortnotevelocity
            case 0xca: // layer_setpan
                temp_a0_5 = *(state->pc++);
                if (cmd == 0xc1) {
                    layer->velocitySquare = (f32)(temp_a0_5 * temp_a0_5);
                } else {
                    layer->pan = (f32) temp_a0_5 / US_FLOAT(128.0);
                }
                break;

            case 0xc2: // layer_transpose; set transposition in semitones
            case 0xc9: // layer_setshortnoteduration
                temp_a0_6 = *(state->pc++);
                if (cmd == 0xc9) {
                    layer->noteDuration = temp_a0_6;
                } else {
                    layer->transposition = temp_a0_6;
                }
                break;

            case 0xc4: // layer_somethingon
            case 0xc5: // layer_somethingoff
                //! copt needs a ternary:
                //layer->continuousNotes = (cmd == 0xc4) ? TRUE : FALSE;
                if (cmd == 0xc4) {
                    temp8 = TRUE;
                } else {
                    temp8 = FALSE;
                }
                layer->continuousNotes = temp8;
                seq_channel_layer_note_decay(layer);
                break;

            case 0xc3: // layer_setshortnotedefaultplaypercentage
                M64_READ_COMPRESSED_U16(state, sp3A)
                layer->shortNoteDefaultPlayPercentage = sp3A;
                break;

            case 0xc6: // layer_setinstr
                M64_READ_U8(state, instId);
                if (instId < 127) {
                    get_instrument(seqChannel, instId, &(*layer).instrument, &(*layer).adsr, instId, 1);
                }
                break;

            case 0xc7: // layer_portamento
                M64_READ_U8(state, (*layer).portamento.mode);
                M64_READ_U8(state, PORTAMENTO_TGT_NOTE);
                PORTAMENTO_TGT_NOTE = PORTAMENTO_TGT_NOTE + (*layer).transposition + (*seqPlayer).transposition + (*seqChannel).transposition;
                if (PORTAMENTO_TGT_NOTE >= 0x80) {
                    PORTAMENTO_TGT_NOTE = 0;
                }
                layer->portamentoTargetNote = PORTAMENTO_TGT_NOTE;

                // If special, the next param is u8 instead of var
                if (PORTAMENTO_IS_SPECIAL((*(layer)).portamento)) {
                    layer->portamentoTime = *((state)->pc++);
                    break;
                }

                M64_READ_COMPRESSED_U16(state, sp3A)
                layer->portamentoTime = sp3A;
                break;

            case 0xc8: // layer_disableportamento
                layer->portamento.mode = 0;
                break;

            default:
                switch (cmd & 0xf0) {
                    case 0xd0: // layer_setshortnotevelocityfromtable
                        sp3A = seqPlayer->shortNoteVelocityTable[cmd & 0xf];
                        (*(layer)).velocitySquare = (f32)(sp3A * sp3A);
                        break;
                    case 0xe0: // layer_setshortnotedurationfromtable
                        (*(layer)).noteDuration = seqPlayer->shortNoteDurationTable[cmd & 0xf];
                        break;
                }
        }
    }
    if (cmd == 0xc0) // layer_delay
    {
        M64_READ_COMPRESSED_U16(state, layer->delay);
        layer->stopSomething = TRUE;
    } else {
        layer->stopSomething = FALSE;

        if (seqChannel->largeNotes ) {
            switch (cmd & 0xc0) {
                case 0x00: // layer_note0 (play percentage, velocity, duration)
                    M64_READ_COMPRESSED_U16(state, sp3A)
                    vel = *((*state).pc++);
                    layer->noteDuration = *(state->pc++);
                    layer->playPercentage = sp3A;
                    goto l1090;

                case 0x40: // layer_note1 (play percentage, velocity)
                    M64_READ_COMPRESSED_U16(state, sp3A)
                    vel = *(state->pc++);
                    layer->noteDuration = 0;
                    layer->playPercentage = sp3A;
                    goto l1090;

                    
                case 0x80: // layer_note2 (velocity, duration; uses last play percentage)
                    sp3A = layer->playPercentage;
                    vel = *(state->pc++);
                    layer->noteDuration = *(state->pc++);
                    goto l1090;
            }
l1090:
            cmdSemitone = (cmd - (cmd & 0xc0));
            layer->velocitySquare = vel * vel;
        } else {
            switch (cmd & 0xc0) {
                case 0x00: // play note, type 0 (play percentage)
                    M64_READ_COMPRESSED_U16(state, sp3A)
                    layer->playPercentage = sp3A;
                    break;

                case 0x40: // play note, type 1 (uses default play percentage)
                    sp3A = layer->shortNoteDefaultPlayPercentage;
                    break;

                case 0x80: // play note, type 2 (uses last play percentage)
                    sp3A = layer->playPercentage;
                    break;
            }

            cmdSemitone = cmd - (cmd & 0xc0);
        }

        layer->delay = sp3A;
        layer->duration = layer->noteDuration * sp3A / 256;
        if ((seqPlayer->muted && (seqChannel->muteBehavior & MUTE_BEHAVIOR_STOP_NOTES) != 0)
            || seqChannel->stopSomething2
            || !seqChannel->hasInstrument
        ) {
            layer->stopSomething = TRUE;
        } else {
            if (seqChannel->instOrWave == 0) { // drum
                //DRUM_INDEX = cmdSemitone;
                DRUM_INDEX += (*seqChannel).transposition + (*layer).transposition;
                if (DRUM_INDEX >= gCtlEntries[seqChannel->bankId].numDrums) {
                    DRUM_INDEX = gCtlEntries[seqChannel->bankId].numDrums;
                    if (DRUM_INDEX == 0) {
                        // this goto looks a bit like a function return...
                        layer->stopSomething = TRUE;
                        goto skip;
                    }

                    DRUM_INDEX--;
                }

                drum = gCtlEntries[seqChannel->bankId].drums[DRUM_INDEX];
                if (drum == NULL) {
                    layer->stopSomething = TRUE;
                } else {
                    layer->adsr.envelope = drum->envelope;
                    layer->adsr.releaseRate = drum->releaseRate;
                    layer->pan = FLOAT_CAST(drum->pan) / US_FLOAT(128.0);
                    layer->sound = &drum->sound;
                    layer->freqScale = layer->sound->tuning;
                }

            skip:;
            } else { // instrument
                //SEMITONE = cmdSemitone;
                SEMITONE += (*(seqPlayer)).transposition + (*(seqChannel)).transposition + (*(layer)).transposition;
                if (SEMITONE >= 0x80) {
                    layer->stopSomething = TRUE;
                } else {
                    instrument = layer->instrument;
                    if (instrument == NULL) {
                        instrument = seqChannel->instrument;
                    }

                    if (layer->portamento.mode != 0) {
                        //! copt needs a ternary:
                        //usedSemitone = (layer->portamentoTargetNote < SEMITONE) ? SEMITONE : layer->portamentoTargetNote;
                        if (layer->portamentoTargetNote < SEMITONE) {
                            USED_SEMITONE = SEMITONE;
                        } else {
                            USED_SEMITONE = layer->portamentoTargetNote;
                        }

                        if (instrument != NULL) {
                            sound = (u8) USED_SEMITONE < instrument->normalRangeLo ? &instrument->lowNotesSound
                                  : (u8) USED_SEMITONE <= instrument->normalRangeHi ?
                                        &instrument->normalNotesSound : &instrument->highNotesSound;

                            sameSound = (sound == (*layer).sound);
                            layer->sound = sound;
                            tuning = (*sound).tuning;
                        } else {
                            layer->sound = NULL;
                            tuning = 1.0f;
                        }

                        temp_f2 = gNoteFrequencies[SEMITONE] * tuning;
                        temp_f12 = gNoteFrequencies[layer->portamentoTargetNote] * tuning;

                        portamento = &layer->portamento;
                        switch (PORTAMENTO_MODE(layer->portamento)) {
                            case PORTAMENTO_MODE_1:
                            case PORTAMENTO_MODE_3:
                            case PORTAMENTO_MODE_5:
                                sp24 = temp_f2;
                                freqScale = temp_f12;
                                goto l13cc;

                            case PORTAMENTO_MODE_2:
                            case PORTAMENTO_MODE_4:
                                freqScale = temp_f2;
                                sp24 = temp_f12;
                                goto l13cc;
                        }
l13cc:
                        portamento->extent = sp24 / freqScale - US_FLOAT(1.0);
                        if (PORTAMENTO_IS_SPECIAL((*(layer)).portamento)) {
                            portamento->speed = US_FLOAT(32512.0) * FLOAT_CAST((*(seqPlayer)).tempo)
                                                / ((f32)(*(layer)).delay * (f32) gTempoInternalToExternal
                                                   * FLOAT_CAST((*(layer)).portamentoTime));
                        } else {
                            portamento->speed = US_FLOAT(127.0) / FLOAT_CAST((*(layer)).portamentoTime);
                        }
                        portamento->cur = 0.0f;
                        layer->freqScale = freqScale;
                        if (PORTAMENTO_MODE((*(layer)).portamento) == PORTAMENTO_MODE_5) {
                            layer->portamentoTargetNote = SEMITONE;
                        }
                    } else if (instrument != NULL) {
                        sound = SEMITONE < instrument->normalRangeLo ?
                                         &instrument->lowNotesSound : SEMITONE <= instrument->normalRangeHi ?
                                         &instrument->normalNotesSound : &instrument->highNotesSound;

                        sameSound = (sound == (*(layer)).sound);
                        layer->sound = sound;
                        layer->freqScale = gNoteFrequencies[SEMITONE] * (*(sound)).tuning;
                    } else {
                        layer->sound = NULL;
                        layer->freqScale = gNoteFrequencies[SEMITONE];
                    }
                }
            }
        }
    }

    if (layer->stopSomething ) {
        if (layer->note != NULL || layer->continuousNotes) {
            seq_channel_layer_note_decay(layer);
        }
        return;
    }

    cmd = FALSE;
    if (!layer->continuousNotes) {
        cmd = TRUE;
    } else if (layer->note == NULL || layer->status == SOUND_LOAD_STATUS_NOT_LOADED) {
        cmd = TRUE;
    } else if (sameSound == FALSE) {
        seq_channel_layer_note_decay(layer);
        cmd = TRUE;
    } else if (layer->sound == NULL) {
        init_synthetic_wave(layer->note, layer);
    }

    if (cmd != FALSE) {
        (*(layer)).note = alloc_note(layer);
    }

    if (layer->note != NULL && layer->note->parentLayer == layer) {
        note_vibrato_init(layer->note);
    }
}

void init_note_list(struct AudioListItem *list) {
    list->prev = list;
    list->next = list;
    list->u.count = 0;
}

void init_note_lists(struct NotePool *pool) {
    init_note_list(&pool->disabled);
    init_note_list(&pool->decaying);
    init_note_list(&pool->releasing);
    init_note_list(&pool->active);
    pool->disabled.pool = pool;
    pool->decaying.pool = pool;
    pool->releasing.pool = pool;
    pool->active.pool = pool;
}


void note_pool_clear(struct NotePool *pool) {
    s32 i;
    struct AudioListItem *source;
    struct AudioListItem *cur;
    struct AudioListItem *dest;
    s32 j; // unused in EU

    for (i = 0; i < 4; i++) {
        switch (i) {
            case 0:
                source = &pool->disabled;
                dest = &gNoteFreeLists.disabled;
                break;

            case 1:
                source = &pool->decaying;
                dest = &gNoteFreeLists.decaying;
                break;

            case 2:
                source = &pool->releasing;
                dest = &gNoteFreeLists.releasing;
                break;

            case 3:
                source = &pool->active;
                dest = &gNoteFreeLists.active;
                break;
        }

#ifdef VERSION_EU
        for (;;) {
            cur = source->next;
            if (cur == source || cur == NULL) {
                break;
            }
            audio_list_remove(cur);
            audio_list_push_back(dest, cur);
        }
#else
        j = 0;
        do {
            cur = source->next;
            if (cur == source) {
                break;
            }
            audio_list_remove(cur);
            audio_list_push_back(dest, cur);
            j++;
        } while (j <= gMaxSimultaneousNotes);
#endif
    }
}

void note_pool_fill(struct NotePool *pool, s32 count) {
    s32 i;
    s32 j;
    struct Note *note;
    struct AudioListItem *source;
    struct AudioListItem *dest;

    note_pool_clear(pool);

    for (i = 0, j = 0; j < count; i++) {
        if (i == 4) {
            return;
        }

        switch (i) {
            case 0:
                source = &gNoteFreeLists.disabled;
                dest = &pool->disabled;
                break;

            case 1:
                source = &gNoteFreeLists.decaying;
                dest = &pool->decaying;
                break;

            case 2:
                source = &gNoteFreeLists.releasing;
                dest = &pool->releasing;
                break;

            case 3:
                source = &gNoteFreeLists.active;
                dest = &pool->active;
                break;
        }

        while (j < count) {
            note = audio_list_pop_back(source);
            if (note == NULL) {
                break;
            }
            audio_list_push_back(dest, &note->listItem);
            j++;
        }
    }
}

void audio_list_push_front(struct AudioListItem *list, struct AudioListItem *item) {
    // add 'item' to the front of the list given by 'list', if it's not in any list
    if (item->prev == NULL) {
        item->prev = list;
        item->next = list->next;
        list->next->prev = item;
        list->next = item;
        list->u.count++;
        item->pool = list->pool;
    }
}

void audio_list_remove(struct AudioListItem *item) {
    // remove 'item' from the list it's in, if any
    if (item->prev != NULL) {
        item->prev->next = item->next;
        item->next->prev = item->prev;
        item->prev = NULL;
    }
}

__attribute__((always_inline))  inline struct Note *pop_node_with_value_less_equal(struct AudioListItem *list, s32 limit) {
    struct AudioListItem *cur = list->next;
    struct AudioListItem *best;

    if (cur == list) {
        return NULL;
    }

    for (best = cur; cur != list; cur = cur->next) {
        if (((struct Note *) best->u.value)->priority >= ((struct Note *) cur->u.value)->priority) {
            best = cur;
        }
    }

#ifdef VERSION_EU
    if (best == NULL) {
        return NULL;
    }

    if (limit <= ((struct Note *) best->u.value)->priority) {
        return NULL;
    }
#else
    if (limit < ((struct Note *) best->u.value)->priority) {
        return NULL;
    }
#endif

    audio_list_remove(best);
    return best->u.value;
}

#if defined(VERSION_EU)
void note_init_for_layer(struct Note *note, struct SequenceChannelLayer *seqLayer) {
    s16 instId;
    struct NoteSubEu *sub = &note->noteSubEu;

    note->prevParentLayer = NO_LAYER;
    note->parentLayer = seqLayer;
    note->priority = seqLayer->seqChannel->notePriority;
    seqLayer->notePropertiesNeedInit = TRUE;
    seqLayer->status = SOUND_LOAD_STATUS_DISCARDABLE; // "loaded"
    seqLayer->note = note;
    seqLayer->noteVelocity = 0.0f;
    note_init(note);
    instId = seqLayer->instOrWave;
    if (instId == 0xff) {
        instId = seqLayer->seqChannel->instOrWave;
    }
    sub->sound.audioBankSound = seqLayer->sound;

    if (instId >= 0x80) {
        sub->isSyntheticWave = TRUE;
    } else {
        sub->isSyntheticWave = FALSE;
    }

    if (sub->isSyntheticWave) {
        build_synthetic_wave(note, seqLayer, instId);
    }
    sub->bankId = seqLayer->seqChannel->bankId;
    sub->stereoHeadsetEffects = seqLayer->seqChannel->stereoHeadsetEffects;
    sub->reverbIndex = seqLayer->seqChannel->reverbIndex & 3;
}
#else
s32 note_init_for_layer(struct Note *note, struct SequenceChannelLayer *seqLayer) {
    note->prevParentLayer = NO_LAYER;
    note->parentLayer = seqLayer;
    note->priority = seqLayer->seqChannel->notePriority;
    if (IS_BANK_LOAD_COMPLETE(seqLayer->seqChannel->bankId) == FALSE) {
        return TRUE;
    }

    note->bankId = seqLayer->seqChannel->bankId;
    note->stereoHeadsetEffects = seqLayer->seqChannel->stereoHeadsetEffects;
    note->sound = seqLayer->sound;
    seqLayer->status = SOUND_LOAD_STATUS_DISCARDABLE; // "loaded"
    seqLayer->note = note;
    if (note->sound == NULL) {
        build_synthetic_wave(note, seqLayer);
    }
    note_init(note);
    return FALSE;
}
#endif

__attribute__((always_inline))  inline void func_80319728(struct Note *note, struct SequenceChannelLayer *seqLayer) {
    seq_channel_layer_note_release(note->parentLayer);
    note->wantedParentLayer = seqLayer;
}

void note_release_and_take_ownership(struct Note *note, struct SequenceChannelLayer *seqLayer) {
    note->wantedParentLayer = seqLayer;
    note->priority = NOTE_PRIORITY_STOPPING;
#ifdef VERSION_EU
    note->adsr.fadeOutVel = gAudioBufferParameters.updatesPerFrameInv;
#else
    note->adsr.fadeOutVel = 0x8000 / gAudioUpdatesPerFrame;
#endif
    note->adsr.action |= ADSR_ACTION_RELEASE;
}

struct Note *alloc_note_from_disabled(struct NotePool *pool, struct SequenceChannelLayer *seqLayer) {
    struct Note *note = audio_list_pop_back(&pool->disabled);
    if (note != NULL) {
#ifdef VERSION_EU
        note_init_for_layer(note, seqLayer);
#else
        if (note_init_for_layer(note, seqLayer) ) {
            audio_list_push_front(&gNoteFreeLists.disabled, &note->listItem);
            return NULL;
        }
#endif
        audio_list_push_front(&pool->active, &note->listItem);
    }
    return note;
}

struct Note *alloc_note_from_decaying(struct NotePool *pool, struct SequenceChannelLayer *seqLayer) {
    struct Note *note = audio_list_pop_back(&pool->decaying);
    if (note != NULL) {
        note_release_and_take_ownership(note, seqLayer);
        audio_list_push_back(&pool->releasing, &note->listItem);
    }
    return note;
}

struct Note *alloc_note_from_active(struct NotePool *pool, struct SequenceChannelLayer *seqLayer) {
    struct Note *note =
        pop_node_with_value_less_equal(&pool->active, seqLayer->seqChannel->notePriority);
    if (note != NULL) {
        func_80319728(note, seqLayer);
        audio_list_push_back(&pool->releasing, &note->listItem);
    }
    return note;
}

struct Note *alloc_note(struct SequenceChannelLayer *seqLayer) {
    struct Note *ret;
    u32 policy = seqLayer->seqChannel->noteAllocPolicy;

    if (policy & NOTE_ALLOC_LAYER) {
        ret = seqLayer->note;
        if (ret != NULL && ret->prevParentLayer == seqLayer
#ifdef VERSION_EU
                && ret->wantedParentLayer == NO_LAYER
#endif
                ) {
            note_release_and_take_ownership(ret, seqLayer);
            audio_list_remove(&ret->listItem);
#ifdef VERSION_EU
            audio_list_push_back(&ret->listItem.pool->releasing, &ret->listItem);
#else
            audio_list_push_back(&gNoteFreeLists.releasing, &ret->listItem);
#endif
            return ret;
        }
    }

    if (policy & NOTE_ALLOC_CHANNEL) {
        if (!(ret = alloc_note_from_disabled(&seqLayer->seqChannel->notePool, seqLayer))
            && !(ret = alloc_note_from_decaying(&seqLayer->seqChannel->notePool, seqLayer))
            && !(ret = alloc_note_from_active(&seqLayer->seqChannel->notePool, seqLayer))) {
            seqLayer->status = SOUND_LOAD_STATUS_NOT_LOADED;
            return NULL;
        }
        return ret;
    }

    if (policy & NOTE_ALLOC_SEQ) {
        if (!(ret = alloc_note_from_disabled(&seqLayer->seqChannel->notePool, seqLayer))
            && !(ret = alloc_note_from_disabled(&seqLayer->seqChannel->seqPlayer->notePool, seqLayer))
            && !(ret = alloc_note_from_decaying(&seqLayer->seqChannel->notePool, seqLayer))
            && !(ret = alloc_note_from_decaying(&seqLayer->seqChannel->seqPlayer->notePool, seqLayer))
            && !(ret = alloc_note_from_active(&seqLayer->seqChannel->notePool, seqLayer))
            && !(ret = alloc_note_from_active(&seqLayer->seqChannel->seqPlayer->notePool, seqLayer))) {
            seqLayer->status = SOUND_LOAD_STATUS_NOT_LOADED;
            return NULL;
        }
        return ret;
    }

    if (policy & NOTE_ALLOC_GLOBAL_FREELIST) {
        if (!(ret = alloc_note_from_disabled(&gNoteFreeLists, seqLayer))
            && !(ret = alloc_note_from_decaying(&gNoteFreeLists, seqLayer))
            && !(ret = alloc_note_from_active(&gNoteFreeLists, seqLayer))) {
            seqLayer->status = SOUND_LOAD_STATUS_NOT_LOADED;
            return NULL;
        }
        return ret;
    }

    if (!(ret = alloc_note_from_disabled(&seqLayer->seqChannel->notePool, seqLayer))
        && !(ret = alloc_note_from_disabled(&seqLayer->seqChannel->seqPlayer->notePool, seqLayer))
        && !(ret = alloc_note_from_disabled(&gNoteFreeLists, seqLayer))
        && !(ret = alloc_note_from_decaying(&seqLayer->seqChannel->notePool, seqLayer))
        && !(ret = alloc_note_from_decaying(&seqLayer->seqChannel->seqPlayer->notePool, seqLayer))
        && !(ret = alloc_note_from_decaying(&gNoteFreeLists, seqLayer))
        && !(ret = alloc_note_from_active(&seqLayer->seqChannel->notePool, seqLayer))
        && !(ret = alloc_note_from_active(&seqLayer->seqChannel->seqPlayer->notePool, seqLayer))
        && !(ret = alloc_note_from_active(&gNoteFreeLists, seqLayer))) {
        seqLayer->status = SOUND_LOAD_STATUS_NOT_LOADED;
        return NULL;
    }
    return ret;
}

#ifndef VERSION_EU
__attribute__((always_inline))  inline void reclaim_notes(void) {
    struct Note *note;
    s32 i;
    s32 cond;

    for (i = 0; i < gMaxSimultaneousNotes; i++) {
        note = &gNotes[i];
        if (note->parentLayer != NO_LAYER) {
            cond = FALSE;
            if (!note->parentLayer->enabled && note->priority >= NOTE_PRIORITY_MIN) {
                cond = TRUE;
            } else if (note->parentLayer->seqChannel == NULL) {
                audio_list_push_back(&gLayerFreeList, &note->parentLayer->listItem);
                seq_channel_layer_disable(note->parentLayer);
                note->priority = NOTE_PRIORITY_STOPPING;
            } else if (note->parentLayer->seqChannel->seqPlayer == NULL) {
                sequence_channel_disable(note->parentLayer->seqChannel);
                note->priority = NOTE_PRIORITY_STOPPING;
            } else if (note->parentLayer->seqChannel->seqPlayer->muted) {
                if (note->parentLayer->seqChannel->muteBehavior
                    & (MUTE_BEHAVIOR_STOP_SCRIPT | MUTE_BEHAVIOR_STOP_NOTES)) {
                    cond = TRUE;
                }
            } else {
                cond = FALSE;
            }

            if (cond) {
                seq_channel_layer_note_release(note->parentLayer);
                audio_list_remove(&note->listItem);
                audio_list_push_front(&note->listItem.pool->disabled, &note->listItem);
                note->priority = NOTE_PRIORITY_STOPPING;
            }
        }
    }
}
#endif


void note_init_all(void) {
    struct Note *note;
    s32 i;

    for (i = 0; i < gMaxSimultaneousNotes; i++) {
        note = &gNotes[i];
#ifdef VERSION_EU
        note->noteSubEu = gZeroNoteSub;
#else
        note->enabled = FALSE;
        note->stereoStrongRight = FALSE;
        note->stereoStrongLeft = FALSE;
        note->stereoHeadsetEffects = FALSE;
#endif
        note->priority = NOTE_PRIORITY_DISABLED;
        note->parentLayer = NO_LAYER;
        note->wantedParentLayer = NO_LAYER;
        note->prevParentLayer = NO_LAYER;
#ifdef VERSION_EU
        note->waveId = 0;
#else
        note->reverb = 0;
        note->usesHeadsetPanEffects = FALSE;
        note->sampleCount = 0;
        note->instOrWave = 0;
        note->targetVolLeft = 0;
        note->targetVolRight = 0;
        note->frequency = 0.0f;
#endif
        note->attributes.velocity = 0.0f;
        note->adsrVolScale = 0;
        note->adsr.state = ADSR_STATE_DISABLED;
        note->adsr.action = 0;
        note->vibratoState.active = FALSE;
        note->portamento.cur = 0.0f;
        note->portamento.speed = 0.0f;
#ifdef VERSION_EU
        note->synthesisState.synthesisBuffers = soundAlloc(&gNotesAndBuffersPool, sizeof(struct NoteSynthesisBuffers));
#else
        note->synthesisBuffers = soundAlloc(&gNotesAndBuffersPool, sizeof(struct NoteSynthesisBuffers));
#endif
    }
}


#ifdef VERSION_EU
u64 *synthesis_process_note(struct Note *note, struct NoteSubEu *noteSubEu, struct NoteSynthesisState *synthesisState, s16 *aiBuf, s32 bufLen, u64 *cmd);
u64 *load_wave_samples(u64 *cmd, struct NoteSubEu *noteSubEu, struct NoteSynthesisState *synthesisState, s32 nSamplesToLoad);
u64 *final_resample(u64 *cmd, struct NoteSynthesisState *synthesisState, s32 count, u16 pitch, u16 dmemIn, u32 flags);
u64 *process_envelope(u64 *cmd, struct NoteSubEu *noteSubEu, struct NoteSynthesisState *synthesisState, s32 nSamples, u16 inBuf, s32 headsetPanSettings, u32 flags);
u64 *note_apply_headset_pan_effects(u64 *cmd, struct NoteSubEu *noteSubEu, struct NoteSynthesisState *note, s32 bufLen, s32 flags, s32 leftRight);
#else
u64 *synthesis_process_notes(s16 *aiBuf, s32 bufLen, u64 *cmd);
u64 *load_wave_samples(u64 *cmd, struct Note *note, s32 nSamplesToLoad);
u64 *final_resample(u64 *cmd, struct Note *note, s32 count, s32 pitch, s32 dmemIn, u32 flags);
u64 *process_envelope(u64 *cmd, struct Note *note, s32 nSamples, s32 inBuf, s32 headsetPanSettings);
__attribute__((always_inline))  inline u64 *process_envelope_inner(u64 *cmd, struct Note *note, s32 nSamples, u32 inBuf,
                            s32 headsetPanSettings, struct VolumeChange *vol);
u64 *note_apply_headset_pan_effects(u64 *cmd, struct Note *note, s32 bufLen, s32 flags, s32 leftRight);
#endif


struct SynthesisReverb gSynthesisReverb;

void seq_channel_layer_process_script(struct SequenceChannelLayer *layer);
void sequence_channel_process_script(struct SequenceChannel *seqChannel);
s32 get_instrument(struct SequenceChannel *seqChannel, s32 instId, struct Instrument **instOut,
                  struct AdsrSettings *adsr);

void sequence_channel_init(struct SequenceChannel *seqChannel) {
    s32 i;

    seqChannel->enabled = FALSE;
    seqChannel->finished = FALSE;
    seqChannel->stopScript = FALSE;
    seqChannel->stopSomething2 = FALSE;
    seqChannel->hasInstrument = FALSE;
    seqChannel->stereoHeadsetEffects = FALSE;
    seqChannel->transposition = 0;
    seqChannel->largeNotes = FALSE;
    seqChannel->scriptState.depth = 0;
    seqChannel->volume = 1.0f;
    seqChannel->volumeScale = 1.0f;
    seqChannel->freqScale = 1.0f;
    seqChannel->pan = 0.5f;
    seqChannel->panChannelWeight = 1.0f;
    seqChannel->reverb = 0;
    seqChannel->notePriority = NOTE_PRIORITY_DEFAULT;
    seqChannel->delay = 0;
    seqChannel->adsr.envelope = gDefaultEnvelope;
    seqChannel->adsr.releaseRate = 0x20;
    seqChannel->adsr.sustain = 0;
    seqChannel->vibratoRateTarget = 0x800;
    seqChannel->vibratoRateStart = 0x800;
    seqChannel->vibratoExtentTarget = 0;
    seqChannel->vibratoExtentStart = 0;
    seqChannel->vibratoRateChangeDelay = 0;
    seqChannel->vibratoExtentChangeDelay = 0;
    seqChannel->vibratoDelay = 0;

    for (i = 0; i < 8; i++) {
        seqChannel->soundScriptIO[i] = -1;
    }

    init_note_lists(&seqChannel->notePool);
}

s32 seq_channel_set_layer(struct SequenceChannel *seqChannel, s32 layerIndex) {
    struct SequenceChannelLayer *layer;

    if (seqChannel->layers[layerIndex] == NULL) {
#ifdef VERSION_EU
        struct SequenceChannelLayer *layer;
#endif
        layer = audio_list_pop_back(&gLayerFreeList);
        seqChannel->layers[layerIndex] = layer;
        if (layer == NULL) {
            seqChannel->layers[layerIndex] = NULL;
            return -1;
        }
    } else {
        seq_channel_layer_note_decay(seqChannel->layers[layerIndex]);
    }

    layer = seqChannel->layers[layerIndex];
    layer->seqChannel = seqChannel;
    layer->adsr = seqChannel->adsr;
    layer->adsr.releaseRate = 0;
    layer->enabled = TRUE;
    layer->stopSomething = FALSE;
    layer->continuousNotes = FALSE;
    layer->finished = FALSE;
#ifdef VERSION_EU
    layer->ignoreDrumPan = FALSE;
#endif
    layer->portamento.mode = 0;
    layer->scriptState.depth = 0;
    layer->status = SOUND_LOAD_STATUS_NOT_LOADED;
    layer->noteDuration = 0x80;
#ifdef VERSION_EU
    layer->pan = 0x40;
#endif
    layer->transposition = 0;
    layer->delay = 0;
    layer->duration = 0;
    layer->note = NULL;
    layer->instrument = NULL;
#ifdef VERSION_EU
    layer->freqScale = 1.0f;
    layer->velocitySquare = 0.0f;
    layer->instOrWave = 0xff;
#else
    layer->velocitySquare = 0.0f;
    layer->pan = 0.5f;
#endif
    return 0;
}

void seq_channel_layer_disable(struct SequenceChannelLayer *layer) {
    if (layer != NULL) {
        seq_channel_layer_note_decay(layer);
        layer->enabled = FALSE;
        layer->finished = TRUE;
    }
}

void seq_channel_layer_free(struct SequenceChannel *seqChannel, s32 layerIndex) {
    struct SequenceChannelLayer *layer = seqChannel->layers[layerIndex];

    if (layer != NULL) {
#ifdef VERSION_EU
        audio_list_push_back(&gLayerFreeList, &layer->listItem);
#else
        struct AudioListItem *item = &layer->listItem;
        if (item->prev == NULL) {
            gLayerFreeList.prev->next = item;
            item->prev = gLayerFreeList.prev;
            item->next = &gLayerFreeList;
            gLayerFreeList.prev = item;
            gLayerFreeList.u.count++;
            item->pool = gLayerFreeList.pool;
        }
#endif
        seq_channel_layer_disable(layer);
        seqChannel->layers[layerIndex] = NULL;
    }
}

void sequence_channel_disable(struct SequenceChannel *seqChannel) {
    s32 i;
    for (i = 0; i < LAYERS_MAX; i++) {
        seq_channel_layer_free(seqChannel, i);
    }

    note_pool_clear(&seqChannel->notePool);
    seqChannel->enabled = FALSE;
    seqChannel->finished = TRUE;
}

__attribute__((always_inline))  inline struct SequenceChannel *allocate_sequence_channel(void) {
    s32 i;
    for (i = 0; i < ARRAY_COUNT(gSequenceChannels); i++) {
        if (gSequenceChannels[i].seqPlayer == NULL) {
#ifdef VERSION_EU
            return &gSequenceChannels[i];
#else
            return gSequenceChannels + i;
#endif
        }
    }
    return &gSequenceChannelNone;
}

__attribute__((always_inline))  inline void sequence_player_init_channels(struct SequencePlayer *seqPlayer, u16 channelBits) {
    struct SequenceChannel *seqChannel;
    s32 i;

    for (i = 0; i < CHANNELS_MAX; i++) {
        if (channelBits & 1) {
            seqChannel = seqPlayer->channels[i];
            if (IS_SEQUENCE_CHANNEL_VALID(seqChannel)  && seqChannel->seqPlayer == seqPlayer) {
                sequence_channel_disable(seqChannel);
                seqChannel->seqPlayer = NULL;
            }
            seqChannel = allocate_sequence_channel();
            if (IS_SEQUENCE_CHANNEL_VALID(seqChannel) == FALSE) {
             //   gAudioErrorFlags = i + 0x10000;
                seqPlayer->channels[i] = seqChannel;
            } else {
                sequence_channel_init(seqChannel);
                seqPlayer->channels[i] = seqChannel;
                seqChannel->seqPlayer = seqPlayer;
                seqChannel->bankId = seqPlayer->defaultBank[0];
                seqChannel->muteBehavior = seqPlayer->muteBehavior;
                seqChannel->noteAllocPolicy = seqPlayer->noteAllocPolicy;
            }
        }
#ifdef VERSION_EU
        channelBits = channelBits >> 1;
#else
        channelBits >>= 1;
#endif
    }
}

void sequence_player_disable_channels(struct SequencePlayer *seqPlayer, u16 channelBits) {
    struct SequenceChannel *seqChannel;
    s32 i;

    for (i = 0; i < CHANNELS_MAX; i++) {
        if (channelBits & 1) {
            seqChannel = seqPlayer->channels[i];
            if (IS_SEQUENCE_CHANNEL_VALID(seqChannel) ) {
                if (seqChannel->seqPlayer == seqPlayer) {
                    sequence_channel_disable(seqChannel);
                    seqChannel->seqPlayer = NULL;
                }
#ifdef VERSION_EU
                if (0) {}
#endif
                seqPlayer->channels[i] = &gSequenceChannelNone;
            }
        }
#ifdef VERSION_EU
        channelBits = channelBits >> 1;
#else
        channelBits >>= 1;
#endif
    }
}

void sequence_channel_enable(struct SequencePlayer *seqPlayer, u8 channelIndex, void *arg2) {
    struct SequenceChannel *seqChannel = seqPlayer->channels[channelIndex];
    s32 i;

#ifdef VERSION_EU
    if (IS_SEQUENCE_CHANNEL_VALID(seqChannel) == FALSE) {
        struct SequencePlayer *bgMusic = &gSequencePlayers[0];
        struct SequencePlayer *miscMusic = &gSequencePlayers[1];

        if (seqPlayer == bgMusic) {
        } else if (seqPlayer == miscMusic) {
        } else {
        }
    } else {
#else
    if (IS_SEQUENCE_CHANNEL_VALID(seqChannel) != FALSE) {
#endif
        seqChannel->enabled = TRUE;
        seqChannel->finished = FALSE;
        seqChannel->scriptState.depth = 0;
        seqChannel->scriptState.pc = arg2;
        seqChannel->delay = 0;
        for (i = 0; i < LAYERS_MAX; i++) {
            if (seqChannel->layers[i] != NULL) {
                seq_channel_layer_free(seqChannel, i);
            }
        }
    }
}

void sequence_player_disable(struct SequencePlayer *seqPlayer) {
    sequence_player_disable_channels(seqPlayer, 0xffff);
    note_pool_clear(&seqPlayer->notePool);
    seqPlayer->finished = TRUE;
    seqPlayer->enabled = FALSE;

    if (IS_SEQ_LOAD_COMPLETE(seqPlayer->seqId)) {
        gSeqLoadStatus[seqPlayer->seqId] = SOUND_LOAD_STATUS_DISCARDABLE;
    }

    if (IS_BANK_LOAD_COMPLETE(seqPlayer->defaultBank[0])) {
        gBankLoadStatus[seqPlayer->defaultBank[0]] = SOUND_LOAD_STATUS_DISCARDABLE;
    }

    // (Note that if this is called from alloc_bank_or_seq, the side will get swapped
    // later in that function. Thus, we signal that we want to load into the slot
    // of the bank that we no longer need.)
#ifdef VERSION_EU
    if (seqPlayer->defaultBank[0] == gBankLoadedPool.temporary.entries[0].id) {
        gBankLoadedPool.temporary.nextSide = 1;
    } else if (seqPlayer->defaultBank[0] == gBankLoadedPool.temporary.entries[1].id) {
        gBankLoadedPool.temporary.nextSide = 0;
    }
#else
    if (gBankLoadedPool.temporary.entries[0].id == seqPlayer->defaultBank[0]) {
        gBankLoadedPool.temporary.nextSide = 1;
    } else if (gBankLoadedPool.temporary.entries[1].id == seqPlayer->defaultBank[0]) {
        gBankLoadedPool.temporary.nextSide = 0;
    }
#endif
}

/**
 * Add an item to the end of a list, if it's not already in any list.
 */
void audio_list_push_back(struct AudioListItem *list, struct AudioListItem *item) {
    if (item->prev == NULL) {
        list->prev->next = item;
        item->prev = list->prev;
        item->next = list;
        list->prev = item;
        list->u.count++;
        item->pool = list->pool;
    }
}

/**
 * Remove the last item from a list, and return it (or NULL if empty).
 */
void *audio_list_pop_back(struct AudioListItem *list) {
    struct AudioListItem *item = list->prev;
    if (item == list) {
        return NULL;
    }
    item->prev->next = list;
    list->prev = item->prev;
    item->prev = NULL;
    list->u.count--;
    return item->u.value;
}

__attribute__((always_inline))  inline void init_layer_freelist(void) {
    s32 i;

    gLayerFreeList.prev = &gLayerFreeList;
    gLayerFreeList.next = &gLayerFreeList;
    gLayerFreeList.u.count = 0;
    gLayerFreeList.pool = NULL;

    for (i = 0; i < ARRAY_COUNT(gSequenceLayers); i++) {
#ifdef VERSION_EU
        gSequenceLayers[i].listItem.u.value = &gSequenceLayers[i];
#else
        gSequenceLayers[i].listItem.u.value = gSequenceLayers + i;
#endif
        gSequenceLayers[i].listItem.prev = NULL;
        audio_list_push_back(&gLayerFreeList, &gSequenceLayers[i].listItem);
    }
}

__attribute__((always_inline))  inline s32 m64_read_u8(struct M64ScriptState *state) {
    u8 *midiArg = state->pc++;
    return *midiArg;
}

__attribute__((always_inline))  inline s32 m64_read_s16(struct M64ScriptState *state) {
    s32 ret = *(state->pc++) << 8;
    return *(state->pc++) | ret;
}

__attribute__((always_inline))  inline s32 m64_read_compressed_u16(struct M64ScriptState *state) {
    s32 ret = *(state->pc++);
    if (ret & 0x80) {
        ret = (ret << 8) & 0x7f00;
        ret = *(state->pc++) | ret;
    }
    return ret;
}
#include "seq_channel_layer_process_script.h"

s32 get_instrument(struct SequenceChannel *seqChannel, s32 instId, struct Instrument **instOut, struct AdsrSettings *adsr) {
    struct Instrument *inst;
#ifdef VERSION_EU
    inst = get_instrument_inner(seqChannel->bankId, instId);
    if (inst == NULL)
    {
        *instOut = NULL;
        return 0;
    }
    adsr->envelope = inst->envelope;
    adsr->releaseRate = inst->releaseRate;
    *instOut = inst;
    instId++;
    return instId;
#else

    if (instId >= gCtlEntries[seqChannel->bankId].numInstruments) {
        instId = gCtlEntries[seqChannel->bankId].numInstruments;
        if (instId == 0) {
            return 0;
        }
        instId--;
    }

    inst = gCtlEntries[seqChannel->bankId].instruments[instId];
    if (inst == NULL) {
        struct SequenceChannel seqChannelCpy = *seqChannel;

        while (instId != 0xff) {
            inst = gCtlEntries[seqChannelCpy.bankId].instruments[instId];
            if (inst != NULL) {
                break;
            }
            instId--;
        }
    }

    if (((uintptr_t) gBankLoadedPool.persistent.pool.start <= (uintptr_t) inst
         && (uintptr_t) inst <= (uintptr_t)(gBankLoadedPool.persistent.pool.start
                    + gBankLoadedPool.persistent.pool.size))
        || ((uintptr_t) gBankLoadedPool.temporary.pool.start <= (uintptr_t) inst
            && (uintptr_t) inst <= (uintptr_t)(gBankLoadedPool.temporary.pool.start
                                   + gBankLoadedPool.temporary.pool.size))) {
        adsr->envelope = inst->envelope;
        adsr->releaseRate = inst->releaseRate;
        *instOut = inst;
        instId++;
        return instId;
    }

  //  gAudioErrorFlags = instId + 0x20000;
    *instOut = NULL;
    return 0;
#endif
}

__attribute__((always_inline))  inline void set_instrument(struct SequenceChannel *seqChannel, u8 instId) {
    if (instId >= 0x80) {
        seqChannel->instOrWave = instId;
        seqChannel->instrument = NULL;
    } else if (instId == 0x7f) {
        seqChannel->instOrWave = 0;
        seqChannel->instrument = (struct Instrument *) 1;
    } else {
#ifdef VERSION_EU
        if ((seqChannel->instOrWave =
            get_instrument(seqChannel, instId, &seqChannel->instrument, &seqChannel->adsr)) == 0)
#else
        seqChannel->instOrWave =
            get_instrument(seqChannel, instId, &seqChannel->instrument, &seqChannel->adsr);
        if (seqChannel->instOrWave == 0)
#endif
        {
            seqChannel->hasInstrument = FALSE;
            return;
        }
    }
    seqChannel->hasInstrument = TRUE;
}

__attribute__((always_inline))  inline void sequence_channel_set_volume(struct SequenceChannel *seqChannel, u8 volume) {
    seqChannel->volume = FLOAT_CAST(volume) / US_FLOAT(127.0);
}

//rodata: 0xf3e30
void sequence_channel_process_script(struct SequenceChannel *seqChannel) {
    struct M64ScriptState *state;
    struct SequencePlayer *seqPlayer;
    u16 sp5A;
    u8 cmd;    // v1, s1
    u8 loBits; // t0, a0
    s32 offset;
    s8 value; // sp53, 4b
    u8 temp;
    s8 tempSigned;
    s32 i;
#ifdef VERSION_EU
    u8 *arr;
#endif

    if (!seqChannel->enabled) {
        return;
    }

    if (seqChannel->stopScript) {
        for (i = 0; i < LAYERS_MAX; i++) {
            if (seqChannel->layers[i] != NULL) {
                seq_channel_layer_process_script(seqChannel->layers[i]);
            }
        }
        return;
    }

    seqPlayer = seqChannel->seqPlayer;
    if (seqPlayer->muted && (seqChannel->muteBehavior & MUTE_BEHAVIOR_STOP_SCRIPT) != 0) {
        return;
    }

    if (seqChannel->delay != 0) {
        seqChannel->delay--;
    }

    state = &seqChannel->scriptState;
    if (seqChannel->delay == 0) {
        for (;;) {
            cmd = m64_read_u8(state);
#ifndef VERSION_EU
            if (cmd == 0xff) // chan_end
            {
                // This fixes a reordering in 'case 0x90', somehow
                sp5A = state->depth;
                if (sp5A == 0) {
                    sequence_channel_disable(seqChannel);
                    break;
                }
                state->depth--, state->pc = state->stack[state->depth];
            }
            if (cmd == 0xfe) // chan_delay1
            {
                break;
            }
            if (cmd == 0xfd) // chan_delay
            {
                seqChannel->delay = m64_read_compressed_u16(state);
                break;
            }
            if (cmd == 0xf3) // chan_hang
            {
                seqChannel->stopScript = TRUE;
                break;
            }
#endif

            // (new_var = cmd fixes order of s1/s2, but causes a reordering
            // towards the bottom of the function)
            if (cmd > 0xc0) {
                switch (cmd) {
                    case 0xff: // chan_end
#ifdef VERSION_EU
                        if (state->depth == 0) {
                            sequence_channel_disable(seqChannel);
                            goto out;
                        } else {
                            state->pc = state->stack[--state->depth];
                        }
#endif
                        break;

#ifdef VERSION_EU
                    case 0xfe: // chan_delay1
                        goto out;

                    case 0xfd: // chan_delay
                        seqChannel->delay = m64_read_compressed_u16(state);
                        goto out;

                    case 0xea:
                        seqChannel->stopScript = TRUE;
                        goto out;
#endif
                    case 0xfc: // chan_call
                        sp5A = m64_read_s16(state);
#ifdef VERSION_EU
                        state->stack[state->depth++] = state->pc;
#else
                        state->depth++, state->stack[state->depth - 1] = state->pc;
#endif
                        state->pc = seqPlayer->seqData + sp5A;
                        break;

                    case 0xf8: // chan_loop; loop start, N iterations (or 256 if N = 0)
                        state->remLoopIters[state->depth] = m64_read_u8(state);
#ifdef VERSION_EU
                        state->stack[state->depth++] = state->pc;
#else
                        state->depth++, state->stack[state->depth - 1] = state->pc;
#endif
                        break;

                    case 0xf7: // chan_loopend
                        state->remLoopIters[state->depth - 1]--;
                        if (state->remLoopIters[state->depth - 1] != 0) {
                            state->pc = state->stack[state->depth - 1];
                        } else {
                            state->depth--;
                        }
                        break;

                    case 0xf6: // chan_break; break loop, if combined with jump
                        state->depth--;
                        break;

                    case 0xfb: // chan_jump
                    case 0xfa: // chan_beqz
                    case 0xf9: // chan_bltz
                    case 0xf5: // chan_bgez
                        sp5A = m64_read_s16(state);
                        if (cmd == 0xfa && value != 0)
                            break;
                        if (cmd == 0xf9 && value >= 0)
                            break;
                        if (cmd == 0xf5 && value < 0)
                            break;
                        state->pc = seqPlayer->seqData + sp5A;
                        break;

#ifdef VERSION_EU
                    case 0xf4:
                    case 0xf3:
                    case 0xf2:
                        tempSigned = m64_read_u8(state);
                        if (cmd == 0xf3 && value != 0)
                            break;
                        if (cmd == 0xf2 && value >= 0)
                            break;
                        state->pc += tempSigned;
                        break;
#endif

#ifdef VERSION_EU
                    case 0xf1: // chan_reservenotes
#else
                    case 0xf2: // chan_reservenotes
#endif
                        // seqChannel->notePool should live in a saved register
                        note_pool_clear(&seqChannel->notePool);
                        temp = m64_read_u8(state);
                        note_pool_fill(&seqChannel->notePool, temp);
                        break;

#ifdef VERSION_EU
                    case 0xf0: // chan_unreservenotes
#else
                    case 0xf1: // chan_unreservenotes
#endif
                        note_pool_clear(&seqChannel->notePool);
                        break;

                    case 0xc2: // chan_setdyntable
                        sp5A = m64_read_s16(state);
                        seqChannel->dynTable = (void *) (seqPlayer->seqData + sp5A);
                        break;

                    case 0xc5: // chan_dynsetdyntable
                        if (value != -1) {
                            sp5A = (*seqChannel->dynTable)[value][1]
                                   + ((*seqChannel->dynTable)[value][0] << 8);
                            seqChannel->dynTable = (void *) (seqPlayer->seqData + sp5A);
                        }
                        break;

#ifdef VERSION_EU
                    case 0xeb:
                        temp = m64_read_u8(state);
                        // Switch to the temp's (0-indexed) bank in this sequence's
                        // bank set. Note that in the binary format (not in the JSON!)
                        // the banks are listed backwards, so we counts from the back.
                        // (gAlBankSets[offset] is number of banks)
                        offset = ((u16 *) gAlBankSets)[seqPlayer->seqId];
                        temp = gAlBankSets[offset + gAlBankSets[offset] - temp];
                        // temp should be in a saved register across this call
                        if (get_bank_or_seq(&gBankLoadedPool, 2, temp) != NULL) {
                            seqChannel->bankId = temp;
                        }
                        // fallthrough
#endif

                    case 0xc1: // chan_setinstr ("set program"?)
                        set_instrument(seqChannel, m64_read_u8(state));
                        break;

                    case 0xc3: // chan_largenotesoff
                        seqChannel->largeNotes = FALSE;
                        break;

                    case 0xc4: // chan_largenoteson
                        seqChannel->largeNotes = TRUE;
                        break;

                    case 0xdf: // chan_setvol
                        sequence_channel_set_volume(seqChannel, m64_read_u8(state));
#ifdef VERSION_EU
                        seqChannel->changes.as_bitfields.volume = TRUE;
#endif
                        break;

                    case 0xe0: // chan_setvolscale
                        seqChannel->volumeScale = FLOAT_CAST(m64_read_u8(state)) / US_FLOAT(128.0);
#ifdef VERSION_EU
                        seqChannel->changes.as_bitfields.volume = TRUE;
#endif
                        break;

                    case 0xde: // chan_freqscale; pitch bend using raw frequency multiplier N/2^15 (N is u16)
                        sp5A = m64_read_s16(state);
#ifdef VERSION_EU
                        seqChannel->changes.as_bitfields.freqScale = TRUE;
#endif
                        seqChannel->freqScale = FLOAT_CAST(sp5A) / US_FLOAT(32768.0);
                        break;

                    case 0xd3: // chan_pitchbend; pitch bend by <= 1 octave in either direction (-127..127)
                        // (m64_read_u8(state) is really s8 here)
                        temp = m64_read_u8(state) + 127;
                        seqChannel->freqScale = gPitchBendFrequencyScale[temp];
#ifdef VERSION_EU
                        seqChannel->changes.as_bitfields.freqScale = TRUE;
#endif
                        break;

                    case 0xdd: // chan_setpan
#ifdef VERSION_EU
                        seqChannel->newPan = m64_read_u8(state);
                        seqChannel->changes.as_bitfields.pan = TRUE;
#else
                        seqChannel->pan = FLOAT_CAST(m64_read_u8(state)) / US_FLOAT(128.0);
#endif
                        break;

                    case 0xdc: // chan_setpanmix; set proportion of pan to come from channel (0..128)
#ifdef VERSION_EU
                        seqChannel->panChannelWeight = m64_read_u8(state);
                        seqChannel->changes.as_bitfields.pan = TRUE;
#else
                        seqChannel->panChannelWeight = FLOAT_CAST(m64_read_u8(state)) / US_FLOAT(128.0);
#endif
                        break;

                    case 0xdb: // chan_transpose; set transposition in semitones
                        tempSigned = *state->pc;
                        state->pc++;
                        seqChannel->transposition = tempSigned;
                        break;

                    case 0xda: // chan_setenvelope
                        sp5A = m64_read_s16(state);
                        seqChannel->adsr.envelope = (struct AdsrEnvelope *) (seqPlayer->seqData + sp5A);
                        break;

                    case 0xd9: // chan_setdecayrelease
                        seqChannel->adsr.releaseRate = m64_read_u8(state);
                        break;

                    case 0xd8: // chan_setvibratoextent
                        seqChannel->vibratoExtentTarget = m64_read_u8(state) * 8;
                        seqChannel->vibratoExtentStart = 0;
                        seqChannel->vibratoExtentChangeDelay = 0;
                        break;

                    case 0xd7: // chan_setvibratorate
                        seqChannel->vibratoRateStart = seqChannel->vibratoRateTarget =
                            m64_read_u8(state) * 32;
                        seqChannel->vibratoRateChangeDelay = 0;
                        break;

                    case 0xe2: // chan_setvibratoextentlinear
                        seqChannel->vibratoExtentStart = m64_read_u8(state) * 8;
                        seqChannel->vibratoExtentTarget = m64_read_u8(state) * 8;
                        seqChannel->vibratoExtentChangeDelay = m64_read_u8(state) * 16;
                        break;

                    case 0xe1: // chan_setvibratoratelinear
                        seqChannel->vibratoRateStart = m64_read_u8(state) * 32;
                        seqChannel->vibratoRateTarget = m64_read_u8(state) * 32;
                        seqChannel->vibratoRateChangeDelay = m64_read_u8(state) * 16;
                        break;

                    case 0xe3: // chan_setvibratodelay
                        seqChannel->vibratoDelay = m64_read_u8(state) * 16;
                        break;

#ifndef VERSION_EU
                    case 0xd6: // chan_setupdatesperframe_unimplemented
                        temp = m64_read_u8(state);
                        if (temp == 0) {
                            temp = gAudioUpdatesPerFrame;
                        }
                        break;
#endif

                    case 0xd4: // chan_setreverb
                        seqChannel->reverb = m64_read_u8(state);
                        break;

                    case 0xc6: // chan_setbank; switch bank within set
                        {
                        u8 temp = m64_read_u8(state);
                        // Switch to the temp's (0-indexed) bank in this sequence's
                        // bank set. Note that in the binary format (not in the JSON!)
                        // the banks are listed backwards, so we counts from the back.
                        // (gAlBankSets[offset] is number of banks)
                        offset = ((u16 *) gAlBankSets)[seqPlayer->seqId];
                        temp = gAlBankSets[offset + gAlBankSets[offset] - temp];
                        // temp should be in a saved register across this call
                        if (get_bank_or_seq(&gBankLoadedPool, 2, temp) != NULL) {
                            seqChannel->bankId = temp;
                        }
                        }
                        break;

                    case 0xc7: // chan_writeseq; write to sequence data (!)
                        // sp38 doesn't go on the stack
                        {
                        u8 sp38;
                        u8 temp;
                        sp38 = value;
                        temp = m64_read_u8(state);
                        seqPlayer->seqData[(u16)m64_read_s16(state)] = sp38 + temp;
                        }
                        break;

                    case 0xc8: // chan_subtract
                    case 0xc9: // chan_bitand
                    case 0xcc: // chan_setval
                        temp = m64_read_u8(state);
                        if (cmd == 0xc8) {
                            value -= temp;
                        } else if (cmd == 0xcc) {
                            value = temp;
                        } else {
                            value &= temp;
                        }
                        break;

                    case 0xca: // chan_setmutebhv
                        seqChannel->muteBehavior = m64_read_u8(state);
                        break;

                    case 0xcb: // chan_readseq
                        sp5A = m64_read_s16(state);
                        value = seqPlayer->seqData[sp5A + value];
                        break;

                    case 0xd0: // chan_stereoheadseteffects
                        seqChannel->stereoHeadsetEffects = m64_read_u8(state);
                        break;

                    case 0xd1: // chan_setnoteallocationpolicy
                        seqChannel->noteAllocPolicy = m64_read_u8(state);
                        break;

                    case 0xd2: // chan_setsustain
#ifdef VERSION_EU
                        seqChannel->adsr.sustain = m64_read_u8(state);
#else
                        seqChannel->adsr.sustain = m64_read_u8(state) << 8;
#endif
                        break;
#ifdef VERSION_EU
                    case 0xe5:
                        seqChannel->reverbIndex = m64_read_u8(state);
                        break;
#endif
                    case 0xe4: // chan_dyncall
                        if (value != -1) {
                            u8(*thingy)[2] = *seqChannel->dynTable;
#ifdef VERSION_EU
                            state->stack[state->depth++] = state->pc;
#else
                            state->depth++, state->stack[state->depth - 1] = state->pc;
#endif
                            sp5A = thingy[value][1] + (thingy[value][0] << 8);
                            state->pc = seqPlayer->seqData + sp5A;
                        }
                        break;

#ifdef VERSION_EU
                    case 0xe6:
                        seqChannel->bookOffset = m64_read_u8(state);
                        break;

                    case 0xe7:
                        sp5A = m64_read_s16(state);
                        arr = seqPlayer->seqData + sp5A;
                        seqChannel->muteBehavior = *arr++;
                        seqChannel->noteAllocPolicy = *arr++;
                        seqChannel->notePriority = *arr++;
                        seqChannel->transposition = (s8) *arr++;
                        seqChannel->newPan = *arr++;
                        seqChannel->panChannelWeight = *arr++;
                        seqChannel->reverb = *arr++;
                        seqChannel->reverbIndex = *arr++; // reverb index?
                        seqChannel->changes.as_bitfields.pan = TRUE;
                        break;

                    case 0xe8:
                        seqChannel->muteBehavior = m64_read_u8(state);
                        seqChannel->noteAllocPolicy = m64_read_u8(state);
                        seqChannel->notePriority = m64_read_u8(state);
                        seqChannel->transposition = (s8) m64_read_u8(state);
                        seqChannel->newPan = m64_read_u8(state);
                        seqChannel->panChannelWeight = m64_read_u8(state);
                        seqChannel->reverb = m64_read_u8(state);
                        seqChannel->reverbIndex = m64_read_u8(state);
                        seqChannel->changes.as_bitfields.pan = TRUE;
                        break;

                    case 0xec:
                        seqChannel->vibratoExtentTarget = 0;
                        seqChannel->vibratoExtentStart = 0;
                        seqChannel->vibratoExtentChangeDelay = 0;
                        seqChannel->vibratoRateTarget = 0;
                        seqChannel->vibratoRateStart = 0;
                        seqChannel->vibratoRateChangeDelay = 0;
                        seqChannel->freqScale = 1.0f;
                        break;

                    case 0xe9:
                        seqChannel->notePriority = m64_read_u8(state);
                        break;
#endif
                }
            } else {
                // loBits is recomputed a lot
                loBits = cmd & 0xf;
                // #define loBits (cmd & 0xf)
                switch (cmd & 0xf0) {
                    case 0x00: // chan_testlayerfinished
                        if (seqChannel->layers[loBits] != NULL) {
                            value = seqChannel->layers[loBits]->finished;
                        }
#ifdef VERSION_EU
                        else {
                            value = -1;
                        }
#endif
                        break;

                    case 0x70: // chan_iowriteval; write data back to audio lib
                        seqChannel->soundScriptIO[loBits] = value;
                        break;

                    case 0x80: // chan_ioreadval; read data from audio lib
                        value = seqChannel->soundScriptIO[loBits];
                        if (loBits < 4) {
                            seqChannel->soundScriptIO[loBits] = -1;
                        }
                        break;

                    case 0x50: // chan_ioreadvalsub; subtract with read data from audio lib
                        value -= seqChannel->soundScriptIO[loBits];
                        break;

#ifdef VERSION_EU
                    case 0x60:
                        seqChannel->delay = loBits;
                        goto out;
#endif

                    case 0x90: // chan_setlayer
                        sp5A = m64_read_s16(state);
                        if (seq_channel_set_layer(seqChannel, loBits) == 0) {
                            seqChannel->layers[loBits]->scriptState.pc = seqPlayer->seqData + sp5A;
                        }
                        break;

                    case 0xa0: // chan_freelayer
                        seq_channel_layer_free(seqChannel, loBits);
                        break;

                    case 0xb0: // chan_dynsetlayer
                        if (value != -1 && seq_channel_set_layer(seqChannel, loBits) != -1) {
                            sp5A = ((*seqChannel->dynTable)[value][0] << 8)
                                   + (*seqChannel->dynTable)[value][1];
                            seqChannel->layers[loBits]->scriptState.pc = seqPlayer->seqData + sp5A;
                        }
                        break;

#ifndef VERSION_EU
                    case 0x60: // chan_setnotepriority (arg must be >= 2)
                        seqChannel->notePriority = loBits;
                        break;
#endif

                    case 0x10: // chan_startchannel
                        sp5A = m64_read_s16(state);
                        sequence_channel_enable(seqPlayer, loBits, seqPlayer->seqData + sp5A);
                        break;

                    case 0x20: // chan_disablechannel
                        sequence_channel_disable(seqPlayer->channels[loBits]);
                        break;

                    case 0x30: // chan_iowriteval2; write data back to audio lib for another channel
                        seqPlayer->channels[loBits]->soundScriptIO[m64_read_u8(state)] = value;
                        break;

                    case 0x40: // chan_ioreadval2; read data from audio lib from another channel
                        value = seqPlayer->channels[loBits]->soundScriptIO[m64_read_u8(state)];
                        break;
                }
            }
        }
    }
#ifdef VERSION_EU
    out:
#endif

    for (i = 0; i < LAYERS_MAX; i++) {
        if (seqChannel->layers[i] != 0) {
            seq_channel_layer_process_script(seqChannel->layers[i]);
        }
    }
#undef loBits
}

void sequence_player_process_sequence(struct SequencePlayer *seqPlayer) {
    u8 cmd;
    u8 loBits;
    u8 temp;
    s32 value;
    s32 i;
    u16 u16v;
    u8 *tempPtr;
    struct M64ScriptState *state;
#ifdef VERSION_EU
    s32 temp32;
#endif

    if (seqPlayer->enabled == FALSE) {
        return;
    }

    if (seqPlayer->bankDmaInProgress ) {
#ifdef VERSION_EU
        if (osRecvMesg(&seqPlayer->bankDmaMesgQueue, NULL, 0) == -1) {
            return;
        }
        if (seqPlayer->bankDmaRemaining == 0) {
            seqPlayer->bankDmaInProgress = FALSE;
            patch_audio_bank((struct AudioBank *)(gCtlEntries[seqPlayer->loadingBankId].instruments - 1),
                             gAlTbl->seqArray[seqPlayer->loadingBankId].offset,
                             gCtlEntries[seqPlayer->loadingBankId].numInstruments,
                             gCtlEntries[seqPlayer->loadingBankId].numDrums);
            gCtlEntries[seqPlayer->loadingBankId].drums =
                ((struct AudioBank *)(gCtlEntries[seqPlayer->loadingBankId].instruments - 1))->drums;
            gBankLoadStatus[seqPlayer->loadingBankId] = SOUND_LOAD_STATUS_COMPLETE;
        } else {
            audio_dma_partial_copy_async(&seqPlayer->bankDmaCurrDevAddr, &seqPlayer->bankDmaCurrMemAddr,
                                         &seqPlayer->bankDmaRemaining, &seqPlayer->bankDmaMesgQueue,
                                         &seqPlayer->bankDmaIoMesg);
        }
#else
        if (seqPlayer->bankDmaMesg == NULL) {
            return;
        }
        if (seqPlayer->bankDmaRemaining == 0) {
            seqPlayer->bankDmaInProgress = FALSE;
            patch_audio_bank(seqPlayer->loadingBank, gAlTbl->seqArray[seqPlayer->loadingBankId].offset,
                          seqPlayer->loadingBankNumInstruments, seqPlayer->loadingBankNumDrums);
            gCtlEntries[seqPlayer->loadingBankId].numInstruments = seqPlayer->loadingBankNumInstruments;
            gCtlEntries[seqPlayer->loadingBankId].numDrums = seqPlayer->loadingBankNumDrums;
            gCtlEntries[seqPlayer->loadingBankId].instruments = seqPlayer->loadingBank->instruments;
            gCtlEntries[seqPlayer->loadingBankId].drums = seqPlayer->loadingBank->drums;
            gBankLoadStatus[seqPlayer->loadingBankId] = SOUND_LOAD_STATUS_COMPLETE;
        } else {
            osCreateMesgQueue(&seqPlayer->bankDmaMesgQueue, &seqPlayer->bankDmaMesg, 1);
            seqPlayer->bankDmaMesg = NULL;
            audio_dma_partial_copy_async(&seqPlayer->bankDmaCurrDevAddr, &seqPlayer->bankDmaCurrMemAddr,
                                         &seqPlayer->bankDmaRemaining, &seqPlayer->bankDmaMesgQueue,
                                         &seqPlayer->bankDmaIoMesg);
        }
#endif
        return;
    }

    if (seqPlayer->seqDmaInProgress ) {
#ifdef VERSION_EU
        if (osRecvMesg(&seqPlayer->seqDmaMesgQueue, NULL, 0) == -1) {
            return;
        }
#ifndef AVOID_UB
        if (temp) {
        }
#endif
#else
        if (seqPlayer->seqDmaMesg == NULL) {
            return;
        }
#endif
        seqPlayer->seqDmaInProgress = FALSE;
        gSeqLoadStatus[seqPlayer->seqId] = SOUND_LOAD_STATUS_COMPLETE;
    }

    // If discarded, bail out.
    if (IS_SEQ_LOAD_COMPLETE(seqPlayer->seqId) == FALSE
        || IS_BANK_LOAD_COMPLETE(seqPlayer->defaultBank[0]) == FALSE) {
        sequence_player_disable(seqPlayer);
        return;
    }

    // Remove possible SOUND_LOAD_STATUS_DISCARDABLE marks.
    gSeqLoadStatus[seqPlayer->seqId] = SOUND_LOAD_STATUS_COMPLETE;
    gBankLoadStatus[seqPlayer->defaultBank[0]] = SOUND_LOAD_STATUS_COMPLETE;

    if (seqPlayer->muted && (seqPlayer->muteBehavior & MUTE_BEHAVIOR_STOP_SCRIPT) != 0) {
        return;
    }

    // Check if we surpass the number of ticks needed for a tatum, else stop.
    seqPlayer->tempoAcc += seqPlayer->tempo;
    if (seqPlayer->tempoAcc < gTempoInternalToExternal) {
        return;
    }
    seqPlayer->tempoAcc -= (u16) gTempoInternalToExternal;

    state = &seqPlayer->scriptState;
    if (seqPlayer->delay > 1) {
#ifndef AVOID_UB
        if (temp) {
        }
#endif
        seqPlayer->delay--;
    } else {
#ifdef VERSION_EU
        seqPlayer->recalculateVolume = 1;
#endif
        for (;;) {
            cmd = m64_read_u8(state);
            if (cmd == 0xff) // seq_end
            {
                if (state->depth == 0) {
                    sequence_player_disable(seqPlayer);
                    break;
                }
#ifdef VERSION_EU
                state->pc = state->stack[--state->depth];
#else
                state->depth--, state->pc = state->stack[state->depth];
#endif
            }

            if (cmd == 0xfd) // seq_delay
            {
                seqPlayer->delay = m64_read_compressed_u16(state);
                break;
            }

            if (cmd == 0xfe) // seq_delay1
            {
                seqPlayer->delay = 1;
                break;
            }

            if (cmd >= 0xc0) {
                switch (cmd) {
                    case 0xff: // seq_end
                        break;

                    case 0xfc: // seq_call
                        u16v = m64_read_s16(state);
#ifdef VERSION_EU
                        state->stack[state->depth++] = state->pc;
#else
                        state->depth++, state->stack[state->depth - 1] = state->pc;
#endif
                        state->pc = seqPlayer->seqData + u16v;
                        break;

                    case 0xf8: // seq_loop; loop start, N iterations (or 256 if N = 0)
                        state->remLoopIters[state->depth] = m64_read_u8(state);
#ifdef VERSION_EU
                        state->stack[state->depth++] = state->pc;
#else
                        state->depth++, state->stack[state->depth - 1] = state->pc;
#endif
                        break;

                    case 0xf7: // seq_loopend
                        state->remLoopIters[state->depth - 1]--;
                        if (state->remLoopIters[state->depth - 1] != 0) {
                            state->pc = state->stack[state->depth - 1];
                        } else {
                            state->depth--;
                        }
                        break;

                    case 0xfb: // seq_jump
                    case 0xfa: // seq_beqz; jump if == 0
                    case 0xf9: // seq_bltz; jump if < 0
                    case 0xf5: // seq_bgez; jump if >= 0
                        u16v = m64_read_s16(state);
                        if (cmd == 0xfa && value != 0) {
                            break;
                        }
                        if (cmd == 0xf9 && value >= 0) {
                            break;
                        }
                        if (cmd == 0xf5 && value < 0) {
                            break;
                        }
                        state->pc = seqPlayer->seqData + u16v;
                        break;

#ifdef VERSION_EU
                    case 0xf4:
                    case 0xf3:
                    case 0xf2:
                        temp = m64_read_u8(state);
                        if (cmd == 0xf3 && value != 0) {
                            break;
                        }
                        if (cmd == 0xf2 && value >= 0) {
                            break;
                        }
                        state->pc += (s8)temp;
                        break;
#endif

#ifdef VERSION_EU
                    case 0xf1: // seq_reservenotes
#else
                    case 0xf2: // seq_reservenotes
#endif
                        note_pool_clear(&seqPlayer->notePool);
                        note_pool_fill(&seqPlayer->notePool, m64_read_u8(state));
                        break;

#ifdef VERSION_EU
                    case 0xf0: // seq_unreservenotes
#else
                    case 0xf1: // seq_unreservenotes
#endif
                        note_pool_clear(&seqPlayer->notePool);
                        break;

                    case 0xdf: // seq_transpose; set transposition in semitones
                        seqPlayer->transposition = 0;
                        // fallthrough

                    case 0xde: // seq_transposerel; add transposition
                        seqPlayer->transposition += (s8) m64_read_u8(state);
                        break;

                    case 0xdd: // seq_settempo (bpm)
                    case 0xdc: // seq_addtempo (bpm)
                        temp = m64_read_u8(state);
                        if (cmd == 0xdd) {
                            seqPlayer->tempo = temp * TEMPO_SCALE;
                        } else {
                            seqPlayer->tempo += (s8) temp * TEMPO_SCALE;
                        }

                        if (seqPlayer->tempo > gTempoInternalToExternal) {
                            seqPlayer->tempo = gTempoInternalToExternal;
                        }

                        if ((s16) seqPlayer->tempo <= 0) {
                            seqPlayer->tempo = 1;
                        }
                        break;

#ifdef VERSION_EU
                    case 0xda:
                        temp = m64_read_u8(state);
                        u16v = m64_read_s16(state);
                        switch (temp) {
                            case SEQUENCE_PLAYER_STATE_0:
                            case SEQUENCE_PLAYER_STATE_FADE_OUT:
                                if (seqPlayer->state != SEQUENCE_PLAYER_STATE_2) {
                                    seqPlayer->fadeTimerUnkEu = u16v;
                                    seqPlayer->state = temp;
                                }
                                break;
                            case SEQUENCE_PLAYER_STATE_2:
                                seqPlayer->fadeTimer = u16v;
                                seqPlayer->state = temp;
                                seqPlayer->fadeVelocity = (0.0f - seqPlayer->fadeVolume) / (s32) (u16v & 0xFFFFu);
                                break;
                        }
                        break;

                    case 0xdb:
                        temp32 = m64_read_u8(state);
                        switch (seqPlayer->state) {
                            case SEQUENCE_PLAYER_STATE_2:
                                break;
                            case SEQUENCE_PLAYER_STATE_FADE_OUT:
                                seqPlayer->state = SEQUENCE_PLAYER_STATE_0;
                                seqPlayer->fadeVolume = 0.0f;
                                // fallthrough
                            case SEQUENCE_PLAYER_STATE_0:
                                seqPlayer->fadeTimer = seqPlayer->fadeTimerUnkEu;
                                if (seqPlayer->fadeTimerUnkEu != 0) {
                                    seqPlayer->fadeVelocity = (temp32 / 127.0f - seqPlayer->fadeVolume) / FLOAT_CAST(seqPlayer->fadeTimer);
                                } else {
                                    seqPlayer->fadeVolume = temp32 / 127.0f;
                                }
                        }
                        break;
#else
                    case 0xdb: // seq_setvol
                        temp = m64_read_u8(state);
                        switch (seqPlayer->state) {
                            case SEQUENCE_PLAYER_STATE_2:
                                if (seqPlayer->fadeTimer != 0) {
                                    f32 targetVolume = FLOAT_CAST(temp) / US_FLOAT(127.0);
                                    seqPlayer->fadeVelocity = (targetVolume - seqPlayer->fadeVolume)
                                                              / FLOAT_CAST(seqPlayer->fadeTimer);
                                    break;
                                }
                                // fallthrough
                            case SEQUENCE_PLAYER_STATE_0:
                                seqPlayer->fadeVolume = FLOAT_CAST(temp) / US_FLOAT(127.0);
                                break;
                            case SEQUENCE_PLAYER_STATE_FADE_OUT:
                            case SEQUENCE_PLAYER_STATE_4:
                                seqPlayer->volume = FLOAT_CAST(temp) / US_FLOAT(127.0);
                                break;
                        }
                        break;

                    case 0xda: // seq_changevol
                        temp = m64_read_u8(state);
                        seqPlayer->fadeVolume =
                            seqPlayer->fadeVolume + (f32) (s8)temp / US_FLOAT(127.0);
                        break;
#endif

#ifdef VERSION_EU
                    case 0xd9:
                        temp = m64_read_u8(state);
                        seqPlayer->fadeVolumeScale = (s8)temp / 127.0f;
                        break;
#endif

                    case 0xd7: // seq_initchannels
                        u16v = m64_read_s16(state);
                        sequence_player_init_channels(seqPlayer, u16v);
                        break;

                    case 0xd6: // seq_disablechannels
                        u16v = m64_read_s16(state);
                        sequence_player_disable_channels(seqPlayer, u16v);
                        break;

                    case 0xd5: // seq_setmutescale
                        temp = m64_read_u8(state);
                        seqPlayer->muteVolumeScale = (f32) (s8)temp / US_FLOAT(127.0);
                        break;

                    case 0xd4: // seq_mute
                        seqPlayer->muted = TRUE;
                        break;

                    case 0xd3: // seq_setmutebhv
                        seqPlayer->muteBehavior = m64_read_u8(state);
                        break;

                    case 0xd2: // seq_setshortnotevelocitytable
                    case 0xd1: // seq_setshortnotedurationtable
                        u16v = m64_read_s16(state);
                        tempPtr = seqPlayer->seqData + u16v;
                        if (cmd == 0xd2) {
                            seqPlayer->shortNoteVelocityTable = tempPtr;
                        } else {
                            seqPlayer->shortNoteDurationTable = tempPtr;
                        }
                        break;

                    case 0xd0: // seq_setnoteallocationpolicy
                        seqPlayer->noteAllocPolicy = m64_read_u8(state);
                        break;

                    case 0xcc: // seq_setval
                        value = m64_read_u8(state);
                        break;

                    case 0xc9: // seq_bitand
#ifdef VERSION_EU
                        value &= m64_read_u8(state);
#else
                        value = m64_read_u8(state) & value;
#endif
                        break;

                    case 0xc8: // seq_subtract
                        value = value - m64_read_u8(state);
                        break;
                }
            } else {
                loBits = cmd & 0xf;
                switch (cmd & 0xf0) {
                    case 0x00: // seq_testchdisabled
#ifdef VERSION_EU
                        value = seqPlayer->channels[loBits]->finished;
#else
                        if (IS_SEQUENCE_CHANNEL_VALID(seqPlayer->channels[loBits]) ) {
                            value = seqPlayer->channels[loBits]->finished;
                        }
#endif
                        break;
                    case 0x10:
                        break;
                    case 0x20:
                        break;
                    case 0x40:
                        break;
                    case 0x50: // seq_subvariation
#ifdef VERSION_EU
                        value -= seqPlayer->seqVariationEu[0];
#else
                        value -= seqPlayer->seqVariation;
#endif
                        break;
                    case 0x60:
                        break;
                    case 0x70: // seq_setvariation
#ifdef VERSION_EU
                        seqPlayer->seqVariationEu[0] = value;
#else
                        seqPlayer->seqVariation = value;
#endif
                        break;
                    case 0x80: // seq_getvariation
#ifdef VERSION_EU
                        value = seqPlayer->seqVariationEu[0];
#else
                        value = seqPlayer->seqVariation;
#endif
                        break;
                    case 0x90: // seq_startchannel
                        u16v = m64_read_s16(state);
                        sequence_channel_enable(seqPlayer, loBits, seqPlayer->seqData + u16v);
                        break;
                    case 0xa0:
                        break;
#ifndef VERSION_EU
                    case 0xd8: // (this makes no sense)
                        break;
                    case 0xd9:
                        break;
#endif
                }
            }
        }
    }

    for (i = 0; i < CHANNELS_MAX; i++) {
#ifdef VERSION_EU
        if (IS_SEQUENCE_CHANNEL_VALID(seqPlayer->channels[i]) ) {
            sequence_channel_process_script(seqPlayer->channels[i]);
        }
#else
        if (seqPlayer->channels[i] != &gSequenceChannelNone) {
            sequence_channel_process_script(seqPlayer->channels[i]);
        }
#endif
    }
}

// This runs 240 times per second.
__attribute__((always_inline))  inline void process_sequences() {
    s32 i;
    for (i = 0; i < SEQUENCE_PLAYERS; i++) {
        if (gSequencePlayers[i].enabled ) {
            sequence_player_process_sequence(gSequencePlayers + i);
            sequence_player_process_sound(gSequencePlayers + i);
        }
    }
    reclaim_notes();
    process_notes();
}
void sequence_player_process_sequence(struct SequencePlayer *seqPlayer);
void init_sequence_player(u32 player) {
    struct SequencePlayer *seqPlayer = &gSequencePlayers[player];
#ifdef VERSION_EU
    sequence_player_disable(seqPlayer);
#endif
    seqPlayer->muted = FALSE;
    seqPlayer->delay = 0;
#ifdef VERSION_EU
    seqPlayer->state = 1;
#else
    seqPlayer->state = SEQUENCE_PLAYER_STATE_0;
#endif
    seqPlayer->fadeTimer = 0;
#ifdef VERSION_EU
    seqPlayer->fadeTimerUnkEu = 0;
#endif
    seqPlayer->tempoAcc = 0;
    seqPlayer->tempo = 120 * TEMPO_SCALE; // 120 BPM
    seqPlayer->transposition = 0;
    seqPlayer->muteBehavior = MUTE_BEHAVIOR_STOP_SCRIPT | MUTE_BEHAVIOR_STOP_NOTES | MUTE_BEHAVIOR_SOFTEN;
    seqPlayer->noteAllocPolicy = 0;
    seqPlayer->shortNoteVelocityTable = gDefaultShortNoteVelocityTable;
    seqPlayer->shortNoteDurationTable = gDefaultShortNoteDurationTable;
    seqPlayer->fadeVolume = 1.0f;
#ifdef VERSION_EU
    seqPlayer->fadeVolumeScale = 1.0f;
#endif
    seqPlayer->fadeVelocity = 0.0f;
    seqPlayer->volume = 0.0f;
    seqPlayer->muteVolumeScale = 0.5f;
}

void init_sequence_players(void) {
    // Initialization function, called from audio_init
    s32 i, j;

    for (i = 0; i < ARRAY_COUNT(gSequenceChannels); i++) {
        gSequenceChannels[i].seqPlayer = NULL;
        gSequenceChannels[i].enabled = FALSE;
#ifndef VERSION_EU
    }

    for (i = 0; i < ARRAY_COUNT(gSequenceChannels); i++) {
#endif
        // @bug Size of wrong array. Zeroes out second half of gSequenceChannels[0],
        // all of gSequenceChannels[1..31], and part of gSequenceLayers[0].
        // However, this is only called at startup, so it's harmless.
#ifdef AVOID_UB
#define LAYERS_SIZE LAYERS_MAX
#else
#define LAYERS_SIZE ARRAY_COUNT(gSequenceLayers)
#endif
        for (j = 0; j < LAYERS_SIZE; j++) {
            gSequenceChannels[i].layers[j] = NULL;
        }
    }

    init_layer_freelist();

    for (i = 0; i < ARRAY_COUNT(gSequenceLayers); i++) {
        gSequenceLayers[i].seqChannel = NULL;
        gSequenceLayers[i].enabled = FALSE;
    }

    for (i = 0; i < SEQUENCE_PLAYERS; i++) {
        for (j = 0; j < CHANNELS_MAX; j++) {
            gSequencePlayers[i].channels[j] = &gSequenceChannelNone;
        }

#ifdef VERSION_EU
        gSequencePlayers[i].seqVariationEu[0] = -1;
#else
        gSequencePlayers[i].seqVariation = -1;
#endif
        gSequencePlayers[i].bankDmaInProgress = FALSE;
        gSequencePlayers[i].seqDmaInProgress = FALSE;
        init_note_lists(&gSequencePlayers[i].notePool);
        init_sequence_player(i);
    }
}


__attribute__((always_inline))  inline void prepare_reverb_ring_buffer(s32 chunkLen, u32 updateIndex) {
    struct ReverbRingBufferItem *item;
    s32 srcPos;
    s32 dstPos;
    s32 nSamples;
    s32 numSamplesAfterDownsampling;
    s32 excessiveSamples;
    if (gReverbDownsampleRate != 1) {
        if (gSynthesisReverb.framesLeftToIgnore == 0) {
            // Now that the RSP has finished, downsample the samples produced two frames ago by skipping
            // samples.
            item = &gSynthesisReverb.items[gSynthesisReverb.curFrame][updateIndex];

            // Touches both left and right since they are adjacent in memory
            osInvalDCache(item->toDownsampleLeft, DEFAULT_LEN_2CH);

            for (srcPos = 0, dstPos = 0; dstPos < item->lengths[0] / 2;
                 srcPos += gReverbDownsampleRate, dstPos++) {
                gSynthesisReverb.ringBuffer.left[dstPos + item->startPos] =
                    item->toDownsampleLeft[srcPos];
                gSynthesisReverb.ringBuffer.right[dstPos + item->startPos] =
                    item->toDownsampleRight[srcPos];
            }
            for (dstPos = 0; dstPos < item->lengths[1] / 2; srcPos += gReverbDownsampleRate, dstPos++) {
                gSynthesisReverb.ringBuffer.left[dstPos] = item->toDownsampleLeft[srcPos];
                gSynthesisReverb.ringBuffer.right[dstPos] = item->toDownsampleRight[srcPos];
            }
        }
    }
    item = &gSynthesisReverb.items[gSynthesisReverb.curFrame][updateIndex];

    numSamplesAfterDownsampling = chunkLen / gReverbDownsampleRate;
    if (((numSamplesAfterDownsampling + gSynthesisReverb.nextRingBufferPos) - gSynthesisReverb.bufSizePerChannel) < 0) {
        // There is space in the ring buffer before it wraps around
        item->lengths[0] = numSamplesAfterDownsampling * 2;
        item->lengths[1] = 0;
        item->startPos = (s32) gSynthesisReverb.nextRingBufferPos;
        gSynthesisReverb.nextRingBufferPos += numSamplesAfterDownsampling;
    } else {
        // Ring buffer wrapped around
        excessiveSamples =
            (numSamplesAfterDownsampling + gSynthesisReverb.nextRingBufferPos) - gSynthesisReverb.bufSizePerChannel;
        nSamples = numSamplesAfterDownsampling - excessiveSamples;
        item->lengths[0] = nSamples * 2;
        item->lengths[1] = excessiveSamples * 2;
        item->startPos = gSynthesisReverb.nextRingBufferPos;
        gSynthesisReverb.nextRingBufferPos = excessiveSamples;
    }
    // These fields are never read later
    item->numSamplesAfterDownsampling = numSamplesAfterDownsampling;
    item->chunkLen = chunkLen;
}


s32 get_volume_ramping(s32 sourceVol, s32 targetVol, s32 arg2) {
    // This roughly computes 2^16 * (targetVol / sourceVol) ^ (8 / arg2),
    // but with discretizations of targetVol, sourceVol and arg2.
    f32 ret;
    switch (arg2) {
        default:
            ret = gVolRampingLhs136[targetVol >> 8] * gVolRampingRhs136[sourceVol >> 8];
            break;
        case 128:
            ret = gVolRampingLhs128[targetVol >> 8] * gVolRampingRhs128[sourceVol >> 8];
            break;
        case 136:
            ret = gVolRampingLhs136[targetVol >> 8] * gVolRampingRhs136[sourceVol >> 8];
            break;
        case 144:
            ret = gVolRampingLhs144[targetVol >> 8] * gVolRampingRhs144[sourceVol >> 8];
            break;
    }
    return ret;
}



__attribute__((always_inline))   inline u64 *synthesis_do_one_audio_update(s16 *aiBuf, s32 bufLen, u64 *cmd, u32 updateIndex) {
    s32 ra;
    s32 t4;
    struct ReverbRingBufferItem *v1;
    s32 temp;

    v1 = &gSynthesisReverb.items[gSynthesisReverb.curFrame][updateIndex];

    if (gSynthesisReverb.useReverb == 0) {
        aClearBuffer(cmd++, DMEM_ADDR_LEFT_CH, DEFAULT_LEN_2CH);
        cmd = synthesis_process_notes(aiBuf, bufLen, cmd);
    } else {
        if (gReverbDownsampleRate == 1) {
            // Put the oldest samples in the ring buffer into the wet channels
            aSetLoadBufferPair(cmd++, 0, v1->startPos);
            if (v1->lengths[1] != 0) {
                // Ring buffer wrapped
                aSetLoadBufferPair(cmd++, v1->lengths[0], 0);
                temp = 0;
            }

            // Use the reverb sound as initial sound for this audio update
            aDMEMMove(cmd++, DMEM_ADDR_WET_LEFT_CH, DMEM_ADDR_LEFT_CH, DEFAULT_LEN_2CH);

            // (Hopefully) lower the volume of the wet channels. New reverb will later be mixed into
            // these channels.
            aSetBuffer(cmd++, 0, 0, 0, DEFAULT_LEN_2CH);
            // 0x8000 here is -100%
            aMix(cmd++, 0, /*gain*/ 0x8000 + gSynthesisReverb.reverbGain, /*in*/ DMEM_ADDR_WET_LEFT_CH,
                 /*out*/ DMEM_ADDR_WET_LEFT_CH);
        } else {
            // Same as above but upsample the previously downsampled samples used for reverb first
            temp = 0; //! jesus christ
            t4 = (v1->startPos & 7) * 2;
            ra = ALIGN(v1->lengths[0] + t4, 4);
            aSetLoadBufferPair(cmd++, 0, v1->startPos - t4 / 2);
            if (v1->lengths[1] != 0) {
                // Ring buffer wrapped
                aSetLoadBufferPair(cmd++, ra, 0);
                //! We need an empty statement (even an empty ';') here to make the function match (because IDO).
                //! However, copt removes extraneous statements and dead code. So we need to trick copt
                //! into thinking 'temp' could be undefined, and luckily the compiler optimizes out the
                //! useless assignment.
                ra = ra + temp;
            }
            aSetBuffer(cmd++, 0, t4 + DMEM_ADDR_WET_LEFT_CH, DMEM_ADDR_LEFT_CH, bufLen << 1);
            aResample(cmd++, gSynthesisReverb.resampleFlags, (u16) gSynthesisReverb.resampleRate, VIRTUAL_TO_PHYSICAL2(gSynthesisReverb.resampleStateLeft));
            aSetBuffer(cmd++, 0, t4 + DMEM_ADDR_WET_RIGHT_CH, DMEM_ADDR_RIGHT_CH, bufLen << 1);
            aResample(cmd++, gSynthesisReverb.resampleFlags, (u16) gSynthesisReverb.resampleRate, VIRTUAL_TO_PHYSICAL2(gSynthesisReverb.resampleStateRight));
            aSetBuffer(cmd++, 0, 0, 0, DEFAULT_LEN_2CH);
            aMix(cmd++, 0, /*gain*/ 0x8000 + gSynthesisReverb.reverbGain, /*in*/ DMEM_ADDR_LEFT_CH, /*out*/ DMEM_ADDR_LEFT_CH);
            aDMEMMove(cmd++, DMEM_ADDR_LEFT_CH, DMEM_ADDR_WET_LEFT_CH, DEFAULT_LEN_2CH);
        }
        cmd = synthesis_process_notes(aiBuf, bufLen, cmd);
        if (gReverbDownsampleRate == 1) {
            aSetSaveBufferPair(cmd++, 0, v1->lengths[0], v1->startPos);
            if (v1->lengths[1] != 0) {
                // Ring buffer wrapped
                aSetSaveBufferPair(cmd++, v1->lengths[0], v1->lengths[1], 0);
            }
        } else {
            // Downsampling is done later by CPU when RSP is done, therefore we need to have double
            // buffering. Left and right buffers are adjacent in memory.
            aSetBuffer(cmd++, 0, 0, DMEM_ADDR_WET_LEFT_CH, DEFAULT_LEN_2CH);
            aSaveBuffer(cmd++, VIRTUAL_TO_PHYSICAL2(gSynthesisReverb.items[gSynthesisReverb.curFrame][updateIndex].toDownsampleLeft));
            gSynthesisReverb.resampleFlags = 0;
        }
    }
    return cmd;
}

u64 *synthesis_execute(u64 *cmdBuf, s32 *writtenCmds, s16 *aiBuf, s32 bufLen) {
    s32 chunkLen;
    register s32 i;
    u32 *aiBufPtr = (u32 *) aiBuf;
    u64 *cmd = cmdBuf + 1;
    s32 v0;

    aSegment(cmdBuf, 0, 0);

    for (i = gAudioUpdatesPerFrame; i > 0; i--) {
        if (i == 1) {
            // 'bufLen' will automatically be divisible by 8, no need to round
            chunkLen = bufLen;
        } else {
            v0 = bufLen / i;
            // chunkLen = v0 rounded to nearest multiple of 8
            chunkLen = v0 - (v0 & 7);

            if ((v0 & 7) >= 4) {
                chunkLen += 8;
            }
        }
        process_sequences();
        if (gSynthesisReverb.useReverb != 0) {
            prepare_reverb_ring_buffer(chunkLen, gAudioUpdatesPerFrame - i);
        }
        cmd = synthesis_do_one_audio_update((s16 *) aiBufPtr, chunkLen, cmd, gAudioUpdatesPerFrame - i);
        bufLen -= chunkLen;
        aiBufPtr += chunkLen;
    }
    if (gSynthesisReverb.framesLeftToIgnore != 0) {
        gSynthesisReverb.framesLeftToIgnore--;
    }
    gSynthesisReverb.curFrame ^= 1;
    *writtenCmds = cmd - cmdBuf;
    return cmd;
}

#ifdef VERSION_EU
// Processes just one note, not all
u64 *synthesis_process_note(struct Note *note, struct NoteSubEu *noteSubEu, struct NoteSynthesisState *synthesisState, UNUSED s16 *aiBuf, s32 bufLen, u64 *cmd) {
    UNUSED s32 pad0[3];
#else
u64 *synthesis_process_notes(s16 *aiBuf, s32 bufLen, u64 *cmd) {
    s32 noteIndex;                           // sp174
    struct Note *note;                       // s7
#endif
    struct AudioBankSample *audioBookSample; // sp164, sp138
    struct AdpcmLoop *loopInfo;              // sp160, sp134
    s16 *curLoadedBook = NULL;               // sp154, sp130
#ifndef VERSION_EU
    s32 resamplingRateFixedPoint;            // sp5c, sp11A
#endif
    s32 noteFinished;                        // 150 t2, sp124
    s32 restart;                             // 14c t3, sp120
    s32 flags;                               // sp148, sp11C
#ifdef VERSION_EU
    u16 resamplingRateFixedPoint;            // sp5c, sp11A
#endif
#ifdef VERSION_EU
    s32 sp130;  //sp128, sp104
    UNUSED u32 pad9;
#else
    s32 sp130;  //sp128, sp104
#endif
    s32 nAdpcmSamplesProcessed; // signed required for US
    s32 t0;
#ifdef VERSION_EU
    u8 *sampleAddr;                          // sp120, spF4
    s32 s6;
#else
    s32 s6;
    u8 *sampleAddr;                          // sp120, spF4
#endif

    // sp6c is a temporary!

#ifdef VERSION_EU
    s32 samplesLenAdjusted; // 108,      spEC
    // Might have been used to store (samplesLenFixedPoint >> 0x10), but doing so causes strange
    // behavior with the break near the end of the loop, causing US and JP to need a goto instead
    UNUSED s32 samplesLenInt;
    s32 endPos;             // sp110,    spE4
    s32 nSamplesToProcess;  // sp10c/a0, spE0
    s32 s2;
#else
    // Might have been used to store (samplesLenFixedPoint >> 0x10), but doing so causes strange
    // behavior with the break near the end of the loop, causing US and JP to need a goto instead
    s32 samplesLenAdjusted; // 108
    s32 s2;
    s32 endPos;             // sp110,    spE4
    s32 nSamplesToProcess;  // sp10c/a0, spE0
#endif

    s32 s0;
    s32 s3;
    s32 s5; //s4

    u32 samplesLenFixedPoint;    // v1_1
    s32 nSamplesInThisIteration; // v1_2
    u32 a3;
#ifndef VERSION_EU
    s32 t9;
#endif
    u8 *v0_2;
    s32 nParts;                 // spE8, spBC
    s32 curPart;                // spE4, spB8

#ifndef VERSION_EU
    f32 resamplingRate; // f12
#endif
    s32 temp;

#ifdef VERSION_EU
    s32 s5Aligned;
#endif
    s32 resampledTempLen;                    // spD8, spAC
    s32 noteSamplesDmemAddrBeforeResampling; // spD6, spAA


#ifndef VERSION_EU
    for (noteIndex = 0; noteIndex < gMaxSimultaneousNotes; noteIndex++) {
        note = &gNotes[noteIndex];
#ifdef VERSION_US
        //! This function requires note->enabled to be volatile, but it breaks other functions like note_enable.
        //! Casting to a struct with just the volatile bitfield works, but there may be a better way to match.
        if (((struct vNote *)note)->enabled && IS_BANK_LOAD_COMPLETE(note->bankId) == FALSE) {
#else
        if (IS_BANK_LOAD_COMPLETE(note->bankId) == FALSE) {
#endif
         //   gAudioErrorFlags = (note->bankId << 8) + noteIndex + 0x1000000;
        } else if (((struct vNote *)note)->enabled) {
#else
        if (note->noteSubEu.enabled == FALSE) {
            return cmd;
        } else {
#endif
            flags = 0;
#ifdef VERSION_EU
            tempBufLen = bufLen;
#endif

#ifdef VERSION_EU
            if (noteSubEu->needsInit ) {
#else
            if (note->needsInit ) {
#endif
                flags = A_INIT;
#ifndef VERSION_EU
                note->samplePosInt = 0;
                note->samplePosFrac = 0;
#else
                synthesisState->restart = FALSE;
                synthesisState->samplePosInt = 0;
                synthesisState->samplePosFrac = 0;
                synthesisState->curVolLeft = 1;
                synthesisState->curVolRight = 1;
                synthesisState->prevHeadsetPanRight = 0;
                synthesisState->prevHeadsetPanLeft = 0;
#endif
            }

#ifndef VERSION_EU
            if (note->frequency < US_FLOAT(2.0)) {
                nParts = 1;
                if (note->frequency > US_FLOAT(1.99996)) {
                    note->frequency = US_FLOAT(1.99996);
                }
                resamplingRate = note->frequency;
            } else {
                // If frequency is > 2.0, the processing must be split into two parts
                nParts = 2;
                if (note->frequency >= US_FLOAT(3.99993)) {
                    note->frequency = US_FLOAT(3.99993);
                }
                resamplingRate = note->frequency * US_FLOAT(.5);
            }

            resamplingRateFixedPoint = (resamplingRate * 32768.0f);
            samplesLenFixedPoint = note->samplePosFrac + (resamplingRateFixedPoint * bufLen) * 2;
            note->samplePosFrac = samplesLenFixedPoint & 0xFFFF; // 16-bit store, can't reuse
#else
            resamplingRateFixedPoint = noteSubEu->resamplingRateFixedPoint;
            nParts = noteSubEu->hasTwoAdpcmParts + 1;
            samplesLenFixedPoint = (resamplingRateFixedPoint * tempBufLen * 2) + synthesisState->samplePosFrac;
            synthesisState->samplePosFrac = samplesLenFixedPoint & 0xFFFF;
#endif

#ifdef VERSION_EU
            if (noteSubEu->isSyntheticWave) {
                cmd = load_wave_samples(cmd, noteSubEu, synthesisState, samplesLenFixedPoint >> 0x10);
                noteSamplesDmemAddrBeforeResampling = (synthesisState->samplePosInt * 2) + DMEM_ADDR_UNCOMPRESSED_NOTE;
                synthesisState->samplePosInt += samplesLenFixedPoint >> 0x10;
            }
#else
            if (note->sound == NULL) {
                // A wave synthesis note (not ADPCM)

                cmd = load_wave_samples(cmd, note, samplesLenFixedPoint >> 0x10);
                noteSamplesDmemAddrBeforeResampling = DMEM_ADDR_UNCOMPRESSED_NOTE + note->samplePosInt * 2;
                note->samplePosInt += (samplesLenFixedPoint >> 0x10);
                flags = 0;
            }
#endif
            else {
                // ADPCM note

#ifdef VERSION_EU
                audioBookSample = noteSubEu->sound.audioBankSound->sample;
#else
                audioBookSample = note->sound->sample;
#endif

                loopInfo = audioBookSample->loop;
                endPos = loopInfo->end;
                sampleAddr = audioBookSample->sampleAddr;
                resampledTempLen = 0;
                for (curPart = 0; curPart < nParts; curPart++) {
                    nAdpcmSamplesProcessed = 0; // s8
                    s5 = 0;                     // s4

                    if (nParts == 1) {
                        samplesLenAdjusted = samplesLenFixedPoint >> 0x10;
                    } else if ((samplesLenFixedPoint >> 0x10) & 1) {
                        samplesLenAdjusted = ((samplesLenFixedPoint >> 0x10) & ~1) + (curPart * 2);
                    }
                    else {
                        samplesLenAdjusted = (samplesLenFixedPoint >> 0x10);
                    }

                    if (curLoadedBook != audioBookSample->book->book) {
                        u32 nEntries; // v1
                        curLoadedBook = audioBookSample->book->book;
#ifdef VERSION_EU
                        nEntries = 16 * audioBookSample->book->order * audioBookSample->book->npredictors;
                        aLoadADPCM(cmd++, nEntries, VIRTUAL_TO_PHYSICAL2(curLoadedBook + noteSubEu->bookOffset));
#else
                        nEntries = audioBookSample->book->order * audioBookSample->book->npredictors;
                        aLoadADPCM(cmd++, nEntries * 16, VIRTUAL_TO_PHYSICAL2(curLoadedBook));
#endif
                    }

#ifdef VERSION_EU
                    if (noteSubEu->bookOffset) {
                        curLoadedBook = (s16 *) &euUnknownData_80301950; // what's this? never read
                    }
#endif

                    while (nAdpcmSamplesProcessed != samplesLenAdjusted) {
                        s32 samplesRemaining; // v1
                        s32 s0;

                        noteFinished = FALSE;
                        restart = FALSE;
                        nSamplesToProcess = samplesLenAdjusted - nAdpcmSamplesProcessed;
#ifdef VERSION_EU
                        s2 = synthesisState->samplePosInt & 0xf;
                        samplesRemaining = endPos - synthesisState->samplePosInt;
#else
                        s2 = note->samplePosInt & 0xf;
                        samplesRemaining = endPos - note->samplePosInt;
#endif

#ifdef VERSION_EU
                        if (s2 == 0 && synthesisState->restart == FALSE) {
                            s2 = 16;
                        }
#else
                        if (s2 == 0 && note->restart == FALSE) {
                            s2 = 16;
                        }
#endif
                        s6 = 16 - s2; // a1

                        if (nSamplesToProcess < samplesRemaining) {
                            t0 = (nSamplesToProcess - s6 + 0xf) / 16;
                            s0 = t0 * 16;
                            s3 = s6 + s0 - nSamplesToProcess;
                        } else {
#ifndef VERSION_EU
                            s0 = samplesRemaining + s2 - 0x10;
#else
                            s0 = samplesRemaining - s6;
#endif
                            s3 = 0;
                            if (s0 <= 0) {
                                s0 = 0;
                                s6 = samplesRemaining;
                            }
                            t0 = (s0 + 0xf) / 16;
                            if (loopInfo->count != 0) {
                                // Loop around and restart
                                restart = 1;
                            } else {
                                noteFinished = 1;
                            }
                        }

                        if (t0 != 0) {
#ifdef VERSION_EU
                            temp = (synthesisState->samplePosInt - s2 + 0x10) / 16;
                            if (audioBookSample->loaded == 0x81) {
                                v0_2 = sampleAddr + temp * 9;
                            } else {
                                v0_2 = dma_sample_data(
                                    (uintptr_t) (sampleAddr + temp * 9),
                                    t0 * 9, flags, &synthesisState->sampleDmaIndex);
                            }
#else
                            temp = (note->samplePosInt - s2 + 0x10) / 16;
                            v0_2 = dma_sample_data(
                                (uintptr_t) (sampleAddr + temp * 9),
                                t0 * 9, flags, &note->sampleDmaIndex);
#endif
                            a3 = (u32)((uintptr_t) v0_2 & 0xf);
                            aSetBuffer(cmd++, 0, DMEM_ADDR_COMPRESSED_ADPCM_DATA, 0, t0 * 9 + a3);
                            aLoadBuffer(cmd++, VIRTUAL_TO_PHYSICAL2(v0_2 - a3));
                        } else {
                            s0 = 0;
                            a3 = 0;
                        }

#ifdef VERSION_EU
                        if (synthesisState->restart != FALSE) {
                            aSetLoop(cmd++, VIRTUAL_TO_PHYSICAL2(audioBookSample->loop->state));
                            flags = A_LOOP; // = 2
                            synthesisState->restart = FALSE;
                        }
#else
                        if (note->restart != FALSE) {
                            aSetLoop(cmd++, VIRTUAL_TO_PHYSICAL2(audioBookSample->loop->state));
                            flags = A_LOOP; // = 2
                            note->restart = FALSE;
                        }
#endif

                        nSamplesInThisIteration = s0 + s6 - s3;
#ifdef VERSION_EU
                        if (nAdpcmSamplesProcessed == 0) {
                            aSetBuffer(cmd++, 0, DMEM_ADDR_COMPRESSED_ADPCM_DATA + a3,
                                       DMEM_ADDR_UNCOMPRESSED_NOTE, s0 * 2);
                            aADPCMdec(cmd++, flags,
                                      VIRTUAL_TO_PHYSICAL2(synthesisState->synthesisBuffers->adpcmdecState));
                            sp130 = s2 * 2;
                        } else {
                            s5Aligned = ALIGN(s5, 5);
                            aSetBuffer(cmd++, 0, DMEM_ADDR_COMPRESSED_ADPCM_DATA + a3,
                                       DMEM_ADDR_UNCOMPRESSED_NOTE + s5Aligned, s0 * 2);
                            aADPCMdec(cmd++, flags,
                                      VIRTUAL_TO_PHYSICAL2(synthesisState->synthesisBuffers->adpcmdecState));
                            aDMEMMove(cmd++, DMEM_ADDR_UNCOMPRESSED_NOTE + s5Aligned + (s2 * 2),
                                      DMEM_ADDR_UNCOMPRESSED_NOTE + s5, (nSamplesInThisIteration) * 2);
                        }
#else
                        if (nAdpcmSamplesProcessed == 0) {
                            aSetBuffer(cmd++, 0, DMEM_ADDR_COMPRESSED_ADPCM_DATA + a3, DMEM_ADDR_UNCOMPRESSED_NOTE, s0 * 2);
                            aADPCMdec(cmd++, flags, VIRTUAL_TO_PHYSICAL2(note->synthesisBuffers->adpcmdecState));
                            sp130 = s2 * 2;
                        } else {
                            aSetBuffer(cmd++, 0, DMEM_ADDR_COMPRESSED_ADPCM_DATA + a3, DMEM_ADDR_UNCOMPRESSED_NOTE + ALIGN(s5, 5), s0 * 2);
                            aADPCMdec(cmd++, flags, VIRTUAL_TO_PHYSICAL2(note->synthesisBuffers->adpcmdecState));
                            aDMEMMove(cmd++, DMEM_ADDR_UNCOMPRESSED_NOTE + ALIGN(s5, 5) + (s2 * 2), DMEM_ADDR_UNCOMPRESSED_NOTE + s5, (nSamplesInThisIteration) * 2);
                        }
#endif

                        nAdpcmSamplesProcessed += nSamplesInThisIteration;

                        switch (flags) {
                            case A_INIT: // = 1
                                sp130 = 0;
                                s5 = s0 * 2 + s5;
                                break;

                            case A_LOOP: // = 2
                                s5 = nSamplesInThisIteration * 2 + s5;
                                break;

                            default:
                                if (s5 != 0) {
                                    s5 = nSamplesInThisIteration * 2 + s5;
                                } else {
                                    s5 = (s2 + nSamplesInThisIteration) * 2;
                                }
                                break;
                        }
                        flags = 0;

                        if (noteFinished) {
                            aClearBuffer(cmd++, DMEM_ADDR_UNCOMPRESSED_NOTE + s5,
                                         (samplesLenAdjusted - nAdpcmSamplesProcessed) * 2);
#ifdef VERSION_EU
                            noteSubEu->finished = 1;
                            note->noteSubEu.finished = 1;
                            note->noteSubEu.enabled = 0;
#else
                            note->samplePosInt = 0;
                            note->finished = 1;
                            ((struct vNote *)note)->enabled = 0;
#endif
                            break;
                        }
#ifdef VERSION_EU
                        if (restart) {
                            synthesisState->restart = TRUE;
                            synthesisState->samplePosInt = loopInfo->start;
                        } else {
                            synthesisState->samplePosInt += nSamplesToProcess;
                        }
#else
                        if (restart) {
                            note->restart = TRUE;
                            note->samplePosInt = loopInfo->start;
                        } else {
                            note->samplePosInt += nSamplesToProcess;
                        }
#endif
                    }

                    switch (nParts) {
                        case 1:
                            noteSamplesDmemAddrBeforeResampling = DMEM_ADDR_UNCOMPRESSED_NOTE + sp130;
                            break;

                        case 2:
                            switch (curPart) {
                                case 0:
                                    aSetBuffer(cmd++, 0, DMEM_ADDR_UNCOMPRESSED_NOTE + sp130, DMEM_ADDR_ADPCM_RESAMPLED, samplesLenAdjusted + 4);
#ifdef VERSION_EU
                                    aResample(cmd++, A_INIT, 0xff60, VIRTUAL_TO_PHYSICAL2(synthesisState->synthesisBuffers->dummyResampleState));
#else
                                    aResample(cmd++, A_INIT, 0xff60, VIRTUAL_TO_PHYSICAL2(note->synthesisBuffers->dummyResampleState));
#endif
                                    resampledTempLen = samplesLenAdjusted + 4;
                                    noteSamplesDmemAddrBeforeResampling = DMEM_ADDR_ADPCM_RESAMPLED + 4;
#ifdef VERSION_EU
                                    if (noteSubEu->finished != FALSE) {
#else
                                    if (note->finished != FALSE) {
#endif
                                        aClearBuffer(cmd++, DMEM_ADDR_ADPCM_RESAMPLED + resampledTempLen, samplesLenAdjusted + 0x10);
                                    }
                                    break;

                                case 1:
                                    aSetBuffer(cmd++, 0, DMEM_ADDR_UNCOMPRESSED_NOTE + sp130,
                                               DMEM_ADDR_ADPCM_RESAMPLED2,
                                               samplesLenAdjusted + 8);
#ifdef VERSION_EU
                                    aResample(cmd++, A_INIT, 0xff60,
                                              VIRTUAL_TO_PHYSICAL2(
                                                  synthesisState->synthesisBuffers->dummyResampleState));
#else
                                    aResample(cmd++, A_INIT, 0xff60,
                                              VIRTUAL_TO_PHYSICAL2(
                                                  note->synthesisBuffers->dummyResampleState));
#endif
                                    aDMEMMove(cmd++, DMEM_ADDR_ADPCM_RESAMPLED2 + 4,
                                              DMEM_ADDR_ADPCM_RESAMPLED + resampledTempLen,
                                              samplesLenAdjusted + 4);
                                    break;
                            }
                    }

#ifdef VERSION_EU
                    if (noteSubEu->finished != FALSE) {
#else
                    if (note->finished != FALSE) {
#endif
                        break;
                    }
                }
            }

            flags = 0;

#ifdef VERSION_EU
            if (noteSubEu->needsInit ) {
                flags = A_INIT;
                noteSubEu->needsInit = FALSE;
            }

            cmd = final_resample(cmd, synthesisState, bufLen * 2, resamplingRateFixedPoint,
                                 noteSamplesDmemAddrBeforeResampling, flags);
#else
            if (note->needsInit ) {
                flags = A_INIT;
                note->needsInit = FALSE;
            }

            cmd = final_resample(cmd, note, bufLen * 2, resamplingRateFixedPoint,
                                 noteSamplesDmemAddrBeforeResampling, flags);
#endif

#ifndef VERSION_EU
            if (note->headsetPanRight != 0 || note->prevHeadsetPanRight != 0) {
                s0 = 1;
            } else if (note->headsetPanLeft != 0 || note->prevHeadsetPanLeft != 0) {
                s0 = 2;
#else
            if (noteSubEu->headsetPanRight != 0 || synthesisState->prevHeadsetPanRight != 0) {
                s0 = 1;
            } else if (noteSubEu->headsetPanLeft != 0 || synthesisState->prevHeadsetPanLeft != 0) {
                s0 = 2;
#endif
            } else {
                s0 = 0;
            }

#ifdef VERSION_EU
            cmd = process_envelope(cmd, noteSubEu, synthesisState, bufLen, 0, s0, flags);
#else
            cmd = process_envelope(cmd, note, bufLen, 0, s0);
#endif

#ifdef VERSION_EU
            if (noteSubEu->usesHeadsetPanEffects) {
                cmd = note_apply_headset_pan_effects(cmd, noteSubEu, synthesisState, bufLen * 2, flags, s0);
            }
#else
            if (note->usesHeadsetPanEffects) {
                cmd = note_apply_headset_pan_effects(cmd, note, bufLen * 2, flags, s0);
            }
#endif
        }
#ifndef VERSION_EU
    }

    t9 = bufLen * 2;
    aSetBuffer(cmd++, 0, 0, DMEM_ADDR_TEMP, t9);
    aInterleave(cmd++, DMEM_ADDR_LEFT_CH, DMEM_ADDR_RIGHT_CH);
    t9 *= 2;
    aSetBuffer(cmd++, 0, 0, DMEM_ADDR_TEMP, t9);
    aSaveBuffer(cmd++, VIRTUAL_TO_PHYSICAL2(aiBuf));
#endif

    return cmd;
}

#ifdef VERSION_EU
u64 *load_wave_samples(u64 *cmd, struct NoteSubEu *noteSubEu, struct NoteSynthesisState *synthesisState, s32 nSamplesToLoad) {
    s32 a3;
    s32 i;
    s32 repeats;
    aSetBuffer(cmd++, /*flags*/ 0, /*dmemin*/ DMEM_ADDR_UNCOMPRESSED_NOTE, /*dmemout*/ 0, /*count*/ 128);
    aLoadBuffer(cmd++, VIRTUAL_TO_PHYSICAL2(noteSubEu->sound.samples));
    synthesisState->samplePosInt &= 0x3f;
    a3 = 64 - synthesisState->samplePosInt;
    if (a3 < nSamplesToLoad) {
        repeats = (nSamplesToLoad - a3 + 63) / 64;
        for (i = 0; i < repeats; i++) {
            aDMEMMove(cmd++,
                      /*dmemin*/ DMEM_ADDR_UNCOMPRESSED_NOTE,
                      /*dmemout*/ DMEM_ADDR_UNCOMPRESSED_NOTE + (1 + i) * 128,
                      /*count*/ 128);
        }
    }
    return cmd;
}
#else
u64 *load_wave_samples(u64 *cmd, struct Note *note, s32 nSamplesToLoad) {
    s32 a3;
    s32 i;
    aSetBuffer(cmd++, /*flags*/ 0, /*dmemin*/ DMEM_ADDR_UNCOMPRESSED_NOTE, /*dmemout*/ 0,
               /*count*/ sizeof(note->synthesisBuffers->samples));
    aLoadBuffer(cmd++, VIRTUAL_TO_PHYSICAL2(note->synthesisBuffers->samples));
    note->samplePosInt &= (note->sampleCount - 1);
    a3 = 64 - note->samplePosInt;
    if (a3 < nSamplesToLoad) {
        for (i = 0; i <= (nSamplesToLoad - a3 + 63) / 64 - 1; i++) {
            aDMEMMove(cmd++, /*dmemin*/ DMEM_ADDR_UNCOMPRESSED_NOTE, /*dmemout*/ DMEM_ADDR_UNCOMPRESSED_NOTE + (1 + i) * sizeof(note->synthesisBuffers->samples), /*count*/ sizeof(note->synthesisBuffers->samples));
        }
    }
    return cmd;
}
#endif

#ifdef VERSION_EU
u64 *final_resample(u64 *cmd, struct NoteSynthesisState *synthesisState, s32 count, u16 pitch, u16 dmemIn, u32 flags) {
    aSetBuffer(cmd++, /*flags*/ 0, dmemIn, /*dmemout*/ 0, count);
    aResample(cmd++, flags, pitch, VIRTUAL_TO_PHYSICAL2(synthesisState->synthesisBuffers->finalResampleState));
    return cmd;
}
#else
u64 *final_resample(u64 *cmd, struct Note *note, s32 count, s32 pitch, s32 dmemIn, u32 flags) {
    aSetBuffer(cmd++, /*flags*/ 0, dmemIn, /*dmemout*/ 0, count);
    aResample(cmd++, flags, pitch, VIRTUAL_TO_PHYSICAL2(note->synthesisBuffers->finalResampleState));
    return cmd;
}
#endif

#ifndef VERSION_EU
u64 *process_envelope(u64 *cmd, struct Note *note, s32 nSamples, s32 inBuf, s32 headsetPanSettings) {
    struct VolumeChange vol;
    vol.sourceLeft = note->curVolLeft;
    vol.sourceRight = note->curVolRight;
    vol.targetLeft = note->targetVolLeft;
    vol.targetRight = note->targetVolRight;
    note->curVolLeft = vol.targetLeft;
    note->curVolRight = vol.targetRight;
    return process_envelope_inner(cmd, note, nSamples, inBuf, headsetPanSettings, &vol);
}

__attribute__((always_inline))  inline u64 *process_envelope_inner(u64 *cmd, struct Note *note, s32 nSamples, u32 inBuf,
                            s32 headsetPanSettings, struct VolumeChange *vol) {
    s32 mixerFlags;
    s32 rampLeft, rampRight;
#else
u64 *process_envelope(u64 *cmd, struct NoteSubEu *note, struct NoteSynthesisState *synthesisState, s32 nSamples, u16 inBuf, s32 headsetPanSettings, UNUSED u32 flags) {
    UNUSED u8 pad1[20];
    u16 sourceRight;
    u16 sourceLeft;
    UNUSED u8 pad2[4];
    u16 targetLeft;
    u16 targetRight;
    s32 mixerFlags;
    s32 rampLeft;
    s32 rampRight;

    sourceLeft = synthesisState->curVolLeft;
    sourceRight = synthesisState->curVolRight;
    targetLeft = (note->targetVolLeft << 5);
    targetRight = (note->targetVolRight << 5);
    if (targetLeft == 0) {
        targetLeft++;
    }
    if (targetRight == 0) {
        targetRight++;
    }
    synthesisState->curVolLeft = targetLeft;
    synthesisState->curVolRight = targetRight;
#endif

    // For aEnvMixer, five buffers and count are set using aSetBuffer.
    // in, dry left, count without A_AUX flag.
    // dry right, wet left, wet right with A_AUX flag.

    if (note->usesHeadsetPanEffects) {
        aClearBuffer(cmd++, DMEM_ADDR_NOTE_PAN_TEMP, DEFAULT_LEN_1CH);

        switch (headsetPanSettings) {
            case 1:
                aSetBuffer(cmd++, 0, inBuf, DMEM_ADDR_NOTE_PAN_TEMP, nSamples * 2);
                aSetBuffer(cmd++, A_AUX, DMEM_ADDR_RIGHT_CH, DMEM_ADDR_WET_LEFT_CH,
                           DMEM_ADDR_WET_RIGHT_CH);
                break;
            case 2:
                aSetBuffer(cmd++, 0, inBuf, DMEM_ADDR_LEFT_CH, nSamples * 2);
                aSetBuffer(cmd++, A_AUX, DMEM_ADDR_NOTE_PAN_TEMP, DMEM_ADDR_WET_LEFT_CH,
                           DMEM_ADDR_WET_RIGHT_CH);
                break;
            default:
                aSetBuffer(cmd++, 0, inBuf, DMEM_ADDR_LEFT_CH, nSamples * 2);
                aSetBuffer(cmd++, A_AUX, DMEM_ADDR_RIGHT_CH, DMEM_ADDR_WET_LEFT_CH,
                           DMEM_ADDR_WET_RIGHT_CH);
                break;
        }
    } else {
        // It's a bit unclear what the "stereo strong" concept does.
        // Instead of mixing the opposite channel to the normal buffers, the sound is first
        // mixed into a temporary buffer and then subtracted from the normal buffer.
        if (note->stereoStrongRight) {
            aClearBuffer(cmd++, DMEM_ADDR_STEREO_STRONG_TEMP_DRY, DEFAULT_LEN_2CH);
            aSetBuffer(cmd++, 0, inBuf, DMEM_ADDR_STEREO_STRONG_TEMP_DRY, nSamples * 2);
            aSetBuffer(cmd++, A_AUX, DMEM_ADDR_RIGHT_CH, DMEM_ADDR_STEREO_STRONG_TEMP_WET,
                       DMEM_ADDR_WET_RIGHT_CH);
        } else if (note->stereoStrongLeft) {
            aClearBuffer(cmd++, DMEM_ADDR_STEREO_STRONG_TEMP_DRY, DEFAULT_LEN_2CH);
            aSetBuffer(cmd++, 0, inBuf, DMEM_ADDR_LEFT_CH, nSamples * 2);
            aSetBuffer(cmd++, A_AUX, DMEM_ADDR_STEREO_STRONG_TEMP_DRY, DMEM_ADDR_WET_LEFT_CH,
                       DMEM_ADDR_STEREO_STRONG_TEMP_WET);
        } else {
            aSetBuffer(cmd++, 0, inBuf, DMEM_ADDR_LEFT_CH, nSamples * 2);
            aSetBuffer(cmd++, A_AUX, DMEM_ADDR_RIGHT_CH, DMEM_ADDR_WET_LEFT_CH, DMEM_ADDR_WET_RIGHT_CH);
        }
    }

#ifdef VERSION_EU
    if (targetLeft == sourceLeft && targetRight == sourceRight && !note->envMixerNeedsInit) {
#else
    if (vol->targetLeft == vol->sourceLeft && vol->targetRight == vol->sourceRight
        && !note->envMixerNeedsInit) {
#endif
        mixerFlags = A_CONTINUE;
    } else {
        mixerFlags = A_INIT;

#ifdef VERSION_EU
        rampLeft = gCurrentLeftVolRamping[targetLeft >> 5] * gCurrentRightVolRamping[sourceLeft >> 5];
        rampRight = gCurrentLeftVolRamping[targetRight >> 5] * gCurrentRightVolRamping[sourceRight >> 5];
#else
        rampLeft = get_volume_ramping(vol->sourceLeft, vol->targetLeft, nSamples);
        rampRight = get_volume_ramping(vol->sourceRight, vol->targetRight, nSamples);
#endif

        // The operation's parameters change meanings depending on flags
#ifdef VERSION_EU
        aSetVolume(cmd++, A_VOL | A_LEFT, sourceLeft, 0, 0);
        aSetVolume(cmd++, A_VOL | A_RIGHT, sourceRight, 0, 0);
        aSetVolume32(cmd++, A_RATE | A_LEFT, targetLeft, rampLeft);
        aSetVolume32(cmd++, A_RATE | A_RIGHT, targetRight, rampRight);
        aSetVolume(cmd++, A_AUX, gVolume, 0, note->reverbVol << 8);
#else
        aSetVolume(cmd++, A_VOL | A_LEFT, vol->sourceLeft, 0, 0);
        aSetVolume(cmd++, A_VOL | A_RIGHT, vol->sourceRight, 0, 0);
        aSetVolume32(cmd++, A_RATE | A_LEFT, vol->targetLeft, rampLeft);
        aSetVolume32(cmd++, A_RATE | A_RIGHT, vol->targetRight, rampRight);
        aSetVolume(cmd++, A_AUX, gVolume, 0, note->reverbVol);
#endif
    }

#ifdef VERSION_EU
    if (gUseReverb && note->reverbVol != 0) {
        aEnvMixer(cmd++, mixerFlags | A_AUX,
                  VIRTUAL_TO_PHYSICAL2(synthesisState->synthesisBuffers->mixEnvelopeState));
#else
    if (gSynthesisReverb.useReverb && note->reverb) {
        aEnvMixer(cmd++, mixerFlags | A_AUX,
                  VIRTUAL_TO_PHYSICAL2(note->synthesisBuffers->mixEnvelopeState));
#endif
        if (note->stereoStrongRight) {
            aSetBuffer(cmd++, 0, 0, 0, nSamples * 2);
            // 0x8000 is -100%, so subtract sound instead of adding...
            aMix(cmd++, 0, /*gain*/ 0x8000, /*in*/ DMEM_ADDR_STEREO_STRONG_TEMP_DRY,
                 /*out*/ DMEM_ADDR_LEFT_CH);
            aMix(cmd++, 0, /*gain*/ 0x8000, /*in*/ DMEM_ADDR_STEREO_STRONG_TEMP_WET,
                 /*out*/ DMEM_ADDR_WET_LEFT_CH);
        } else if (note->stereoStrongLeft) {
            aSetBuffer(cmd++, 0, 0, 0, nSamples * 2);
            aMix(cmd++, 0, /*gain*/ 0x8000, /*in*/ DMEM_ADDR_STEREO_STRONG_TEMP_DRY,
                 /*out*/ DMEM_ADDR_RIGHT_CH);
            aMix(cmd++, 0, /*gain*/ 0x8000, /*in*/ DMEM_ADDR_STEREO_STRONG_TEMP_WET,
                 /*out*/ DMEM_ADDR_WET_RIGHT_CH);
        }
    } else {
#ifdef VERSION_EU
        aEnvMixer(cmd++, mixerFlags, VIRTUAL_TO_PHYSICAL2(synthesisState->synthesisBuffers->mixEnvelopeState));
#else
        aEnvMixer(cmd++, mixerFlags, VIRTUAL_TO_PHYSICAL2(note->synthesisBuffers->mixEnvelopeState));
#endif
        if (note->stereoStrongRight) {
            aSetBuffer(cmd++, 0, 0, 0, nSamples * 2);
            aMix(cmd++, 0, /*gain*/ 0x8000, /*in*/ DMEM_ADDR_STEREO_STRONG_TEMP_DRY,
                 /*out*/ DMEM_ADDR_LEFT_CH);
        } else if (note->stereoStrongLeft) {
            aSetBuffer(cmd++, 0, 0, 0, nSamples * 2);
            aMix(cmd++, 0, /*gain*/ 0x8000, /*in*/ DMEM_ADDR_STEREO_STRONG_TEMP_DRY,
                 /*out*/ DMEM_ADDR_RIGHT_CH);
        }
    }
    return cmd;
}

#ifdef VERSION_EU
u64 *note_apply_headset_pan_effects(u64 *cmd, struct NoteSubEu *noteSubEu, struct NoteSynthesisState *note, s32 bufLen, s32 flags, s32 leftRight) {
#else
u64 *note_apply_headset_pan_effects(u64 *cmd, struct Note *note, s32 bufLen, s32 flags, s32 leftRight) {
#endif
    s32 dest;
    s32 pitch; // t2
    s32 prevPanShift;
    s32 panShift;

    switch (leftRight) {
        case 1:
            dest = DMEM_ADDR_LEFT_CH;
#ifndef VERSION_EU
            panShift = note->headsetPanRight;
#else
            panShift = noteSubEu->headsetPanRight;
#endif
            note->prevHeadsetPanLeft = 0;
            prevPanShift = note->prevHeadsetPanRight;
            note->prevHeadsetPanRight = panShift;
            break;
        case 2:
            dest = DMEM_ADDR_RIGHT_CH;
#ifndef VERSION_EU
            panShift = note->headsetPanLeft;
#else
            panShift = noteSubEu->headsetPanLeft;
#endif
            note->prevHeadsetPanRight = 0;

            prevPanShift = note->prevHeadsetPanLeft;
            note->prevHeadsetPanLeft = panShift;
            break;
        default:
            return cmd;
    }

    if (flags != 1) // A_INIT?
    {
        // Slightly adjust the sample rate in order to fit a change in pan shift
        if (prevPanShift == 0) {
            // Kind of a hack that moves the first samples into the resample state
            aDMEMMove(cmd++, DMEM_ADDR_NOTE_PAN_TEMP, DMEM_ADDR_TEMP, 8);
            aClearBuffer(cmd++, 8, 8); // Set pitch accumulator to 0 in the resample state
            aDMEMMove(cmd++, DMEM_ADDR_NOTE_PAN_TEMP, DMEM_ADDR_TEMP + 0x10,
                      0x10); // No idea, result seems to be overwritten later

            aSetBuffer(cmd++, 0, 0, DMEM_ADDR_TEMP, 32);
            aSaveBuffer(cmd++, VIRTUAL_TO_PHYSICAL2(note->synthesisBuffers->panResampleState));

#ifdef VERSION_EU
            pitch = (bufLen << 0xf) / (bufLen + panShift - prevPanShift + 8);
            if (pitch) {
            }
#else
            pitch = (bufLen << 0xf) / (panShift + bufLen - prevPanShift + 8);
#endif
            aSetBuffer(cmd++, 0, DMEM_ADDR_NOTE_PAN_TEMP + 8, DMEM_ADDR_TEMP, panShift + bufLen - prevPanShift);
            aResample(cmd++, 0, pitch, VIRTUAL_TO_PHYSICAL2(note->synthesisBuffers->panResampleState));
        } else {
            if (panShift == 0) {
                pitch = (bufLen << 0xf) / (bufLen - prevPanShift - 4);
            } else {
                pitch = (bufLen << 0xf) / (bufLen + panShift - prevPanShift);
            }

            aSetBuffer(cmd++, 0, DMEM_ADDR_NOTE_PAN_TEMP, DMEM_ADDR_TEMP, panShift + bufLen - prevPanShift);
            aResample(cmd++, 0, pitch, VIRTUAL_TO_PHYSICAL2(note->synthesisBuffers->panResampleState));
        }

        if (prevPanShift != 0) {
            aSetBuffer(cmd++, 0, DMEM_ADDR_NOTE_PAN_TEMP, 0, prevPanShift);
            aLoadBuffer(cmd++, VIRTUAL_TO_PHYSICAL2(note->synthesisBuffers->panSamplesBuffer));
            aDMEMMove(cmd++, DMEM_ADDR_TEMP, DMEM_ADDR_NOTE_PAN_TEMP + prevPanShift, panShift + bufLen - prevPanShift);
        } else {
            aDMEMMove(cmd++, DMEM_ADDR_TEMP, DMEM_ADDR_NOTE_PAN_TEMP, panShift + bufLen - prevPanShift);
        }
    } else {
        // Just shift right
        aDMEMMove(cmd++, DMEM_ADDR_NOTE_PAN_TEMP, DMEM_ADDR_TEMP, bufLen);
        aDMEMMove(cmd++, DMEM_ADDR_TEMP, DMEM_ADDR_NOTE_PAN_TEMP + panShift, bufLen);
        aClearBuffer(cmd++, DMEM_ADDR_NOTE_PAN_TEMP, panShift);
    }

    if (panShift) {
        // Save excessive samples for next iteration
        aSetBuffer(cmd++, 0, 0, DMEM_ADDR_NOTE_PAN_TEMP + bufLen, panShift);
        aSaveBuffer(cmd++, VIRTUAL_TO_PHYSICAL2(note->synthesisBuffers->panSamplesBuffer));
    }

    aSetBuffer(cmd++, 0, 0, 0, bufLen);
    aMix(cmd++, 0, /*gain*/ 0x7fff, /*in*/ DMEM_ADDR_NOTE_PAN_TEMP, /*out*/ dest);

    return cmd;
}

#if !defined(VERSION_EU)
// Moved to playback.c in EU

__attribute__((always_inline))  inline void note_init_volume(struct Note *note) {
    note->targetVolLeft = 0;
    note->targetVolRight = 0;
    note->reverb = 0;
    note->reverbVol = 0;
    note->curVolLeft = 1;
    note->curVolRight = 1;
    note->frequency = 0.0f;
}

void note_set_vel_pan_reverb(struct Note *note, f32 velocity, f32 pan, s32 reverb) {
    s32 panIndex;
    f32 volLeft;
    f32 volRight;
#ifdef VERSION_JP
    panIndex = MIN((s32)(pan * 127.5), 127);
#else
    panIndex = (s32)(pan * 127.5f) & 127;
#endif
    if (note->stereoHeadsetEffects) {
        u8 strongLeft;
        u8 strongRight;
        strongLeft = FALSE;
        strongRight = FALSE;
        note->headsetPanLeft = 0;
        note->headsetPanRight = 0;
        note->usesHeadsetPanEffects = FALSE;
        volLeft = gStereoPanVolume[panIndex];
        volRight = gStereoPanVolume[127 - panIndex];
        if (panIndex < 0x20) {
            strongLeft = TRUE;
        } else if (panIndex > 0x60) {
            strongRight = TRUE;
        }
        note->stereoStrongRight = strongRight;
        note->stereoStrongLeft = strongLeft;
    }  else {
        volLeft = gDefaultPanVolume[panIndex];
        volRight = gDefaultPanVolume[127 - panIndex];
    }

    if (velocity < 0) {
        velocity = 0;
    }
#ifdef VERSION_JP
    note->targetVolLeft = (u16)(velocity * volLeft) & ~0x80FF; // 0x7F00, but that doesn't match
    note->targetVolRight = (u16)(velocity * volRight) & ~0x80FF;
#else
    note->targetVolLeft = (s32)(velocity * volLeft) & ~0x80FF;
    note->targetVolRight = (s32)(velocity * volRight) & ~0x80FF;
#endif
    if (note->targetVolLeft == 0) {
        note->targetVolLeft++;
    }
    if (note->targetVolRight == 0) {
        note->targetVolRight++;
    }
    if (note->reverb != reverb) {
        note->reverb = reverb;
        note->reverbVol = reverb << 8;
        note->envMixerNeedsInit = TRUE;
        return;
    }

    if (note->needsInit) {
        note->envMixerNeedsInit = TRUE;
    } else {
        note->envMixerNeedsInit = FALSE;
    }
}

__attribute__((always_inline))  inline void note_set_frequency(struct Note *note, f32 frequency) {
    note->frequency = frequency;
}

void note_enable(struct Note *note) {
    note->enabled = TRUE;
    note->needsInit = TRUE;
    note->restart = FALSE;
    note->finished = FALSE;
    note->stereoStrongRight = FALSE;
    note->stereoStrongLeft = FALSE;
    note->usesHeadsetPanEffects = FALSE;
    note->headsetPanLeft = 0;
    note->headsetPanRight = 0;
    note->prevHeadsetPanRight = 0;
    note->prevHeadsetPanLeft = 0;
}

void note_disable(struct Note *note) {
    if (note->needsInit ) {
        note->needsInit = FALSE;
    } else {
        note_set_vel_pan_reverb(note, 0, .5f, 0);
    }
    note->priority = NOTE_PRIORITY_DISABLED;
    note->enabled = FALSE;
    note->finished = FALSE;
    note->parentLayer = NO_LAYER;
    note->prevParentLayer = NO_LAYER;
}
#endif
