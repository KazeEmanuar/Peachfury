#include <ultra64.h>

#include "heap.h"
#include "data.h"
#include "load.h"
#include "synthesis.h"
#include "seqplayer.h"
#include "effects.h"
#include "game/OPT_FOR_SIZE.h"

#pragma __attribute__((optimize("align-functions=32")))
#define ALIGN16(val) (((val) + 0xF) & ~0xF)

struct PoolSplit {
    u32 wantSeq;
    u32 wantBank;
    u32 wantUnused;
    u32 wantCustom;
}; // size = 0x10

struct PoolSplit2 {
    u32 wantPersistent;
    u32 wantTemporary;
}; // size = 0x8

s16 gVolume;
s8 gReverbDownsampleRate;

struct SoundAllocPool gAudioSessionPool;
struct SoundAllocPool gAudioInitPool;
struct SoundAllocPool gNotesAndBuffersPool;
struct SoundAllocPool gSeqAndBankPool;
struct SoundAllocPool gPersistentCommonPool;
struct SoundAllocPool gTemporaryCommonPool;

struct SoundMultiPool gSeqLoadedPool;
struct SoundMultiPool gBankLoadedPool;

struct PoolSplit sSessionPoolSplit;
struct PoolSplit2 sSeqAndBankPoolSplit;
struct PoolSplit sPersistentCommonPoolSplit;
struct PoolSplit sTemporaryCommonPoolSplit;

u8 gBankLoadStatus[0x40];
u8 gSeqLoadStatus[0x100];

#ifdef VERSION_EU
volatile u8 gAudioResetStatus;
u8 gAudioResetPresetIdToLoad;
s32 gAudioResetFadeOutFramesLeft;
#endif


extern s32 gMaxAudioCmds;

#ifdef VERSION_EU
/** 
 * Assuming 'k' in [9, 24],
 * Computes a newton's method step for f(x) = x^k - d
 */
f64 root_newton_step(f64 x, s32 k, f64 d)
{
    f64 deg2 = x * x;
    f64 deg4 = deg2 * deg2;
    f64 deg8 = deg4 * deg4;
    s32 degree = k - 9;
    f64 fx;

    f64 deriv = deg8;
    if (degree & 1) {
        deriv *= x;
    }
    if (degree & 2) {
        deriv *= deg2;
    }
    if (degree & 4) {
        deriv *= deg4;
    }
    if (degree & 8) {
        deriv *= deg8;
    }
    fx = deriv * x - d;
    deriv = k * deriv;
    return x - fx / deriv;
}

/** 
 * Assuming 'k' in [9, 24],
 * Computes d ^ (1/k)
 *
 * @return the root, or 1.0 if d is 0
 */
f64 kth_root(f64 d, s32 k) {
    f64 root = 1.5;
    f64 next;
    f64 diff;
    s32 i;
    if (d == 0.0) {
        root = 1.0;
    } else {
        for (i = 0; i < 64; i++) {
            if (1) {
            }
            next = root_newton_step(root, k, d);
            diff = next - root;

            if (diff < 0) {
                diff = -diff;
            }

            if (diff < 1e-07) {
                root = next;
                break;
            } else {
                root = next;
            }
        }
    }

    return root;
}
#endif

#ifdef VERSION_EU
void build_vol_rampings_table(s32 UNUSED unused, s32 len) {
    s32 i;
    s32 step;
    s32 d;
    s32 k = len / 8;

    for (step = 0, i = 0; i < 0x400; step += 32, i++) {
        d = step;
        if (step == 0) {
            d = 1;
        }

        gLeftVolRampings[0][i]  = kth_root(      d, k - 1);
        gRightVolRampings[0][i] = kth_root(1.0 / d, k - 1) * 65536.0;
        gLeftVolRampings[1][i]  = kth_root(      d, k);
        gRightVolRampings[1][i] = kth_root(1.0 / d, k) * 65536.0;
        gLeftVolRampings[2][i]  = kth_root(      d, k + 1);
        gRightVolRampings[2][i] = kth_root(1.0 / d, k + 1) * 65536.0;
    }
}
#endif

void reset_bank_and_seq_load_status(void) {
    s32 i;

    for (i = 0; i < 64; i++) {
        gBankLoadStatus[i] = SOUND_LOAD_STATUS_NOT_LOADED;
    }

    for (i = 0; i < 256; i++) {
        gSeqLoadStatus[i] = SOUND_LOAD_STATUS_NOT_LOADED;
    }
}

void discard_bank(s32 bankId) {
    s32 i;

    for (i = 0; i < gMaxSimultaneousNotes; i++) {
        struct Note *note = &gNotes[i];

#ifdef VERSION_EU
        if (note->noteSubEu.bankId == bankId) {
#else
        if (note->bankId == bankId) {
#endif
            if (note->priority >= NOTE_PRIORITY_MIN) {
                note->parentLayer->enabled = FALSE;
                note->parentLayer->finished = TRUE;
            }
            note_disable(note);
            audio_list_remove(&note->listItem);
            audio_list_push_back(&gNoteFreeLists.disabled, &note->listItem);
        }
    }
}

void discard_sequence(s32 seqId) {
    s32 i;

    for (i = 0; i < SEQUENCE_PLAYERS; i++) {
        if (gSequencePlayers[i].enabled && gSequencePlayers[i].seqId == seqId) {
#ifdef VERSION_EU
            sequence_player_disable(&gSequencePlayers[i]);
#else
            sequence_player_disable(gSequencePlayers + i);
#endif
        }
    }
}

void *soundAlloc(struct SoundAllocPool *pool, u32 size) {
#ifdef VERSION_EU
    u8 *start;
    u8 *pos;
    u32 alignedSize = ALIGN16(size);

    start = pool->cur;
    if (start + alignedSize <= pool->start + pool->size) {
        pool->cur += alignedSize;
        for (pos = start; pos < pool->cur; pos++) {
            *pos = 0;
        }
    } else {
        return NULL;
    }
    return start;
#else
    u8 *start;
    s32 last;
    s32 i;

    if ((pool->cur + ALIGN16(size) <= pool->size + pool->start)) {
        start = pool->cur;
        pool->cur += ALIGN16(size);
        last = pool->cur - start - 1;
        for (i = 0; i <= last; i++) {
            start[i] = 0;
        }
    } else {
        return NULL;
    }
    return start;
#endif
}

void sound_alloc_pool_init(struct SoundAllocPool *pool, void *memAddr, u32 size) {
    pool->cur = pool->start = (u8 *) ALIGN16((uintptr_t) memAddr);
    pool->size = size;
    pool->unused = 0;
}

void persistent_pool_clear(struct PersistentPool *persistent) {
    persistent->pool.unused = 0;
    persistent->pool.cur = persistent->pool.start;
    persistent->numEntries = 0;
}

void temporary_pool_clear(struct TemporaryPool *temporary) {
    temporary->pool.unused = 0;
    temporary->pool.cur = temporary->pool.start;
    temporary->nextSide = 0;
    temporary->entries[0].ptr = temporary->pool.start;
#ifdef VERSION_EU
    temporary->entries[1].ptr = temporary->pool.start + temporary->pool.size;
#else
    temporary->entries[1].ptr = temporary->pool.size + temporary->pool.start;
#endif
    temporary->entries[0].id = -1;
    temporary->entries[1].id = -1;
}

void sound_init_main_pools(s32 sizeForAudioInitPool) {
    sound_alloc_pool_init(&gAudioInitPool, gAudioHeap, sizeForAudioInitPool);
    sound_alloc_pool_init(&gAudioSessionPool, gAudioHeap + sizeForAudioInitPool, gAudioHeapSize - sizeForAudioInitPool);
}

void session_pools_init(struct PoolSplit *a) {
    gAudioSessionPool.cur = gAudioSessionPool.start;
    sound_alloc_pool_init(&gNotesAndBuffersPool, soundAlloc(&gAudioSessionPool, a->wantSeq), a->wantSeq);
    sound_alloc_pool_init(&gSeqAndBankPool, soundAlloc(&gAudioSessionPool, a->wantCustom), a->wantCustom);
}

void seq_and_bank_pool_init(struct PoolSplit2 *a) {
    gSeqAndBankPool.cur = gSeqAndBankPool.start;
    sound_alloc_pool_init(&gPersistentCommonPool, soundAlloc(&gSeqAndBankPool, a->wantPersistent), a->wantPersistent);
    sound_alloc_pool_init(&gTemporaryCommonPool, soundAlloc(&gSeqAndBankPool, a->wantTemporary), a->wantTemporary);
}

void persistent_pools_init(struct PoolSplit *a) {
    gPersistentCommonPool.cur = gPersistentCommonPool.start;
    sound_alloc_pool_init(&gSeqLoadedPool.persistent.pool, soundAlloc(&gPersistentCommonPool, a->wantSeq), a->wantSeq);
    sound_alloc_pool_init(&gBankLoadedPool.persistent.pool, soundAlloc(&gPersistentCommonPool, a->wantBank), a->wantBank);
    persistent_pool_clear(&gSeqLoadedPool.persistent);
    persistent_pool_clear(&gBankLoadedPool.persistent);
}

void temporary_pools_init(struct PoolSplit *a) {
    gTemporaryCommonPool.cur = gTemporaryCommonPool.start;
    sound_alloc_pool_init(&gSeqLoadedPool.temporary.pool, soundAlloc(&gTemporaryCommonPool, a->wantSeq), a->wantSeq);
    sound_alloc_pool_init(&gBankLoadedPool.temporary.pool, soundAlloc(&gTemporaryCommonPool, a->wantBank), a->wantBank);
    temporary_pool_clear(&gSeqLoadedPool.temporary);
    temporary_pool_clear(&gBankLoadedPool.temporary);
}


void *alloc_bank_or_seq(struct SoundMultiPool *arg0, s32 arg1, s32 size, s32 arg3, s32 id) {
    // arg3 = 0, 1 or 2?

    struct TemporaryPool *tp; // sp30
    struct PersistentPool *persistent = &arg0->persistent;
    struct SoundAllocPool *pool;
    void *ret;
    u32 nullID = -1;
    u8 *table;
    u8 isSound;
#ifndef VERSION_EU
    u16 firstVal;
    u16 secondVal;
    u32 bothDiscardable;
    u32 leftDiscardable, rightDiscardable;
    u32 leftNotLoaded, rightNotLoaded;
    u32 leftAvail, rightAvail;
#endif

    if (arg3 == 0) {
        tp = &arg0->temporary;
        if (arg0 == &gSeqLoadedPool) {
            table = gSeqLoadStatus;
            isSound = FALSE;
        } else if (arg0 == &gBankLoadedPool) {
            table = gBankLoadStatus;
            isSound = TRUE;
        }

        firstVal  = (tp->entries[0].id == (s8)nullID ? SOUND_LOAD_STATUS_NOT_LOADED : table[tp->entries[0].id]); // a3, a2
        secondVal = (tp->entries[1].id == (s8)nullID ? SOUND_LOAD_STATUS_NOT_LOADED : table[tp->entries[1].id]); // a1

#ifndef VERSION_EU
        leftNotLoaded = (firstVal == SOUND_LOAD_STATUS_NOT_LOADED);
        leftDiscardable = (firstVal == SOUND_LOAD_STATUS_DISCARDABLE); // t0
        leftAvail = (firstVal != SOUND_LOAD_STATUS_IN_PROGRESS);
        rightNotLoaded = (secondVal == SOUND_LOAD_STATUS_NOT_LOADED);
        rightDiscardable = (secondVal == SOUND_LOAD_STATUS_DISCARDABLE);
        rightAvail = (secondVal != SOUND_LOAD_STATUS_IN_PROGRESS);
        bothDiscardable = (leftDiscardable && rightDiscardable); // a0

        if (leftNotLoaded) {
            tp->nextSide = 0;
        } else if (rightNotLoaded) {
            tp->nextSide = 1;
        } else if (bothDiscardable) {
            // Use the opposite side from last time.
        } else if (firstVal == SOUND_LOAD_STATUS_DISCARDABLE) { //??!
            tp->nextSide = 0;
        } else if (rightDiscardable) {
            tp->nextSide = 1;
        } else if (leftAvail) {
            tp->nextSide = 0;
        } else if (rightAvail) {
            tp->nextSide = 1;
        } else {
            // Both left and right sides are being loaded into.
            return NULL;
        }
#else
        if (firstVal == SOUND_LOAD_STATUS_NOT_LOADED) {
            tp->nextSide = 0;
        } else if (secondVal == SOUND_LOAD_STATUS_NOT_LOADED) {
            tp->nextSide = 1;
        } else if ((firstVal == SOUND_LOAD_STATUS_DISCARDABLE) && (secondVal == SOUND_LOAD_STATUS_DISCARDABLE)) {
            // Use the opposite side from last time.
        } else if (firstVal == SOUND_LOAD_STATUS_DISCARDABLE) {
            tp->nextSide = 0;
        } else if (secondVal == SOUND_LOAD_STATUS_DISCARDABLE) {
            tp->nextSide = 1;
        } else if (firstVal != SOUND_LOAD_STATUS_IN_PROGRESS) {
            tp->nextSide = 0;
        } else if (secondVal != SOUND_LOAD_STATUS_IN_PROGRESS) {
            tp->nextSide = 1;
        } else {
            // Both left and right sides are being loaded into.
            return NULL;
        }
#endif

        pool = &arg0->temporary.pool; // a1
        if (tp->entries[tp->nextSide].id != (s8)nullID) {
            table[tp->entries[tp->nextSide].id] = SOUND_LOAD_STATUS_NOT_LOADED;
            if (isSound ) {
                discard_bank(tp->entries[tp->nextSide].id);
            }
        }

        switch (tp->nextSide) {
            case 0:
                tp->entries[0].ptr = pool->start;
                tp->entries[0].id = id;
                tp->entries[0].size = size;

                pool->cur = pool->start + size;

                if (tp->entries[1].ptr < pool->cur) {
                    // Throw out the entry on the other side if it doesn't fit.
                    // (possible @bug: what if it's currently being loaded?)
                    table[tp->entries[1].id] = SOUND_LOAD_STATUS_NOT_LOADED;

                    switch (isSound) {
                        case FALSE:
                            discard_sequence(tp->entries[1].id);
                            break;
                        case TRUE:
                            discard_bank(tp->entries[1].id);
                            break;
                    }

                    tp->entries[1].id = (s32)nullID;
#ifdef VERSION_EU
                    tp->entries[1].ptr = pool->start + pool->size;
#else
                    tp->entries[1].ptr = pool->size + pool->start;
#endif
                }

                ret = tp->entries[0].ptr;
                break;

            case 1:
#ifdef VERSION_EU
                tp->entries[1].ptr = pool->start + pool->size - size - 0x10;
#else
                tp->entries[1].ptr = pool->size + pool->start - size - 0x10;
#endif
                tp->entries[1].id = id;
                tp->entries[1].size = size;

                if (tp->entries[1].ptr < pool->cur) {
                    table[tp->entries[0].id] = SOUND_LOAD_STATUS_NOT_LOADED;

                    switch (isSound) {
                        case FALSE:
                            discard_sequence(tp->entries[0].id);
                            break;
                        case TRUE:
                            discard_bank(tp->entries[0].id);
                            break;
                    }

                    tp->entries[0].id = (s32)nullID;
                    pool->cur = pool->start;
                }

                ret = tp->entries[1].ptr;
                break;

            default:
                return NULL;
        }

        // Switch sides for next time in case both entries are
        // SOUND_LOAD_STATUS_DISCARDABLE.
        tp->nextSide ^= 1;

        return ret;
    }

#ifdef VERSION_EU
    ret = soundAlloc(&arg0->persistent.pool, arg1 * size);
    arg0->persistent.entries[arg0->persistent.numEntries].ptr = ret;

    if (ret == NULL)
#else
    persistent->entries[persistent->numEntries].ptr = soundAlloc(&persistent->pool, arg1 * size);

    if (persistent->entries[persistent->numEntries].ptr == NULL)
#endif
    {
        switch (arg3) {
            case 2:
#ifdef VERSION_EU
                return alloc_bank_or_seq(arg0, arg1, size, 0, id);
#else
                // Prevent tail call optimization.
                ret = alloc_bank_or_seq(arg0, arg1, size, 0, id);
                return ret;
#endif
            case 1:
                return NULL;
        }
    }

    // TODO: why is this guaranteed to write <= 32 entries...?
    // Because the buffer is small enough that more don't fit?
    persistent->entries[persistent->numEntries].id = id;
    persistent->entries[persistent->numEntries].size = size;
#ifdef VERSION_EU
    return persistent->entries[persistent->numEntries++].ptr;
#else
    persistent->numEntries++; return persistent->entries[persistent->numEntries - 1].ptr;
#endif
}

void *get_bank_or_seq(struct SoundMultiPool *arg0, s32 arg1, s32 id) {
    u32 i;
    struct TemporaryPool *temporary = &arg0->temporary;

    if (arg1 == 0) {
        // Try not to overwrite sound that we have just accessed, by setting nextSide appropriately.
        if (temporary->entries[0].id == id) {
            temporary->nextSide = 1;
            return temporary->entries[0].ptr;
        } else if (temporary->entries[1].id == id) {
            temporary->nextSide = 0;
            return temporary->entries[1].ptr;
        }
        return NULL;
    } else {
        struct PersistentPool *persistent = &arg0->persistent;
        for (i = 0; i < persistent->numEntries; i++) {
            if (id == persistent->entries[i].id) {
                return persistent->entries[i].ptr;
            }
        }

        if (arg1 == 2) {
            // Prevent tail call optimization by using a temporary.
            // (Did they compile with -Wo,-notail?)
            return get_bank_or_seq(arg0, 0, id);
        }
        return NULL;
    }
}


#ifdef VERSION_EU
void decrease_reverb_gain(void) {
    s32 i;
    for (i = 0; i < gNumSynthesisReverbs; i++) {
        gSynthesisReverbs[i].reverbGain -= gSynthesisReverbs[i].reverbGain / 8;
    }
}
#else
void decrease_reverb_gain(void) {
    gSynthesisReverb.reverbGain -= gSynthesisReverb.reverbGain / 4;
}
#endif

#ifdef VERSION_EU
s32 audio_shut_down_and_reset_step(void) {
    s32 i;
    s32 j;
    switch (gAudioResetStatus) {
        case 5:
            for (i = 0; i < SEQUENCE_PLAYERS; i++) {
                sequence_player_disable(&gSequencePlayers[i]);
            }
            gAudioResetFadeOutFramesLeft = 4;
            gAudioResetStatus--;
            break;
        case 4:
            if (gAudioResetFadeOutFramesLeft != 0) {
                gAudioResetFadeOutFramesLeft--;
                decrease_reverb_gain();
            } else {
                for (i = 0; i < gMaxSimultaneousNotes; i++) {
                    if (gNotes[i].noteSubEu.enabled && gNotes[i].adsr.state != ADSR_STATE_DISABLED) {
                        gNotes[i].adsr.fadeOutVel = gAudioBufferParameters.updatesPerFrameInv;
                        gNotes[i].adsr.action |= ADSR_ACTION_RELEASE;
                    }
                }
                gAudioResetFadeOutFramesLeft = 16;
                gAudioResetStatus--;
            }
            break;
        case 3:
            if (gAudioResetFadeOutFramesLeft != 0) {
                gAudioResetFadeOutFramesLeft--;
                decrease_reverb_gain();
            } else {
                for (i = 0; i < NUMAIBUFFERS; i++) {
                    for (j = 0; j < (s32) (AIBUFFER_LEN / sizeof(s16)); j++) {
                        gAiBuffers[i][j] = 0;
                    }
                }
                gAudioResetFadeOutFramesLeft = 4;
                gAudioResetStatus--;
            }
            break;
        case 2:
            if (gAudioResetFadeOutFramesLeft != 0) {
                gAudioResetFadeOutFramesLeft--;
            } else {
                gAudioResetStatus--;
            }
            break;
        case 1:
            audio_reset_session();
            gAudioResetStatus = 0;
    }
    if (gAudioResetStatus < 3) {
        return 0;
    }
    return 1;
}
#else
/**
 * Waits until a specified number of audio frames have been created
 */
void wait_for_audio_frames(s32 frames) {
    gAudioFrameCount = 0;
    // Sound thread will update gAudioFrameCount
    while (gAudioFrameCount < frames) {
        // spin
    }
}
#endif

__attribute__((always_inline))  inline void init_note_free_list(void) {
    s32 i;

    init_note_lists(&gNoteFreeLists);
    for (i = 0; i < gMaxSimultaneousNotes; i++) {
        gNotes[i].listItem.u.value = &gNotes[i];
        gNotes[i].listItem.prev = NULL;
        audio_list_push_back(&gNoteFreeLists.disabled, &gNotes[i].listItem);
    }
}
void audio_reset_session(struct AudioSessionSettings *preset) {
    s16 *mem;
    s8 updatesPerFrame;
    s32 reverbWindowSize;
    s32 k;
    s32 i;
    s32 j;
    s32 persistentMem;
    s32 temporaryMem;
    s32 totalMem;
    s32 wantMisc;
    s32 frames;
    s32 remainingDmas;

    if (gAudioLoadLock != AUDIO_LOCK_UNINITIALIZED) {
        decrease_reverb_gain();
        for (i = 0; i < gMaxSimultaneousNotes; i++) {
            if (gNotes[i].enabled && gNotes[i].adsr.state != ADSR_STATE_DISABLED) {
                gNotes[i].adsr.fadeOutVel = 0x8000 / gAudioUpdatesPerFrame;
                gNotes[i].adsr.action |= ADSR_ACTION_RELEASE;
            }
        }

        // Wait for all notes to stop playing
        frames = 0;
        for (;;) {
            wait_for_audio_frames(1);
            frames++;
            if (frames > 4 * 60) {
                // Break after 4 seconds
                break;
            }

            for (i = 0; i < gMaxSimultaneousNotes; i++) {
                if (gNotes[i].enabled)
                    break;
            }

            if (i == gMaxSimultaneousNotes) {
                // All zero, break early
                break;
            }
        }

        // Wait for the reverb to finish as well
        decrease_reverb_gain();
        wait_for_audio_frames(3);

        // The audio interface is double buffered; thus, we have to take the
        // load lock for 2 frames for the buffers to free up before we can
        // repurpose memory. Make that 3 frames, just in case.
        gAudioLoadLock = AUDIO_LOCK_LOADING;
        wait_for_audio_frames(3);

        remainingDmas = gCurrAudioFrameDmaCount;
        while (remainingDmas > 0) {
            for (i = 0; i < gCurrAudioFrameDmaCount; i++) {
                if (osRecvMesg(&gCurrAudioFrameDmaQueue, NULL, OS_MESG_NOBLOCK) == 0)
                    remainingDmas--;
            }
        }
        gCurrAudioFrameDmaCount = 0;

        for (j = 0; j < NUMAIBUFFERS; j++) {
            for (k = 0; k < (s32) (AIBUFFER_LEN / sizeof(s16)); k++) {
                gAiBuffers[j][k] = 0;
            }
        }
    }

    gSampleDmaNumListItems = 0;
    reverbWindowSize = preset->reverbWindowSize;
    gAiFrequency = osAiSetFrequency(preset->frequency);
    gMaxSimultaneousNotes = preset->maxSimultaneousNotes;
    gSamplesPerFrameTarget = ALIGN16(gAiFrequency / 60);
    gReverbDownsampleRate = preset->reverbDownsampleRate;
    gVolume = preset->volume;
    gMinAiBufferLength = gSamplesPerFrameTarget - 0x10;
    updatesPerFrame = gSamplesPerFrameTarget / 160 + 1;
    gAudioUpdatesPerFrame = gSamplesPerFrameTarget / 160 + 1;

    // Compute conversion ratio from the internal unit tatums/tick to the
    // external beats/minute (JP) or tatums/minute (US). In practice this is
    // 300 on JP and 14360 on US.
    gTempoInternalToExternal = (u32)(updatesPerFrame * 2880000.0f / gTatumsPerBeat / 16.713f);
    gMaxAudioCmds = gMaxSimultaneousNotes * 20 * updatesPerFrame + 320;

    persistentMem = DOUBLE_SIZE_ON_64_BIT(preset->persistentBankMem + preset->persistentSeqMem);
    temporaryMem = DOUBLE_SIZE_ON_64_BIT(preset->temporaryBankMem + preset->temporarySeqMem);
    totalMem = persistentMem + temporaryMem;
    wantMisc = gAudioSessionPool.size - totalMem - 0x100;
    sSessionPoolSplit.wantSeq = wantMisc;
    sSessionPoolSplit.wantCustom = totalMem;
    session_pools_init(&sSessionPoolSplit);
    sSeqAndBankPoolSplit.wantPersistent = persistentMem;
    sSeqAndBankPoolSplit.wantTemporary = temporaryMem;
    seq_and_bank_pool_init(&sSeqAndBankPoolSplit);
    sPersistentCommonPoolSplit.wantSeq = DOUBLE_SIZE_ON_64_BIT(preset->persistentSeqMem);
    sPersistentCommonPoolSplit.wantBank = DOUBLE_SIZE_ON_64_BIT(preset->persistentBankMem);
    sPersistentCommonPoolSplit.wantUnused = 0;
    persistent_pools_init(&sPersistentCommonPoolSplit);
    sTemporaryCommonPoolSplit.wantSeq = DOUBLE_SIZE_ON_64_BIT(preset->temporarySeqMem);
    sTemporaryCommonPoolSplit.wantBank = DOUBLE_SIZE_ON_64_BIT(preset->temporaryBankMem);
    sTemporaryCommonPoolSplit.wantUnused = 0;
    temporary_pools_init(&sTemporaryCommonPoolSplit);
    reset_bank_and_seq_load_status();

    for (j = 0; j < 2; j++) {
        gAudioCmdBuffers[j] = soundAlloc(&gNotesAndBuffersPool, gMaxAudioCmds * sizeof(u64));
    }

    gNotes = soundAlloc(&gNotesAndBuffersPool, gMaxSimultaneousNotes * sizeof(struct Note));
    note_init_all();
    init_note_free_list();

    if (reverbWindowSize == 0) {
        gSynthesisReverb.useReverb = 0;
    } else {
        gSynthesisReverb.useReverb = 8;
        gSynthesisReverb.ringBuffer.left = soundAlloc(&gNotesAndBuffersPool, reverbWindowSize * 2);
        gSynthesisReverb.ringBuffer.right = soundAlloc(&gNotesAndBuffersPool, reverbWindowSize * 2);
        gSynthesisReverb.nextRingBufferPos = 0;
        gSynthesisReverb.unkC = 0;
        gSynthesisReverb.curFrame = 0;
        gSynthesisReverb.bufSizePerChannel = reverbWindowSize;
        gSynthesisReverb.reverbGain = preset->reverbGain;
        gSynthesisReverb.framesLeftToIgnore = 2;
        if (gReverbDownsampleRate != 1) {
            gSynthesisReverb.resampleFlags = A_INIT;
            gSynthesisReverb.resampleRate = 0x8000 / gReverbDownsampleRate;
            gSynthesisReverb.resampleStateLeft = soundAlloc(&gNotesAndBuffersPool, 16 * sizeof(s16));
            gSynthesisReverb.resampleStateRight = soundAlloc(&gNotesAndBuffersPool, 16 * sizeof(s16));
            gSynthesisReverb.unk24 = soundAlloc(&gNotesAndBuffersPool, 16 * sizeof(s16));
            gSynthesisReverb.unk28 = soundAlloc(&gNotesAndBuffersPool, 16 * sizeof(s16));
            for (i = 0; i < gAudioUpdatesPerFrame; i++) {
                mem = soundAlloc(&gNotesAndBuffersPool, DEFAULT_LEN_2CH);
                gSynthesisReverb.items[0][i].toDownsampleLeft = mem;
                gSynthesisReverb.items[0][i].toDownsampleRight = mem + DEFAULT_LEN_1CH / sizeof(s16);
                mem = soundAlloc(&gNotesAndBuffersPool, DEFAULT_LEN_2CH);
                gSynthesisReverb.items[1][i].toDownsampleLeft = mem;
                gSynthesisReverb.items[1][i].toDownsampleRight = mem + DEFAULT_LEN_1CH / sizeof(s16);
            }
        }
    }

    init_sample_dma_buffers(gMaxSimultaneousNotes);


    osWritebackDCacheAll();

    if (gAudioLoadLock != AUDIO_LOCK_UNINITIALIZED) {
        gAudioLoadLock = AUDIO_LOCK_NOT_LOADING;
    }
}
