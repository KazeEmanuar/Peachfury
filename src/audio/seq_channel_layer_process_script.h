//! Copt inlining for US/JP. Here be dragons

#include <ultra64.h>
#include <macros.h>

#include "heap.h"
#include "data.h"
#include "load.h"
#include "seqplayer.h"
#include "external.h"
#include "effects.h"

#define PORTAMENTO_IS_SPECIAL(x) ((x).mode & 0x80)
#define PORTAMENTO_MODE(x) ((x).mode & ~0x80)
#define PORTAMENTO_MODE_1 1
#define PORTAMENTO_MODE_2 2
#define PORTAMENTO_MODE_3 3
#define PORTAMENTO_MODE_4 4
#define PORTAMENTO_MODE_5 5

#define COPT 0
#if COPT
#define M64_READ_U8(state, dst) \
    dst = m64_read_u8(state);
#else
#define M64_READ_U8(state, dst) \
{                               \
    u8 * _ptr_pc;               \
    u8  _pc;                    \
    _ptr_pc = (*state).pc;      \
    ((*state).pc)++;            \
    _pc = *_ptr_pc;             \
    dst = _pc;                  \
}
#endif


#if COPT
#define M64_READ_S16(state, dst) \
    dst = m64_read_s16(state);
#else
#define M64_READ_S16(state, dst)    \
{                                   \
    s16 _ret;                       \
    _ret = *(*state).pc << 8;       \
    ((*state).pc)++;                \
    _ret = *(*state).pc | _ret;     \
    ((*state).pc)++;                \
    dst = _ret;                     \
}
#endif
#if COPT
#define M64_READ_COMPRESSED_U16(state, dst) \
    dst = m64_read_compressed_u16(state);
#else
#define M64_READ_COMPRESSED_U16(state, dst) \
{                                           \
    u16 ret = *(state->pc++);               \
    if (ret & 0x80) {                       \
        ret = (ret << 8) & 0x7f00;          \
        ret = *(state->pc++) | ret;         \
    }                                       \
    dst = ret;                              \
}
#endif

#if COPT
#define GET_INSTRUMENT(seqChannel, instId, _instOut, _adsr, dst, l) \
    dst = get_instrument(seqChannel, instId, _instOut, _adsr);
#else
#define GET_INSTRUMENT(seqChannel, instId, _instOut, _adsr, dst, l) \
{ \
struct AdsrSettings *adsr = _adsr; \
struct Instrument **instOut = _instOut;\
    u8 _instId = instId; \
    struct Instrument *inst; \
    UNUSED u32 pad; \
        /* copt inlines instId here  */ \
    if (instId >= gCtlEntries[(*seqChannel).bankId].numInstruments) { \
        _instId = gCtlEntries[(*seqChannel).bankId].numInstruments; \
        if (_instId == 0) { \
            dst = 0; \
            goto ret ## l; \
        } \
        _instId--; \
    } \
    inst = gCtlEntries[(*seqChannel).bankId].instruments[_instId]; \
    if (inst == NULL) { \
        while (_instId != 0xff) { \
            inst = gCtlEntries[(*seqChannel).bankId].instruments[_instId]; \
            if (inst != NULL) { \
                goto gi ## l; \
            } \
            _instId--; \
        } \
        gi ## l:; \
    } \
    if (((uintptr_t) gBankLoadedPool.persistent.pool.start <= (uintptr_t) inst \
         && (uintptr_t) inst <= (uintptr_t)(gBankLoadedPool.persistent.pool.start \
                                          + gBankLoadedPool.persistent.pool.size)) \
        || ((uintptr_t) gBankLoadedPool.temporary.pool.start <= (uintptr_t) inst \
            && (uintptr_t) inst <= (uintptr_t)(gBankLoadedPool.temporary.pool.start \
                                             + gBankLoadedPool.temporary.pool.size))) { \
        (*adsr).envelope = (*inst).envelope; \
        (*adsr).releaseRate = (*inst).releaseRate; \
        *instOut = inst; \
        _instId++; \
        goto ret ## l; \
    } \
  //  gAudioErrorFlags = _instId + 0x20000; \
    *instOut = NULL; \
    ret ## l: ; \
}
#endif

#ifdef VERSION_EU
    #define PORTAMENTO_TGT_NOTE cmd
    #define DRUM_INDEX          cmd
    #define SEMITONE            cmd
    #define USED_SEMITONE       vel
#else
    #define PORTAMENTO_TGT_NOTE portamentoTargetNote
    #define DRUM_INDEX          cmdSemitone
    #define SEMITONE            cmdSemitone
    #define USED_SEMITONE       usedSemitone
#endif
