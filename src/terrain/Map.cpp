#include <nitro.h>
#include <nnsys/fnd.h>
#include <nnsys/g3d.h>
#include "core.h"
#include "util.h"
#include "terrain.h"
#include "TerrainManager.h"
#include "terrain/track/TrackPiece.h"
#include "terrain/scenery/SceneryObject.h"
#include "terrain/scenery/RCT2Tree1.h"
#include "tgp.h"
#include "Map.h"

Map::Map()
	: mGridEnabled(FALSE)
{
	NNS_FND_INIT_LIST(&mTrackList, TrackPiece, mLink);
	NNS_FND_INIT_LIST(&mSceneryList, SceneryObject, mLink);
	mTerrainManager = new TerrainManager();
	mVtx = (uint8_t*)Util_LoadFileToBuffer("/data/map/terrain.hmap", NULL, false);
	
	//mVtx = new uint8_t[128 * 128];
	//MI_CpuFillFast(mVtx, 0x80808080, 128 * 128);
	mNormals = new VecFx10[128 * 128];
	RecalculateNormals(0, 127, 0, 127);
}

Map::~Map()
{
	delete mTerrainManager;
}

#define Y_OFFSET 40 //100

#include <nitro/itcm_begin.h>

void Map::RecalculateNormals(int xstart, int xend, int zstart, int zend)
{
	for (int y = zstart; y < zend && y < 127; y++)
	{
		//VecFx32 faceNorms[2][3][2];
		for (int x = xstart; x < xend && x < 127; x++)
		{
			//shift normals
			/*faceNorms[0][0][0] = faceNorms[0][1][0];
			faceNorms[0][0][1] = faceNorms[0][1][1];
			faceNorms[0][1][0] = faceNorms[0][2][0];
			faceNorms[0][1][1] = faceNorms[0][2][1];
			faceNorms[1][0][0] = faceNorms[1][1][0];
			faceNorms[1][0][1] = faceNorms[1][1][1];
			faceNorms[1][1][0] = faceNorms[1][2][0];
			faceNorms[1][1][1] = faceNorms[1][2][1];

			//calculate 16 face normals
			for (int fy = -1; fy <= 0; fy++)
			{
				//for (int fx = -1; fx <= 1; fx++)
				//{
				int fx = 1;
				const VecFx32 vtxFace[4] =
				{
					{ 0, (mVtx[(y + fy) * 128 + x + fx] - Y_OFFSET) * 128 * FX32_ONE, 0 },
					{ 0, (mVtx[(y + fy + 1) * 128 + x + fx] - Y_OFFSET) * 128 * FX32_ONE,  FX32_ONE },
					{ FX32_ONE, (mVtx[(y + fy) * 128 + (x + fx + 1)] - Y_OFFSET) * 128 * FX32_ONE,  0 },
					{ FX32_ONE, (mVtx[(y + fy + 1) * 128 + (x + fx + 1)] - Y_OFFSET) * 128 * FX32_ONE, FX32_ONE }
				};
				VecFx32 tmpA, tmpB, tmpC, normalA, normalB;
				VEC_Subtract(&vtxFace[1], &vtxFace[0], &tmpA);
				VEC_Subtract(&vtxFace[2], &vtxFace[0], &tmpB);
				VEC_CrossProduct(&tmpA, &tmpB, &tmpC);
				VEC_Normalize(&tmpC, &faceNorms[fy + 1][fx + 1][0]);

				VEC_Subtract(&vtxFace[3], &vtxFace[1], &tmpA);
				VEC_Subtract(&vtxFace[2], &vtxFace[1], &tmpB);
				VEC_CrossProduct(&tmpA, &tmpB, &tmpC);
				VEC_Normalize(&tmpC, &faceNorms[fy + 1][fx + 1][1]);
				//}
			}*/

			/*VecFx32 thisvtx = { x * FX32_ONE, (mVtx[y * 128 + x] - Y_OFFSET) * 128, y * FX32_ONE };

			VecFx32 norms[4];

			{
				const VecFx32 vtxFace[4] =
				{
					{ x * FX32_ONE, (mVtx[(y - 1) * 128 + x] - Y_OFFSET) * 128, (y - 1) * FX32_ONE },
					{ (x - 1) * FX32_ONE, (mVtx[y * 128 + x - 1] - Y_OFFSET) * 128, y * FX32_ONE },
					thisvtx
				};

				VecFx32 tmpA, tmpB, tmpC;
				VEC_Subtract(&vtxFace[1], &vtxFace[0], &tmpA);
				VEC_Subtract(&vtxFace[2], &vtxFace[0], &tmpB);
				VEC_CrossProduct(&tmpA, &tmpB, &tmpC);
				VEC_Normalize(&tmpC, &norms[0]);
			}

			{
				const VecFx32 vtxFace[4] =
				{
					{ x * FX32_ONE, (mVtx[(y - 1) * 128 + x] - Y_OFFSET) * 128, (y - 1) * FX32_ONE },
					thisvtx,
					{ (x - 1) * FX32_ONE, (mVtx[(y - 1) * 128 + x + 1] - Y_OFFSET) * 128, (y - 1) * FX32_ONE }
				};

				VecFx32 tmpA, tmpB, tmpC;
				VEC_Subtract(&vtxFace[1], &vtxFace[0], &tmpA);
				VEC_Subtract(&vtxFace[2], &vtxFace[0], &tmpB);
				VEC_CrossProduct(&tmpA, &tmpB, &tmpC);
				VEC_Normalize(&tmpC, &norms[1]);
			}

			{
				const VecFx32 vtxFace[4] =
				{
					{ (x - 1) * FX32_ONE, (mVtx[y * 128 + x - 1] - Y_OFFSET) * 128, y * FX32_ONE },
					{ x * FX32_ONE, (mVtx[(y + 1) * 128 + x] - Y_OFFSET) * 128, (y + 1) * FX32_ONE },
					thisvtx
				};

				VecFx32 tmpA, tmpB, tmpC;
				VEC_Subtract(&vtxFace[1], &vtxFace[0], &tmpA);
				VEC_Subtract(&vtxFace[2], &vtxFace[0], &tmpB);
				VEC_CrossProduct(&tmpA, &tmpB, &tmpC);
				VEC_Normalize(&tmpC, &norms[2]);
			}

			{
				const VecFx32 vtxFace[4] =
				{
					thisvtx,
					{ x * FX32_ONE, (mVtx[(y + 1) * 128 + x] - Y_OFFSET) * 128, (y + 1) * FX32_ONE },
					{ (x + 1) * FX32_ONE, (mVtx[y * 128 + x + 1] - Y_OFFSET) * 128, y * FX32_ONE }
				};

				VecFx32 tmpA, tmpB, tmpC;
				VEC_Subtract(&vtxFace[1], &vtxFace[0], &tmpA);
				VEC_Subtract(&vtxFace[2], &vtxFace[0], &tmpB);
				VEC_CrossProduct(&tmpA, &tmpB, &tmpC);
				VEC_Normalize(&tmpC, &norms[3]);
			}


			VecFx32 norm;
			norm.x = norms[0].x + norms[1].x + norms[2].x + norms[3].x;

			norm.y = norms[0].y + norms[1].y + norms[2].y + norms[3].y;

			norm.z = norms[0].z + norms[1].z + norms[2].z + norms[3].z;*/

			fx32 hl = (mVtx[y * 128 + x - 1] - Y_OFFSET) * 128; //TERRAIN(t, x - 1, z);
			fx32 hr = (mVtx[y * 128 + x + 1] - Y_OFFSET) * 128; //TERRAIN(t, x + 1, z);
			fx32 hd = (mVtx[(y + 1) * 128 + x] - Y_OFFSET) * 128; //TERRAIN(t, x, z + 1); /* Terrain expands towards -Z /
			fx32 hu = (mVtx[(y - 1) * 128 + x] - Y_OFFSET) * 128; //TERRAIN(t, x, z - 1);
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

void Map::Render(int xstart, int xend, int zstart, int zend, bool picking, int selectedMapX, int selectedMapZ, VecFx32* camPos)
{
	texture_t* tex = mTerrainManager->GetTerrainTexture(0);
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
	G3_Translate(-32 * FX32_ONE, 0, -32 * FX32_ONE);
	G3_PushMtx();
	{
		G3_Scale(FX32_ONE, 128 * FX32_ONE / 64, FX32_ONE);
		int i = 0;
		for (int y = zstart; y < zend && y < 127; y++)
		{
			for (int x = xstart; x < xend && x < 127; x++)
			{
				fx32 diff_x = x * FX32_ONE - camPos->x - 32 * FX32_ONE;
				fx32 diff_z = y * FX32_ONE - camPos->z - 32 * FX32_ONE;
				fx32 dist = FX_Mul(diff_x, diff_x) + FX_Mul(diff_z, diff_z);
				if (dist <= (8 * 8 * FX32_ONE))
				{
					reg_G3X_GXFIFO = G3OP_MTX_PUSH;
					{
						if (picking)
							G3_MaterialColorSpecEmi(0, PICKING_COLOR(PICKING_TYPE_MAP, i + 1), FALSE);
						else if (selectedMapX == x && selectedMapZ == y)
						{
							reg_G3X_GXFIFO = GX_PACK_OP(G3OP_DIF_AMB, G3OP_SPE_EMI, G3OP_POLYGON_ATTR, G3OP_NOP);
							{
								reg_G3X_GXFIFO = GX_PACK_DIFFAMB_PARAM(GX_RGB(0, 0, 0), GX_RGB(0, 0, 0), FALSE);
								reg_G3X_GXFIFO = GX_PACK_SPECEMI_PARAM(GX_RGB(0, 0, 0), GX_RGB(31, 31, 31), FALSE);
								reg_G3X_GXFIFO = GX_PACK_POLYGONATTR_PARAM(GX_LIGHTMASK_0, GX_POLYGONMODE_TOON, GX_CULL_NONE, 1, 31, GX_POLYGON_ATTR_MISC_FOG);
							}
						}
						else if (mGridEnabled)
							G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, ((x & 1) ^ (y & 1)) << 1, 31, GX_POLYGON_ATTR_MISC_FOG);

						reg_G3X_GXFIFO = GX_PACK_OP(G3OP_MTX_TRANS, G3OP_BEGIN, G3OP_TEXCOORD, G3OP_NORMAL);
						{
							/*G3_Translate*/
							reg_G3X_GXFIFO = x * FX32_ONE;
							reg_G3X_GXFIFO = 0;
							reg_G3X_GXFIFO = y * FX32_ONE;
							reg_G3X_GXFIFO = GX_PACK_BEGIN_PARAM(GX_BEGIN_TRIANGLE_STRIP);
							reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM(0, 0);
							reg_G3X_GXFIFO = mNormals[y * 128 + x];
						}
						reg_G3X_GXFIFO = GX_PACK_OP(G3OP_VTX_10, G3OP_TEXCOORD, G3OP_NORMAL, G3OP_VTX_10);
						{
							reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM(0, (mVtx[y * 128 + x] - Y_OFFSET) << 6, 0);
							reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM(0, (8 << tex->nitroHeight) * FX32_ONE);
							reg_G3X_GXFIFO = mNormals[(y + 1) * 128 + x];
							reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM(0, (mVtx[(y + 1) * 128 + x] - Y_OFFSET) << 6, FX32_ONE);
						}
						reg_G3X_GXFIFO = GX_PACK_OP(G3OP_TEXCOORD, G3OP_NORMAL, G3OP_VTX_10, G3OP_TEXCOORD);
						{
							reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM((8 << tex->nitroWidth) * FX32_ONE, 0);
							reg_G3X_GXFIFO = mNormals[y * 128 + (x + 1)];
							reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM(FX32_ONE, (mVtx[y * 128 + (x + 1)] - Y_OFFSET) << 6, 0);
							reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM((8 << tex->nitroWidth) * FX32_ONE, (8 << tex->nitroHeight) * FX32_ONE);
						}
						reg_G3X_GXFIFO = GX_PACK_OP(G3OP_NORMAL, G3OP_VTX_10, G3OP_END, G3OP_NOP);
						{
							reg_G3X_GXFIFO = mNormals[(y + 1) * 128 + (x + 1)];
							reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM(FX32_ONE, (mVtx[(y + 1) * 128 + (x + 1)] - Y_OFFSET) << 6, FX32_ONE);
						}

						if (!picking && selectedMapX == x && selectedMapZ == y)
						{
							reg_G3X_GXFIFO = GX_PACK_OP(G3OP_DIF_AMB, G3OP_SPE_EMI, G3OP_POLYGON_ATTR, G3OP_NOP);
							{
								reg_G3X_GXFIFO = GX_PACK_DIFFAMB_PARAM(GX_RGB(30, 30, 30), GX_RGB(5, 5, 5), FALSE);
								reg_G3X_GXFIFO = GX_PACK_SPECEMI_PARAM(GX_RGB(3, 3, 3), GX_RGB(0, 0, 0), FALSE);
								reg_G3X_GXFIFO = GX_PACK_POLYGONATTR_PARAM(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_FOG);
							}
						}
					}
					G3_PopMtx(1);
				}
				i++;
			}
		}
		for (int y = zstart & ~1; y < zend && y < 127; y+=2)
		{
			for (int x = xstart & ~1; x < xend && x < 127; x+=2)
			{
				fx32 diff_x = x * FX32_ONE - camPos->x - 32 * FX32_ONE;
				fx32 diff_z = y * FX32_ONE - camPos->z - 32 * FX32_ONE;
				fx32 dist = FX_Mul(diff_x, diff_x) + FX_Mul(diff_z, diff_z);
				if (dist >= (6 * 6 * FX32_ONE) && dist <= (16 * 16 * FX32_ONE))
				{
					reg_G3X_GXFIFO = GX_PACK_OP(G3OP_MTX_PUSH, G3OP_MTX_TRANS, G3OP_BEGIN, G3OP_TEXCOORD);
					{
						/*G3_Translate*/
						reg_G3X_GXFIFO = x * FX32_ONE;
						reg_G3X_GXFIFO = 0;
						reg_G3X_GXFIFO = y * FX32_ONE;
						reg_G3X_GXFIFO = GX_PACK_BEGIN_PARAM(GX_BEGIN_TRIANGLE_STRIP);
						reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM(0, 0);
					}
					reg_G3X_GXFIFO = GX_PACK_OP(G3OP_NORMAL, G3OP_VTX_10, G3OP_TEXCOORD, G3OP_NORMAL);
					{
						reg_G3X_GXFIFO = mNormals[y * 128 + x];
						reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM(0, (mVtx[y * 128 + x] - Y_OFFSET) << 6, 0);
						reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM(0, (8 << tex->nitroHeight) * FX32_ONE * 2);
						reg_G3X_GXFIFO = mNormals[(y + 2) * 128 + x];
					}
					reg_G3X_GXFIFO = GX_PACK_OP(G3OP_VTX_10, G3OP_TEXCOORD, G3OP_NORMAL, G3OP_VTX_10);
					{
						reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM(0, (mVtx[(y + 2) * 128 + x] - Y_OFFSET) << 6, 2 * FX32_ONE);
						reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM((8 << tex->nitroWidth) * FX32_ONE * 2, 0);
						reg_G3X_GXFIFO = mNormals[y * 128 + (x + 2)];
						reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM(2 * FX32_ONE, (mVtx[y * 128 + (x + 2)] - Y_OFFSET) << 6, 0);
					}
					reg_G3X_GXFIFO = GX_PACK_OP(G3OP_TEXCOORD, G3OP_NORMAL, G3OP_VTX_10, G3OP_END);
					{
						reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM((8 << tex->nitroWidth) * FX32_ONE * 2, (8 << tex->nitroHeight) * FX32_ONE * 2);
						reg_G3X_GXFIFO = mNormals[(y + 2) * 128 + (x + 2)];
						reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM(2 * FX32_ONE, (mVtx[(y + 2) * 128 + (x + 2)] - Y_OFFSET) << 6, 2 * FX32_ONE);
					}
					G3_PopMtx(1);
				}
			}
		}
		for (int y = zstart & ~3; y < zend && y < 127; y += 4)
		{
			for (int x = xstart & ~3; x < xend && x < 127; x += 4)
			{
				fx32 diff_x = x * FX32_ONE - camPos->x - 32 * FX32_ONE;
				fx32 diff_z = y * FX32_ONE - camPos->z - 32 * FX32_ONE;
				fx32 dist = FX_Mul(diff_x, diff_x) + FX_Mul(diff_z, diff_z);
				if (dist >= (12 * 12 * FX32_ONE))
				{
					reg_G3X_GXFIFO = GX_PACK_OP(G3OP_MTX_PUSH, G3OP_MTX_TRANS, G3OP_BEGIN, G3OP_TEXCOORD);
					{
						/*G3_Translate*/
						reg_G3X_GXFIFO = x * FX32_ONE;
						reg_G3X_GXFIFO = 0;
						reg_G3X_GXFIFO = y * FX32_ONE;
						reg_G3X_GXFIFO = GX_PACK_BEGIN_PARAM(GX_BEGIN_TRIANGLE_STRIP);
						reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM(0, 0);
					}
					reg_G3X_GXFIFO = GX_PACK_OP(G3OP_NORMAL, G3OP_VTX_10, G3OP_TEXCOORD, G3OP_NORMAL);
					{
						reg_G3X_GXFIFO = mNormals[y * 128 + x];
						reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM(0, (mVtx[y * 128 + x] - Y_OFFSET) << 6, 0);
						reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM(0, (8 << tex->nitroHeight) * FX32_ONE * 4);
						reg_G3X_GXFIFO = mNormals[(y + 4) * 128 + x];
					}
					reg_G3X_GXFIFO = GX_PACK_OP(G3OP_VTX_10, G3OP_TEXCOORD, G3OP_NORMAL, G3OP_VTX_10);
					{
						reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM(0, (mVtx[(y + 4) * 128 + x] - Y_OFFSET) << 6, 4 * FX32_ONE);
						reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM((8 << tex->nitroWidth) * FX32_ONE * 4, 0);
						reg_G3X_GXFIFO = mNormals[y * 128 + (x + 4)];
						reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM(4 * FX32_ONE, (mVtx[y * 128 + (x + 4)] - Y_OFFSET) << 6, 0);
					}
					reg_G3X_GXFIFO = GX_PACK_OP(G3OP_TEXCOORD, G3OP_NORMAL, G3OP_VTX_10, G3OP_END);
					{
						reg_G3X_GXFIFO = GX_PACK_TEXCOORD_PARAM((8 << tex->nitroWidth) * FX32_ONE * 4, (8 << tex->nitroHeight) * FX32_ONE * 4);
						reg_G3X_GXFIFO = mNormals[(y + 4) * 128 + (x + 4)];
						reg_G3X_GXFIFO = GX_PACK_VTX10_PARAM(4 * FX32_ONE, (mVtx[(y + 4) * 128 + (x + 4)] - Y_OFFSET) << 6, 4 * FX32_ONE);
					}
					G3_PopMtx(1);
				}
			}
		}
	}
	G3_PopMtx(1);
	/*G3_Translate(-32 * FX32_ONE, 0, -32 * FX32_ONE);
	G3_PushMtx();
	{
		int i = 0;
		for (int y = zstart; y < zend; y++)
		{
			for (int x = xstart; x < xend; x++)
			{
				G3_PushMtx();
				{
					if (picking) G3_MaterialColorSpecEmi(0, PICKING_COLOR(PICKING_TYPE_MAP, i + 1), FALSE);
					else if (selectedMapX == x && selectedMapZ == y)
					{
						G3_MaterialColorDiffAmb(GX_RGB(0, 0, 0), GX_RGB(0, 0, 0), FALSE);
						G3_MaterialColorSpecEmi(GX_RGB(0, 0, 0), GX_RGB(31, 31, 31), FALSE);
						G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_TOON, GX_CULL_NONE, 1, 31, GX_POLYGON_ATTR_MISC_FOG);
					}
					else if (mGridEnabled)
						G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, ((x & 1) ^ (y & 1)) << 1, 31, GX_POLYGON_ATTR_MISC_FOG);
					G3_Translate(x * FX32_ONE, 0, y * FX32_ONE);
					tile_render(&mTiles[y][x], mTerrainManager);
					if (!picking && selectedMapX == x && selectedMapZ == y)
					{
						G3_MaterialColorDiffAmb(GX_RGB(21, 21, 21), GX_RGB(15, 15, 15), FALSE);
						G3_MaterialColorSpecEmi(GX_RGB(0, 0, 0), GX_RGB(0, 0, 0), FALSE);
						G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_FOG);
					}
				}
				G3_PopMtx(1);
				i++;
			}
		}
		//NOCASH_Printf("Total quads: %d", i);
	}
	G3_PopMtx(1);*/
	if (!picking)
	{
		TrackPiece* trackPiece = NULL;
		while ((trackPiece = (TrackPiece*)NNS_FndGetNextListObject(&mTrackList, trackPiece)) != NULL)
		{
			if (trackPiece->mPosition.x >= xstart && trackPiece->mPosition.x < xend &&
				trackPiece->mPosition.z >= zstart && trackPiece->mPosition.z < zend)
			{
				if (picking) G3_MaterialColorSpecEmi(0, 0, FALSE);
				trackPiece->Render(mTerrainManager);
			}
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
}

#include <nitro/itcm_end.h>

/**
* This function takes care of the fact that land in OpenTTD can never differ
* more than 1 in height
*/
void Map::FixSlopes()
{
	uint32_t width, height;
	int row, col;
	uint8_t current_tile;

	/* Adjust height difference to maximum one horizontal/vertical change. */
	width = 64;// MapSizeX();
	height = 64;// MapSizeY();

	/* Top and left edge */
	for (row = 0; (uint32_t)row < height; row++) {
		for (col = 0; (uint32_t)col < width; col++) {
			current_tile = 255;//MAX_TILE_HEIGHT;
			if (col != 0) {
				/* Find lowest tile; either the top or left one */
				current_tile = mTiles[row][col - 1].y; //TileHeight(TileXY(col - 1, row)); // top edge
			}
			if (row != 0) {
				if (/*TileHeight(TileXY(col, row - 1))*/mTiles[row - 1][col].y < current_tile) {
					current_tile = mTiles[row - 1][col].y;//TileHeight(TileXY(col, row - 1)); // left edge
				}
			}

			/* Does the height differ more than one? */
			if (/*TileHeight(TileXY(col, row))*/mTiles[row][col].y >= (uint32_t)current_tile + 2) {
				/* Then change the height to be no more than one */
				//SetTileHeight(TileXY(col, row), current_tile + 1);
				mTiles[row][col].y = current_tile + 1;
			}
		}
	}

	/* Bottom and right edge */
	for (row = height - 1; row >= 0; row--) {
		for (col = width - 1; col >= 0; col--) {
			current_tile = 255;//MAX_TILE_HEIGHT;
			if ((uint32_t)col != width - 1) {
				/* Find lowest tile; either the bottom and right one */
				current_tile = mTiles[row][col + 1].y; //TileHeight(TileXY(col + 1, row)); // bottom edge
			}

			if ((uint32_t)row != height - 1) {
				if (/*TileHeight(TileXY(col, row + 1))*/mTiles[row + 1][col].y < current_tile) {
					current_tile = mTiles[row + 1][col].y;//TileHeight(TileXY(col, row + 1)); // right edge
				}
			}

			/* Does the height differ more than one? */
			if (/*TileHeight(TileXY(col, row))*/mTiles[row][col].y >= (uint32_t)current_tile + 2) {
				/* Then change the height to be no more than one */
				//SetTileHeight(TileXY(col, row), current_tile + 1);
				mTiles[row][col].y = current_tile + 1;
			}
		}
	}
}

# define DECLARE_ENUM_AS_BIT_SET(mask_t) \
	inline mask_t operator | (mask_t m1, mask_t m2) {return (mask_t)((int)m1 | m2);} \
	inline mask_t operator & (mask_t m1, mask_t m2) {return (mask_t)((int)m1 & m2);} \
	inline mask_t operator ^ (mask_t m1, mask_t m2) {return (mask_t)((int)m1 ^ m2);} \
	inline mask_t& operator |= (mask_t& m1, mask_t m2) {m1 = m1 | m2; return m1;} \
	inline mask_t& operator &= (mask_t& m1, mask_t m2) {m1 = m1 & m2; return m1;} \
	inline mask_t& operator ^= (mask_t& m1, mask_t m2) {m1 = m1 ^ m2; return m1;} \
	inline mask_t operator ~(mask_t m) {return (mask_t)(~(int)m);}

enum Slope {
	SLOPE_FLAT = 0x00,                                  ///< a flat tile
	SLOPE_W = 0x01,                                  ///< the west corner of the tile is raised
	SLOPE_S = 0x02,                                  ///< the south corner of the tile is raised
	SLOPE_E = 0x04,                                  ///< the east corner of the tile is raised
	SLOPE_N = 0x08,                                  ///< the north corner of the tile is raised
	SLOPE_STEEP = 0x10,                                  ///< indicates the slope is steep
	SLOPE_NW = SLOPE_N | SLOPE_W,                     ///< north and west corner are raised
	SLOPE_SW = SLOPE_S | SLOPE_W,                     ///< south and west corner are raised
	SLOPE_SE = SLOPE_S | SLOPE_E,                     ///< south and east corner are raised
	SLOPE_NE = SLOPE_N | SLOPE_E,                     ///< north and east corner are raised
	SLOPE_EW = SLOPE_E | SLOPE_W,                     ///< east and west corner are raised
	SLOPE_NS = SLOPE_N | SLOPE_S,                     ///< north and south corner are raised
	SLOPE_ELEVATED = SLOPE_N | SLOPE_E | SLOPE_S | SLOPE_W, ///< bit mask containing all 'simple' slopes
	SLOPE_NWS = SLOPE_N | SLOPE_W | SLOPE_S,           ///< north, west and south corner are raised
	SLOPE_WSE = SLOPE_W | SLOPE_S | SLOPE_E,           ///< west, south and east corner are raised
	SLOPE_SEN = SLOPE_S | SLOPE_E | SLOPE_N,           ///< south, east and north corner are raised
	SLOPE_ENW = SLOPE_E | SLOPE_N | SLOPE_W,           ///< east, north and west corner are raised
	SLOPE_STEEP_W = SLOPE_STEEP | SLOPE_NWS,               ///< a steep slope falling to east (from west)
	SLOPE_STEEP_S = SLOPE_STEEP | SLOPE_WSE,               ///< a steep slope falling to north (from south)
	SLOPE_STEEP_E = SLOPE_STEEP | SLOPE_SEN,               ///< a steep slope falling to west (from east)
	SLOPE_STEEP_N = SLOPE_STEEP | SLOPE_ENW               ///< a steep slope falling to south (from north)
};
DECLARE_ENUM_AS_BIT_SET(Slope)

void Map::GenerateLandscape()
{
	GenerateTerrainPerlin(this);
	FixSlopes();
	//fix edges
	for (int y = 1; y < 64 - 1; y++)
	{
		for (int x = 1; x < 64 - 1; x++)
		{
			//if (x == MapMaxX() || y == MapMaxY()) {
			//	if (h != NULL) *h = TileHeight(tile);
			//	return SLOPE_FLAT;
			//}

			int hnorth = mTiles[y][x].y;                    // Height of the North corner.
			int hwest = mTiles[y][x - 1].y; // Height of the West corner.
			int heast = mTiles[y + 1][x].y; // Height of the East corner.
			int hsouth = mTiles[y + 1][x - 1].y; // Height of the South corner.

			int hminnw = MATH_MIN(hnorth, hwest);
			int hmines = MATH_MIN(heast, hsouth);
			int hmin = MATH_MIN(hminnw, hmines);

			int hmaxnw = MATH_MAX(hnorth, hwest);
			int hmaxes = MATH_MAX(heast, hsouth);
			int hmax = MATH_MAX(hmaxnw, hmaxes);

			Slope r = SLOPE_FLAT;

			if (hnorth != hmin) r |= SLOPE_N;
				//mTiles[y][x].rtCorner = TILE_CORNER_UP;
			if (hwest != hmin) r |= SLOPE_W;
				//mTiles[y][x].ltCorner = TILE_CORNER_UP;
			if (heast != hmin) r |= SLOPE_E;
				//mTiles[y][x].rbCorner = TILE_CORNER_UP;
			if (hsouth != hmin) r |= SLOPE_S;
				//mTiles[y][x].lbCorner = TILE_CORNER_UP;

			if (hmax - hmin == 2) r |= SLOPE_STEEP;

			mTiles[y][x].corners = r;
			/*{
				if (mTiles[y][x].rtCorner == TILE_CORNER_FLAT)
				{
					mTiles[y][x].rtCorner = TILE_CORNER_DOWN;
					mTiles[y][x].ltCorner = TILE_CORNER_FLAT;
					mTiles[y][x].rbCorner = TILE_CORNER_FLAT;
				}
				else if (mTiles[y][x].ltCorner == TILE_CORNER_FLAT)
				{
					mTiles[y][x].ltCorner = TILE_CORNER_DOWN;
					mTiles[y][x].lbCorner = TILE_CORNER_FLAT;
					mTiles[y][x].rtCorner = TILE_CORNER_FLAT;
				}
				else if (mTiles[y][x].rbCorner == TILE_CORNER_FLAT)
				{
					mTiles[y][x].rbCorner = TILE_CORNER_DOWN;
					mTiles[y][x].rtCorner = TILE_CORNER_FLAT;
					mTiles[y][x].lbCorner = TILE_CORNER_FLAT;
				}
				else if (mTiles[y][x].lbCorner == TILE_CORNER_FLAT)
				{
					mTiles[y][x].lbCorner = TILE_CORNER_DOWN;
					mTiles[y][x].ltCorner = TILE_CORNER_FLAT;
					mTiles[y][x].rbCorner = TILE_CORNER_FLAT;
				}
			}*/

			//GetTileSlopeGivenHeight(hnorth, hwest, heast, hsouth, h);
		}
	}
	for (int y = 1; y < 64 - 1; y++)
	{
		for (int x = 1; x < 64 - 1; x++)
		{
			//convert to train ds format
			Slope r = (Slope)mTiles[y][x].corners;
			mTiles[y][x].corners = 0;
			if (!(r & SLOPE_STEEP))
			{
				if(r & SLOPE_N)
					mTiles[y][x].rtCorner = TILE_CORNER_UP;
				if (r & SLOPE_W)
					mTiles[y][x].ltCorner = TILE_CORNER_UP;
				if (r & SLOPE_E)
					mTiles[y][x].rbCorner = TILE_CORNER_UP;
				if (r & SLOPE_S)
					mTiles[y][x].lbCorner = TILE_CORNER_UP;

				if (r & SLOPE_N)
					mTiles[y][x].y--;
			}
			else
			{
				if (r & SLOPE_N)
					mTiles[y][x].rtCorner = TILE_CORNER_UP;
				if (r & SLOPE_W)
					mTiles[y][x].ltCorner = TILE_CORNER_UP;
				if (r & SLOPE_E)
					mTiles[y][x].rbCorner = TILE_CORNER_UP;
				if (r & SLOPE_S)
					mTiles[y][x].lbCorner = TILE_CORNER_UP;

				if (mTiles[y][x].rtCorner == TILE_CORNER_FLAT)
				{
					mTiles[y][x].rtCorner = TILE_CORNER_DOWN;
					mTiles[y][x].ltCorner = TILE_CORNER_FLAT;
					mTiles[y][x].rbCorner = TILE_CORNER_FLAT;
				}
				else if (mTiles[y][x].ltCorner == TILE_CORNER_FLAT)
				{
					mTiles[y][x].ltCorner = TILE_CORNER_DOWN;
					mTiles[y][x].lbCorner = TILE_CORNER_FLAT;
					mTiles[y][x].rtCorner = TILE_CORNER_FLAT;
				}
				else if (mTiles[y][x].rbCorner == TILE_CORNER_FLAT)
				{
					mTiles[y][x].rbCorner = TILE_CORNER_DOWN;
					mTiles[y][x].rtCorner = TILE_CORNER_FLAT;
					mTiles[y][x].lbCorner = TILE_CORNER_FLAT;
				}
				else if (mTiles[y][x].lbCorner == TILE_CORNER_FLAT)
				{
					mTiles[y][x].lbCorner = TILE_CORNER_DOWN;
					mTiles[y][x].ltCorner = TILE_CORNER_FLAT;
					mTiles[y][x].rbCorner = TILE_CORNER_FLAT;
				}

				if (mTiles[y][x].rtCorner == TILE_CORNER_UP)
					mTiles[y][x].y--;
				else if (mTiles[y][x].rtCorner == TILE_CORNER_DOWN)
					mTiles[y][x].y++;
			}
		}
	}
	//ConvertGroundTilesIntoWaterTiles();
	//CreateRivers();
}

static inline uint32_t GB(const uint32_t x, const uint8_t s, const uint8_t n)
{
	return (x >> s) & (((uint32_t)1U << n) - 1);
}

static inline uint32_t ScaleByMapSize(uint32_t n)
{
	/* Subtract 12 from shift in order to prevent integer overflow
	* for large values of n. It's safe since the min mapsize is 64x64. */
	return FX_DivS32(n << (6 + 6 - 12), 1 << 4);
}

void Map::PlaceTreesRandomly()
{
	int i, j, ht;

	MATHRandContext32 randcontext;
	MATH_InitRand32(&randcontext, OS_GetTick());

	i = ScaleByMapSize(/*DEFAULT_TREE_STEPS*/1000);
	//if (_game_mode == GM_EDITOR) i /= EDITOR_TREE_DIV;
	do {
		//uint32_t r = Random();
		//TileIndex tile = RandomTileSeed(r);
		uint32_t tile = MATH_Rand32(&randcontext, 0) % ((64 * 64) - 1);
		AddSceneryObject(new RCT2Tree1(tile & 0x3F, mTiles[(tile >> 6) & 0x3F][tile & 0x3F].y, (tile >> 6) & 0x3F, 0));

		//IncreaseGeneratingWorldProgress(GWP_TREE);

		//if (CanPlantTreesOnTile(tile, true)) {
			//PlaceTree(tile, r);
			/*if (_settings_game.game_creation.tree_placer != TP_IMPROVED) continue;

			/* Place a number of trees based on the tile height.
			*  This gives a cool effect of multiple trees close together.
			*  It is almost real life ;) /
			ht = GetTileZ(tile);
			/* The higher we get, the more trees we plant /
			j = GetTileZ(tile) * 2;
			/* Above snowline more trees! /
			if (_settings_game.game_creation.landscape == LT_ARCTIC && ht > GetSnowLine()) j *= 3;
			while (j--) {
				PlaceTreeAtSameHeight(tile, ht);
			}*/
		//}
	} while (--i);
}

void Map::GenerateTrees()
{
	uint32_t i, total;

	//if (_settings_game.game_creation.tree_placer == TP_NONE) return;

	//switch (_settings_game.game_creation.tree_placer) {
	//case TP_ORIGINAL: i = _settings_game.game_creation.landscape == LT_ARCTIC ? 15 : 6; break;
	//case TP_IMPROVED: i = _settings_game.game_creation.landscape == LT_ARCTIC ? 4 : 2; break;
	//default: NOT_REACHED();
	//}
	i = 6;

	total = ScaleByMapSize(1000);//DEFAULT_TREE_STEPS);
	//if (_settings_game.game_creation.landscape == LT_TROPIC) total += ScaleByMapSize(DEFAULT_RAINFOREST_TREE_STEPS);
	total *= i;
	//uint32_t num_groups = /*(_settings_game.game_creation.landscape != LT_TOYLAND) ? */ScaleByMapSize(GB(Random(), 0, 5) + 25);// : 0;
	//total += num_groups * 1000;// DEFAULT_TREE_STEPS;
	//SetGeneratingWorldProgress(GWP_TREE, total);

	//if (num_groups != 0) PlaceTreeGroups(num_groups);

	for (; i != 0; i--) {
		PlaceTreesRandomly();
	}
}