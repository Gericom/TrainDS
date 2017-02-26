#include <nitro.h>
#include <nnsys/fnd.h>
#include <nnsys/g3d.h>
#include "core.h"
#include "util.h"
#include "terrain.h"
#include "TerrainManager.h"
#include "terrain/track/TrackPieceEx.h"
#include "terrain/track/FlexTrack.h"
#include "terrain/scenery/SceneryObject.h"
#include "terrain/scenery/RCT2Tree1.h"
#include "managers/TerrainTextureManager.h"
#include "Map.h"

Map::Map()
	: mGridEnabled(FALSE), mGhostPiece(NULL)
{
	NNS_FND_INIT_LIST(&mTrackList, TrackPieceEx, mLink);
	NNS_FND_INIT_LIST(&mSceneryList, SceneryObject, mLink);
	mTerrainManager = new TerrainManager();
	mVtx = (uint8_t*)Util_LoadFileToBuffer("/data/map/terrain.hmap", NULL, false);
	mTextures = (uint8_t*)Util_LoadFileToBuffer("/data/map/terrain.tmap", NULL, false);

	mTerrainTextureManager = new TerrainTextureManager();

	//mLodLevels = new uint8_t[128 * 128];
	//MI_CpuFillFast(mLodLevels, 0xFFFFFFFF, 128 * 128);

	//mVtx = new uint8_t[128 * 128];
	//MI_CpuFillFast(mVtx, 0x80808080, 128 * 128);
	mNormals = new VecFx10[128 * 128];
	RecalculateNormals(0, 127, 0, 127);
}

Map::~Map()
{
	delete mTerrainManager;
}

#define Y_SCALE 768 //128

#define Y_OFFSET 128 //40 //100

#include <nitro/itcm_begin.h>

void Map::RecalculateNormals(int xstart, int xend, int zstart, int zend)
{
	for (int y = zstart; y < zend && y < 127; y++)
	{
		for (int x = xstart; x < xend && x < 127; x++)
		{
			fx32 hl = (mVtx[y * 128 + x - 1] - Y_OFFSET) * Y_SCALE; //TERRAIN(t, x - 1, z);
			fx32 hr = (mVtx[y * 128 + x + 1] - Y_OFFSET) * Y_SCALE; //TERRAIN(t, x + 1, z);
			fx32 hd = (mVtx[(y + 1) * 128 + x] - Y_OFFSET) * Y_SCALE; //TERRAIN(t, x, z + 1); /* Terrain expands towards -Z /
			fx32 hu = (mVtx[(y - 1) * 128 + x] - Y_OFFSET) * Y_SCALE; //TERRAIN(t, x, z - 1);
			VecFx32 norm = { hl - hr, 2 * FX32_ONE, hd - hu };

			VEC_Normalize(&norm, &norm);

			if (norm.x > GX_FX32_FX10_MAX) norm.x = GX_FX32_FX10_MAX;
			else if (norm.x < GX_FX32_FX10_MIN) norm.x = GX_FX32_FX10_MIN;
			if (norm.y > GX_FX32_FX10_MAX) norm.y = GX_FX32_FX10_MAX;
			else if (norm.y < GX_FX32_FX10_MIN) norm.y = GX_FX32_FX10_MIN;
			if (norm.z > GX_FX32_FX10_MAX) norm.z = GX_FX32_FX10_MAX;
			else if (norm.z < GX_FX32_FX10_MIN) norm.z = GX_FX32_FX10_MIN;

			mNormals[y * 128 + x] = GX_VECFX10(norm.x, norm.y, norm.z);
		}
	}
}

extern "C" void render_tile(VecFx10* pNorm, uint8_t* pVtx, int x, int y);

void Map::Render(int xstart, int xend, int zstart, int zend, int xstart2, int xend2, int zstart2, int zend2, bool picking, int selectedMapX, int selectedMapZ, VecFx32* camPos, VecFx32* camDir)
{
	texture_t* tex = mTerrainManager->GetTerrainTexture(0);
	/*G3_TexImageParam((GXTexFmt)tex->nitroFormat,       // use alpha texture
		GX_TEXGEN_TEXCOORD,    // use texcoord
		(GXTexSizeS)tex->nitroWidth,        // 16 pixels
		(GXTexSizeT)tex->nitroHeight,        // 16 pixels
		GX_TEXREPEAT_ST,     // no repeat
		GX_TEXFLIP_NONE,       // no flip
		GX_TEXPLTTCOLOR0_USE,  // use color 0 of the palette
		NNS_GfdGetTexKeyAddr(tex->texKey)     // the offset of the texture image
	);
	G3_TexPlttBase(NNS_GfdGetPlttKeyAddr(tex->plttKey), (GXTexFmt)tex->nitroFormat);*/

	//fx32 d = -(camDir->x * camPos->x + camDir->y * camPos->y + camDir->z * camPos->z);

	G3_Translate(-32 * FX32_ONE, 0, -32 * FX32_ONE);
	G3_PushMtx();
	{
		G3_Scale(FX32_ONE / 64 * FX32_ONE, Y_SCALE * FX32_ONE / 64, FX32_ONE / 64 * FX32_ONE);
		G3_Translate(0, -Y_OFFSET * 64, 0);
		//MI_CpuClearFast(mLodLevels, 128 * 128);
		int i = 0;
		for (int y = zstart; y < zend && y < 127; y++)
		{
			for (int x = xstart; x < xend && x < 127; x++)
			{
				VecFx32 diff = {
					x * FX32_ONE + FX32_HALF - camPos->x - 32 * FX32_ONE,
					(mVtx[y * 128 + x] - Y_OFFSET) * Y_SCALE - camPos->y,
					y * FX32_ONE + FX32_HALF - camPos->z - 32 * FX32_ONE
				};

				fx32 top = FX_Mul(camDir->x, diff.x) + FX_Mul(camDir->y, diff.y) + FX_Mul(camDir->z, diff.z);// +d; //VEC_DotProduct(camDir, &diff);*/
				//if (top < 0) top = 0;
				//fx32 dist = FX_Mul(diff.x, diff.x) + FX_Mul(diff.y, diff.y) + FX_Mul(diff.z, diff.z);
				//NOCASH_Printf("old: %d; new: %d", FX_Sqrt(dist), top);

				//fx32 dist = FX_Mul(top, top);//FX_Mul(diff.x, diff.x) + FX_Mul(diff.y, diff.y) + FX_Mul(diff.z, diff.z);
				if ((!picking && top <= (10 * FX32_ONE)) || (picking && top <= (20 * FX32_ONE)))
				{
					//mLodLevels[y * 128 + x] = 1;
					//mLodLevels[y * 128 + x + 1] = 1;
					//mLodLevels[(y + 1) * 128 + x] = 1;
					//mLodLevels[(y + 1) * 128 + x + 1] = 1;
					if (picking)
						G3_MaterialColorSpecEmi(0, PICKING_COLOR(PICKING_TYPE_MAP, i + 1), FALSE);
					/*else if (selectedMapX == x && selectedMapZ == y)
					{
						reg_G3X_GXFIFO = GX_PACK_OP(G3OP_DIF_AMB, G3OP_SPE_EMI, G3OP_POLYGON_ATTR, G3OP_NOP);
						{
							reg_G3X_GXFIFO = GX_PACK_DIFFAMB_PARAM(GX_RGB(0, 0, 0), GX_RGB(0, 0, 0), FALSE);
							reg_G3X_GXFIFO = GX_PACK_SPECEMI_PARAM(GX_RGB(0, 0, 0), GX_RGB(31, 31, 31), FALSE);
							reg_G3X_GXFIFO = GX_PACK_POLYGONATTR_PARAM(GX_LIGHTMASK_0, GX_POLYGONMODE_TOON, GX_CULL_NONE, 1, 31, GX_POLYGON_ATTR_MISC_FOG | GX_POLYGON_ATTR_MISC_FAR_CLIPPING);
						}
					}*/
					else if (mGridEnabled)
						G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, ((x & 1) ^ (y & 1)) << 1, 31, GX_POLYGON_ATTR_MISC_FOG | GX_POLYGON_ATTR_MISC_FAR_CLIPPING);
					
					if (!picking)
					{
						uint32_t texOffset = mTerrainTextureManager->GetTextureAddress(
							mTextures[y * 128 + x],
							mTextures[y * 128 + x + 1],
							mTextures[(y + 1) * 128 + x],
							mTextures[(y + 1) * 128 + x + 1]);
						G3_TexImageParam(GX_TEXFMT_DIRECT,       // use alpha texture
							GX_TEXGEN_TEXCOORD,    // use texcoord
							GX_TEXSIZE_S16,        // 16 pixels
							GX_TEXSIZE_T16,        // 16 pixels
							GX_TEXREPEAT_ST,     // no repeat
							GX_TEXFLIP_NONE,       // no flip
							GX_TEXPLTTCOLOR0_USE,  // use color 0 of the palette
							texOffset //NNS_GfdGetTexKeyAddr(tex->texKey)     // the offset of the texture image
						);
					}

/*#include <nitro/itcm_end.h>
					static u32 commandList[] =
					{
						GX_PACK_OP(G3OP_BEGIN, G3OP_TEXCOORD, G3OP_NORMAL, G3OP_VTX_10),
						GX_PACK_BEGIN_PARAM(GX_BEGIN_TRIANGLE_STRIP),
						GX_PACK_TEXCOORD_PARAM(0, 0),
						0,
						0,
						GX_PACK_OP(G3OP_TEXCOORD, G3OP_NORMAL, G3OP_VTX_10, G3OP_TEXCOORD),
						GX_PACK_TEXCOORD_PARAM(0, 16 * FX32_ONE),
						0,
						0,
						GX_PACK_TEXCOORD_PARAM(16 * FX32_ONE, 0),
						GX_PACK_OP(G3OP_NORMAL, G3OP_VTX_10, G3OP_TEXCOORD, G3OP_NORMAL),
						0,
						0,
						GX_PACK_TEXCOORD_PARAM(16 * FX32_ONE, 16 * FX32_ONE),
						0,
						GX_PACK_OP(G3OP_VTX_10, G3OP_END, G3OP_NOP, G3OP_NOP),
						0
					};
#include <nitro/itcm_begin.h>

					commandList[3] = mNormals[y * 128 + x];
					commandList[4] = GX_PACK_VTX10_PARAM(x << 6, (mVtx[y * 128 + x] - Y_OFFSET) << 6, y << 6);
					commandList[7] = mNormals[(y + 1) * 128 + x];
					commandList[8] = GX_PACK_VTX10_PARAM(x << 6, (mVtx[(y + 1) * 128 + x] - Y_OFFSET) << 6, (y << 6) + (1 << 6));
					commandList[11] = mNormals[y * 128 + (x + 1)];
					commandList[12] = GX_PACK_VTX10_PARAM((x << 6) + (1 << 6), (mVtx[y * 128 + (x + 1)] - Y_OFFSET) << 6, y << 6);
					commandList[14] = mNormals[(y + 1) * 128 + (x + 1)];
					commandList[16] = GX_PACK_VTX10_PARAM((x << 6) + (1 << 6), (mVtx[(y + 1) * 128 + (x + 1)] - Y_OFFSET) << 6, (y << 6) + (1 << 6));

					//MI_CpuSend32(&commandList[0], &reg_G3X_GXFIFO, sizeof(commandList));
					MI_SendGXCommandFast(3, &commandList[0], sizeof(commandList));*/

					//TODO: Include the loop in this function:
					render_tile(&mNormals[y * 128 + x], &mVtx[y * 128 + x], x, y);
					/*reg_G3X_GXFIFO = GX_PACK_OP(G3OP_BEGIN, G3OP_TEXCOORD, G3OP_NORMAL, G3OP_VTX_10);
					{
						reg_G3X_GXFIFO = GX_PACK_BEGIN_PARAM(GX_BEGIN_TRIANGLE_STRIP);
						reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM(0, 0);
						reg_G3X_GXFIFO = mNormals[y * 128 + x];
						reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM(x << 6, (mVtx[y * 128 + x] - Y_OFFSET) << 6, y << 6);
					}*/
					//reg_G3X_GXFIFO = GX_PACK_OP(G3OP_TEXCOORD, G3OP_NORMAL, G3OP_VTX_10, G3OP_TEXCOORD);
					//{
					//	reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM(0, 16 * FX32_ONE);
					//	reg_G3X_GXFIFO = mNormals[(y + 1) * 128 + x];
					//	reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM(x << 6, (mVtx[(y + 1) * 128 + x] - Y_OFFSET) << 6, (y << 6) + (1 << 6));
					//	reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM(16 * FX32_ONE, 0);
					//}
					//reg_G3X_GXFIFO = GX_PACK_OP(G3OP_NORMAL, G3OP_VTX_10, G3OP_TEXCOORD, G3OP_NORMAL);
					//{						
					//	reg_G3X_GXFIFO = mNormals[y * 128 + (x + 1)];
					//	reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM((x << 6) + (1 << 6), (mVtx[y * 128 + (x + 1)] - Y_OFFSET) << 6, y << 6);
					//	reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM(16 * FX32_ONE, 16 * FX32_ONE);
					//	reg_G3X_GXFIFO = mNormals[(y + 1) * 128 + (x + 1)];
					//}
					//reg_G3X_GXFIFO = GX_PACK_OP(G3OP_VTX_10, G3OP_END, G3OP_NOP, G3OP_NOP);
					//{
					//	reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM((x << 6) + (1 << 6), (mVtx[(y + 1) * 128 + (x + 1)] - Y_OFFSET) << 6, (y << 6) + (1 << 6));
					//}

					/*if (!picking && selectedMapX == x && selectedMapZ == y)
					{
						reg_G3X_GXFIFO = GX_PACK_OP(G3OP_DIF_AMB, G3OP_SPE_EMI, G3OP_POLYGON_ATTR, G3OP_NOP);
						{
							reg_G3X_GXFIFO = GX_PACK_DIFFAMB_PARAM(GX_RGB(31, 31, 31), /*GX_RGB(5, 5, 5)/GX_RGB(10, 10, 10), FALSE);
							reg_G3X_GXFIFO = GX_PACK_SPECEMI_PARAM(/*GX_RGB(3, 3, 3)/GX_RGB(1, 1, 1), GX_RGB(0, 0, 0), FALSE);
							reg_G3X_GXFIFO = GX_PACK_POLYGONATTR_PARAM(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_FOG | GX_POLYGON_ATTR_MISC_FAR_CLIPPING);
						}
					}*/
				}
				i++;
			}
		}
		if (!picking)
		{
			G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_FOG | GX_POLYGON_ATTR_MISC_FAR_CLIPPING);
			for (int y = zstart2 & ~1; y < (zend2 | 1) && y < 127; y += 2)
			{
				for (int x = xstart2 & ~1; x < (xend2 | 1) && x < 127; x += 2)
				{
					//fx32 diff_x = x * FX32_ONE + FX32_ONE - camPos->x - 32 * FX32_ONE;
					//fx32 diff_y = (mVtx[y * 128 + x] - Y_OFFSET) * Y_SCALE - camPos->y;
					//fx32 diff_z = y * FX32_ONE + FX32_ONE - camPos->z - 32 * FX32_ONE;
					//fx32 dist = FX_Mul(diff_x, diff_x) + FX_Mul(diff_y, diff_y) + FX_Mul(diff_z, diff_z);

					VecFx32 diff = {
						x * FX32_ONE + FX32_ONE - camPos->x - 32 * FX32_ONE,
						(mVtx[y * 128 + x] - Y_OFFSET) * Y_SCALE - camPos->y,
						y * FX32_ONE + FX32_ONE - camPos->z - 32 * FX32_ONE
					};

					fx32 top = VEC_DotProduct(camDir, &diff);

					//fx32 dist = FX_Mul(top, top);//FX_Mul(diff.x, diff.x) + FX_Mul(diff.y, diff.y) + FX_Mul(diff.z, diff.z);
					if (top >= /*(6 * 6 * FX32_ONE)*/(9 * FX32_ONE) && top <= (30 * FX32_ONE))
					{
						/*for (int y2 = 0; y2 < 3; y2++)
						{
							for (int x2 = 0; x2 < 3; x2++)
							{
								if (mLodLevels[(y + y2) * 128 + (x + x2)] & 1)
									goto skip_tile;
							}
						}
						for (int y2 = 0; y2 < 3; y2++)
						{
							for (int x2 = 0; x2 < 3; x2++)
							{
								mLodLevels[(y + y2) * 128 + (x + x2)] = 2;
							}
						}*/
						tex = mTerrainManager->GetTerrainTexture(mTextures[(y + 1) * 128 + x + 1]);
						G3_TexImageParam((GXTexFmt)tex->nitroFormat,       // use alpha texture
							GX_TEXGEN_TEXCOORD,    // use texcoord
							(GXTexSizeS)tex->nitroWidth,        // 16 pixels
							(GXTexSizeT)tex->nitroHeight,        // 16 pixels
							GX_TEXREPEAT_ST,     // no repeat
							GX_TEXFLIP_NONE,       // no flip
							GX_TEXPLTTCOLOR0_USE,  // use color 0 of the palette
							NNS_GfdGetTexKeyAddr(tex->texKey)     // the offset of the texture image
						);
						G3_TexPlttBase(NNS_GfdGetPlttKeyAddr(tex->plttKey), (GXTexFmt)tex->nitroFormat);

						/*uint32_t texOffset = mTerrainTextureManager->GetTextureAddress(
							mTextures[y * 128 + x],
							mTextures[y * 128 + x + 2],
							mTextures[(y + 2) * 128 + x],
							mTextures[(y + 2) * 128 + x + 2]);
						G3_TexImageParam(GX_TEXFMT_DIRECT,       // use alpha texture
							GX_TEXGEN_NONE,    // use texcoord
							GX_TEXSIZE_S16,        // 16 pixels
							GX_TEXSIZE_T16,        // 16 pixels
							GX_TEXREPEAT_ST,     // no repeat
							GX_TEXFLIP_NONE,       // no flip
							GX_TEXPLTTCOLOR0_USE,  // use color 0 of the palette
							texOffset //NNS_GfdGetTexKeyAddr(tex->texKey)     // the offset of the texture image
						);*/

/*#include <nitro/itcm_end.h>
						static u32 commandList2[] =
						{
							GX_PACK_OP(G3OP_MTX_PUSH, G3OP_MTX_TRANS, G3OP_BEGIN, G3OP_TEXCOORD),
							0,0,0,
							GX_PACK_BEGIN_PARAM(GX_BEGIN_TRIANGLE_STRIP),
							GX_PACK_TEXCOORD_PARAM(0, 0),
							GX_PACK_OP(G3OP_NORMAL, G3OP_VTX_10, G3OP_TEXCOORD, G3OP_NORMAL),
							0,
							0,
							0,
							0,
							GX_PACK_OP(G3OP_VTX_10, G3OP_TEXCOORD, G3OP_NORMAL, G3OP_VTX_10),
							0,
							0,
							0,
							0,
							GX_PACK_OP(G3OP_TEXCOORD, G3OP_NORMAL, G3OP_VTX_10, G3OP_END),
							0,
							0,
							0
						};
#include <nitro/itcm_begin.h>
						commandList2[1] = x * FX32_ONE;
						commandList2[3] = y * FX32_ONE;
						commandList2[7] = mNormals[y * 128 + x];
						commandList2[8] = GX_PACK_VTX10_PARAM(0, (mVtx[y * 128 + x] - Y_OFFSET) << 6, 0);
						commandList2[9] = GX_PACK_TEXCOORD_PARAM(0, (8 << tex->nitroHeight) * FX32_ONE * 2);
						commandList2[10] = mNormals[(y + 2) * 128 + x];
						commandList2[12] = GX_PACK_VTX10_PARAM(0, (mVtx[(y + 2) * 128 + x] - Y_OFFSET) << 6, 2 * FX32_ONE);
						commandList2[13] = GX_PACK_TEXCOORD_PARAM((8 << tex->nitroWidth) * FX32_ONE * 2, 0);
						commandList2[14] = mNormals[y * 128 + (x + 2)];
						commandList2[15] = GX_PACK_VTX10_PARAM(2 * FX32_ONE, (mVtx[y * 128 + (x + 2)] - Y_OFFSET) << 6, 0);
						commandList2[17] = GX_PACK_TEXCOORD_PARAM((8 << tex->nitroWidth) * FX32_ONE * 2, (8 << tex->nitroHeight) * FX32_ONE * 2);
						commandList2[18] = mNormals[(y + 2) * 128 + (x + 2)];
						commandList2[19] = GX_PACK_VTX10_PARAM(2 * FX32_ONE, (mVtx[(y + 2) * 128 + (x + 2)] - Y_OFFSET) << 6, 2 * FX32_ONE);

						MI_SendGXCommandFast(3, &commandList2[0], sizeof(commandList2));*/
						reg_G3X_GXFIFO = GX_PACK_OP(G3OP_BEGIN, G3OP_TEXCOORD, G3OP_NORMAL, G3OP_VTX_10);
						{
							reg_G3X_GXFIFO = GX_PACK_BEGIN_PARAM(GX_BEGIN_TRIANGLE_STRIP);
							reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM(0, 0);
							reg_G3X_GXFIFO = mNormals[y * 128 + x];
							reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM(x << 6, mVtx[y * 128 + x] << 6, y << 6);
						}
						reg_G3X_GXFIFO = GX_PACK_OP(G3OP_TEXCOORD, G3OP_NORMAL, G3OP_VTX_10, G3OP_TEXCOORD);
						{
							reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM(0, (8 << tex->nitroHeight) * FX32_ONE * 2);
							reg_G3X_GXFIFO = mNormals[(y + 2) * 128 + x];
							reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM(x << 6, mVtx[(y + 2) * 128 + x] << 6, (y << 6) + (2 << 6));
							reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM((8 << tex->nitroWidth) * FX32_ONE * 2, 0);
						}
						reg_G3X_GXFIFO = GX_PACK_OP(G3OP_NORMAL, G3OP_VTX_10, G3OP_TEXCOORD, G3OP_NORMAL);
						{
							reg_G3X_GXFIFO = mNormals[y * 128 + (x + 2)];
							reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM((x << 6) + (2 << 6), mVtx[y * 128 + (x + 2)] << 6, y << 6);
							reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM((8 << tex->nitroWidth) * FX32_ONE * 2, (8 << tex->nitroHeight) * FX32_ONE * 2);
							reg_G3X_GXFIFO = mNormals[(y + 2) * 128 + (x + 2)];
						}
						reg_G3X_GXFIFO = GX_PACK_OP(G3OP_VTX_10, G3OP_END, G3OP_NOP, G3OP_NOP);
						{
							reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM((x << 6) + (2 << 6), mVtx[(y + 2) * 128 + (x + 2)] << 6, (y << 6) + (2 << 6));
						}
					}
				}
			}
			/*for (int y = zstart2 & ~3; y < (zend2 | 3) && y < 127; y += 4)
			{
				for (int x = xstart2 & ~3; x < (xend2 | 3) && x < 127; x += 4)
				{
					//fx32 diff_x = x * FX32_ONE + 2 * FX32_ONE - camPos->x - 32 * FX32_ONE;
					//fx32 diff_y = (mVtx[y * 128 + x] - Y_OFFSET) * Y_SCALE - camPos->y;
					//fx32 diff_z = y * FX32_ONE + 2 * FX32_ONE - camPos->z - 32 * FX32_ONE;
					//fx32 dist = FX_Mul(diff_x, diff_x) + FX_Mul(diff_y, diff_y) + FX_Mul(diff_z, diff_z);
					VecFx32 diff = {
						x * FX32_ONE + 2 * FX32_ONE - camPos->x - 32 * FX32_ONE,
						(mVtx[y * 128 + x] - Y_OFFSET) * Y_SCALE - camPos->y,
						y * FX32_ONE + 2 * FX32_ONE - camPos->z - 32 * FX32_ONE
					};

					fx32 top = VEC_DotProduct(camDir, &diff);

					//fx32 dist = FX_Mul(top, top);//FX_Mul(diff.x, diff.x) + FX_Mul(diff.y, diff.y) + FX_Mul(diff.z, diff.z);
					if (top >= (30 * FX32_ONE))//(10 * 10 * FX32_ONE))
					{
						//for (int y2 = 0; y2 < 5; y2++)
						//{
						//	for (int x2 = 0; x2 < 5; x2++)
						//	{
						//		mLodLevels[(y + y2) * 128 + (x + x2)] |= 4;
						//	}
						//}
						tex = mTerrainManager->GetTerrainTexture(mTextures[(y + 2) * 128 + x + 2]);
						G3_TexImageParam((GXTexFmt)tex->nitroFormat,       // use alpha texture
							GX_TEXGEN_TEXCOORD,    // use texcoord
							(GXTexSizeS)tex->nitroWidth,        // 16 pixels
							(GXTexSizeT)tex->nitroHeight,        // 16 pixels
							GX_TEXREPEAT_ST,     // no repeat
							GX_TEXFLIP_NONE,       // no flip
							GX_TEXPLTTCOLOR0_USE,  // use color 0 of the palette
							NNS_GfdGetTexKeyAddr(tex->texKey)     // the offset of the texture image
						);
						G3_TexPlttBase(NNS_GfdGetPlttKeyAddr(tex->plttKey), (GXTexFmt)tex->nitroFormat);
						/*uint32_t texOffset = mTerrainTextureManager->GetTextureAddress(
							mTextures[y * 128 + x],
							mTextures[y * 128 + x + 4],
							mTextures[(y + 4) * 128 + x],
							mTextures[(y + 4) * 128 + x + 4]);
						G3_TexImageParam(GX_TEXFMT_DIRECT,       // use alpha texture
							GX_TEXGEN_NONE,    // use texcoord
							GX_TEXSIZE_S16,        // 16 pixels
							GX_TEXSIZE_T16,        // 16 pixels
							GX_TEXREPEAT_ST,     // no repeat
							GX_TEXFLIP_NONE,       // no flip
							GX_TEXPLTTCOLOR0_USE,  // use color 0 of the palette
							texOffset //NNS_GfdGetTexKeyAddr(tex->texKey)     // the offset of the texture image
						);/

						reg_G3X_GXFIFO = GX_PACK_OP(G3OP_BEGIN, G3OP_TEXCOORD, G3OP_NORMAL, G3OP_VTX_10);
						{
							reg_G3X_GXFIFO = GX_PACK_BEGIN_PARAM(GX_BEGIN_TRIANGLE_STRIP);
							reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM(0, 0);
							reg_G3X_GXFIFO = mNormals[y * 128 + x];
							reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM(x << 6, mVtx[y * 128 + x] << 6, y << 6);
						}
						reg_G3X_GXFIFO = GX_PACK_OP(G3OP_TEXCOORD, G3OP_NORMAL, G3OP_VTX_10, G3OP_TEXCOORD);
						{						
							reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM(0, (8 << tex->nitroHeight) * FX32_ONE * 4);
							reg_G3X_GXFIFO = mNormals[(y + 4) * 128 + x];
							reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM(x << 6, mVtx[(y + 4) * 128 + x] << 6, (y << 6) + (4 << 6));
							reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM((8 << tex->nitroWidth) * FX32_ONE * 4, 0);
						}
						reg_G3X_GXFIFO = GX_PACK_OP(G3OP_NORMAL, G3OP_VTX_10, G3OP_TEXCOORD, G3OP_NORMAL);
						{
							reg_G3X_GXFIFO = mNormals[y * 128 + (x + 4)];
							reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM((x << 6) + (4 << 6), mVtx[y * 128 + (x + 4)] << 6, y << 6);
							reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM((8 << tex->nitroWidth) * FX32_ONE * 4, (8 << tex->nitroHeight) * FX32_ONE * 4);
							reg_G3X_GXFIFO = mNormals[(y + 4) * 128 + (x + 4)];
						}
						reg_G3X_GXFIFO = GX_PACK_OP(G3OP_VTX_10, G3OP_END, G3OP_NOP, G3OP_NOP);
						{		
							reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM((x << 6) + (4 << 6), mVtx[(y + 4) * 128 + (x + 4)] << 6, (y << 6) + (4 << 6));
						}
					}
				}
			}*/
			G3_TexImageParam(GX_TEXFMT_NONE,       // use alpha texture
				GX_TEXGEN_NONE,    // use texcoord
				(GXTexSizeS)0,        // 16 pixels
				(GXTexSizeT)0,        // 16 pixels
				GX_TEXREPEAT_NONE,     // no repeat
				GX_TEXFLIP_NONE,       // no flip
				GX_TEXPLTTCOLOR0_USE,  // use color 0 of the palette
				0     // the offset of the texture image
			);
			/*for (int y = zstart; y < zend && y < 127; y++)
			{
				int last = -1;
				for (int x = xstart; x < xend && x < 127; x++)
				{
					bool contains2 = false;
					bool contains1 = false;
					bool contains0 = false;
					for (int y2 = 0; y2 < 2; y2++)
					{
						for (int x2 = 0; x2 < 2; x2++)
						{
							if (mLodLevels[(y + y2) * 128 + (x + x2)] == 2)
								contains2 = true;
							if (mLodLevels[(y + y2) * 128 + (x + x2)] == 1)
								contains1 = true;
							if (mLodLevels[(y + y2) * 128 + (x + x2)] == 0)
								contains0 = true;
						}
					}
					if (!contains0 && contains1 && !contains2)
					{
						//regular 1x1 square
						if (last >= 0 && last != 1)
						{
							//mark this shit
							G3_Begin(GX_BEGIN_TRIANGLES);
							G3_Color(GX_RGB(31, 0, 0));
							G3_Vtx10(x << 6, mVtx[y * 128 + x] << 6, y << 6);
							G3_Vtx10(x << 6, mVtx[(y + 1) * 128 + x] << 6, (y + 1) << 6);
							G3_Vtx10(x << 6, mVtx[(y + 1) * 128 + x] << 6, (y + 1) << 6);
							G3_End();
						}
						last = 1;
					}
					else if (!contains0 && !contains1 && contains2)
					{
						//part of regular 2x2 square
						if (last >= 0 && last != 2)
						{
							//mark this shit
							G3_Begin(GX_BEGIN_TRIANGLES);
							G3_Color(GX_RGB(31, 0, 0));
							G3_Vtx10(x << 6, mVtx[y * 128 + x] << 6, y << 6);
							G3_Vtx10(x << 6, mVtx[(y + 1) * 128 + x] << 6, (y + 1) << 6);
							G3_Vtx10(x << 6, mVtx[(y + 1) * 128 + x] << 6, (y + 1) << 6);
							G3_End();
						}
						last = 2;
					}
					else if (contains0 && !contains1 && !contains2)
					{
						//gap
						last = 0;
					}
				}
			}*/
			/*for (int y = zstart & ~1; y < (zend | 1) && y < 127; y += 2)
			{
				for (int x = xstart & ~1; x < (xend | 1) && x < 127; x += 2)
				{
					bool contains1 = false;
					bool contains2 = false;
					for (int y2 = 0; y2 < 3; y2++)
					{
						for (int x2 = 0; x2 < 3; x2++)
						{
							if (mLodLevels[(y + y2) * 128 + (x + x2)] == 1)
								contains1 = true;
							if (mLodLevels[(y + y2) * 128 + (x + x2)] == 2)
								contains2 = true;
						}
					}
					if (!contains1 || !contains2)
						continue;

					//check if this 2x2 block has gaps
					NOCASH_Printf("%d%d%d\n%d%d%d\n%d%d%d\n",
							mLodLevels[y * 128 + x], mLodLevels[y * 128 + x + 1], mLodLevels[y * 128 + x + 2],
							mLodLevels[(y + 1) * 128 + x], mLodLevels[(y + 1) * 128 + x + 1], mLodLevels[(y + 1) * 128 + x + 2],
							mLodLevels[(y + 2) * 128 + x], mLodLevels[(y + 2) * 128 + x + 1], mLodLevels[(y + 2) * 128 + x + 2]
						);

					//if (mLodLevels[y * 128 + x] == 2 && mLodLevels[y * 128 + x + 1] == 1)
					//{
					//	NOCASH_Printf("x stitch");
					//}

					/*if (mLodLevels[y * 128 + x] == 3)
					{
						if (mLodLevels[y * 128 + x + 1] == 3)
						{
							G3_Begin(GX_BEGIN_TRIANGLES);
							G3_Color(GX_RGB(31, 0, 0));
							G3_Vtx10((x & ~1) << 6, mVtx[(y & ~1) * 128 + (x & ~1)] << 6, (y & ~1) << 6);
							G3_Vtx10(((x & ~1) + 1) << 6, mVtx[(y & ~1) * 128 + (x & ~1) + 1] << 6, (y & ~1) << 6);
							G3_Vtx10(((x & ~1) + 2) << 6, mVtx[(y & ~1) * 128 + (x & ~1) + 2] << 6, (y & ~1) << 6);
							G3_End();
						}
						else if (mLodLevels[(y + 1) * 128 + x] == 3)
						{
							G3_Begin(GX_BEGIN_TRIANGLES);
							G3_Color(GX_RGB(31, 0, 0));
							G3_Vtx10((x & ~1) << 6, mVtx[(y & ~1) * 128 + (x & ~1)] << 6, (y & ~1) << 6);
							G3_Vtx10((x & ~1) << 6, mVtx[((y & ~1) + 1) * 128 + (x & ~1)] << 6, ((y & ~1) + 1) << 6);
							G3_Vtx10((x & ~1) << 6, mVtx[((y & ~1) + 2) * 128 + (x & ~1)] << 6, ((y & ~1) + 2) << 6);
							G3_End();
						}
					}/
					/*if (mLodLevels[y * 128 + x] == 2 && mLodLevels[(y + 1) * 128 + x + 1] != 2)
					{
						for (int x2 = x + 1, y2 = y + 1; x2 < xend && x2 < 127 && y2 < zend && y2 < 127; x2++, y2++)
						{
							if (mLodLevels[y2 * 128 + x2] != 0)
							{
								//NOCASH_Printf("xy stitch %d(%d)-%d(%d)", x, mLodLevels[y * 128 + x], x2, mLodLevels[y * 128 + x2]);

								/*G3_Begin(GX_BEGIN_TRIANGLE_STRIP);
								G3_Color(GX_RGB(31, 0, 0));
								G3_Vtx10(x << 6, mVtx[y * 128 + x] << 6, y << 6);
								G3_Vtx10(x << 6, mVtx[y2 * 128 + x] << 6, y2 << 6);
								G3_Vtx10(x2 << 6, mVtx[y * 128 + x2] << 6, y << 6);
								G3_Vtx10(x2 << 6, mVtx[y2 * 128 + x2] << 6, y2 << 6);
								G3_End();/
								G3_Begin(GX_BEGIN_TRIANGLES);
								G3_Color(GX_RGB(31, 0, 0));
								G3_Vtx10(x << 6, mVtx[y * 128 + x] << 6, y << 6);
								G3_Vtx10(x2 << 6, mVtx[y2 * 128 + x2] << 6, y2 << 6);
								G3_Vtx10(x2 << 6, mVtx[y2 * 128 + x2] << 6, y2 << 6);
								G3_End();

								break;
							}
						}
					}
					if (mLodLevels[y * 128 + x] == 2 && mLodLevels[(y - 1) * 128 + x - 1] != 2)
					{
						for (int x2 = x - 1, y2 = y - 1; x2 >= xstart && y2 >= zstart; x2--, y2--)
						{
							if (mLodLevels[y2 * 128 + x2] != 0)
							{
								//NOCASH_Printf("xy stitch %d(%d)-%d(%d)", x, mLodLevels[y * 128 + x], x2, mLodLevels[y * 128 + x2]);

								/*G3_Begin(GX_BEGIN_TRIANGLE_STRIP);
								G3_Color(GX_RGB(31, 0, 0));
								G3_Vtx10(x2 << 6, mVtx[y2 * 128 + x2] << 6, y2 << 6);
								G3_Vtx10(x2 << 6, mVtx[y * 128 + x2] << 6, y << 6);
								G3_Vtx10(x << 6, mVtx[y2 * 128 + x] << 6, y2 << 6);
								G3_Vtx10(x << 6, mVtx[y * 128 + x] << 6, y << 6);
								G3_End();/
								G3_Begin(GX_BEGIN_TRIANGLES);
								G3_Color(GX_RGB(31, 0, 0));
								G3_Vtx10(x << 6, mVtx[y * 128 + x] << 6, y << 6);
								G3_Vtx10(x2 << 6, mVtx[y2 * 128 + x2] << 6, y2 << 6);
								G3_Vtx10(x2 << 6, mVtx[y2 * 128 + x2] << 6, y2 << 6);
								G3_End();

								break;
							}
						}
					}/
					/*if (mLodLevels[y * 128 + x] == 2 && mLodLevels[y * 128 + x - 1] != 2)
					{
						for (int x2 = x - 1; x2 >= xstart; x2--)
						{
							if (mLodLevels[y * 128 + x2] != 0)
							{
								NOCASH_Printf("x stitch %d(%d)-%d(%d)", x, mLodLevels[y * 128 + x], x2, mLodLevels[y * 128 + x2]);

								G3_Begin(GX_BEGIN_TRIANGLE_STRIP);
								G3_Color(GX_RGB(31, 0, 0));	
								G3_Vtx10(x2 << 6, mVtx[y * 128 + x2] << 6, y << 6);
								G3_Vtx10(x2 << 6, mVtx[(y + 1) * 128 + x2] << 6, (y + 1) << 6);
								G3_Vtx10(x << 6, mVtx[y * 128 + x] << 6, y << 6);
								G3_Vtx10(x << 6, mVtx[(y + 1) * 128 + x] << 6, (y + 1) << 6);
								G3_End();

								break;
							}
						}
					}/
					/*if (mLodLevels[y * 128 + x] == 2 && mLodLevels[(y + 1) * 128 + x] != 2)
					{
						for (int y2 = y + 1; y2 < zend && y2 < 127; y2++)
						{
							if (mLodLevels[y2 * 128 + x] != 0)
							{
								NOCASH_Printf("y stitch %d(%d)-%d(%d)", y, mLodLevels[y * 128 + x], y2, mLodLevels[y2 * 128 + x]);
								G3_Begin(GX_BEGIN_TRIANGLE_STRIP);
								G3_Color(GX_RGB(31, 0, 0));
								G3_Vtx10(x << 6, mVtx[y * 128 + x] << 6, y << 6);
								G3_Vtx10(x << 6, mVtx[y2 * 128 + x] << 6, y2 << 6);
								G3_Vtx10((x + 1) << 6, mVtx[y * 128 + x + 1] << 6, y << 6);
								G3_Vtx10((x + 1) << 6, mVtx[y2 * 128 + x + 1] << 6, y2 << 6);
								G3_End();
								break;
							}
						}
					}
					if (mLodLevels[y * 128 + x] == 2 && mLodLevels[(y - 1) * 128 + x] != 2)
					{
						for (int y2 = y - 1; y2 >= zstart; y2--)
						{
							if (mLodLevels[y2 * 128 + x] != 0)
							{
								NOCASH_Printf("y stitch %d(%d)-%d(%d)", y, mLodLevels[y * 128 + x], y2, mLodLevels[y2 * 128 + x]);
								G3_Begin(GX_BEGIN_TRIANGLE_STRIP);
								G3_Color(GX_RGB(31, 0, 0));
								G3_Vtx10(x << 6, mVtx[y2 * 128 + x] << 6, y2 << 6);
								G3_Vtx10(x << 6, mVtx[y * 128 + x] << 6, y << 6);
								G3_Vtx10((x + 1) << 6, mVtx[y2 * 128 + x + 1] << 6, y2 << 6);
								G3_Vtx10((x + 1) << 6, mVtx[y * 128 + x + 1] << 6, y << 6);	
								G3_End();
								break;
							}
						}
					}/
				}
			}*/
		}
	}
	G3_PopMtx(1);
	if (!picking)
	{
		TrackPieceEx* trackPiece = NULL;
		while ((trackPiece = (TrackPieceEx*)NNS_FndGetNextListObject(&mTrackList, trackPiece)) != NULL)
		{
			//if (trackPiece->mPosition.x >= xstart && trackPiece->mPosition.x < xend &&
			//	trackPiece->mPosition.z >= zstart && trackPiece->mPosition.z < zend)
			//{
				if (picking) G3_MaterialColorSpecEmi(0, 0, FALSE);
				trackPiece->Render(this, mTerrainManager);
			//}
		}
		if (mGhostPiece != NULL)
		{
			if (picking) G3_MaterialColorSpecEmi(0, 0, FALSE);
			mGhostPiece->Render(this, mTerrainManager);
		}
		trackPiece = NULL;
		while ((trackPiece = (TrackPieceEx*)NNS_FndGetNextListObject(&mTrackList, trackPiece)) != NULL)
		{
			trackPiece->RenderMarkers(this, mTerrainManager);
		}
		if (mGhostPiece != NULL)
		{
			mGhostPiece->RenderMarkers(this, mTerrainManager);
		}
		SceneryObject* sceneryObject = NULL;
		while ((sceneryObject = (SceneryObject*)NNS_FndGetNextListObject(&mSceneryList, sceneryObject)) != NULL)
		{
			if (sceneryObject->mPosition.x >= xstart && sceneryObject->mPosition.x < xend &&
				sceneryObject->mPosition.z >= zstart && sceneryObject->mPosition.z < zend)
			{
				if (picking) G3_MaterialColorSpecEmi(0, 0, FALSE);
				sceneryObject->Render(mTerrainManager);
			}
		}
	}
	mTerrainManager->mTrackMarkerRotation += FX32_CONST(2);
	if (mTerrainManager->mTrackMarkerRotation >= 360 * FX32_ONE)
		mTerrainManager->mTrackMarkerRotation -= 360 * FX32_ONE;
}

#include <nitro/itcm_end.h>

static fx32 sign(VecFx32* p1, VecFx32* p2, VecFx32* p3)
{
	return FX_Mul(p1->x - p3->x, p2->z - p3->z) - FX_Mul(p2->x - p3->x, p1->z - p3->z);
}

static bool PointInTriangle(VecFx32* pt, VecFx32* v1, VecFx32* v2, VecFx32* v3)
{
	bool b1, b2, b3;

	b1 = sign(pt, v1, v2) < 0;
	b2 = sign(pt, v2, v3) < 0;
	b3 = sign(pt, v3, v1) < 0;

	return ((b1 == b2) && (b2 == b3));
}

bool Map::ScreenPosToWorldPos(int screenX, int screenY, int mapX, int mapY, VecFx32* result)
{
	VecFx32 near;
	VecFx32 far;
	NNS_G3dScrPosToWorldLine(screenX, screenY, &near, &far);

	VecFx32 nf;
	VEC_Subtract(&far, &near, &nf);

	//get the plane of the map square
	VecFx32 a =
	{
		mapX * FX32_ONE - 32 * FX32_ONE,
		(mVtx[mapY * 128 + mapX] - Y_OFFSET) * Y_SCALE,
		mapY * FX32_ONE - 32 * FX32_ONE
	};
	VecFx32 b =
	{
		mapX * FX32_ONE - 32 * FX32_ONE,
		(mVtx[(mapY + 1) * 128 + mapX] - Y_OFFSET) * Y_SCALE,
		(mapY + 1) * FX32_ONE - 32 * FX32_ONE
	};
	VecFx32 c =
	{
		(mapX + 1) * FX32_ONE - 32 * FX32_ONE,
		(mVtx[mapY * 128 + mapX + 1] - Y_OFFSET) * Y_SCALE,
		mapY * FX32_ONE - 32 * FX32_ONE
	};
	VecFx32 d =
	{
		(mapX + 1) * FX32_ONE - 32 * FX32_ONE,
		(mVtx[(mapY + 1) * 128 + mapX + 1] - Y_OFFSET) * Y_SCALE,
		(mapY + 1) * FX32_ONE - 32 * FX32_ONE
	};

	VecFx32 ab;
	VEC_Subtract(&b, &a, &ab);
	VecFx32 ac;
	VEC_Subtract(&c, &a, &ac);
	VecFx32 abXac;
	VEC_CrossProduct(&ab, &ac, &abXac);

	//try the first triangle
	fx32 pa = abXac.x;
	fx32 pb = abXac.y;
	fx32 pc = abXac.z;
	fx32 pd = -(FX_Mul(pa, a.x) + FX_Mul(pb, a.y) + FX_Mul(pc, a.z));


	fx32 top = FX_Mul(pa, near.x) + pd + FX_Mul(pb, near.y) + FX_Mul(pc, near.z);
	fx32 bottom = FX_Mul(pa, near.x - far.x) + FX_Mul(pb, near.y - far.y) + FX_Mul(pc, near.z - far.z);
	fx32 interx = near.x - FX_Div(FX_Mul(near.x - far.x, top), bottom);
	fx32 intery = near.y - FX_Div(FX_Mul(near.y - far.y, top), bottom);
	fx32 interz = near.z - FX_Div(FX_Mul(near.z - far.z, top), bottom);

	result->x = interx;
	result->y = intery;
	result->z = interz;

	if (PointInTriangle(result, &a, &b, &c))
		return true;

	VEC_Subtract(&b, &c, &ab);
	VEC_Subtract(&d, &c, &ac);
	VEC_CrossProduct(&ab, &ac, &abXac);

	//try the first triangle
	pa = abXac.x;
	pb = abXac.y;
	pc = abXac.z;
	pd = -(FX_Mul(pa, c.x) + FX_Mul(pb, c.y) + FX_Mul(pc, c.z));

	top = FX_Mul(pa, near.x) + pd + FX_Mul(pb, near.y) + FX_Mul(pc, near.z);
	bottom = FX_Mul(pa, near.x - far.x) + FX_Mul(pb, near.y - far.y) + FX_Mul(pc, near.z - far.z);
	interx = near.x - FX_Div(FX_Mul(near.x - far.x, top), bottom);
	intery = near.y - FX_Div(FX_Mul(near.y - far.y, top), bottom);
	interz = near.z - FX_Div(FX_Mul(near.z - far.z, top), bottom);

	result->x = interx;
	result->y = intery;
	result->z = interz;

	return PointInTriangle(result, &c, &b, &d);
}

fx32 Map::GetYOnMap(fx32 x, fx32 z)
{
	VecFx32 near = { x, 256 * FX32_ONE, z };
	VecFx32 far = { x, -256 * FX32_ONE, z };

	int mapX = (x + 32 * FX32_ONE) >> FX32_SHIFT;
	int mapY = (z + 32 * FX32_ONE) >> FX32_SHIFT;

	//get the plane of the map square
	VecFx32 a =
	{
		mapX * FX32_ONE - 32 * FX32_ONE,
		(mVtx[mapY * 128 + mapX] - Y_OFFSET) * Y_SCALE,
		mapY * FX32_ONE - 32 * FX32_ONE
	};
	VecFx32 b =
	{
		mapX * FX32_ONE - 32 * FX32_ONE,
		(mVtx[(mapY + 1) * 128 + mapX] - Y_OFFSET) * Y_SCALE,
		(mapY + 1) * FX32_ONE - 32 * FX32_ONE
	};
	VecFx32 c =
	{
		(mapX + 1) * FX32_ONE - 32 * FX32_ONE,
		(mVtx[mapY * 128 + mapX + 1] - Y_OFFSET) * Y_SCALE,
		mapY * FX32_ONE - 32 * FX32_ONE
	};
	VecFx32 d =
	{
		(mapX + 1) * FX32_ONE - 32 * FX32_ONE,
		(mVtx[(mapY + 1) * 128 + mapX + 1] - Y_OFFSET) * Y_SCALE,
		(mapY + 1) * FX32_ONE - 32 * FX32_ONE
	};
	VecFx32 ab, ac, abXac;
	if (PointInTriangle(&near, &a, &b, &c))
	{
		VEC_Subtract(&b, &a, &ab);
		VEC_Subtract(&c, &a, &ac);
		VEC_CrossProduct(&ab, &ac, &abXac);

		//try the first triangle
		fx32 pa = abXac.x;
		fx32 pb = abXac.y;
		fx32 pc = abXac.z;
		fx32 pd = -(FX_Mul(pa, a.x) + FX_Mul(pb, a.y) + FX_Mul(pc, a.z));


		fx32 top = FX_Mul(pa, near.x) + pd + FX_Mul(pb, near.y) + FX_Mul(pc, near.z);
		fx32 bottom = FX_Mul(pa, near.x - far.x) + FX_Mul(pb, near.y - far.y) + FX_Mul(pc, near.z - far.z);
		return near.y - FX_Div(FX_Mul(near.y - far.y, top), bottom);
	}
	else
	{
		VEC_Subtract(&b, &c, &ab);
		VEC_Subtract(&d, &c, &ac);
		VEC_CrossProduct(&ab, &ac, &abXac);

		//try the first triangle
		fx32 pa = abXac.x;
		fx32 pb = abXac.y;
		fx32 pc = abXac.z;
		fx32 pd = -(FX_Mul(pa, c.x) + FX_Mul(pb, c.y) + FX_Mul(pc, c.z));

		fx32 top = FX_Mul(pa, near.x) + pd + FX_Mul(pb, near.y) + FX_Mul(pc, near.z);
		fx32 bottom = FX_Mul(pa, near.x - far.x) + FX_Mul(pb, near.y - far.y) + FX_Mul(pc, near.z - far.z);
		return near.y - FX_Div(FX_Mul(near.y - far.y, top), bottom);
	}
}

//We should get rid of the flextrack in this method
void Map::TrySnapGhostTrack(int inPoint, TrackPieceEx* ignore)
{
	if (mGhostPiece == NULL)
		return;
	VecFx32 ghostEnd;// = ((FlexTrack*)mGhostPiece)->mPoints[1];
	mGhostPiece->GetConnectionPoint(inPoint, &ghostEnd);
	TrackPieceEx* trackPiece = NULL;
	while ((trackPiece = (TrackPieceEx*)NNS_FndGetNextListObject(&mTrackList, trackPiece)) != NULL)
	{
		if (trackPiece == ignore)
			continue;
		int nrConnectors = trackPiece->GetNrConnectionPoints();
		for (int i = 0; i < nrConnectors; i++)
		{
			VecFx32 pos;
			trackPiece->GetConnectionPoint(i, &pos);
			fx32 sedist =
				FX_Mul(pos.x - ghostEnd.x, pos.x - ghostEnd.x) +
				FX_Mul(pos.y - ghostEnd.y, pos.y - ghostEnd.y) +
				FX_Mul(pos.z - ghostEnd.z, pos.z - ghostEnd.z);
			if (sedist <= FX32_ONE / 16)//>> 2)
			{
				mGhostPiece->Connect(inPoint, trackPiece, i, true);
				return;
			}
		}
	}
	mGhostPiece->Disconnect(inPoint);
}