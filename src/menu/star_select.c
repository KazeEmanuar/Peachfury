#include "src/game/texscroll.h"
#include <PR/ultratypes.h>

#include "audio/external.h"
#include "behavior_data.h"
#include "engine/behavior_script.h"
#include "engine/graph_node.h"
#include "eu_translation.h"
#include "game/area.h"
#include "game/game_init.h"
#include "game/ingame_menu.h"
#include "game/level_update.h"
#include "game/memory.h"
#include "game/object_helpers.h"
#include "game/object_list_processor.h"
#include "game/save_file.h"
#include "game/segment2.h"
#include "game/segment7.h"
#include "sm64.h"
#include "star_select.h"
#include "text_strings.h"
#include "prevent_bss_reordering.h"
#include "game/OPT_FOR_SIZE.h"

/**
 * @file star_select.c
 * This file implements how the star select screen (act selector) function.
 * That includes handles what stars can be selected, star selector types,
 * strings, act values, and star selector model rendering if a star is collected or not.
 */

// Star Selector count models printed in the act selector menu.
static struct Object *sStarSelectorModels[8];

// The act the course is loaded as, affects whether some objects spawn.
static s8 sLoadedActNum;

// Number of obtained stars, excluding the coin star.
static u8 sObtainedStars;

// Total number of stars that appear in the act selector menu.
static s8 sVisibleStars;

// Act selected when the act menu is first opened.
static u8 sInitSelectedActNum;

// Index value of the act selected in the act menu.
s8 sSelectedActIndex = 0;

// Index value of the star that is selectable in the act menu.
// Excluding the next star, it doesn't count other transparent stars.
static s8 sSelectableStarIndex = 0;

// Act Selector menu timer that keeps counting until you choose an act.
static s32 sActSelectorMenuTimer = 0;

/**
 * Act Selector Star Type Loop Action
 * Defines a select type for a star in the act selector.
 */
extern u16 random_u16(void);
void bhv_act_selector_star_type_loop(void) {
    random_u16();
    gCurrentObject->oFloorHeight = -16000.f;
    switch (gCurrentObject->oStarSelectorType) {
        // If a star is not selected, don't rotate or change size
        case STAR_SELECTOR_NOT_SELECTED:
            gCurrentObject->oStarSelectorSize -= 0.1;
            if (gCurrentObject->oStarSelectorSize < 1.0) {
                gCurrentObject->oStarSelectorSize = 1.0;
            }
            gCurrentObject->oFaceAngleYaw = 0;
            break;
        // If a star is selected, rotate and slightly increase size
        case STAR_SELECTOR_SELECTED:
            gCurrentObject->oStarSelectorSize += 0.1;
            if (gCurrentObject->oStarSelectorSize > 1.3) {
                gCurrentObject->oStarSelectorSize = 1.3;
            }
            gCurrentObject->oFaceAngleYaw += 0x800;
            break;
        // If the 100 coin star is selected, rotate
        case STAR_SELECTOR_100_COINS:
            gCurrentObject->oFaceAngleYaw += 0x800;
            break;
    }
    
    //CuckyDev: Star Select transition
    if (gCurrentObject->oStarSelectorType != STAR_SELECTOR_100_COINS)
    {
        if (gCurrentObject->oStarSelectorTimer)
        {
            cur_obj_scale(0.0f);
            gCurrentObject->oStarSelectorTimer--;
        }
        else
        {
            cur_obj_scale(gCurrentObject->oStarSelectorSize);
            gCurrentObject->oPosZ *= 0.85f;
        }
    }
    else
    {
        gCurrentObject->oPosY += (-280.0f - gCurrentObject->oPosY) * 0.2f;
    }
}

/**
 * Renders the 100 coin star with an special star selector type.
 */
void render_100_coin_star(u8 stars) {
    if (stars & (1 << 6)) {
        // If the 100 coin star has been collected, create a new star selector next to the coin score.
        sStarSelectorModels[6] = spawn_object_abs_with_rot(
            gCurrentObject, 0, MODEL_STAR, bhvActSelectorStarType, 240, -340, 100, 0, 0, 0);
        sStarSelectorModels[6]->oStarSelectorSize = 0.8;
        sStarSelectorModels[6]->oStarSelectorType = STAR_SELECTOR_100_COINS;
    }
}

/**
 * Act Selector Init Action
 * Checks how many stars has been obtained in a course, to render
 * the correct star models, the 100 coin star and also handles
 * checks of what star should be next in sInitSelectedActNum.
 */
extern s8 checkMissionActive(u8 actNum);
extern u16 gRandomSeed16;
void bhv_act_selector_init(void) {
    s16 i = 0;
    s32 selectorModelIDs[10];
    struct Object *yosh;
    u8 stars = save_file_get_star_flags(gCurrSaveFileNum - 1, gCurrCourseNum - 1);
    gRandomSeed16 = 0; // reset RNG

    sVisibleStars = 0;
    while (i != sObtainedStars) {
        if (stars & (1 << sVisibleStars)) { // Star has been collected
            selectorModelIDs[sVisibleStars] = MODEL_STAR;
            i++;
        } else { // Star has not been collected
            selectorModelIDs[sVisibleStars] = MODEL_TRANSPARENT_STAR;
            // If this is the first star that has not been collected, set
            // the default selection to this star.
            if (!sInitSelectedActNum) {
                sInitSelectedActNum = sVisibleStars + 1;
                sSelectableStarIndex = sVisibleStars;
            }
        }
        sVisibleStars++;
    }

    // If the stars have been collected in order so far, show the next star.
    if (sVisibleStars == sObtainedStars && sVisibleStars != 6) {
        selectorModelIDs[sVisibleStars] = MODEL_TRANSPARENT_STAR;
        sInitSelectedActNum = sVisibleStars + 1;
        sSelectableStarIndex = sVisibleStars;
        sVisibleStars++;
    }

    // If all stars have been collected, set the default selection to the last star.
    if (sObtainedStars == 6) {
        sInitSelectedActNum = sVisibleStars;
    }

    // Render star selector objects
    for (i = 0; i < sVisibleStars; i++) {
        sStarSelectorModels[i] =
            spawn_object_abs_with_rot(gCurrentObject, 0, selectorModelIDs[i], bhvActSelectorStarType,
                                      75 + sVisibleStars * -75 + i * 152, 0, 1250, 0, 0, 0);
        sStarSelectorModels[i]->oStarSelectorTimer = i * 3;
        sStarSelectorModels[i]->oStarSelectorSize = 1.0f;
        if (selectorModelIDs[i] == MODEL_STAR) {
            if (checkMissionActive(i) != -1) {
                yosh =
                    spawn_object_abs_with_rot(gCurrentObject, 0, 0xf7, bhvYoshiActSelect,
                                              75 + sVisibleStars * -75 + i * 152, -500, 100, 0, 0, 0);
                yosh->oBehParams2ndByte = checkMissionActive(i);
                yosh->oOpacity = i;
            }
        }
    }

    render_100_coin_star(stars);
}

/**
 * Act Selector Loop Action
 * Handles star selector scrolling depending of what stars are
 * selectable, whenever all 6 stars are obtained or not.
 * Also handles 2 star selector types whenever the star is selected
 * or not, the types are defined in bhv_act_selector_star_type_loop.
 */
void bhv_act_selector_loop(void) {
    s8 i;
    u8 starIndexCounter;
    u8 stars = save_file_get_star_flags(gCurrSaveFileNum - 1, gCurrCourseNum - 1);

    if (sObtainedStars != 6) {
        // Sometimes, stars are not selectable even if they appear on the screen.
        // This code filters selectable and non-selectable stars.
        sSelectedActIndex = 0;
        handle_menu_scrolling(MENU_SCROLL_HORIZONTAL, &sSelectableStarIndex, 0, sObtainedStars);
        starIndexCounter = sSelectableStarIndex;
        for (i = 0; i < sVisibleStars; i++) {
            // Can the star be selected (is it either already completed or the first non-completed
            // mission)
            if ((stars & (1 << i)) || i + 1 == sInitSelectedActNum) {
                if (starIndexCounter
                    == 0) { // We have reached the sSelectableStarIndex-th selectable star.
                    sSelectedActIndex = i;
                    break;
                }
                starIndexCounter--;
            }
        }
    } else {
        // If all stars are collected then they are all selectable.
        handle_menu_scrolling(MENU_SCROLL_HORIZONTAL, &sSelectableStarIndex, 0, sVisibleStars - 1);
        sSelectedActIndex = sSelectableStarIndex;
    }

    // Star selector type handler
    for (i = 0; i < sVisibleStars; i++) {
        if (sSelectedActIndex == i) {
            sStarSelectorModels[i]->oStarSelectorType = STAR_SELECTOR_SELECTED;
        } else {
            sStarSelectorModels[i]->oStarSelectorType = STAR_SELECTOR_NOT_SELECTED;
        }
    }
}

#define XC 85
#define YC 195
// 0x03029400
static const Vtx vert_courseICON[] = {
    { { { -32 + XC, -32 + YC, 0 }, 0, { 0, 2016 }, { 0xff, 0xff, 0xff, 0xff } } },
    { { { 0 + XC, -32 + YC, 0 }, 0, { 992, 2016 }, { 0xff, 0xff, 0xff, 0xff } } },
    { { { 0 + XC, 32 + YC, 0 }, 0, { 992, 0 }, { 0xff, 0xff, 0xff, 0xff } } },
    { { { -32 + XC, 32 + YC, 0 }, 0, { 0, 0 }, { 0xff, 0xff, 0xff, 0xff } } },
    { { { 0 + XC, -32 + YC, 0 }, 0, { 1, 2016 }, { 0xff, 0xff, 0xff, 0xff } } },
    { { { 32 + XC, -32 + YC, 0 }, 0, { 1024, 2016 }, { 0xff, 0xff, 0xff, 0xff } } },
    { { { 32 + XC, 32 + YC, 0 }, 0, { 1024, 0 }, { 0xff, 0xff, 0xff, 0xff } } },
    { { { 0 + XC, 32 + YC, 0 }, 0, { 1, 0 }, { 0xff, 0xff, 0xff, 0xff } } },
};
extern ALIGNED8 const u8 course11[];
extern ALIGNED8 const u8 course12[];
extern ALIGNED8 const u8 course21[];
extern ALIGNED8 const u8 course22[];
extern ALIGNED8 const u8 course31[];
extern ALIGNED8 const u8 course32[];
extern ALIGNED8 const u8 course41[];
extern ALIGNED8 const u8 course42[];
extern ALIGNED8 const u8 course51[];
extern ALIGNED8 const u8 course52[];

Gfx textureTable[15][2] = {
    { gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course11),
      gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course12) },
    { gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course31),
      gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course32) },
    { gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course21),
      gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course22) },
    { gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course41),
      gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course42) },
    { gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course51),
      gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course52) },
    { gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course11),
      gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course12) },
    { gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course11),
      gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course12) },
    { gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course11),
      gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course12) },
    { gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course11),
      gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course12) },
    { gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course11),
      gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course12) },
    { gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course11),
      gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course12) },
    { gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course11),
      gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course12) },
    { gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course11),
      gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course12) },
    { gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course11),
      gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course12) },
    { gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course11),
      gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course12) },
};
// 0x03029480 - 0x03029530
Gfx dl_courseICON[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsDPSetRenderMode(G_RM_TEX_EDGE, G_RM_TEX_EDGE2),
    gsDPSetTextureFilter(G_TF_POINT),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(vert_courseICON, 8, 0),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR,
                G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD,
                G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course11),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, course12),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSP2Triangles(4, 5, 6, 0x0, 4, 6, 7, 0x0),
    gsSPEndDisplayList(),
};
/**
 * Print the course number selected with the wood rgba16 course texture.
 */
void print_course_number(void) {
    u8 courseNum[4];
    Gfx *DL = &dl_courseICON;
    dl_courseICON[11] = textureTable[gCurrCourseNum - 1][0];
    dl_courseICON[15] = textureTable[gCurrCourseNum - 1][1];

    // create_dl_translation_matrix(MENU_MTX_PUSH, 158.0f, -81.0f, 0.0f);

    // Full wood texture in JP & US, lower part of it on EU
    // gSPDisplayList(gDisplayListHead++, dl_menu_rgba16_wood_course);

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    gSPDisplayList(gDisplayListHead++, &dl_courseICON);

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);

    int_to_str(gCurrCourseNum, courseNum);

    if (gCurrCourseNum < 10) { // 1 digit number
                               //  print_hud_lut_string(HUD_LUT_GLOBAL, 152, 185, courseNum);
    } else {                   // 2 digit number
                               //   print_hud_lut_string(HUD_LUT_GLOBAL, 143, 185, courseNum);
    }
}

#if defined(VERSION_JP) || defined(VERSION_SH)
#define ACT_NAME_X 158
#else
#define ACT_NAME_X 163
#endif

/**
 * Print act selector strings, some with special checks.
 */
s16 get_str_x_pos_from_center2(s16 centerPos, u8 *str, f32 scale) {
    s16 strPos = 0;
    f32 spacesWidth = 0.0f;

    while (str[strPos] != DIALOG_CHAR_TERMINATOR) {
        spacesWidth += 12.f;
        strPos++;
    }
    return (s16)(centerPos - (s16)(spacesWidth / 2.0));
}

struct LevelNameRenderer {
    u8 line1X;
    u8 line1Y;
    u8 line1length;
    u8 line2X;
    u8 line2Y;
    u8 line2length;
    u8 line3X;
    u8 line3Y;
    u8 line3length;
};
#define XOFFSET 16
#define YOFFSET 2

struct LevelNameRenderer starSelects[15] = {
    { 108, 30, 5, 102, 49, 100, 0, 0, 0 }, { 108, 35, 9, 110, 57, 100, 0, 0, 0 },
    { 110, 32, 7, 108, 50, 100, 0, 0, 0 }, { 116, 25, 5, 134, 40, 5, 152, 55, 5 },
    { 108, 30, 10, 120, 51, 100, 0, 0, 0 },  { 80, 35, 5, 120, 51, 100, 0, 0, 0 },
    { 80, 35, 5, 120, 51, 100, 0, 0, 0 },  { 80, 35, 5, 120, 51, 100, 0, 0, 0 },
    { 80, 35, 5, 120, 51, 100, 0, 0, 0 },  { 80, 35, 5, 120, 51, 100, 0, 0, 0 },
    { 80, 35, 5, 120, 51, 100, 0, 0, 0 },  { 80, 35, 5, 120, 51, 100, 0, 0, 0 },
    { 80, 35, 5, 120, 51, 100, 0, 0, 0 },  { 80, 35, 5, 120, 51, 100, 0, 0, 0 },
    { 80, 35, 5, 120, 51, 100, 0, 0, 0 },
};

u16 letters[40] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
void renderCourseName(u8 *currLevelName) {
    u8 index = 0;
    u8 currLine = 0;
    u16 currX = starSelects[gCurrCourseNum - 1].line1X;
    u16 currY = starSelects[gCurrCourseNum - 1].line1Y;
    u8 lettersLeft = starSelects[gCurrCourseNum - 1].line1length;
    while (currLevelName[index] != 0xFF) {
        letters[index] = (currLevelName[index] << 8) + 0xff;
        if ((letters[index] / 256) < 0x40) {
            print_hud_lut_string(HUD_LUT_GLOBAL, currX, currY, &letters[index]);
        }
        currY -= YOFFSET;
        currX += XOFFSET;
        index++;
        lettersLeft--;
        if (!lettersLeft) {
            if (!currLine) {
                currX = starSelects[gCurrCourseNum - 1].line2X;
                currY = starSelects[gCurrCourseNum - 1].line2Y;
                lettersLeft = starSelects[gCurrCourseNum - 1].line2length;
                currLine++;
            } else {
                currX = starSelects[gCurrCourseNum - 1].line3X;
                currY = starSelects[gCurrCourseNum - 1].line3Y;
                lettersLeft = starSelects[gCurrCourseNum - 1].line3length;
            }
        }
    }
    //    print_hud_lut_string(HUD_LUT_GLOBAL, 80, 135, currLevelName);
}

void print_act_selector_strings(void) {
    unsigned char myScore[] = { TEXT_MYSCORE };

    u8 **levelNameTbl = segmented_to_virtual(seg2_course_name_table);
    u8 *currLevelName = segmented_to_virtual(levelNameTbl[gCurrCourseNum - 1]);
    u8 **actNameTbl = segmented_to_virtual(seg2_act_name_table);
    u8 *selectedActName;
    s16 lvlNameX;
    s16 actNameX;
    Mtx *mtx;
    s8 i;

    create_dl_ortho_matrix();

#define SCORE_Y_COORDINATE 16
#define SCORE_X_COORDINATE 99
    // Print the coin highscore.
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    print_hud_my_score_coins(1, gCurrSaveFileNum - 1, gCurrCourseNum - 1, 144,
                             221 - SCORE_Y_COORDINATE);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    // Print the "MY SCORE" text if the coin score is more than 0
    if (save_file_get_course_coin_score(gCurrSaveFileNum - 1, gCurrCourseNum - 1) != 0) {
        gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 255);
        print_generic_stringBIG(SCORE_X_COORDINATE + 1, SCORE_Y_COORDINATE - 1, myScore);
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
        print_generic_stringBIG(SCORE_X_COORDINATE, SCORE_Y_COORDINATE, myScore);
    }
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    mtx = alloc_display_list(sizeof(Mtx));
    if (mtx == NULL) {
        return;
    }
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx++), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    print_course_number();
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    lvlNameX = get_str_x_pos_from_center2(160, currLevelName + 3, 16.0f);
    // print_hud_lut_string(HUD_LUT_GLOBAL, lvlNameX, 35, currLevelName + 3);
    renderCourseName(currLevelName + 3);

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    if (sVisibleStars != 0) {
        selectedActName =
            segmented_to_virtual(actNameTbl[(gCurrCourseNum - 1) * 6 + sSelectedActIndex]);
#define ACT_NAME_Y 55
        actNameX = get_str_x_pos_from_center(ACT_NAME_X, selectedActName, 12.0f);
        gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 255);
        print_generic_stringBIG(actNameX, ACT_NAME_Y, selectedActName);
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
        print_generic_stringBIG(actNameX - 1, ACT_NAME_Y + 1, selectedActName);
    }

    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

extern Gfx selectborders_Cube_mesh_tri_0[]; // vert colored borders
extern Vtx selectborders_Cube_mesh_vtx_0[8];

extern Gfx selectborders_Cube_mesh_tri_3[]; // vert colors and scroll
extern Vtx selectborders_Cube_mesh_vtx_3[9];
int scrollTimer = 0;

u8 CourseRGBS[36][3][3] = { { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xff, 0x45, 0x00 }, { 0x7f, 0x22, 0x00 }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0x00, 0xFF, 0x00 }, { 0x00, 0x3f, 0x00 }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0x3f, 0x3f, 0xFF }, { 0xAF, 0x2f, 0x2f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0x00, 0x00, 0xCF }, { 0x00, 0x00, 0x4f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } },
                            { { 0xFF, 0xFF, 0xFF }, { 0x7f, 0x7f, 0x7f }, { 0x00, 0x00, 0x00 } } };

/**
 * Geo function that Print act selector strings.
 *!@bug: This geo function is missing the third param. Harmless in practice due to o32 convention.
 */
#ifdef AVOID_UB
Gfx *geo_act_selector_strings(s16 callContext, UNUSED struct GraphNode *node, UNUSED void *context) {
#else
Gfx *geo_act_selector_strings(s16 callContext, UNUSED struct GraphNode *node) {
#endif
    if (callContext == GEO_CONTEXT_RENDER) {
        print_act_selector_strings();
    }
    return NULL;
}

Gfx *geo_act_select_scroll(s16 callContext, UNUSED struct GraphNode *node) {
    Vtx *a = segmented_to_virtual(selectborders_Cube_mesh_vtx_3);
    int x = 0x0004;
    s8 i = 0;

    if (callContext == GEO_CONTEXT_RENDER) {
        scrollTimer++;
        if (scrollTimer == 0x0400) {
            scrollTimer = 0;
            x -= 0x1000;
        }
        for (i = 0; i < 9; i++) {
            a[i].n.tc[1] -= x;
        }

        for (i = 0; i < 9; i++) {
            if (a[i].v.ob[2] < -12) {
                a[i].v.flag = 1;
            }
            if (a[i].v.ob[2] < -750) {
                a[i].v.flag = 2;
            }
        }
        for (i = 0; i < 9; i++) {
            a[i].v.cn[0] = CourseRGBS[gCurrLevelNum][a[i].v.flag][0];
            a[i].v.cn[1] = CourseRGBS[gCurrLevelNum][a[i].v.flag][1];
            a[i].v.cn[2] = CourseRGBS[gCurrLevelNum][a[i].v.flag][2];
        }

        a = segmented_to_virtual(selectborders_Cube_mesh_vtx_0);
        for (i = 0; i < 8; i++) {
            if (absi(a[i].v.ob[1]) < 160) {
                a[i].v.flag = 1;
            }
        }
        for (i = 0; i < 8; i++) {
            a[i].v.cn[0] = CourseRGBS[gCurrLevelNum][a[i].v.flag][0];
            a[i].v.cn[1] = CourseRGBS[gCurrLevelNum][a[i].v.flag][1];
            a[i].v.cn[2] = CourseRGBS[gCurrLevelNum][a[i].v.flag][2];
        }
    }
    return NULL;
}
/**
 * Initiates act selector values before entering a main course.
 * Also load how much stars a course has, without counting the 100 coin star.
 */
s32 lvl_init_act_selector_values_and_stars(UNUSED s32 arg, UNUSED s32 unused) {
    u8 stars = save_file_get_star_flags(gCurrSaveFileNum - 1, gCurrCourseNum - 1);

    sLoadedActNum = 0;
    sInitSelectedActNum = 0;
    sVisibleStars = 0;
    sActSelectorMenuTimer = 0;
    sObtainedStars = save_file_get_course_star_count(gCurrSaveFileNum - 1, gCurrCourseNum - 1);

    // Don't count 100 coin star
    if (stars & (1 << 6)) {
        sObtainedStars--;
    }

    //! no return value
#ifdef AVOID_UB
    return 0;
#endif
}

extern u8 missionActive;
extern int savingtimer;
/**
 * Loads act selector button actions with selected act value checks.
 * Also updates objects and returns act number selected after is chosen.
 */
s32 lvl_update_obj_and_load_act_button_actions(UNUSED s32 arg, UNUSED s32 unused) {
        read_controller_inputs();
    if (sActSelectorMenuTimer >= 11) {
        // If any of these buttons are pressed, play sound and go to course act
        if ((gPlayer1Controller->buttonPressed & A_BUTTON)
            || (gPlayer1Controller->buttonPressed & START_BUTTON)
            || (gPlayer1Controller->buttonPressed & B_BUTTON)) {
            play_sound(SOUND_MENU_STAR_SOUND_LETS_A_GO, gDefaultSoundArgs);
            savingtimer = 0;
            save_file_set_statetimer();
            if (sInitSelectedActNum >= sSelectedActIndex + 1) {
                sLoadedActNum = sSelectedActIndex + 1;
            } else {
                sLoadedActNum = sInitSelectedActNum;
            }
            gDialogCourseActNum = sSelectedActIndex + 1;
            missionActive = checkMissionActive(sLoadedActNum - 1);
        }
    }

    area_update_objects();
    scroll_textures();
    sActSelectorMenuTimer++;
    return sLoadedActNum;
}
