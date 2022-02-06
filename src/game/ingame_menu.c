#include <ultra64.h>

#include "actors/common1.h"
#include "area.h"
#include "audio/external.h"
#include "camera.h"
#include "course_table.h"
#include "dialog_ids.h"
#include "engine/math_util.h"
#include "eu_translation.h"
#include "game_init.h"
#include "gfx_dimensions.h"
#include "ingame_menu.h"
#include "level_update.h"
#include "levels/castle_grounds/header.h"
#include "memory.h"
#include "print.h"
#include "save_file.h"
#include "segment2.h"
#include "segment7.h"
#include "seq_ids.h"
#include "sm64.h"
#include "text_strings.h"
#include "types.h"
#include "../../enhancements/puppycam.h"
#include "OPT_FOR_SIZE.h"

extern s16 SCREEN_WIDTH_MATH;
extern s16 SCREEN_HEIGHT_MATH;
extern s16 BORDER_HEIGHT_MATH;
extern s16 BORDER_WIDTH_MATH;
u8 bowsertextID = 0;
u16 renderBowserText = 0;
u16 gDialogColorFadeTimer;
s8 gLastDialogLineNum;
s32 gDialogVariable;
u16 gDialogTextAlpha;
#if defined(VERSION_EU)
s16 gDialogX; // D_8032F69A
s16 gDialogY; // D_8032F69C
#endif
s16 gCutsceneMsgXOffset;
s16 gCutsceneMsgYOffset;
s8 gRedCoinsCollected;
s16 lwrBound;
u8 *gStr = NULL;

extern u8 gLastCompletedCourseNum;
extern u8 gLastCompletedStarNum;

enum DialogBoxState {
    DIALOG_STATE_OPENING,
    DIALOG_STATE_VERTICAL,
    DIALOG_STATE_HORIZONTAL,
    DIALOG_STATE_CLOSING
};

enum DialogBoxPageState { DIALOG_PAGE_STATE_NONE, DIALOG_PAGE_STATE_SCROLL, DIALOG_PAGE_STATE_END };

enum DialogBoxType {
    DIALOG_TYPE_ROTATE, // used in NPCs and level messages
    DIALOG_TYPE_ZOOM    // used in signposts and wall signs and etc
};

enum DialogMark { DIALOG_MARK_NONE = 0, DIALOG_MARK_DAKUTEN = 1, DIALOG_MARK_HANDAKUTEN = 2 };

#define DEFAULT_DIALOG_BOX_ANGLE 90.0f
#define DEFAULT_DIALOG_BOX_SCALE 19.0f

#if !defined(VERSION_JP) && !defined(VERSION_SH)
u8 gDialogCharWidths[256] = { // TODO: Is there a way to auto generate this?
    7, 7, 7,  7, 7, 7, 7, 7, 7, 7, 6,  6, 6, 6,  6, 6, 6, 6, 5, 6, 6, 5, 8, 8, 6, 6, 6, 6, 6, 5, 6, 6,
    8, 7, 6,  6, 6, 5, 5, 6, 5, 5, 6,  5, 4, 5,  5, 3, 7, 5, 5, 5, 6, 5, 5, 5, 5, 5, 7, 7, 5, 5, 4, 4,
    8, 6, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 8, 8, 8, 8, 7, 7, 6, 7, 7, 0, 0, 0, 0, 0, 0, 0,
#ifdef VERSION_EU
    6, 6, 6,  0, 6, 6, 6, 0, 0, 0, 0,  0, 0, 0,  0, 4, 5, 5, 5, 5, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0,
    5, 5, 5,  0, 6, 6, 6, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 5, 5, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 5, 6,
    0, 4, 4,  0, 0, 5, 5, 0, 0, 0, 0,  0, 0, 0,  0, 0,
#else
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 6,
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0,
#endif
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0,
#ifdef VERSION_EU
    7, 5, 10, 5, 9, 8, 4, 0, 0, 0, 0,  5, 5, 6,  5, 0,
#else
    7, 5, 10, 5, 9, 8, 4, 0, 0, 0, 0,  0, 0, 0,  0, 0,
#endif
    0, 0, 5,  7, 7, 6, 6, 8, 0, 8, 10, 6, 4, 10, 0, 0
};
#endif

u8 gDialogBoxState = DIALOG_STATE_OPENING;
f32 gDialogBoxOpenTimer = DEFAULT_DIALOG_BOX_ANGLE;
f32 gDialogBoxOpenLerp = DEFAULT_DIALOG_BOX_ANGLE; // Multithreading value.
f32 gDialogBoxScale = DEFAULT_DIALOG_BOX_SCALE;
s16 gDialogScrollOffsetY = 0;
s8 gDialogBoxType = DIALOG_TYPE_ROTATE;
s16 gDialogID = -1;
s16 gLastDialogPageStrPos = 0;
s16 gDialogTextPos = 0;
#ifdef VERSION_EU
s32 gInGameLanguage = 0;
#endif
s8 gDialogLineNum = 1;
s8 gLastDialogResponse = 0;
u8 gMenuHoldKeyIndex = 0;
u8 gMenuHoldKeyTimer = 0;
s32 gDialogResponse = 0;

inline void create_dl_identity_matrix(void) {
    Mtx *matrix = (Mtx *) alloc_display_list(sizeof(Mtx));

#ifndef GBI_FLOATS
    matrix->m[0][0] = 0x00010000;
    matrix->m[1][0] = 0x00000000;
    matrix->m[2][0] = 0x00000000;
    matrix->m[3][0] = 0x00000000;
    matrix->m[0][1] = 0x00000000;
    matrix->m[1][1] = 0x00010000;
    matrix->m[2][1] = 0x00000000;
    matrix->m[3][1] = 0x00000000;
    matrix->m[0][2] = 0x00000001;
    matrix->m[1][2] = 0x00000000;
    matrix->m[2][2] = 0x00000000;
    matrix->m[3][2] = 0x00000000;
    matrix->m[0][3] = 0x00000000;
    matrix->m[1][3] = 0x00000001;
    matrix->m[2][3] = 0x00000000;
    matrix->m[3][3] = 0x00000000;
#else
    guMtxIdent(matrix);
#endif

    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix),
              G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix),
              G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
}

void create_dl_translation_matrix(s8 pushOp, f32 x, f32 y, f32 z) {
    Mtx *matrix = (Mtx *) alloc_display_list(sizeof(Mtx));

    guTranslate(matrix, x, y, z);

    if (pushOp == MENU_MTX_PUSH)
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix),
                  G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);

    if (pushOp == MENU_MTX_NOPUSH)
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix),
                  G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
}

void create_dl_rotation_matrix(s8 pushOp, f32 a, f32 x, f32 y, f32 z) {
    Mtx *matrix = (Mtx *) alloc_display_list(sizeof(Mtx));

    guRotate(matrix, a, x, y, z);

    if (pushOp == MENU_MTX_PUSH)
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix),
                  G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);

    if (pushOp == MENU_MTX_NOPUSH)
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix),
                  G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
}

void create_dl_scale_matrix(s8 pushOp, f32 x, f32 y, f32 z) {
    Mtx *matrix = (Mtx *) alloc_display_list(sizeof(Mtx));

    guScale(matrix, x, y, z);

    if (pushOp == MENU_MTX_PUSH)
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix),
                  G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);

    if (pushOp == MENU_MTX_NOPUSH)
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix),
                  G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
}

void create_dl_ortho_matrix(void) {
    Mtx *matrix = (Mtx *) alloc_display_list(sizeof(Mtx));

    create_dl_identity_matrix();

    guOrtho(matrix, 0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, -10.0f, 10.0f, 1.0f);

    // Should produce G_RDPHALF_1 in Fast3D
    gSPPerspNormalize(gDisplayListHead++, 0xFFFF);

    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix),
              G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH)
}

static u8 *alloc_ia8_text_from_i1(u16 *in, s16 width, s16 height) {
    s32 inPos;
    u16 bitMask;
    u8 *out;
    s16 outPos = 0;

    out = alloc_display_list((u32) width * (u32) height);
    for (inPos = 0; inPos < (width * height) / 16; inPos++) {
        bitMask = 0x8000;

        while (bitMask != 0) {
            if (in[inPos] & bitMask) {
                out[outPos] = 0xFF;
            } else {
                out[outPos] = 0x00;
            }

            bitMask /= 2;
            outPos++;
        }
    }

    return out;
}

void render_generic_char(u8 c) {
    void **fontLUT;
    void *packedTexture;

    fontLUT = segmented_to_virtual(main_font_lut);
    packedTexture = segmented_to_virtual(fontLUT[c]);

    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_IA, G_IM_SIZ_16b, 1,
                       VIRTUAL_TO_PHYSICAL(packedTexture));
    gSPDisplayList(gDisplayListHead++, dl_ia_text_tex_settings);
}

void render_generic_charBIG(u8 c) {
    void **fontLUT;
    void *packedTexture;

    fontLUT = segmented_to_virtual(main_font_lut);
    packedTexture = segmented_to_virtual(fontLUT[c]);

    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_IA, G_IM_SIZ_16b, 1,
                       VIRTUAL_TO_PHYSICAL(packedTexture));
    gSPDisplayList(gDisplayListHead++, dl_ia_text_tex_settingsBIG);
}

#if !defined(VERSION_JP) && !defined(VERSION_SH)
struct MultiTextEntry {
    u8 length;
    u8 str[4];
};

#define TEXT_THE_RAW ASCII_TO_DIALOG('t'), ASCII_TO_DIALOG('h'), ASCII_TO_DIALOG('e'), 0x00
#define TEXT_YOU_RAW ASCII_TO_DIALOG('y'), ASCII_TO_DIALOG('o'), ASCII_TO_DIALOG('u'), 0x00

enum MultiStringIDs { STRING_THE, STRING_YOU };

/*
 * Place the multi-text string according to the ID passed. (US, EU)
 * 0: 'the'
 * 1: 'you'
 */
#ifdef VERSION_US
void render_multi_text_string(s8 multiTextID)
#elif defined(VERSION_EU)
void render_multi_text_string(s16 *xPos, s16 *yPos, s8 multiTextID)
#endif
{
    s8 i;
    struct MultiTextEntry textLengths[2] = {
        { 3, { TEXT_THE_RAW } },
        { 3, { TEXT_YOU_RAW } },
    };

    for (i = 0; i < textLengths[multiTextID].length; i++) {
#ifdef VERSION_US
        render_generic_char(textLengths[multiTextID].str[i]);
        create_dl_translation_matrix(
            MENU_MTX_NOPUSH, (f32) (gDialogCharWidths[textLengths[multiTextID].str[i]]), 0.0f, 0.0f);
#elif defined(VERSION_EU)
        render_generic_char_at_pos(*xPos, *yPos, textLengths[multiTextID].str[i]);
        *xPos += gDialogCharWidths[textLengths[multiTextID].str[i]];
#endif
    }
}
#endif

#if defined(VERSION_JP) || defined(VERSION_SH)
#define MAX_STRING_WIDTH 18
#else
#define MAX_STRING_WIDTH 16
#endif

/**
 * Prints a generic white string.
 * In JP/EU a IA1 texture is used but in US a IA4 texture is used.
 */
void print_generic_string(s16 x, s16 y, const u8 *str) {
    UNUSED s8 mark = DIALOG_MARK_NONE; // unused in EU
    s32 strPos = 0;
    u8 lineNum = 1;

    create_dl_translation_matrix(MENU_MTX_PUSH, x, y, 0.0f);

    while (str[strPos] != DIALOG_CHAR_TERMINATOR) {
        switch (str[strPos]) {
            case DIALOG_CHAR_DAKUTEN:
                mark = DIALOG_MARK_DAKUTEN;
                break;
            case DIALOG_CHAR_PERIOD_OR_HANDAKUTEN:
                mark = DIALOG_MARK_HANDAKUTEN;
                break;
            case DIALOG_CHAR_NEWLINE:
                gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
                create_dl_translation_matrix(MENU_MTX_PUSH, x, y - (lineNum * MAX_STRING_WIDTH), 0.0f);
                lineNum++;
                break;
            case DIALOG_CHAR_PERIOD:
                create_dl_translation_matrix(MENU_MTX_PUSH, -2.0f, -5.0f, 0.0f);
                render_generic_char(DIALOG_CHAR_PERIOD_OR_HANDAKUTEN);
                gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
                break;
            case DIALOG_CHAR_SLASH:
                create_dl_translation_matrix(
                    MENU_MTX_NOPUSH, (f32) (gDialogCharWidths[DIALOG_CHAR_SPACE] * 2), 0.0f, 0.0f);
                break;
            case DIALOG_CHAR_MULTI_THE:
                render_multi_text_string(STRING_THE);
                break;
            case DIALOG_CHAR_MULTI_YOU:
                render_multi_text_string(STRING_YOU);
                break;
            case DIALOG_CHAR_SPACE:
                create_dl_translation_matrix(MENU_MTX_NOPUSH,
                                             (f32) (gDialogCharWidths[DIALOG_CHAR_SPACE]), 0.0f, 0.0f);
                break; // ? needed to match
            default:
                render_generic_char(str[strPos]);
                if (mark != DIALOG_MARK_NONE) {
                    create_dl_translation_matrix(MENU_MTX_PUSH, 5.0f, 5.0f, 0.0f);
                    render_generic_char(mark + 0xEF);
                    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
                    mark = DIALOG_MARK_NONE;
                }
                create_dl_translation_matrix(MENU_MTX_NOPUSH, (f32) (gDialogCharWidths[str[strPos]]),
                                             0.0f, 0.0f);
                break; // what an odd difference. US added a useless break here.
        }
        strPos++;
    }
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void print_generic_stringBIG(s16 x, s16 y, const u8 *str) {
    UNUSED s8 mark = DIALOG_MARK_NONE; // unused in EU
    s32 strPos = 0;
    u8 lineNum = 1;

    create_dl_translation_matrix(MENU_MTX_PUSH, x, y, 0.0f);

    while (str[strPos] != DIALOG_CHAR_TERMINATOR) {
        switch (str[strPos]) {
            case DIALOG_CHAR_DAKUTEN:
                mark = DIALOG_MARK_DAKUTEN;
                break;
            case DIALOG_CHAR_PERIOD_OR_HANDAKUTEN:
                mark = DIALOG_MARK_HANDAKUTEN;
                break;
            case DIALOG_CHAR_NEWLINE:
                gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
                create_dl_translation_matrix(MENU_MTX_PUSH, x, y - (lineNum * MAX_STRING_WIDTH), 0.0f);
                lineNum++;
                break;
            case DIALOG_CHAR_PERIOD:
                create_dl_translation_matrix(MENU_MTX_PUSH, -2.0f, -5.0f, 0.0f);
                render_generic_charBIG(DIALOG_CHAR_PERIOD_OR_HANDAKUTEN);
                gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
                break;
            case DIALOG_CHAR_SLASH:
                create_dl_translation_matrix(
                    MENU_MTX_NOPUSH, (f32) (gDialogCharWidths[DIALOG_CHAR_SPACE] * 2), 0.0f, 0.0f);
                break;
            case DIALOG_CHAR_MULTI_THE:
                render_multi_text_string(STRING_THE);
                break;
            case DIALOG_CHAR_MULTI_YOU:
                render_multi_text_string(STRING_YOU);
                break;
            case DIALOG_CHAR_SPACE:
                create_dl_translation_matrix(MENU_MTX_NOPUSH,
                                             (f32) (gDialogCharWidths[DIALOG_CHAR_SPACE]), 0.0f, 0.0f);
                break; // ? needed to match
            default:
                render_generic_charBIG(str[strPos]);
                if (mark != DIALOG_MARK_NONE) {
                    create_dl_translation_matrix(MENU_MTX_PUSH, 7.5f, 7.5f, 0.0f);
                    render_generic_charBIG(mark + 0xEF);
                    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
                    mark = DIALOG_MARK_NONE;
                }
                create_dl_translation_matrix(MENU_MTX_NOPUSH,
                                             (f32) (gDialogCharWidths[str[strPos]] * 1.5f), 0.0f, 0.0f);
                break; // what an odd difference. US added a useless break here.
        }
        strPos++;
    }
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/**
 * Prints a hud string depending of the hud table list defined.
 */
void print_hud_lut_string(s8 hudLUT, s16 x, s16 y, const u8 *str) {
    s32 strPos = 0;
    void **hudLUT1 = segmented_to_virtual(menu_hud_lut); // Japanese Menu HUD Color font
    void **hudLUT2 = segmented_to_virtual(main_hud_lut); // 0-9 A-Z HUD Color Font
    u32 curX = x;
    u32 curY = y;

    u32 xStride; // X separation

    if (hudLUT == HUD_LUT_JPMENU) {
        xStride = 16;
    } else {          // HUD_LUT_GLOBAL
        xStride = 12; //? Shindou uses this.
    }

    while (str[strPos] != GLOBAR_CHAR_TERMINATOR) {
        if (str[strPos] == GLOBAL_CHAR_SPACE) {
            if (0) //! dead code
            {
            }
            curX += 8;
            ; //! useless statement
        } else {
            gDPPipeSync(gDisplayListHead++);

            if (hudLUT == HUD_LUT_JPMENU)
                gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1,
                                   hudLUT1[str[strPos]]);

            if (hudLUT == HUD_LUT_GLOBAL)
                gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1,
                                   hudLUT2[str[strPos]]);

            gSPDisplayList(gDisplayListHead++, dl_rgba16_load_tex_block);
            gSPTextureRectangle(gDisplayListHead++, curX << 2, curY << 2, (curX + 16) << 2,
                                (curY + 16) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

            curX += xStride;
        }
        strPos++;
    }
}

void print_menu_generic_string(s16 x, s16 y, const u8 *str) {
    UNUSED s8 mark = DIALOG_MARK_NONE; // unused in EU
    s32 strPos = 0;
    s32 curX = x;
    s32 curY = y;
    void **fontLUT = segmented_to_virtual(menu_font_lut);

    while (str[strPos] != DIALOG_CHAR_TERMINATOR) {
        switch (str[strPos]) {
#ifdef VERSION_EU
            case DIALOG_CHAR_UPPER_A_UMLAUT:
                print_menu_char_umlaut(curX, curY, ASCII_TO_DIALOG('A'));
                curX += gDialogCharWidths[str[strPos]];
                break;
            case DIALOG_CHAR_UPPER_U_UMLAUT:
                print_menu_char_umlaut(curX, curY, ASCII_TO_DIALOG('U'));
                curX += gDialogCharWidths[str[strPos]];
                break;
            case DIALOG_CHAR_UPPER_O_UMLAUT:
                print_menu_char_umlaut(curX, curY, ASCII_TO_DIALOG('O'));
                curX += gDialogCharWidths[str[strPos]];
                break;
#else
            case DIALOG_CHAR_DAKUTEN:
                mark = DIALOG_MARK_DAKUTEN;
                break;
            case DIALOG_CHAR_PERIOD_OR_HANDAKUTEN:
                mark = DIALOG_MARK_HANDAKUTEN;
                break;
#endif
            case DIALOG_CHAR_SPACE:
                curX += 4;
                break;
            default:
                gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_IA, G_IM_SIZ_8b, 1,
                                   fontLUT[str[strPos]]);
                gDPLoadSync(gDisplayListHead++);
                gDPLoadBlock(gDisplayListHead++, G_TX_LOADTILE, 0, 0, 8 * 8 - 1,
                             CALC_DXT(8, G_IM_SIZ_8b_BYTES));
                gSPTextureRectangle(gDisplayListHead++, curX << 2, curY << 2, (curX + 8) << 2,
                                    (curY + 8) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

#ifndef VERSION_EU
                if (mark != DIALOG_MARK_NONE) {
                    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_IA, G_IM_SIZ_8b, 1,
                                       fontLUT[mark + 0xEF]);
                    gDPLoadSync(gDisplayListHead++);
                    gDPLoadBlock(gDisplayListHead++, G_TX_LOADTILE, 0, 0, 8 * 8 - 1,
                                 CALC_DXT(8, G_IM_SIZ_8b_BYTES));
                    gSPTextureRectangle(gDisplayListHead++, (curX + 6) << 2, (curY - 7) << 2,
                                        (curX + 14) << 2, (curY + 1) << 2, G_TX_RENDERTILE, 0, 0,
                                        1 << 10, 1 << 10);

                    mark = DIALOG_MARK_NONE;
                }
#endif
#if defined(VERSION_JP) || defined(VERSION_SH)
                curX += 9;
#else
                curX += gDialogCharWidths[str[strPos]];
#endif
        }
        strPos++;
    }
}

void handle_menu_scrolling(s8 scrollDirection, s8 *currentIndex, s8 minIndex, s8 maxIndex) {
    u8 index = 0;

    if (scrollDirection == MENU_SCROLL_VERTICAL) {
        if (gPlayer1Controller->rawStickY > 60) {
            index++;
        }

        if (gPlayer1Controller->rawStickY < -60) {
            index += 2;
        }
    } else if (scrollDirection == MENU_SCROLL_HORIZONTAL) {
        if (gPlayer1Controller->rawStickX > 60) {
            index += 2;
        }

        if (gPlayer1Controller->rawStickX < -60) {
            index++;
        }
    }

    if (((index ^ gMenuHoldKeyIndex) & index) == 2) {
        if (currentIndex[0] == maxIndex) {
            //! Probably originally a >=, but later replaced with an == and an else statement.
            currentIndex[0] = maxIndex;
        } else {
            play_sound(SOUND_MENU_CHANGE_SELECT, gDefaultSoundArgs);
            currentIndex[0]++;
        }
    }

    if (((index ^ gMenuHoldKeyIndex) & index) == 1) {
        if (currentIndex[0] == minIndex) {
            // Same applies to here as above
        } else {
            play_sound(SOUND_MENU_CHANGE_SELECT, gDefaultSoundArgs);
            currentIndex[0]--;
        }
    }

    if (gMenuHoldKeyTimer == 10) {
        gMenuHoldKeyTimer = 8;
        gMenuHoldKeyIndex = 0;
    } else {
        gMenuHoldKeyTimer++;
        gMenuHoldKeyIndex = index;
    }

    if ((index & 3) == 0) {
        gMenuHoldKeyTimer = 0;
    }
}

// EU has both get_str_x_pos_from_center and get_str_x_pos_from_center_scale
// US and JP only implement one or the other
s16 get_str_x_pos_from_center(s16 centerPos, u8 *str, f32 scale) {
    s16 strPos = 0;
    f32 spacesWidth = 0.0f;

    while (str[strPos] != DIALOG_CHAR_TERMINATOR) {
        spacesWidth += gDialogCharWidths[str[strPos]] * (scale / 8.f);
        strPos++;
    }
    // return the x position of where the string starts as half the string's
    // length from the position of the provided center.
    return (s16) (centerPos - (s16) (spacesWidth / 2.0));
}

s16 get_string_width(u8 *str) {
    s16 strPos = 0;
    s16 width = 0;

    while (str[strPos] != DIALOG_CHAR_TERMINATOR) {
        width += gDialogCharWidths[str[strPos]];
        strPos++;
    }
    return width;
}

u8 gHudSymCoin[] = { GLYPH_COIN, GLYPH_SPACE };
u8 gHudSymX[] = { GLYPH_MULTIPLY, GLYPH_SPACE };

void print_hud_my_score_coins(s32 useCourseCoinScore, s8 fileNum, s8 courseNum, s16 x, s16 y) {
    u8 strNumCoins[4];
    s16 numCoins;

    if (!useCourseCoinScore) {
        numCoins = (u16) (save_file_get_max_coin_score(courseNum) & 0xFFFF);
    } else {
        numCoins = save_file_get_course_coin_score(fileNum, courseNum);
    }

    if (numCoins != 0) {
        print_hud_lut_string(HUD_LUT_GLOBAL, x, y, gHudSymCoin);
        print_hud_lut_string(HUD_LUT_GLOBAL, x + 16, y, gHudSymX);
        int_to_str(numCoins, strNumCoins);
        print_hud_lut_string(HUD_LUT_GLOBAL, x + 32, y, strNumCoins);
    }
}

void print_hud_my_score_stars(s8 fileNum, s8 courseNum, s16 x, s16 y) {
    u8 strStarCount[4];
    s16 starCount;
    u8 textSymStar[] = { GLYPH_STAR, GLYPH_SPACE };
    UNUSED u16 unused;
    u8 textSymX[] = { GLYPH_MULTIPLY, GLYPH_SPACE };

    starCount = save_file_get_course_star_count(fileNum, courseNum);

    if (starCount != 0) {
        print_hud_lut_string(HUD_LUT_GLOBAL, x, y, textSymStar);
        print_hud_lut_string(HUD_LUT_GLOBAL, x + 16, y, textSymX);
        int_to_str(starCount, strStarCount);
        print_hud_lut_string(HUD_LUT_GLOBAL, x + 32, y, strStarCount);
    }
}

void int_to_str(s32 num, u8 *dst) {
    s32 digit1;
    s32 digit2;
    s32 digit3;

    s8 pos = 0;

    if (num > 999) {
        dst[0] = 0x00;
        dst[1] = DIALOG_CHAR_TERMINATOR;
        return;
    }

    digit1 = num / 100;
    digit2 = (num - digit1 * 100) / 10;
    digit3 = (num - digit1 * 100) - (digit2 * 10);

    if (digit1 != 0) {
        dst[pos++] = digit1;
    }

    if (digit2 != 0 || digit1 != 0) {
        dst[pos++] = digit2;
    }

    dst[pos++] = digit3;
    dst[pos] = DIALOG_CHAR_TERMINATOR;
}

void create_dialog_box(s16 dialog) {
    if (gDialogID == -1) {
        gDialogID = dialog;
        gDialogBoxType = DIALOG_TYPE_ROTATE;
    }
}

void create_dialog_box_with_var(s16 dialog, s32 dialogVar) {
    if (gDialogID == -1) {
        gDialogID = dialog;
        gDialogVariable = dialogVar;
        gDialogBoxType = DIALOG_TYPE_ROTATE;
    }
}

void create_dialog_inverted_box(s16 dialog) {
    if (gDialogID == -1) {
        gDialogID = dialog;
        gDialogBoxType = DIALOG_TYPE_ZOOM;
    }
}

void create_dialog_box_with_response(s16 dialog) {
    if (gDialogID == -1) {
        gDialogID = dialog;
        gDialogBoxType = DIALOG_TYPE_ROTATE;
        gLastDialogResponse = 1;
    }
}

void reset_dialog_render_state(void) {
    level_set_transition(0, 0);

    if (gDialogBoxType == DIALOG_TYPE_ZOOM) {
        trigger_cutscene_dialog(2);
    }

    gDialogBoxScale = 19.0f;
    gDialogBoxOpenTimer = 90.0f;
    gDialogBoxOpenLerp = 90.0f;
    gDialogBoxState = DIALOG_STATE_OPENING;
    gDialogID = -1;
    gDialogTextPos = 0;
    gLastDialogResponse = 0;
    gLastDialogPageStrPos = 0;
    gDialogResponse = 0;
}

#define X_VAL1 -7.0f
#define Y_VAL1 5.0
#define Y_VAL2 5.0f

void render_dialog_box_type(struct DialogEntry *dialog, s8 linesPerBox) {
    create_dl_translation_matrix(MENU_MTX_NOPUSH, dialog->leftOffset, dialog->width, 0);
    if (!gMoveSpeed) {
        gDialogBoxOpenLerp = gDialogBoxOpenTimer;
    } else {
        if (gMoveSpeed == 1) {
            gDialogBoxOpenLerp = approach_pos(gDialogBoxOpenLerp, gDialogBoxOpenTimer);
        } else {
            gDialogBoxOpenLerp = approach_posF(gDialogBoxOpenLerp, gDialogBoxOpenTimer);
        }
    }
    switch (gDialogBoxType) {
        case DIALOG_TYPE_ROTATE: // Renders a dialog black box with zoom and rotation
            if (gDialogBoxState == DIALOG_STATE_OPENING || gDialogBoxState == DIALOG_STATE_CLOSING) {
                create_dl_scale_matrix(MENU_MTX_NOPUSH, 1.0 / gDialogBoxScale, 1.0 / gDialogBoxScale,
                                       1.0f);
                // convert the speed into angle
                create_dl_rotation_matrix(MENU_MTX_NOPUSH, gDialogBoxOpenLerp * 4.0f, 0, 0, 1.0f);
            }
            gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 150);
            break;
        case DIALOG_TYPE_ZOOM: // Renders a dialog white box with zoom
            if (gDialogBoxState == DIALOG_STATE_OPENING || gDialogBoxState == DIALOG_STATE_CLOSING) {
                create_dl_translation_matrix(MENU_MTX_NOPUSH, 65.0 - (65.0 / gDialogBoxScale),
                                             (40.0 / gDialogBoxScale) - 40, 0);
                create_dl_scale_matrix(MENU_MTX_NOPUSH, 1.0 / gDialogBoxScale, 1.0 / gDialogBoxScale,
                                       1.0f);
            }
            gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 150);
            break;
    }

    create_dl_translation_matrix(MENU_MTX_PUSH, X_VAL1, Y_VAL1, 0);
    create_dl_scale_matrix(MENU_MTX_NOPUSH, 1.1f, ((f32) linesPerBox / Y_VAL2) + 0.1, 1.0f);

    gSPDisplayList(gDisplayListHead++, dl_draw_text_bg_box);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void change_and_flash_dialog_text_color_lines(s8 colorMode, s8 lineNum) {
    u8 colorFade;

    if (colorMode == 1) {
        if (lineNum == 1) {
            gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
        } else {
            if (lineNum == gDialogLineNum) {
                colorFade = (gSineTable[gDialogColorFadeTimer >> 4] * 50.0f) + 200.0f;
                gDPSetEnvColor(gDisplayListHead++, colorFade, colorFade, colorFade, 255);
            } else {
                gDPSetEnvColor(gDisplayListHead++, 200, 200, 200, 255);
            }
        }
    } else {
        switch (gDialogBoxType) {
            case DIALOG_TYPE_ROTATE:
                break;
            case DIALOG_TYPE_ZOOM:
                gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 255);
                break;
        }
    }
}

#define X_VAL3 0.0f
#define Y_VAL3 16
void handle_dialog_scroll_page_state(s8 lineNum, s8 totalLines, s8 *pageState, s8 *xMatrix,
                                     s16 *linePos) {
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    if (lineNum == totalLines) {
        pageState[0] = DIALOG_PAGE_STATE_SCROLL;
        return;
    }
    create_dl_translation_matrix(MENU_MTX_PUSH, X_VAL3, 2 - (lineNum * Y_VAL3), 0);

    linePos[0] = 0;
    xMatrix[0] = 1;
}

void render_star_count_dialog_text(s8 *xMatrix, s16 *linePos) {
    s8 tensDigit = gDialogVariable / 10;
    s8 onesDigit = gDialogVariable - (tensDigit * 10); // remainder

    if (tensDigit != 0) {
        if (xMatrix[0] != 1) {
            create_dl_translation_matrix(
                MENU_MTX_NOPUSH, (f32) (gDialogCharWidths[DIALOG_CHAR_SPACE] * xMatrix[0]), 0, 0);
        }

        render_generic_char(tensDigit);
        create_dl_translation_matrix(MENU_MTX_NOPUSH, (f32) gDialogCharWidths[tensDigit], 0, 0);
        xMatrix[0] = 1;
        linePos[0]++;
    }

    if (xMatrix[0] != 1) {
        create_dl_translation_matrix(
            MENU_MTX_NOPUSH, (f32) (gDialogCharWidths[DIALOG_CHAR_SPACE] * (xMatrix[0] - 1)), 0, 0);
    }

    render_generic_char(onesDigit);
    create_dl_translation_matrix(MENU_MTX_NOPUSH, (f32) gDialogCharWidths[onesDigit], 0, 0);

    linePos[0]++;
    xMatrix[0] = 1;
}

#if !defined(VERSION_JP) && !defined(VERSION_SH)
#ifdef VERSION_EU
void render_multi_text_string_lines(s8 multiTextId, s8 lineNum, s8 linesPerBox, UNUSED s16 linePos,
                                    s8 lowerBound, struct DialogEntry *dialog)
#else
void render_multi_text_string_lines(s8 multiTextId, s8 lineNum, s16 *linePos, s8 linesPerBox,
                                    s8 xMatrix, s8 lowerBound)
#endif
{
    s8 i;
    struct MultiTextEntry textLengths[2] = {
        { 3, { TEXT_THE_RAW } },
        { 3, { TEXT_YOU_RAW } },
    };

    if (lineNum >= lowerBound && lineNum <= (lowerBound + linesPerBox)) {
#ifdef VERSION_US
        if (linePos[0] != 0 || (xMatrix != 1)) {
            create_dl_translation_matrix(MENU_MTX_NOPUSH,
                                         (gDialogCharWidths[DIALOG_CHAR_SPACE] * (xMatrix - 1)), 0, 0);
        }
#endif
        for (i = 0; i < textLengths[multiTextId].length; i++) {
#ifdef VERSION_EU
            render_generic_dialog_char_at_pos(dialog, gDialogX, gDialogY,
                                              textLengths[multiTextId].str[i]);
            gDialogX += gDialogCharWidths[textLengths[multiTextId].str[i]];
#else
            render_generic_char(textLengths[multiTextId].str[i]);
            create_dl_translation_matrix(MENU_MTX_NOPUSH,
                                         (gDialogCharWidths[textLengths[multiTextId].str[i]]), 0, 0);
#endif
        }
    }
#ifdef VERSION_US
    linePos += textLengths[multiTextId].length;
#endif
}
#endif

#ifdef VERSION_EU
void render_dialog_lowercase_diacritic(struct DialogEntry *dialog, u8 chr, u8 diacritic) {
    render_generic_dialog_char_at_pos(dialog, gDialogX, gDialogY, chr);
    render_generic_dialog_char_at_pos(dialog, gDialogX, gDialogY, diacritic + 0xE7);
    gDialogX += gDialogCharWidths[chr];
}

void render_dialog_uppercase_diacritic(struct DialogEntry *dialog, u8 chr, u8 diacritic) {
    render_generic_dialog_char_at_pos(dialog, gDialogX, gDialogY, chr);
    render_generic_dialog_char_at_pos(dialog, gDialogX, gDialogY - 4, diacritic + 0xE3);
    gDialogX += gDialogCharWidths[chr];
}
#endif

u32 ensure_nonnegative(s16 value) {
    if (value < 0) {
        value = 0;
    }

    return value;
}

f32 currLineSpeed = 0;
#define FRAMESPERLINE (155.f / 4.f)
void getLineSpeed(u8 *line) {
    currLineSpeed = 1.f;
    while ((*line != DIALOG_CHAR_NEWLINE) && (*line != DIALOG_CHAR_TERMINATOR)) {
        currLineSpeed++;
        line++;
    }
    currLineSpeed = currLineSpeed / FRAMESPERLINE;
}

extern const BehaviorScript bhvStateYosh[];
extern const BehaviorScript bhvCutsceneBowser[];
extern struct Object *objExists(const BehaviorScript *behavior);
void handle_dialog_text_and_pages(s8 colorMode, struct DialogEntry *dialog, s8 lowerBound) {
    u8 strChar;
    s8 lineNum = 1;
    s8 totalLines;
    s8 pageState = DIALOG_PAGE_STATE_NONE;
    s8 xMatrix = 1;
    s8 linesPerBox = dialog->linesPerBox;
    s16 strIdx;
    s16 linePos = 0;

    if (gStr == NULL)
        return;

    if (gDialogBoxState == DIALOG_STATE_HORIZONTAL) {
        // If scrolling, consider the number of lines for both
        // the current page and the page being scrolled to.
        totalLines = linesPerBox * 2 + 1;
    } else {
        totalLines = linesPerBox + 1;
    }

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    strIdx = gDialogTextPos;

    if (gDialogBoxState == DIALOG_STATE_HORIZONTAL) {
        create_dl_translation_matrix(MENU_MTX_NOPUSH, 0, (f32) gDialogScrollOffsetY, 0);
    }

    create_dl_translation_matrix(MENU_MTX_PUSH, X_VAL3, 2 - lineNum * Y_VAL3, 0);
#define SPEEDSCALE 0.4f
    if (colorMode == 2) {
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 0, 255);
    } else if (colorMode == 3) {
        gDPSetEnvColor(gDisplayListHead++, 255, 127, 127, 255);
    } else {
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    }
    while (pageState == DIALOG_PAGE_STATE_NONE) {
        if (colorMode == 2) {
            if (objExists(bhvStateYosh)->oBirdSpeed <= strIdx) {
                gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
            }
        }
        if (colorMode == 3) {
            if (objExists(bhvCutsceneBowser)->oBirdSpeed <= strIdx) {
                gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
            }
        }
        change_and_flash_dialog_text_color_lines(colorMode, lineNum);
        strChar = gStr[strIdx];
        switch (strChar) {
            case DIALOG_CHAR_TERMINATOR:
                pageState = DIALOG_PAGE_STATE_END;
                gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
                break;
            case DIALOG_CHAR_NEWLINE:
                lineNum++;
                handle_dialog_scroll_page_state(lineNum, totalLines, &pageState, &xMatrix, &linePos);
                break;
            case DIALOG_CHAR_SPACE:
                xMatrix++;
                linePos++;

                break;
            case DIALOG_CHAR_SLASH:
                xMatrix += 2;
                linePos += 2;
                break;
            case DIALOG_CHAR_MULTI_THE:
                render_multi_text_string_lines(STRING_THE, lineNum, &linePos, linesPerBox, xMatrix,
                                               lowerBound);
                xMatrix = 1;
                break;
            case DIALOG_CHAR_MULTI_YOU:
                render_multi_text_string_lines(STRING_YOU, lineNum, &linePos, linesPerBox, xMatrix,
                                               lowerBound);
                xMatrix = 1;
                break;
            case DIALOG_CHAR_STAR_COUNT:
                render_star_count_dialog_text(&xMatrix, &linePos);
                break;
            default: // any other character
                if (lineNum >= lowerBound && lineNum <= lowerBound + linesPerBox) {
                    if (linePos || xMatrix != 1) {
                        create_dl_translation_matrix(
                            MENU_MTX_NOPUSH,
                            (f32) (gDialogCharWidths[DIALOG_CHAR_SPACE] * (xMatrix - 1)), 0, 0);
                    }

                    render_generic_char(strChar);
                    create_dl_translation_matrix(MENU_MTX_NOPUSH, (f32) (gDialogCharWidths[strChar]), 0,
                                                 0);
                    xMatrix = 1;
                    linePos++;
                }
        }
        strIdx++;
    }
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);

    if (gDialogBoxState == DIALOG_STATE_VERTICAL) {
        if (pageState == DIALOG_PAGE_STATE_END) {
            gLastDialogPageStrPos = -1;
        } else {
            gLastDialogPageStrPos = strIdx;
        }
    }

    gLastDialogLineNum = lineNum;
}

s32 dialog_text_logic(s8 colorMode, struct DialogEntry *dialog, s8 lowerBound) {
    u8 strChar;
    s8 lineNum = 1;
    s8 totalLines;
    s8 pageState = DIALOG_PAGE_STATE_NONE;
    s8 xMatrix = 1;
    s8 linesPerBox = dialog->linesPerBox;
    s16 strIdx;
    s16 linePos = 0;

    gStr = segmented_to_virtual(dialog->str);

    if (gStr == NULL)
        return;

    if (gDialogBoxState == DIALOG_STATE_HORIZONTAL) {
        totalLines = linesPerBox * 2 + 1;
    } else {
        totalLines = linesPerBox + 1;
    }

    strIdx = gDialogTextPos;

#define SPEEDSCALE 0.4f
    if (colorMode == 2) {
        getLineSpeed(&gStr[strIdx]);
    }
    if (colorMode == 3) {
        getLineSpeed(&gStr[strIdx]);
        currLineSpeed *= 2.5f;
        if (bowsertextID) {
            currLineSpeed *= SPEEDSCALE;
        }
    }
    while (pageState == DIALOG_PAGE_STATE_NONE) {
        strChar = gStr[strIdx];
        switch (strChar) {
            case DIALOG_CHAR_TERMINATOR:
                pageState = DIALOG_PAGE_STATE_END;
                gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
                break;
            case DIALOG_CHAR_NEWLINE:
                lineNum++;
                handle_dialog_scroll_page_state(lineNum, totalLines, &pageState, &xMatrix, &linePos);
                if (colorMode == 2) {
                    if (objExists(bhvStateYosh)->oBirdSpeed >= strIdx) {
                        getLineSpeed(&gStr[strIdx + 1]);
                    }
                }
                break;
            case DIALOG_CHAR_SPACE:
                xMatrix++;
                linePos++;

                break;
            case DIALOG_CHAR_SLASH:
                xMatrix += 2;
                linePos += 2;
                break;
            case DIALOG_CHAR_MULTI_THE:
                xMatrix = 1;
                break;
            case DIALOG_CHAR_MULTI_YOU:
                xMatrix = 1;
                break;
            case DIALOG_CHAR_STAR_COUNT:
                break;
            default: // any other character
                if (lineNum >= lowerBound && lineNum <= lowerBound + linesPerBox) {
                    xMatrix = 1;
                    linePos++;
                }
        }
        strIdx++;
    }
    if (colorMode == 2) {
        if (!gMoveSpeed) {
            objExists(bhvStateYosh)->oBirdSpeed += currLineSpeed;
        } else {
            if (gMoveSpeed == 1) {
                objExists(bhvStateYosh)->oBirdSpeed += currLineSpeed * .5f;
            } else {
                objExists(bhvStateYosh)->oBirdSpeed += currLineSpeed * 1.5f;
            }
        }
    }
    if (colorMode == 3) {
        if (!gMoveSpeed) {
            objExists(bhvCutsceneBowser)->oBirdSpeed += currLineSpeed;
        } else {
            if (gMoveSpeed == 1) {
                objExists(bhvCutsceneBowser)->oBirdSpeed += currLineSpeed * .5f;
            } else {
                objExists(bhvCutsceneBowser)->oBirdSpeed += currLineSpeed * 1.5f;
            }
        }
    }

    if (gDialogBoxState == DIALOG_STATE_VERTICAL) {
        if (pageState == DIALOG_PAGE_STATE_END) {
            gLastDialogPageStrPos = -1;
        } else {
            gLastDialogPageStrPos = strIdx;
        }
    }

    gLastDialogLineNum = lineNum;
}

#if defined(VERSION_JP) || defined(VERSION_SH)
#define X_VAL4_1 50
#define X_VAL4_2 25
#define Y_VAL4_1 1
#define Y_VAL4_2 20
#else
#define X_VAL4_1 56
#define X_VAL4_2 47
#define Y_VAL4_1 2
#define Y_VAL4_2 16
#endif

void render_dialog_triangle_choice(void) {
    if (gDialogBoxState == DIALOG_STATE_VERTICAL) {
        handle_menu_scrolling(MENU_SCROLL_HORIZONTAL, &gDialogLineNum, 1, 2);
    }

    create_dl_translation_matrix(MENU_MTX_NOPUSH, (gDialogLineNum * X_VAL4_1) - X_VAL4_2,
                                 Y_VAL4_1 - (gLastDialogLineNum * Y_VAL4_2), 0);

    if (gDialogBoxType == DIALOG_TYPE_ROTATE) {
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    } else {
        gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 255);
    }

    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
}

#define X_VAL5 118.0f
#define Y_VAL5_1 -16
#define Y_VAL5_2 5
#define X_Y_VAL6 0.8f

void render_dialog_string_color(s8 linesPerBox) {
    s32 timer = gGlobalTimer;

    if (timer & 0x08) {
        return;
    }

    create_dl_translation_matrix(MENU_MTX_PUSH, X_VAL5, (linesPerBox * Y_VAL5_1) + Y_VAL5_2, 0);
    create_dl_scale_matrix(MENU_MTX_NOPUSH, X_Y_VAL6, X_Y_VAL6, 1.0f);
    create_dl_rotation_matrix(MENU_MTX_NOPUSH, -DEFAULT_DIALOG_BOX_ANGLE, 0, 0, 1.0f);

    if (gDialogBoxType == DIALOG_TYPE_ROTATE) { // White Text
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    } else { // Black Text
        gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 255);
    }

    if (!objExists(bhvStateYosh)) {
        gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    }
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void handle_special_dialog_text(s16 dialogID) { // dialog ID tables, in order
    // King Bob-omb (Start), Whomp (Start), King Bob-omb (throw him out), Eyerock (Start), Wiggler
    // (Start)
    // Red Switch, Green Switch, Blue Switch, 100 coins star, Bowser Red Coin Star
    s16 dialogStarSound[] = { 10, 11, 12, 13, 14 };
    // King Bob-omb (Start), Whomp (Defeated), King Bob-omb (Defeated, missing in JP), Eyerock
    // (Defeated), Wiggler (Defeated)
    s16 i;

    for (i = 0; i < (s16) ARRAY_COUNT(dialogStarSound); i++) {
        if (dialogStarSound[i] == dialogID && gDialogLineNum == 1) {
            play_sound(SOUND_MENU_STAR_SOUND, gDefaultSoundArgs);
            return;
        }
    }
}

s16 gMenuMode = -1;

u16 gCutsceneMsgFade = 0;
s16 gCutsceneMsgIndex = -1;
s16 gCutsceneMsgDuration = -1;
s16 gCutsceneMsgTimer = 0;
s8 gDialogCameraAngleIndex = CAM_SELECTION_MARIO;
s8 gDialogCourseActNum = 1;

#if defined(VERSION_JP) || defined(VERSION_SH)
#define DIAG_VAL1 20
#define DIAG_VAL3 130
#define DIAG_VAL4 4
#else
#define DIAG_VAL1 16
#define DIAG_VAL3 132 // US & EU
#define DIAG_VAL4 5
#endif
#ifdef VERSION_EU
#define DIAG_VAL2 238
#else
#define DIAG_VAL2 240 // JP & US
#endif

void render_dialog_entries(void) {
    void **dialogTable;
    struct DialogEntry *dialog;
    s8 lowerBound;
    dialogTable = segmented_to_virtual(seg2_dialog_table);
    dialog = segmented_to_virtual(dialogTable[gDialogID]);
    render_dialog_box_type(dialog, dialog->linesPerBox);

    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, ensure_nonnegative(dialog->leftOffset),
                  ensure_nonnegative(DIAG_VAL2 - dialog->width),
                  ensure_nonnegative(DIAG_VAL3 + dialog->leftOffset),
                  ensure_nonnegative(240 + ((dialog->linesPerBox * 80) / DIAG_VAL4) - dialog->width));

    if (objExists(bhvStateYosh) && (objExists(bhvStateYosh)->oAction == 2)) {
        handle_dialog_text_and_pages(2, dialog, lwrBound);
        dialog_text_logic(2, dialog, lwrBound);
    } else {
        handle_dialog_text_and_pages(0, dialog, lwrBound);
        dialog_text_logic(0, dialog, lwrBound);
    }

    if (gLastDialogPageStrPos == -1 && gLastDialogResponse == 1) {
        render_dialog_triangle_choice();
    }
    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 2, 2, SCREEN_WIDTH_MATH / 2,
                  SCREEN_HEIGHT_MATH / 2);
    if (gLastDialogPageStrPos != -1 && gDialogBoxState == DIALOG_STATE_VERTICAL) {
        render_dialog_string_color(dialog->linesPerBox);
    }
}

// Calls a gMenuMode value defined by render_menus_and_dialogs cases
void set_menu_mode(s16 mode) {
    if (gMenuMode == -1) {
        gMenuMode = mode;
    }
}

void set_cutscene_message(s16 xOffset, s16 yOffset, s16 msgIndex, s16 msgDuration) {
    // is message done printing?
    if (gCutsceneMsgIndex == -1) {
        gCutsceneMsgIndex = msgIndex;
        gCutsceneMsgDuration = msgDuration;
        gCutsceneMsgTimer = 0;
        gCutsceneMsgXOffset = xOffset;
        gCutsceneMsgYOffset = yOffset;
        gCutsceneMsgFade = 0;
    }
}

#define STR_X 38
#define STR_Y 142
#define BOWSERTEXT 90
// "Dear Mario" message handler
void print_bowser_message(void) {
    void **dialogTable;
    struct DialogEntry *dialog;
    s8 lowerBound = 1;
    dialogTable = segmented_to_virtual(seg2_dialog_table);
    dialog = segmented_to_virtual(dialogTable[BOWSERTEXT + bowsertextID]);
    gDialogID = BOWSERTEXT + bowsertextID;
    create_dl_translation_matrix(MENU_MTX_NOPUSH, 0, -100 + bowsertextID * 6, -10);

    // render_dialog_box_type(dialog, dialog->linesPerBox);

    if (!gMoveSpeed) {
        gDialogBoxOpenLerp = gDialogBoxOpenTimer;
    } else {
        if (gMoveSpeed == 1) {
            gDialogBoxOpenLerp = approach_pos(gDialogBoxOpenLerp, gDialogBoxOpenTimer);
        } else {
            gDialogBoxOpenLerp = approach_posF(gDialogBoxOpenLerp, gDialogBoxOpenTimer);
        }
    }

    create_dl_translation_matrix(MENU_MTX_NOPUSH, dialog->leftOffset, dialog->width, 0);
    if (gDialogBoxState == DIALOG_STATE_OPENING || gDialogBoxState == DIALOG_STATE_CLOSING) {
        create_dl_scale_matrix(MENU_MTX_NOPUSH, 1.0 / gDialogBoxScale, 1.0 / gDialogBoxScale, 1.0f);
        // convert the speed into angle
        create_dl_rotation_matrix(MENU_MTX_NOPUSH, gDialogBoxOpenLerp * 4.0f, 0, 0, 1.0f);
    }
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 200);

    create_dl_translation_matrix(MENU_MTX_PUSH, X_VAL1, Y_VAL1, 0);
    create_dl_scale_matrix(MENU_MTX_NOPUSH, 1.1f, ((f32) dialog->linesPerBox / Y_VAL2) + 0.1, 1.0f);
    gSPDisplayList(gDisplayListHead++, dl_draw_text_bg_box);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    handle_dialog_text_and_pages(3, dialog, lowerBound);
    dialog_text_logic(3, dialog, lwrBound);
    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 2, 2, SCREEN_WIDTH_MATH / 2,
                  SCREEN_HEIGHT_MATH / 2);
    if (gLastDialogPageStrPos != -1 && gDialogBoxState == DIALOG_STATE_VERTICAL) {
        render_dialog_string_color(dialog->linesPerBox);
    }
}

/**
 * Renders the cannon reticle when Mario is inside a cannon.
 * Formed by four triangles.
 */
extern Gfx *render_cannon_circle_base(void);
void render_hud_cannon_reticle(void) {
    create_dl_translation_matrix(MENU_MTX_PUSH, 160.0f, 120.0f, 0);

    gDPSetEnvColor(gDisplayListHead++, 50, 50, 50, 180);
    create_dl_translation_matrix(MENU_MTX_PUSH, -20.0f, -8.0f, 0);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    create_dl_translation_matrix(MENU_MTX_PUSH, 20.0f, 8.0f, 0);
    create_dl_rotation_matrix(MENU_MTX_NOPUSH, 180.0f, 0, 0, 1.0f);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    create_dl_translation_matrix(MENU_MTX_PUSH, 8.0f, -20.0f, 0);
    create_dl_rotation_matrix(MENU_MTX_NOPUSH, DEFAULT_DIALOG_BOX_ANGLE, 0, 0, 1.0f);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    create_dl_translation_matrix(MENU_MTX_PUSH, -8.0f, 20.0f, 0);
    create_dl_rotation_matrix(MENU_MTX_NOPUSH, -DEFAULT_DIALOG_BOX_ANGLE, 0, 0, 1.0f);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gSPDisplayList(gDisplayListHead++, render_cannon_circle_base());
}

void shade_screen(void) {
    create_dl_translation_matrix(MENU_MTX_PUSH, GFX_DIMENSIONS_FROM_LEFT_EDGE(0), SCREEN_HEIGHT, 0);

    create_dl_scale_matrix(MENU_MTX_NOPUSH, 2.6f, 3.4f, 1.0f);

    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 110);
    gSPDisplayList(gDisplayListHead++, dl_draw_text_bg_box);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

__attribute__((always_inline)) inline void print_animated_red_coin(s16 x, s16 y) {
    s32 timer = gGlobalTimer;

    create_dl_translation_matrix(MENU_MTX_PUSH, x, y, 0);
    create_dl_scale_matrix(MENU_MTX_NOPUSH, 0.2f, 0.2f, 1.0f);
    gDPSetRenderMode(gDisplayListHead++, G_RM_TEX_EDGE, G_RM_TEX_EDGE2);

    switch (timer & 0x0F) {
        case 0:
            gSPDisplayList(gDisplayListHead++, red0);
            break;
        case 1:
            gSPDisplayList(gDisplayListHead++, red1);
            break;
        case 2:
            gSPDisplayList(gDisplayListHead++, red2);
            break;
        case 3:
            gSPDisplayList(gDisplayListHead++, red3);
            break;
        case 4:
            gSPDisplayList(gDisplayListHead++, red4);
            break;
        case 5:
            gSPDisplayList(gDisplayListHead++, red5);
            break;
        case 6:
            gSPDisplayList(gDisplayListHead++, red6);
            break;
        case 7:
            gSPDisplayList(gDisplayListHead++, red7);
            break;
        case 8:
            gSPDisplayList(gDisplayListHead++, red8);
            break;
        case 9:
            gSPDisplayList(gDisplayListHead++, red9);
            break;
        case 10:
            gSPDisplayList(gDisplayListHead++, red10);
            break;
        case 11:
            gSPDisplayList(gDisplayListHead++, red11);
            break;
        case 12:
            gSPDisplayList(gDisplayListHead++, red12);
            break;
        case 13:
            gSPDisplayList(gDisplayListHead++, red13);
            break;
        case 14:
            gSPDisplayList(gDisplayListHead++, red14);
            break;
        case 15:
            gSPDisplayList(gDisplayListHead++, red15);
            break;
    }

    gDPSetRenderMode(gDisplayListHead++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void render_pause_red_coins(void) {
    s8 x;

    for (x = 0; x < gRedCoinsCollected; x++) {
        print_animated_red_coin(GFX_DIMENSIONS_FROM_RIGHT_EDGE(30) - x * 20, 16);
    }
}

#ifdef VERSION_EU
u8 gTextCourseArr[][7] = { { TEXT_COURSE }, { TEXT_COURSE_FR }, { TEXT_COURSE_DE } };
#endif

#if defined(VERSION_JP) || defined(VERSION_SH)
#define CRS_NUM_X1 93
#else
#define CRS_NUM_X1 100
#endif
#ifdef VERSION_EU
#define TXT_STAR_X 89
#define ACT_NAME_X 107
#define LVL_NAME_X 108
#define MYSCORE_X 48
#else
#define TXT_STAR_X 98
#define ACT_NAME_X 116
#define LVL_NAME_X 117
#define MYSCORE_X 62
#endif

void render_pause_my_score_coins(void) {
    u8 textCourse[] = { TEXT_COURSE };
    u8 textMyScore[] = { TEXT_MY_SCORE };
    u8 textStar[] = { TEXT_STAR };
    u8 textUnfilledStar[] = { TEXT_UNFILLED_STAR };

    u8 strCourseNum[4];
    void **courseNameTbl;
    u8 *courseName;
    void **actNameTbl;
    u8 *actName;
    u8 courseIndex;
    u8 starFlags;

    courseNameTbl = segmented_to_virtual(seg2_course_name_table);
    actNameTbl = segmented_to_virtual(seg2_act_name_table);

    courseIndex = gCurrCourseNum - 1;
    starFlags = save_file_get_star_flags(gCurrSaveFileNum - 1, gCurrCourseNum - 1);

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);

    if (courseIndex < COURSE_STAGES_COUNT) {
        print_hud_my_score_coins(1, gCurrSaveFileNum - 1, courseIndex, 178, 103);
        print_hud_my_score_stars(gCurrSaveFileNum - 1, courseIndex, 118, 103);
    }

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);

    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);

    if (courseIndex < COURSE_STAGES_COUNT
        && save_file_get_course_star_count(gCurrSaveFileNum - 1, courseIndex) != 0) {
        print_generic_string(MYSCORE_X, 121, textMyScore);
    }

    courseName = segmented_to_virtual(courseNameTbl[courseIndex]);
#define LEVELNAMEHEIGHT 157
    if (courseIndex < COURSE_STAGES_COUNT) {
        print_generic_string(63, LEVELNAMEHEIGHT, textCourse);
        int_to_str(gCurrCourseNum, strCourseNum);
        print_generic_string(CRS_NUM_X1, LEVELNAMEHEIGHT, strCourseNum);

        actName = segmented_to_virtual(actNameTbl[(gCurrCourseNum - 1) * 6 + gDialogCourseActNum - 1]);

        if (starFlags & (1 << (gDialogCourseActNum - 1))) {
            print_generic_string(TXT_STAR_X, LEVELNAMEHEIGHT - 17, textStar);
        } else {
            print_generic_string(TXT_STAR_X, LEVELNAMEHEIGHT - 17, textUnfilledStar);
        }
        print_generic_string(ACT_NAME_X, LEVELNAMEHEIGHT - 17, actName);
        print_generic_string(LVL_NAME_X, LEVELNAMEHEIGHT, &courseName[3]);
    } else {
        print_generic_string(get_str_x_pos_from_center(159, &courseName[3], 8.0f), LEVELNAMEHEIGHT,
                             &courseName[3]);
    }
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

#define TXT1_X 3
#define TXT2_X 119
#define Y_VAL7 2

#define X_VAL8 4
#define Y_VAL8 2

extern u8 unlimitFPS;
u8 menutimer = 0;
#define arrowoffset 110
inline void render_options(s16 x, s16 y, s8 *index, s16 yIndex) {
    u8 textOn[] = { TEXT_LAKITU_MARIO };
    u8 textOff[] = { TEXT_LAKITU_STOP };
    u8 textCameraAngleR[] = { TEXT_CAMERA_ANGLE_R };
    u8 textCameraAngleL[] = { TEXT_CAMERA_ANGLE_L };
    u8 textCameraAngleZ[] = { TEXT_CAMERA_ANGLE_Z };
    u8 textFPSLimit[] = { TEXT_FPS_LIMIT };
    u8 options[] = { TEXT_OPTIONS };
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);

    print_generic_string(x + 40, y + 30, options);

    print_generic_string(x + 10, y - 2, textCameraAngleR);
    print_generic_string(x + 10, y - 17, textCameraAngleL);
    print_generic_string(x + 10, y - 32, textCameraAngleZ);
    print_generic_string(x + 10, y - 47, textFPSLimit);

    if (newcam_invertX) {
        print_generic_string(x + arrowoffset, y - 2, textOn);
    } else {
        print_generic_string(x + arrowoffset, y - 2, textOff);
    }
    if (newcam_invertY) {
        print_generic_string(x + arrowoffset, y - 17, textOn);
    } else {
        print_generic_string(x + arrowoffset, y - 17, textOff);
    }
    if (newcam_auto) {
        print_generic_string(x + arrowoffset, y - 32, textOn);
    } else {
        print_generic_string(x + arrowoffset, y - 32, textOff);
    }
    if (unlimitFPS) {
        print_generic_string(x + arrowoffset, y - 47, textOn);
    } else {
        print_generic_string(x + arrowoffset, y - 47, textOff);
        gSPDisplayList(gDisplayListHead++, dl_ia_text_end);

        create_dl_translation_matrix(MENU_MTX_PUSH, x - X_VAL8,
                                     (y - ((index[0] - 1) * yIndex)) - Y_VAL8, 0);

        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
        gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
        gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    }
    create_dl_translation_matrix(MENU_MTX_PUSH, x - X_VAL8, (y - ((index[0] - 1) * yIndex)) - Y_VAL8,
                                 0);

    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    if ((abs(gPlayer1Controller->rawStickX) > 60) && !menutimer) {
        if (index[0] == 1) {
            newcam_invertX ^= 1;
            menutimer = 10;
            play_sound(SOUND_MENU_CHANGE_SELECT, gDefaultSoundArgs);
        } else if (index[0] == 2) {
            newcam_invertY ^= 1;
            menutimer = 10;
            play_sound(SOUND_MENU_CHANGE_SELECT, gDefaultSoundArgs);
        } else if (index[0] == 3) {
            newcam_auto ^= 1;
            menutimer = 10;
            play_sound(SOUND_MENU_CHANGE_SELECT, gDefaultSoundArgs);
        } else if (index[0] == 4) {
            unlimitFPS ^= 1;
            menutimer = 10;
            play_sound(SOUND_MENU_CHANGE_SELECT, gDefaultSoundArgs);
        }
    }
    if (menutimer) {
        menutimer--;
    }
}

u8 starTexts[][26] = { { STAR01 }, { STAR02 }, { STAR03 }, { STAR04 }, { STAR05 }, { STAR06 },
                       { STAR07 }, { STAR08 }, { STAR09 }, { STAR10 }, { STAR11 }, { STAR12 },
                       { STAR13 }, { STAR14 }, { STAR15 }, { STAR16 }, { STAR17 }, { STAR18 },
                       { STAR19 }, { STAR20 }, { STAR21 }, { STAR22 }, { STAR23 }, { STAR24 },
                       { STAR25 }, { STAR26 } };

// render next available star
void render_pause_course_options(s16 x, s16 y, s8 *index, s16 yIndex) {
    u8 textContinue[] = { TEXT_CONTINUE };
    u8 textExitCourse[] = { TEXT_EXIT_COURSE };
    u8 textNext[] = { TEXT_NEXT };
    u8 i, j;
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 4; j++) {
            if (!(save_file_get_star_flags(gCurrSaveFileNum - 1, gCurrCourseNum - 1 + i) & 1 << j)) {
                goto DONECHECK;
            }
        }
    }
DONECHECK:
    print_generic_string(x + 20, y + 38, textNext);
    print_generic_string(x + 20, y + 23, starTexts[i * 4 + j]);

    print_generic_string(x + 10, y - 2, textContinue);
    print_generic_string(x + 10, y - 17, textExitCourse);

    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);

    create_dl_translation_matrix(MENU_MTX_PUSH, x - X_VAL8, (y - ((index[0] - 1) * yIndex)) - Y_VAL8,
                                 0);

    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void render_pause_castle_menu_box(s16 x, s16 y) {
    create_dl_translation_matrix(MENU_MTX_PUSH, x - 78, y - 32, 0);
    create_dl_scale_matrix(MENU_MTX_NOPUSH, 1.2f, 0.8f, 1.0f);
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 105);
    gSPDisplayList(gDisplayListHead++, dl_draw_text_bg_box);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    create_dl_translation_matrix(MENU_MTX_PUSH, x + 6, y - 28, 0);
    create_dl_rotation_matrix(MENU_MTX_NOPUSH, DEFAULT_DIALOG_BOX_ANGLE, 0, 0, 1.0f);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    create_dl_translation_matrix(MENU_MTX_PUSH, x - 9, y - 101, 0);
    create_dl_rotation_matrix(MENU_MTX_NOPUSH, 270.0f, 0, 0, 1.0f);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void highlight_last_course_complete_stars(void) {
    u8 courseDone;

    if (gLastCompletedCourseNum == COURSE_NONE) {
        courseDone = 0;
    } else {
        courseDone = gLastCompletedCourseNum - 1;

        if (courseDone >= COURSE_STAGES_COUNT) {
            courseDone = COURSE_STAGES_COUNT;
        }
    }

    gDialogLineNum = courseDone;
}

void print_hud_pause_colorful_str(void) {
    u8 textPause[] = { TEXT_PAUSE };

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);

    print_hud_lut_string(HUD_LUT_GLOBAL, 123, 50, textPause);

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);
}

void render_pause_castle_course_stars(s16 x, s16 y, s16 fileNum, s16 courseNum) {
    s16 hasStar = 0;

    u8 str[COURSE_STAGES_COUNT * 2];

    u8 textStar[] = { TEXT_STAR };

    u8 starFlags = save_file_get_star_flags(fileNum, courseNum);
    u16 starCount = save_file_get_course_star_count(fileNum, courseNum);

    u16 nextStar = 0;

    if (starFlags & 0x40) {
        starCount--;
        print_generic_string(x + 89, y - 5, textStar);
    }

    while (hasStar != starCount) {
        if (starFlags & (1 << nextStar)) {
            str[nextStar * 2] = DIALOG_CHAR_STAR_FILLED;
            hasStar++;
        } else {
            str[nextStar * 2] = DIALOG_CHAR_STAR_OPEN;
        }

        str[nextStar * 2 + 1] = DIALOG_CHAR_SPACE;
        nextStar++;
    }

    if (starCount == nextStar && starCount != 6) {
        str[nextStar * 2] = DIALOG_CHAR_STAR_OPEN;
        str[nextStar * 2 + 1] = DIALOG_CHAR_SPACE;
        nextStar++;
    }

    str[nextStar * 2] = DIALOG_CHAR_TERMINATOR;

    print_generic_string(x + 14, y + 13, str);
}

void render_pause_castle_main_strings(s16 x, s16 y) {
    void **courseNameTbl = segmented_to_virtual(seg2_course_name_table);
    u8 textCoin[] = { TEXT_COIN_X };

    void *courseName;

    u8 strVal[8];
    s16 starNum = gDialogLineNum;

    handle_menu_scrolling(MENU_SCROLL_VERTICAL, &gDialogLineNum, -1, COURSE_STAGES_COUNT + 1);

    if (gDialogLineNum == COURSE_STAGES_COUNT + 1) {
        gDialogLineNum = 0;
    }

    if (gDialogLineNum == -1) {
        gDialogLineNum = COURSE_STAGES_COUNT;
    }

    if (gDialogLineNum != COURSE_STAGES_COUNT) {
        while (save_file_get_course_star_count(gCurrSaveFileNum - 1, gDialogLineNum) == 0) {
            if (gDialogLineNum >= starNum) {
                gDialogLineNum++;
            } else {
                gDialogLineNum--;
            }

            if (gDialogLineNum == COURSE_STAGES_COUNT || gDialogLineNum == -1) {
                gDialogLineNum = COURSE_STAGES_COUNT;
                break;
            }
        }
    }

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);

    if (gDialogLineNum < COURSE_STAGES_COUNT) {
        courseName = segmented_to_virtual(courseNameTbl[gDialogLineNum]);
        render_pause_castle_course_stars(x, y, gCurrSaveFileNum - 1, gDialogLineNum);
        print_generic_string(x + 34, y - 5, textCoin);
        int_to_str(save_file_get_course_coin_score(gCurrSaveFileNum - 1, gDialogLineNum), strVal);
        print_generic_string(x + 54, y - 5, strVal);
    } else {
        u8 textStarX[] = { TEXT_STAR_X };
        courseName = segmented_to_virtual(courseNameTbl[COURSE_MAX]);
        print_generic_string(x + 40, y + 13, textStarX);
        int_to_str(save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_BONUS_STAGES - 1,
                                                  COURSE_MAX - 1),
                   strVal);
        print_generic_string(x + 60, y + 13, strVal);
    }

    print_generic_string(x - 9, y + 30, courseName);

    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

s8 gCourseCompleteCoinsEqual = 0;
s32 gCourseDoneMenuTimer = 0;
s32 gCourseCompleteCoins = 0;
s8 gHudFlash = 0;

s16 render_pause_courses_and_castle(void) {
    u8 textR[] = { TEXT_NORMAL_UPCLOSE };
    u8 textL[] = { TEXT_NORMAL_FIXED };
    shade_screen();
    print_hud_pause_colorful_str();
    switch (gDialogBoxState) {
        case DIALOG_STATE_VERTICAL:
            // render_pause_my_score_coins();
            //  render_pause_red_coins();
#define OPTIONSX 99
#define OPTIONSY 98
            render_pause_course_options(OPTIONSX, OPTIONSY, &gDialogLineNum, 15);
            break;
        case DIALOG_STATE_HORIZONTAL:
            render_pause_castle_menu_box(160, 158);
            render_pause_castle_main_strings(104, 75);
            break;
        case DIALOG_STATE_VERTICAL + 128:
        case DIALOG_STATE_HORIZONTAL + 128:
            render_options(OPTIONSX - 10, OPTIONSY, &gDialogLineNum, 15);
            break;
    }
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
#define MIDOFFSET 80
    print_generic_string(160 + MIDOFFSET, OPTIONSY - 8, textR);
    print_generic_string(160 - MIDOFFSET - 16, OPTIONSY - 8, textL);

    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
    if (gDialogTextAlpha < 250) {
        gDialogTextAlpha += 25;
    }

    return 0;
}
void render_options(s16 x, s16 y, s8 *index, s16 yIndex);
#if defined(VERSION_JP) || defined(VERSION_SH)
#define TXT_HISCORE_X 112
#define TXT_HISCORE_Y 48
#define TXT_CONGRATS_X 60
#else
#define TXT_HISCORE_X 109
#define TXT_HISCORE_Y 36
#define TXT_CONGRATS_X 70
#endif

#define HUD_PRINT_HISCORE 0
#define HUD_PRINT_CONGRATULATIONS 1

void print_hud_course_complete_string(s8 str) {
#ifdef VERSION_EU
    u8 textHiScore[][15] = { { TEXT_HUD_HI_SCORE },
                             { TEXT_HUD_HI_SCORE_FR },
                             { TEXT_HUD_HI_SCORE_DE } };
    u8 textCongratulations[][16] = { { TEXT_HUD_CONGRATULATIONS },
                                     { TEXT_HUD_CONGRATULATIONS_FR },
                                     { TEXT_HUD_CONGRATULATIONS_DE } };
#else
    u8 textHiScore[] = { TEXT_HUD_HI_SCORE };
    u8 textCongratulations[] = { TEXT_HUD_CONGRATULATIONS };
#endif

    u8 colorFade = sins(gDialogColorFadeTimer) * 50.0f + 200.0f;

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, colorFade, colorFade, colorFade, 255);

    if (str == HUD_PRINT_HISCORE) {
#ifdef VERSION_EU
        print_hud_lut_string(HUD_LUT_GLOBAL,
                             get_str_x_pos_from_center_scale(160, textHiScore[gInGameLanguage], 12.0f),
                             36, textHiScore[gInGameLanguage]);
#else
        print_hud_lut_string(HUD_LUT_GLOBAL, TXT_HISCORE_X, TXT_HISCORE_Y, textHiScore);
#endif
    } else { // HUD_PRINT_CONGRATULATIONS
#ifdef VERSION_EU
        print_hud_lut_string(
            HUD_LUT_GLOBAL,
            get_str_x_pos_from_center_scale(160, textCongratulations[gInGameLanguage], 12.0f), 67,
            textCongratulations[gInGameLanguage]);
#else
        print_hud_lut_string(HUD_LUT_GLOBAL, TXT_CONGRATS_X, 67, textCongratulations);
#endif
    }

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);
}

void print_hud_course_complete_coins(s16 x, s16 y) {
    u8 courseCompleteCoinsStr[4];
    u8 hudTextSymCoin[] = { GLYPH_COIN, GLYPH_SPACE };
    u8 hudTextSymX[] = { GLYPH_MULTIPLY, GLYPH_SPACE };

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);

    print_hud_lut_string(HUD_LUT_GLOBAL, x, y, hudTextSymCoin);
    print_hud_lut_string(HUD_LUT_GLOBAL, x + 16, y, hudTextSymX);

    int_to_str(gCourseCompleteCoins, courseCompleteCoinsStr);
    print_hud_lut_string(HUD_LUT_GLOBAL, x + 32, y, courseCompleteCoinsStr);

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);

    if (gCourseCompleteCoins >= gHudDisplay.coins) {
        gCourseCompleteCoinsEqual = 1;
        gCourseCompleteCoins = gHudDisplay.coins;

        if (gGotFileCoinHiScore != 0) {
            print_hud_course_complete_string(HUD_PRINT_HISCORE);
        }
    } else {
        if ((gCourseDoneMenuTimer & 1) || gHudDisplay.coins > 70) {
            gCourseCompleteCoins++;
            play_sound(SOUND_MENU_YOSHI_GAIN_LIVES, gDefaultSoundArgs);
        }

        if (gHudDisplay.coins == gCourseCompleteCoins && gGotFileCoinHiScore != 0) {
            play_sound(SOUND_MENU_MARIO_CASTLE_WARP2, gDefaultSoundArgs);
        }
    }
}

void play_star_fanfare_and_flash_hud(s32 arg, u8 starNum) {
    if (gHudDisplay.coins == gCourseCompleteCoins && (gCurrCourseStarFlags & starNum) == 0
        && gHudFlash == 0) {
        play_star_fanfare();
        gHudFlash = arg;
    }
}

#ifdef VERSION_EU
#define TXT_NAME_X1 centerX
#define TXT_NAME_X2 centerX - 1
#else
#define TXT_NAME_X1 71
#define TXT_NAME_X2 69
#endif
#if defined(VERSION_JP) || defined(VERSION_SH)
#define CRS_NUM_X2 95
#define CRS_NUM_X3 93
#define TXT_CLEAR_X1 205
#define TXT_CLEAR_X2 203
#else
#define CRS_NUM_X2 104
#define CRS_NUM_X3 102
#define TXT_CLEAR_X1 get_string_width(name) + 81
#define TXT_CLEAR_X2 get_string_width(name) + 79
#endif

void render_course_complete_lvl_info_and_hud_str(void) {
#if defined(VERSION_JP) || defined(VERSION_SH)
    u8 textSymStar[] = { GLYPH_STAR, GLYPH_SPACE };
    u8 textCourse[] = { TEXT_COURSE };
    u8 textCatch[] = { TEXT_CATCH };
    u8 textClear[] = { TEXT_CLEAR };
#elif defined(VERSION_EU)
    UNUSED u8 textCatch[] = { TEXT_CATCH }; // unused in EU
    u8 textSymStar[] = { GLYPH_STAR, GLYPH_SPACE };
#define textCourse gTextCourseArr[gInGameLanguage]
#else
    u8 textCourse[] = { TEXT_COURSE };
    UNUSED u8 textCatch[] = { TEXT_CATCH }; // unused in US
    UNUSED u8 textClear[] = { TEXT_CLEAR };
    u8 textSymStar[] = { GLYPH_STAR, GLYPH_SPACE };
#endif

    void **actNameTbl;
    void **courseNameTbl;
    u8 *name;

    u8 strCourseNum[4];

#ifdef VERSION_EU
    s16 centerX;
    switch (gInGameLanguage) {
        case LANGUAGE_ENGLISH:
            actNameTbl = segmented_to_virtual(act_name_table_eu_en);
            courseNameTbl = segmented_to_virtual(course_name_table_eu_en);
            break;
        case LANGUAGE_FRENCH:
            actNameTbl = segmented_to_virtual(act_name_table_eu_fr);
            courseNameTbl = segmented_to_virtual(course_name_table_eu_fr);
            break;
        case LANGUAGE_GERMAN:
            actNameTbl = segmented_to_virtual(act_name_table_eu_de);
            courseNameTbl = segmented_to_virtual(course_name_table_eu_de);
            break;
    }
#else
    actNameTbl = segmented_to_virtual(seg2_act_name_table);
    courseNameTbl = segmented_to_virtual(seg2_course_name_table);
#endif

    if (gLastCompletedCourseNum <= COURSE_STAGES_MAX) {
        print_hud_course_complete_coins(118, 103);
        play_star_fanfare_and_flash_hud(1, 1 << (gLastCompletedStarNum - 1));

        if (gLastCompletedStarNum == 7) {
            name = segmented_to_virtual(actNameTbl[COURSE_STAGES_MAX * 6 + 1]);
        } else {
            name = segmented_to_virtual(
                actNameTbl[(gLastCompletedCourseNum - 1) * 6 + gLastCompletedStarNum - 1]);
        }

        gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
        int_to_str(gLastCompletedCourseNum, strCourseNum);
        gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, gDialogTextAlpha);
        print_generic_string(65, 165, textCourse);
        print_generic_string(CRS_NUM_X2, 165, strCourseNum);
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
        print_generic_string(63, 167, textCourse);
        print_generic_string(CRS_NUM_X3, 167, strCourseNum);
        gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
    } else if (gLastCompletedCourseNum == COURSE_BITDW || gLastCompletedCourseNum == COURSE_BITFS) {
        name = segmented_to_virtual(courseNameTbl[gLastCompletedCourseNum - 1]);
        gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
        gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, gDialogTextAlpha);
#ifdef VERSION_EU
        centerX = get_str_x_pos_from_center(153, name, 12.0f);
#endif
        print_generic_string(TXT_NAME_X1, 130, name);
#ifndef VERSION_EU
        print_generic_string(TXT_CLEAR_X1, 130, textClear);
#endif
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
        print_generic_string(TXT_NAME_X2, 132, name);
#ifndef VERSION_EU
        print_generic_string(TXT_CLEAR_X2, 132, textClear);
#endif
        gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
        print_hud_course_complete_string(HUD_PRINT_CONGRATULATIONS);
        print_hud_course_complete_coins(118, 111);
        play_star_fanfare_and_flash_hud(2, 0); //! 2 isn't defined, originally for key hud?
        return;
    } else if (gLastCompletedCourseNum == 77) {
        name = segmented_to_virtual(actNameTbl[COURSE_STAGES_MAX * 6]);
        print_hud_course_complete_coins(118, 103);
        play_star_fanfare_and_flash_hud(1, 1 << (gLastCompletedStarNum - 1));
    } else {
        name = segmented_to_virtual(actNameTbl[COURSE_STAGES_MAX * 6]);
        print_hud_course_complete_coins(118, 103);
        play_star_fanfare_and_flash_hud(1, 1 << (gLastCompletedStarNum - 1));
    }

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    print_hud_lut_string(HUD_LUT_GLOBAL, 55, 77, textSymStar);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, gDialogTextAlpha);
    print_generic_string(76, 145, name);
#if defined(VERSION_JP) || defined(VERSION_SH)
    print_generic_string(220, 145, textCatch);
#endif
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    print_generic_string(74, 147, name);
#if defined(VERSION_JP) || defined(VERSION_SH)
    print_generic_string(218, 147, textCatch);
#endif
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

#if defined(VERSION_JP) || defined(VERSION_SH)
#define TXT_SAVEOPTIONS_X x + 10
#elif defined(VERSION_US)
#define TXT_SAVEOPTIONS_X x + 12
#elif defined(VERSION_EU)
#define TXT_SAVEOPTIONS_X xOffset
#endif
#if defined(VERSION_JP) || defined(VERSION_SH)
#define TXT_SAVECONT_Y 2
#define TXT_SAVEQUIT_Y 18
#define TXT_CONTNOSAVE_Y 38
#else
#define TXT_SAVECONT_Y 0
#define TXT_SAVEQUIT_Y 20
#define TXT_CONTNOSAVE_Y 40
#endif

#ifdef VERSION_EU
#define X_VAL9 xOffset - 12
void render_save_confirmation(s16 y, s8 *index, s16 sp6e)
#else
#define X_VAL9 x
void render_save_confirmation(s16 x, s16 y, s8 *index, s16 sp6e)
#endif
{
#ifdef VERSION_EU
    u8 textSaveAndContinueArr[][24] = { { TEXT_SAVE_AND_CONTINUE },
                                        { TEXT_SAVE_AND_CONTINUE_FR },
                                        { TEXT_SAVE_AND_CONTINUE_DE } };
    u8 textSaveAndQuitArr[][22] = { { TEXT_SAVE_AND_QUIT },
                                    { TEXT_SAVE_AND_QUIT_FR },
                                    { TEXT_SAVE_AND_QUIT_DE } };
    u8 textContinueWithoutSaveArr[][27] = { { TEXT_CONTINUE_WITHOUT_SAVING },
                                            { TEXT_CONTINUE_WITHOUT_SAVING_FR },
                                            { TEXT_CONTINUE_WITHOUT_SAVING_DE } };
#define textSaveAndContinue textSaveAndContinueArr[gInGameLanguage]
#define textSaveAndQuit textSaveAndQuitArr[gInGameLanguage]
#define textContinueWithoutSave textContinueWithoutSaveArr[gInGameLanguage]
    s16 xOffset = get_str_x_pos_from_center(160, textContinueWithoutSaveArr[gInGameLanguage], 12.0f);
#else
    u8 textSaveAndContinue[] = { TEXT_SAVE_AND_CONTINUE };
    u8 textSaveAndQuit[] = { TEXT_SAVE_AND_QUIT };
    u8 textContinueWithoutSave[] = { TEXT_CONTINUE_WITHOUT_SAVING };
#endif

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);

    print_generic_string(TXT_SAVEOPTIONS_X, y + TXT_SAVECONT_Y, textSaveAndContinue);
    print_generic_string(TXT_SAVEOPTIONS_X, y - TXT_SAVEQUIT_Y, textSaveAndQuit);
    print_generic_string(TXT_SAVEOPTIONS_X, y - TXT_CONTNOSAVE_Y, textContinueWithoutSave);

    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);

    create_dl_translation_matrix(MENU_MTX_PUSH, X_VAL9, y - ((index[0] - 1) * sp6e), 0);

    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

s16 render_course_complete_screen(void) {
#ifdef VERSION_EU
    gInGameLanguage = eu_get_language();
#endif

    switch (gDialogBoxState) {
        case DIALOG_STATE_OPENING:
            render_course_complete_lvl_info_and_hud_str();
            break;
        case DIALOG_STATE_VERTICAL:
            shade_screen();
            render_course_complete_lvl_info_and_hud_str();
#ifdef VERSION_EU
            render_save_confirmation(86, &gDialogLineNum, 20);
#else
            render_save_confirmation(100, 86, &gDialogLineNum, 20);
#endif
            break;
    }

    return 0;
}

void setOptions() {
    if (gPlayer1Controller->buttonPressed & (R_TRIG | Z_TRIG)) {
        gDialogBoxState ^= 0x80;
        gDialogLineNum = 1;
        play_sound(SOUND_MENU_CHANGE_SELECT, gDefaultSoundArgs);
    }
}
u32 timeSinceOpen = 0;
s32 pause_menu_logic(void) {
    s32 num;
    switch (gDialogBoxState) {
        case DIALOG_STATE_OPENING:
            gDialogLineNum = 1;
            gDialogTextAlpha = 0;
            timeSinceOpen = 0;
            level_set_transition(-1, 0);
            play_sound(SOUND_MENU_PAUSE_HIGHPRIO, gDefaultSoundArgs);

            if (gCurrCourseNum >= COURSE_MIN && gCurrCourseNum <= COURSE_MAX) {
                gDialogCameraAngleIndex = CAM_SELECTION_MARIO;
                gDialogBoxState = DIALOG_STATE_VERTICAL;
            } else {
                highlight_last_course_complete_stars();
                gDialogBoxState = DIALOG_STATE_HORIZONTAL;
            }
            break;
        case DIALOG_STATE_VERTICAL:
            setOptions();
            handle_menu_scrolling(MENU_SCROLL_VERTICAL, &gDialogLineNum, 1, 2);
            timeSinceOpen++;
            if (gPlayer1Controller->buttonPressed & A_BUTTON
                || gPlayer1Controller->buttonPressed & START_BUTTON) {
                if (timeSinceOpen > 10) {
                    level_set_transition(0, 0);
                    play_sound(SOUND_MENU_PAUSE_2, gDefaultSoundArgs);
                    gDialogBoxState = DIALOG_STATE_OPENING;
                    gMenuMode = -1;
                    if (gDialogLineNum == 2) {
                        if (sTransitionTimer) {
                            num = 1;
                        } else {
                            num = 2;
                        }
                        // restart game and delete file TODO
                    } else {
                        num = 1;
                    }
                } else {
                    num = 1;
                    if (gDialogLineNum == 1) {
                        level_set_transition(0, 0);
                        play_sound(SOUND_MENU_PAUSE_2, gDefaultSoundArgs);
                        gDialogBoxState = DIALOG_STATE_OPENING;
                        gMenuMode = -1;
                    } else {
                        return 0;
                    }
                }

                return num;
            }
            break;
        case DIALOG_STATE_HORIZONTAL:
            setOptions();
            if (gPlayer1Controller->buttonPressed & A_BUTTON
                || gPlayer1Controller->buttonPressed & START_BUTTON) {
                level_set_transition(0, 0);
                play_sound(SOUND_MENU_PAUSE_2, gDefaultSoundArgs);
                gMenuMode = -1;
                gDialogBoxState = DIALOG_STATE_OPENING;

                return 1;
            }
            break;
        case DIALOG_STATE_VERTICAL + 128:
        case DIALOG_STATE_HORIZONTAL + 128:
            setOptions();
            handle_menu_scrolling(MENU_SCROLL_VERTICAL, &gDialogLineNum, 1, 4);
            if (gPlayer1Controller->buttonPressed & A_BUTTON
                || gPlayer1Controller->buttonPressed & START_BUTTON) {
                level_set_transition(0, 0);
                play_sound(SOUND_MENU_PAUSE_2, gDefaultSoundArgs);
                gMenuMode = -1;
                gDialogBoxState = DIALOG_STATE_OPENING;

                return 1;
            }
            break;
    }

    if (gDialogTextAlpha < 250) {
        gDialogTextAlpha += 25;
    }

    return 0;
}

s32 course_complete_logic(void) {
    s16 num;
    switch (gDialogBoxState) {
        case DIALOG_STATE_OPENING:
            if (gCourseDoneMenuTimer > 100 && gCourseCompleteCoinsEqual == 1) {
                gDialogBoxState = DIALOG_STATE_VERTICAL;
                level_set_transition(-1, 0);
                gDialogTextAlpha = 0;
                gDialogLineNum = 1;
            }
            break;
        case DIALOG_STATE_VERTICAL:
            handle_menu_scrolling(MENU_SCROLL_VERTICAL, &gDialogLineNum, 1, 3);
            if (gCourseDoneMenuTimer > 110
                && (gPlayer1Controller->buttonPressed & A_BUTTON
                    || gPlayer1Controller->buttonPressed & START_BUTTON)) {
                level_set_transition(0, 0);
                play_sound(SOUND_MENU_STAR_SOUND, gDefaultSoundArgs);
                gDialogBoxState = DIALOG_STATE_OPENING;
                gMenuMode = -1;
                num = gDialogLineNum;
                gCourseDoneMenuTimer = 0;
                gCourseCompleteCoins = 0;
                gCourseCompleteCoinsEqual = 0;
                gHudFlash = 0;

                return num;
            }
            break;
    }

    if (gDialogTextAlpha < 250) {
        gDialogTextAlpha += 25;
    }

    gCourseDoneMenuTimer++;

    return 0;
}

void bowser_text_logic(void) {
    void **dialogTable;
    struct DialogEntry *dialog;
    dialogTable = segmented_to_virtual(seg2_dialog_table);
    dialog = segmented_to_virtual(dialogTable[BOWSERTEXT + bowsertextID]);
    gDialogID = BOWSERTEXT + bowsertextID;

    switch (gDialogBoxState) {
        case DIALOG_STATE_OPENING:
            objExists(bhvCutsceneBowser)->oBirdSpeed = 0;
            if (gDialogBoxOpenTimer == DEFAULT_DIALOG_BOX_ANGLE) {
                play_dialog_sound(BOWSERTEXT + bowsertextID);
                play_sound(SOUND_MENU_MESSAGE_APPEAR, gDefaultSoundArgs);
            }

            if (gDialogBoxType == DIALOG_TYPE_ROTATE) {
                gDialogBoxOpenTimer -= 7.5;
                gDialogBoxScale -= 1.5;
            } else {
                gDialogBoxOpenTimer -= 10.0;
                gDialogBoxScale -= 2.0;
            }

            if (gDialogBoxOpenTimer == 0.0f) {
                gDialogBoxState = DIALOG_STATE_VERTICAL;
                gDialogLineNum = 1;
            }
            break;
        case DIALOG_STATE_VERTICAL:
            gDialogBoxOpenTimer = 0.0f;
            if (renderBowserText == 30) {
                gDialogBoxState = DIALOG_STATE_CLOSING;
            }
            break;
        case DIALOG_STATE_CLOSING:
            if (gDialogBoxOpenTimer >= 20.0f) {
                level_set_transition(0, 0);
                play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gDefaultSoundArgs);

                gDialogResponse = gDialogLineNum;
                gDialogID = -1;
                renderBowserText = 0;
                bowsertextID = 2;
                gDialogBoxState = DIALOG_STATE_OPENING;
                gDialogBoxOpenTimer = DEFAULT_DIALOG_BOX_ANGLE;
                gDialogBoxScale = DEFAULT_DIALOG_BOX_SCALE;
                gDialogScrollOffsetY = 0;
                gDialogBoxType = DIALOG_TYPE_ROTATE;
                return;
            }

            gDialogBoxOpenTimer = gDialogBoxOpenTimer + 10.0f;
            gDialogBoxScale = gDialogBoxScale + 2.0f;
            break;
    }
}
extern const BehaviorScript bhvUnagi[];
extern struct CutsceneJump *currentScene;
u16 boxTime = 0;
void dialog_logic(void) {
    void **dialogTable;
    struct DialogEntry *dialog;
    dialogTable = segmented_to_virtual(seg2_dialog_table);
    dialog = segmented_to_virtual(dialogTable[gDialogID]);
    switch (gDialogBoxState) {
        case DIALOG_STATE_OPENING:
            boxTime = 0;
            if (gDialogBoxOpenTimer == DEFAULT_DIALOG_BOX_ANGLE) {
                play_dialog_sound(gDialogID);
                play_sound(SOUND_MENU_MESSAGE_APPEAR, gDefaultSoundArgs);
            }

            if (gDialogBoxType == DIALOG_TYPE_ROTATE) {
                gDialogBoxOpenTimer -= 7.5;
                gDialogBoxScale -= 1.5;
            } else {
                gDialogBoxOpenTimer -= 10.0;
                gDialogBoxScale -= 2.0;
            }

            if (gDialogBoxOpenTimer == 0.0f) {
                gDialogBoxState = DIALOG_STATE_VERTICAL;
                gDialogLineNum = 1;
            }
            lwrBound = 1;
            break;
        case DIALOG_STATE_VERTICAL:
            gDialogBoxOpenTimer = 0.0f;
            gDialogBoxOpenLerp = 0.0f;
            if (objExists(bhvStateYosh) && (objExists(bhvStateYosh)->oAction == 2)) {
#define OPENTIMERCOUNT 10
#define BEATSPERTEXTBOXDMIVIDEDBY4 1
                if ((objExists(bhvStateYosh)->oTimer - OPENTIMERCOUNT)
                        % (155 * BEATSPERTEXTBOXDMIVIDEDBY4)
                    == 1) {
                    if (gLastDialogPageStrPos == -1) {
                        gDialogBoxState = DIALOG_STATE_CLOSING;
                    } else {
                        gDialogBoxState = DIALOG_STATE_HORIZONTAL;
                    }
                }
            } else {
                if (objExists(bhvUnagi)) {
                    boxTime++;
#define MAX_TIME_FOR_AUTO 111
                    if (boxTime == MAX_TIME_FOR_AUTO) {
                        boxTime = 0;
                        goto AdvanceDialog;
                    }
                } else {
                    if ((gPlayer1Controller->buttonPressed & A_BUTTON)
                        || (gPlayer1Controller->buttonPressed & B_BUTTON)) {
                    AdvanceDialog:
                        if (gLastDialogPageStrPos == -1) {
                            handle_special_dialog_text(gDialogID);
                            gDialogBoxState = DIALOG_STATE_CLOSING;
                        } else {
                            gDialogBoxState = DIALOG_STATE_HORIZONTAL;
                            play_sound(SOUND_MENU_MESSAGE_NEXT_PAGE, gDefaultSoundArgs);
                        }
                    }
                }
            }
            lwrBound = 1;
            break;
        case DIALOG_STATE_HORIZONTAL:
            gDialogScrollOffsetY += dialog->linesPerBox * 2;

            if (gDialogScrollOffsetY >= dialog->linesPerBox * DIAG_VAL1) {
                gDialogTextPos = gLastDialogPageStrPos;
                gDialogBoxState = DIALOG_STATE_VERTICAL;
                gDialogScrollOffsetY = 0;
            }
            lwrBound = (gDialogScrollOffsetY / 16) + 1;
            break;
        case DIALOG_STATE_CLOSING:
            if (gDialogBoxOpenTimer == 20.0f) {
                level_set_transition(0, 0);
                play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gDefaultSoundArgs);

                if (gDialogBoxType == DIALOG_TYPE_ZOOM) {
                    trigger_cutscene_dialog(2);
                }

                gDialogResponse = gDialogLineNum;
            }

            gDialogBoxOpenTimer = gDialogBoxOpenTimer + 10.0f;
            gDialogBoxScale = gDialogBoxScale + 2.0f;

            if (gDialogBoxOpenTimer == DEFAULT_DIALOG_BOX_ANGLE) {
                gDialogBoxState = DIALOG_STATE_OPENING;
                gDialogID = -1;
                gDialogTextPos = 0;
                gLastDialogResponse = 0;
                gLastDialogPageStrPos = 0;
                gDialogResponse = 0;
            }
            lwrBound = 1;
            break;
    }
}

void cutscene_handler_logic() {
    if (gCutsceneMsgIndex == -1) {
        return;
    }

    if (gCutsceneMsgTimer < 5) {
        gCutsceneMsgFade += 50;
    }

    if (gCutsceneMsgDuration + 5 < gCutsceneMsgTimer) {
        gCutsceneMsgFade -= 50;
    }

    if (gCutsceneMsgDuration + 10 < gCutsceneMsgTimer) {
        gCutsceneMsgIndex = -1;
        gCutsceneMsgFade = 0;
        gCutsceneMsgTimer = 0;
        return;
    }

    gCutsceneMsgTimer++;
}

s32 ingame_menu_logic(void) {
    s16 mode = 0;
    cutscene_handler_logic();
    if (gMenuMode != -1) {
        switch (gMenuMode) {
            case 0:
            case 1:
                mode = pause_menu_logic();
                break;
            case 2:
            case 3:
                mode = course_complete_logic();
                break;
        }

        gDialogColorFadeTimer = (s16) gDialogColorFadeTimer + 0x1000;

    } else if ((gDialogID != -1) && (!renderBowserText)) {
        dialog_logic();
    } else if (renderBowserText) {
        renderBowserText--;
        bowser_text_logic();
    }

    return mode;
}

extern s16 showStarHUD;
extern s16 starIDHUD;
s16 startextopacity = 0;
// Only case 1 and 2 are used

s16 render_menus_and_dialogs() {
    if (showStarHUD) {
        startextopacity = approach_s16_symmetric(startextopacity, 255, 5);
    } else {
        startextopacity = approach_s16_symmetric(startextopacity, 0, 5);
    }
    if (startextopacity) {
#define STARTEXTY 40
#define MIDDLE 163
        gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
        gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, startextopacity);
        print_generic_stringBIG(get_str_x_pos_from_center(MIDDLE, starTexts[starIDHUD], 12.0f) + 1,
                                STARTEXTY - 9, starTexts[starIDHUD]);
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, startextopacity);
        print_generic_stringBIG(get_str_x_pos_from_center(MIDDLE, starTexts[starIDHUD], 12.0f),
                                STARTEXTY - 8, starTexts[starIDHUD]);

        gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
    }
    if (gMenuMode != -1) {
        switch (gMenuMode) {
            case 0:
                render_pause_courses_and_castle();
                break;
            case 1:
                render_pause_courses_and_castle();
                break;
            case 2:
                render_course_complete_screen();
                break;
            case 3:
                render_course_complete_screen();
                break;
        }

    } else if ((gDialogID != -1) && (!renderBowserText)) {
        render_dialog_entries();
    } else if (renderBowserText) {
        print_bowser_message();
    }
}
