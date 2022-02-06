/* gcc setloop.c -o setloop */
/* ./setloop sound.aiff soundout.aiff start end */

/**
 * Set loop data for an AIFF file.
 */
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

typedef signed char s8;
typedef short s16;
typedef int s32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef float f32;

#define bswap16(x) __builtin_bswap16(x)
#define bswap32(x) __builtin_bswap32(x)
#define BSWAP16(x) x = __builtin_bswap16(x)
#define BSWAP32(x) x = __builtin_bswap32(x)

#define NORETURN __attribute__((noreturn))
#define UNUSED __attribute__((unused))

typedef struct {
    u32 ckID;
    u32 ckSize;
} ChunkHeader;

typedef struct {
    u32 ckID;
    u32 ckSize;
    u32 formType;
} Chunk;

typedef struct {
    s16 numChannels;
    u16 numFramesH;
    u16 numFramesL;
    s16 sampleSize;
    s16 sampleRate[5]; // 80-bit float
    u16 compressionTypeH;
    u16 compressionTypeL;
} CommonChunk;

typedef struct {
    s16 MarkerID;
    u16 positionH;
    u16 positionL;
} Marker;

typedef struct {
    s16 playMode;
    s16 beginLoop;
    s16 endLoop;
} Loop;

typedef struct {
    s8 baseNote;
    s8 detune;
    s8 lowNote;
    s8 highNote;
    s8 lowVelocity;
    s8 highVelocity;
    s16 gain;
    Loop sustainLoop;
    Loop releaseLoop;
} InstrumentChunk;

typedef struct
{
    u32 start;
    u32 end;
    u32 count;
    s16 state[16];
} ALADPCMloop;


static char usage[] = "input.aiff output.aiff [<loopstart> <loopend> | none]";
static const char *progname, *infilename;

#define checked_fread(a, b, c, d) if (fread(a, b, c, d) != c) fail_parse("error parsing file")

NORETURN
void fail_parse(const char *fmt, ...)
{
    char *formatted = NULL;
    va_list ap;
    va_start(ap, fmt);
    int size = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    if (size >= 0) {
        size++;
        formatted = malloc(size);
        if (formatted != NULL) {
            va_start(ap, fmt);
            size = vsnprintf(formatted, size, fmt, ap);
            va_end(ap);
            if (size < 0) {
                free(formatted);
                formatted = NULL;
            }
        }
    }

    if (formatted != NULL) {
        fprintf(stderr, "%s: %s [%s]\n", progname, formatted, infilename);
        free(formatted);
    }
    exit(1);
}

int read_int(const char *in, int *out)
{
    if (in[0] == '0' && (in[1] == 'x' || in[1] == 'X'))
        return sscanf(in + 2, "%x", out);
    return sscanf(in, "%d", out);
}

void write_header(FILE *ofile, const char *id, s32 size)
{
    fwrite(id, 4, 1, ofile);
    BSWAP32(size);
    fwrite(&size, sizeof(s32), 1, ofile);
}

int main(int argc, char **argv)
{
    u8 *soundData = NULL;
    s32 soundDataSize = -1;
    u8 *commData = NULL;
    s32 commDataSize = -1;
    s32 hasLoop = 0;
    s32 loopStart = -1;
    s32 loopEnd = -1;
    Chunk FormChunk;
    ChunkHeader Header;
    InstrumentChunk InstChunk;
    FILE *ifile;
    FILE *ofile;
    progname = argv[0];

    if (argc < 4) {
        fprintf(stderr, "%s %s\n", progname, usage);
        exit(1);
    }

    if (strcmp(argv[3], "none") == 0) {
        hasLoop = 0;
    } else {
        if (argc < 5) {
            fprintf(stderr, "%s %s\n", progname, usage);
            exit(1);
        }
        hasLoop = 1;
        if (!read_int(argv[3], &loopStart) ||
            !read_int(argv[4], &loopEnd)) {
            fprintf(stderr, "failed to parse loop arguments: expected integers\n");
        }
    }

    infilename = argv[1];

    if ((ifile = fopen(infilename, "rb")) == NULL) {
        fail_parse("AIFF file could not be opened");
        exit(1);
    }

    memset(&InstChunk, 0, sizeof(InstChunk));

    checked_fread(&FormChunk, sizeof(FormChunk), 1, ifile);
    BSWAP32(FormChunk.ckID);
    BSWAP32(FormChunk.formType);
    if ((FormChunk.ckID != 0x464f524d) || // FORM
        (FormChunk.formType != 0x41494643 && FormChunk.formType != 0x41494646)) { // AIFC, AIFF
        fail_parse("not an AIFF file");
    }

    for (;;) {
        s32 num = fread(&Header, sizeof(Header), 1, ifile);
        if (num <= 0) break;
        BSWAP32(Header.ckID);
        BSWAP32(Header.ckSize);

        s32 origCkSize = Header.ckSize;
        Header.ckSize++;
        Header.ckSize &= ~1;
        s32 offset = ftell(ifile);

        switch (Header.ckID) {
        case 0x434f4d4d: // COMM
            commData = malloc(Header.ckSize);
            commDataSize = origCkSize;
            checked_fread(commData, Header.ckSize, 1, ifile);
            break;

        case 0x53534e44: // SSND
            soundData = malloc(Header.ckSize);
            soundDataSize = origCkSize;
            checked_fread(soundData, Header.ckSize, 1, ifile);
            break;
        }

        fseek(ifile, offset + Header.ckSize, SEEK_SET);
    }
    fclose(ifile);

    if ((ofile = fopen(argv[2], "wb")) == NULL) {
        fprintf(stderr, "%s: output file could not be opened [%s]\n", progname, argv[2]);
        exit(1);
    }

    // Write an incomplete file header. We'll fill in the size later.
    fwrite("FORM\0\0\0\0AIFF", 12, 1, ofile);

    write_header(ofile, "COMM", commDataSize);
    fwrite(commData, (commDataSize + 1) & ~1, 1, ofile);

    // Loops
    if (hasLoop) {
        const char *markerNames[2] = {"start", "end"};
        Marker markers[2] = {
            {1, loopStart >> 16, loopStart & 0xffff},
            {2, loopEnd >> 16, loopEnd & 0xffff}
        };
        write_header(ofile, "MARK", 2 + 2 * sizeof(Marker) + 1 + 5 + 1 + 3);
        s16 numMarkers = bswap16(2);
        fwrite(&numMarkers, sizeof(s16), 1, ofile);
        for (s32 i = 0; i < 2; i++) {
            u8 len = (u8) strlen(markerNames[i]);
            BSWAP16(markers[i].MarkerID);
            BSWAP16(markers[i].positionH);
            BSWAP16(markers[i].positionL);
            fwrite(&markers[i], sizeof(Marker), 1, ofile);
            fwrite(&len, 1, 1, ofile);
            fwrite(markerNames[i], len, 1, ofile);
        }

        write_header(ofile, "INST", sizeof(InstrumentChunk));
        InstChunk.sustainLoop.playMode = bswap16(1);
        InstChunk.sustainLoop.beginLoop = bswap16(1);
        InstChunk.sustainLoop.endLoop = bswap16(2);
        InstChunk.releaseLoop.playMode = 0;
        InstChunk.releaseLoop.beginLoop = 0;
        InstChunk.releaseLoop.endLoop = 0;
        fwrite(&InstChunk, sizeof(InstrumentChunk), 1, ofile);
    }

    write_header(ofile, "SSND", soundDataSize);
    fwrite(soundData, (soundDataSize + 1) & ~1, 1, ofile);

    // Fix the size in the header
    s32 fileSize = bswap32(ftell(ofile) - 8);
    fseek(ofile, 4, SEEK_SET);
    fwrite(&fileSize, 4, 1, ofile);

    fclose(ofile);
    return 0;
}