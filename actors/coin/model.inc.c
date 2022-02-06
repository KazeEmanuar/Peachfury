ALIGNED8 static const u8 coin_CoinTurn__00000_ia8[] = {
#include "actors/coin/CoinFrame__00000.ia8.inc.c"
};

ALIGNED8 static const u8 coin_CoinTurn__00019_ia8[] = {
#include "actors/coin/CoinFrame__00009.ia8.inc.c"
};

ALIGNED8 static const u8 coin_CoinTurn__00038_ia8[] = {
#include "actors/coin/CoinFrame__00019.ia8.inc.c"
};

ALIGNED8 static const u8 coin_CoinTurn__00056_ia8[] = {
#include "actors/coin/CoinFrame__00028.ia8.inc.c"
};

ALIGNED8 static const u8 coin_CoinTurn__00075_ia8[] = {
#include "actors/coin/CoinFrame__00037.ia8.inc.c"
};

ALIGNED8 static const u8 coin_CoinTurn__00094_ia8[] = {
#include "actors/coin/CoinFrame__00047.ia8.inc.c"
};

ALIGNED8 static const u8 coin_CoinTurn__00113_ia8[] = {
#include "actors/coin/CoinFrame__00056.ia8.inc.c"
};

ALIGNED8 static const u8 coin_CoinTurn__00131_ia8[] = {
#include "actors/coin/CoinFrame__00066.ia8.inc.c"
};
ALIGNED8 static const u8 frame8[] = {
#include "actors/coin/CoinFrame__00075.ia8.inc.c"
};

ALIGNED8 static const u8 frame9[] = {
#include "actors/coin/CoinFrame__00085.ia8.inc.c"
};

ALIGNED8 static const u8 frame10[] = {
#include "actors/coin/CoinFrame__00095.ia8.inc.c"
};

ALIGNED8 static const u8 frame11[] = {
#include "actors/coin/CoinFrame__00104.ia8.inc.c"
};

ALIGNED8 static const u8 frame12[] = {
#include "actors/coin/CoinFrame__00113.ia8.inc.c"
};

ALIGNED8 static const u8 frame13[] = {
#include "actors/coin/CoinFrame__00123.ia8.inc.c"
};

ALIGNED8 static const u8 frame14[] = {
#include "actors/coin/CoinFrame__00132.ia8.inc.c"
};

ALIGNED8 static const u8 frame15[] = {
#include "actors/coin/CoinFrame__00141.ia8.inc.c"
};




Vtx coin_Circle_mesh_vtx_0[4] = {
	{{{-50, 0, 0},0, {-16, 2032},{0x00, 0x00, 0x00, 0x00}}},
	{{{50, 0, 0},0, {2032, 2032},{0x00, 0, 0x00, 0x00}}},
	{{{50, 100, 0},0, {2032, -16},{0x30, 0, 0x00, 0x00}}},
	{{{-50, 100, 0},0, {-16, -16},{0x30, 0x00, 0x00, 0x00}}},
};


Gfx coin_Circle_mesh[] = {
	gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
	gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, SHADE, 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, SHADE, 0, 0, 0, TEXEL0),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPTileSync(),
	gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0, 7, 0, G_TX_CLAMP | G_TX_NOMIRROR, 6, 0, G_TX_CLAMP | G_TX_NOMIRROR, 6, 0),
	gsDPLoadSync(),
	gsDPLoadTile(7, 0, 0, 252, 252),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0, 0, 0, G_TX_CLAMP | G_TX_NOMIRROR, 6, 0, G_TX_CLAMP | G_TX_NOMIRROR, 6, 0),
	gsDPSetTileSize(0, 0, 0, 252, 252),
	gsSPVertex(coin_Circle_mesh_vtx_0 + 0, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
	gsDPPipeSync(),
	gsSPEndDisplayList(),
};


#define YELLOWCOLOR gsDPSetPrimColor(0, 0, 0xff, 0xcc, 0, 255),
Gfx yellow0[] = {
	YELLOWCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00000_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx yellow1[] = {
	YELLOWCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00019_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx yellow2[] = {
	YELLOWCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00038_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx yellow3[] = {
	YELLOWCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00056_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx yellow4[] = {
	YELLOWCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00075_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx yellow5[] = {
	YELLOWCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00094_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx yellow6[] = {
	YELLOWCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00113_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx yellow7[] = {
	YELLOWCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00131_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx yellow8[] = {
	YELLOWCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx yellow9[] = {
	YELLOWCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame9),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx yellow10[] = {
	YELLOWCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame10),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx yellow11[] = {
	YELLOWCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame11),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx yellow12[] = {
	YELLOWCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame12),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx yellow13[] = {
	YELLOWCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame13),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx yellow14[] = {
	YELLOWCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame14),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx yellow15[] = {
	YELLOWCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame15),
    gsSPBranchList(coin_Circle_mesh),
};

#define REDCOLOR gsDPSetPrimColor(0, 0, 255, 64, 64, 255),
Gfx red0[] = {
	REDCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00000_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx red1[] = {
	REDCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00019_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx red2[] = {
	REDCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00038_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx red3[] = {
	REDCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00056_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx red4[] = {
	REDCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00075_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx red5[] = {
	REDCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00094_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx red6[] = {
	REDCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00113_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx red7[] = {
	REDCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00131_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx red8[] = {
	REDCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx red9[] = {
	REDCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame9),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx red10[] = {
	REDCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame10),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx red11[] = {
	REDCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame11),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx red12[] = {
	REDCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame12),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx red13[] = {
	REDCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame13),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx red14[] = {
	REDCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame14),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx red15[] = {
	REDCOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame15),
    gsSPBranchList(coin_Circle_mesh),
};

#define BLUECOLOR gsDPSetPrimColor(0, 0, 80, 125, 255, 255),
Gfx blue0[] = {
	BLUECOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00000_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx blue1[] = {
	BLUECOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00019_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx blue2[] = {
	BLUECOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00038_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx blue3[] = {
	BLUECOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00056_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx blue4[] = {
	BLUECOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00075_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx blue5[] = {
	BLUECOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00094_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx blue6[] = {
	BLUECOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00113_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx blue7[] = {
	BLUECOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_CoinTurn__00131_ia8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx blue8[] = {
	BLUECOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame8),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx blue9[] = {
	BLUECOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame9),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx blue10[] = {
	BLUECOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame10),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx blue11[] = {
	BLUECOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame11),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx blue12[] = {
	BLUECOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame12),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx blue13[] = {
	BLUECOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame13),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx blue14[] = {
	BLUECOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame14),
    gsSPBranchList(coin_Circle_mesh),
};

Gfx blue15[] = {
	BLUECOLOR
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, frame15),
    gsSPBranchList(coin_Circle_mesh),
};

