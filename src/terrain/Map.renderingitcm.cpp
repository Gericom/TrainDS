#include <nitro.h>
#include <nnsys/fnd.h>
#include <nnsys/g3d.h>
#include "core.h"
#include "util.h"
#include "TerrainManager.h"
#include "terrain/track/TrackPieceEx.h"
#include "terrain/track/FlexTrack.h"
#include "terrain/scenery/SceneryObject.h"
#include "terrain/scenery/RCT2Tree1.h"
#include "managers/TerrainTextureManager16.h"
#include "managers/TerrainTextureManager8.h"
#include "Map.h"

void Map::RecalculateNormals(int xstart, int xend, int zstart, int zend)
{
	for (int y = zstart; y < zend && y < 127; y++)
	{
		for (int x = xstart; x < xend && x < 127; x++)
		{
			fx32 hl = (mHMap[y * 128 + x - 1].y - Y_OFFSET) * Y_SCALE; //TERRAIN(t, x - 1, z);
			fx32 hr = (mHMap[y * 128 + x + 1].y - Y_OFFSET) * Y_SCALE; //TERRAIN(t, x + 1, z);
			fx32 hd = (mHMap[(y + 1) * 128 + x].y - Y_OFFSET) * Y_SCALE; //TERRAIN(t, x, z + 1); /* Terrain expands towards -Z /
			fx32 hu = (mHMap[(y - 1) * 128 + x].y - Y_OFFSET) * Y_SCALE; //TERRAIN(t, x, z - 1);
			VecFx32 norm = { hl - hr, 2 * FX32_ONE, hu - hd };

			VEC_Normalize(&norm, &norm);

			if (norm.x > GX_FX32_FX10_MAX) norm.x = GX_FX32_FX10_MAX;
			else if (norm.x < GX_FX32_FX10_MIN) norm.x = GX_FX32_FX10_MIN;
			if (norm.y > GX_FX32_FX10_MAX) norm.y = GX_FX32_FX10_MAX;
			else if (norm.y < GX_FX32_FX10_MIN) norm.y = GX_FX32_FX10_MIN;
			if (norm.z > GX_FX32_FX10_MAX) norm.z = GX_FX32_FX10_MAX;
			else if (norm.z < GX_FX32_FX10_MIN) norm.z = GX_FX32_FX10_MIN;

			mHMap[y * 128 + x].normal = GX_VECFX10(norm.x, norm.y, norm.z);
		}
	}
}

//extern "C" void render_tile(VecFx10* pNorm, uint8_t* pVtx, int x, int y);
extern "C" void render_tile(Map::hvtx_t* vtx, int x, int y);
extern "C" void render_tile2x2(Map::hvtx_t* vtx, int x, int y);

//#define DEBUG_TILE_COUNT

void Map::Render(int xstart, int xend, int zstart, int zend, bool picking, VecFx32* camPos, VecFx32* camDir, int lodLevel)
{
	//fx32 d = -(FX_Mul(camDir->x, camPos->x) + FX_Mul(camDir->y, camPos->y) + FX_Mul(camDir->z, camPos->z));
#ifdef DEBUG_TILE_COUNT
	int count = 0;
#endif
	G3_Translate(-32 * FX32_ONE, 0, -32 * FX32_ONE);
	G3_PushMtx();
	{
		G3_Scale(FX32_ONE / 64 * FX32_ONE, Y_SCALE * FX32_ONE / 128, FX32_ONE / 64 * FX32_ONE);
		G3_Translate(0, -Y_OFFSET * 128, 0);
		if (picking)
		{
			int i = 0;
			fx32 xadd = FX32_HALF - camPos->x - 32 * FX32_ONE;
			fx32 zadd = FX32_HALF - camPos->z - 32 * FX32_ONE;
			for (int y = zstart; y < zend && y + 1 <= 127; y++)
			{
				for (int x = xstart; x < xend && x + 1 <= 127; x++)
				{
					fx32 top = camDir->x * (x * FX32_ONE + xadd) + camDir->y * ((mHMap[y * 128 + x].y - Y_OFFSET) * Y_SCALE - camPos->y) + camDir->z * (y * FX32_ONE + zadd);
					if (top <= (20 * FX32_ONE * FX32_ONE))
					{
						reg_G3X_GXFIFO = GX_PACK_OP(G3OP_BEGIN, G3OP_COLOR, G3OP_VTX_10, G3OP_VTX_10);
						{
							reg_G3X_GXFIFO = GX_PACK_BEGIN_PARAM(GX_BEGIN_TRIANGLE_STRIP);
							reg_G3X_GXFIFO = GX_PACK_COLOR_PARAM(PICKING_COLOR(PICKING_TYPE_MAP, i + 1));
							reg_G3X_GXFIFO = (x << GX_VEC_VTX10_X_SHIFT) | (mHMap[y * 128 + x].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | (y << GX_VEC_VTX10_Z_SHIFT);//GX_PACK_VTX10_PARAM(x << 6, mHMap[y * 128 + x].y << 6, y << 6);
							reg_G3X_GXFIFO = (x << GX_VEC_VTX10_X_SHIFT) | (mHMap[(y + 1) * 128 + x].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | ((y + 1) << GX_VEC_VTX10_Z_SHIFT);//GX_PACK_VTX10_PARAM(x << 6, mHMap[(y + 1) * 128 + x].y << 6, (y << 6) + (1 << 6));
						}
						reg_G3X_GXFIFO = GX_PACK_OP(G3OP_VTX_10, G3OP_VTX_10, G3OP_END, G3OP_NOP);
						{
							reg_G3X_GXFIFO = ((x + 1) << GX_VEC_VTX10_X_SHIFT) | (mHMap[y * 128 + (x + 1)].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | (y << GX_VEC_VTX10_Z_SHIFT);//GX_PACK_VTX10_PARAM((x << 6) + (1 << 6), mHMap[y * 128 + (x + 1)].y << 6, y << 6);
							reg_G3X_GXFIFO = ((x + 1) << GX_VEC_VTX10_X_SHIFT) | (mHMap[(y + 1) * 128 + (x + 1)].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | ((y + 1) << GX_VEC_VTX10_Z_SHIFT);//GX_PACK_VTX10_PARAM((x << 6) + (1 << 6), mHMap[(y + 1) * 128 + (x + 1)].y << 6, (y << 6) + (1 << 6));
						}
#ifdef DEBUG_TILE_COUNT
						count++;
#endif
					}
					i++;
				}
			}
		}
		else
		{
			if (lodLevel == 0)
			{
				//setup texture matrix for vertex transform mode
				MtxFx44 texMtx =
				{
					FX32_ONE * FX32_ONE / 64 * 16 * 16, 0, 0, 0,
					0, 0, 0, 0,
					0, FX32_ONE * FX32_ONE / 64 * 16 * 16, 0, 0,
					0, 0, 0, 0
				};
				G3_MtxMode(GX_MTXMODE_TEXTURE);
				G3_LoadMtx44(&texMtx);
				G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

				fx32 xadd = FX32_HALF - camPos->x - 32 * FX32_ONE;
				fx32 zadd = FX32_HALF - camPos->z - 32 * FX32_ONE;
				//fx32 ything = camDir->y * ((mHMap[((zstart + zend) / 2) * 128 + ((xstart + xend) / 2)].y - Y_OFFSET) * Y_SCALE - camPos->y);
				//fx32 dist = camDir->x * (xstart * FX32_ONE + xadd) + camDir->z * (zstart * FX32_ONE + zadd);
				//fx32 distbase = camDir->x * (xstart * FX32_ONE + xadd) + camDir->z * (zstart * FX32_ONE + zadd) - camDir->y * camPos->y - camDir->y * Y_OFFSET * Y_SCALE;
				for (int y = zstart; y < zend && y + 1 <= 127; y++)
				{
					//fx32 dist = distbase;
					for (int x = xstart; x < xend && x + 1 <= 127; x++)
					{
						fx32 top = 
							camDir->x * (x * FX32_ONE + xadd) + 
							camDir->y * ((mHMap[y * 128 + x].y - Y_OFFSET) * Y_SCALE - camPos->y) + 
							camDir->z * (y * FX32_ONE + zadd); //VEC_DotProduct(camDir, &diff);*/
						//fx32 top = dist + camDir->y * mHMap[y * 128 + x].y * Y_SCALE;
						if (top <= (10 * FX32_ONE * FX32_ONE))
						{
							if (mGridEnabled)
								G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, ((x & 1) ^ (y & 1)) << 1, 31, GX_POLYGON_ATTR_MISC_FOG | GX_POLYGON_ATTR_MISC_FAR_CLIPPING);

							uint32_t texOffset = mTerrainTextureManager16->GetTextureAddress(
								mHMap[y * 128 + x].tex,
								mHMap[y * 128 + x + 1].tex,
								mHMap[(y + 1) * 128 + x].tex,
								mHMap[(y + 1) * 128 + x + 1].tex,
								mHMap[y * 128 + x].texAddress << 3);
							mHMap[y * 128 + x].texAddress = texOffset >> 3;
							//reg_G3_TEXIMAGE_PARAM = 0x1C900000 | (texOffset >> 3);
							reg_G3_TEXIMAGE_PARAM = 0xDC900000 | (texOffset >> 3);

							if (mLodLevels[y * 128 + x] == 1 || mLodLevels[y * 128 + (x + 1)] == 1 || mLodLevels[(y + 1) * 128 + x] == 1 || mLodLevels[(y + 1) * 128 + (x + 1)] == 1)
							{
								reg_G3X_GXFIFO = GX_PACK_OP(G3OP_BEGIN, G3OP_TEXCOORD, G3OP_NORMAL, G3OP_VTX_10);
								{
									reg_G3X_GXFIFO = GX_PACK_BEGIN_PARAM(GX_BEGIN_TRIANGLE_STRIP);
									reg_G3X_GXFIFO = ~(x << 8) ^ (y << 24);
									reg_G3X_GXFIFO = mHMap[y * 128 + x].normal;

									if (mLodLevels[y * 128 + x])
									{
										uint8_t tl = mHMap[(y & ~1) * 128 + (x & ~1)].y;
										uint8_t tr = mHMap[(y & ~1) * 128 + (x & ~1) + 2].y;
										uint8_t bl = mHMap[((y & ~1) + 2) * 128 + (x & ~1)].y;
										uint8_t br = mHMap[((y & ~1) + 2) * 128 + (x & ~1) + 2].y;
										uint16_t newy;
										if ((x & 1) && !(y & 1))
											newy = tl + tr;
										else if (!(x & 1) && (y & 1))
											newy = tl + bl;
										else if ((x & 1) && (y & 1))
											newy = (tl + tr + bl + br) >> 1;
										reg_G3X_GXFIFO = (x << GX_VEC_VTX10_X_SHIFT) | (newy << GX_VEC_VTX10_Y_SHIFT) | (y << GX_VEC_VTX10_Z_SHIFT);
									}
									else
										reg_G3X_GXFIFO = (x << GX_VEC_VTX10_X_SHIFT) | (mHMap[y * 128 + x].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | (y << GX_VEC_VTX10_Z_SHIFT);
								}
								reg_G3X_GXFIFO = GX_PACK_OP(G3OP_NORMAL, G3OP_VTX_10, G3OP_NORMAL, G3OP_VTX_10);
								{
									reg_G3X_GXFIFO = mHMap[(y + 1) * 128 + x].normal;
									if (mLodLevels[(y + 1) * 128 + x])
									{
										uint8_t tl = mHMap[(y & ~1) * 128 + (x & ~1)].y;
										uint8_t tr = mHMap[(y & ~1) * 128 + (x & ~1) + 2].y;
										uint8_t bl = mHMap[((y & ~1) + 2) * 128 + (x & ~1)].y;
										uint8_t br = mHMap[((y & ~1) + 2) * 128 + (x & ~1) + 2].y;
										uint16_t newy;
										if ((x & 1) && !((y + 1) & 1))
											newy = bl + br;
										else if (!(x & 1) && ((y + 1) & 1))
											newy = tl + bl;
										else if ((x & 1) && ((y + 1) & 1))
											newy = (tl + tr + bl + br) >> 1;
										reg_G3X_GXFIFO = (x << GX_VEC_VTX10_X_SHIFT) | (newy << GX_VEC_VTX10_Y_SHIFT) | ((y + 1) << GX_VEC_VTX10_Z_SHIFT);
									}
									else
										reg_G3X_GXFIFO = (x << GX_VEC_VTX10_X_SHIFT) | (mHMap[(y + 1) * 128 + x].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | ((y + 1) << GX_VEC_VTX10_Z_SHIFT);
									reg_G3X_GXFIFO = mHMap[y * 128 + (x + 1)].normal;
									if (mLodLevels[y * 128 + (x + 1)])
									{
										uint8_t tl = mHMap[(y & ~1) * 128 + (x & ~1)].y;
										uint8_t tr = mHMap[(y & ~1) * 128 + (x & ~1) + 2].y;
										uint8_t bl = mHMap[((y & ~1) + 2) * 128 + (x & ~1)].y;
										uint8_t br = mHMap[((y & ~1) + 2) * 128 + (x & ~1) + 2].y;
										uint16_t newy;
										if (((x + 1) & 1) && !(y & 1))
											newy = tl + tr;
										else if (!((x + 1) & 1) && (y & 1))
											newy = tr + br;
										else if (((x + 1) & 1) && (y & 1))
											newy = (tl + tr + bl + br) >> 1;
										reg_G3X_GXFIFO = ((x + 1) << GX_VEC_VTX10_X_SHIFT) | (newy << GX_VEC_VTX10_Y_SHIFT) | (y << GX_VEC_VTX10_Z_SHIFT);
									}
									else
										reg_G3X_GXFIFO = ((x + 1) << GX_VEC_VTX10_X_SHIFT) | (mHMap[y * 128 + (x + 1)].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | (y << GX_VEC_VTX10_Z_SHIFT);
								}
								reg_G3X_GXFIFO = GX_PACK_OP(G3OP_NORMAL, G3OP_VTX_10, G3OP_END, G3OP_NOP);
								{	
									reg_G3X_GXFIFO = mHMap[(y + 1) * 128 + (x + 1)].normal;
									if (mLodLevels[(y + 1) * 128 + (x + 1)])
									{
										uint8_t tl = mHMap[(y & ~1) * 128 + (x & ~1)].y;
										uint8_t tr = mHMap[(y & ~1) * 128 + (x & ~1) + 2].y;
										uint8_t bl = mHMap[((y & ~1) + 2) * 128 + (x & ~1)].y;
										uint8_t br = mHMap[((y & ~1) + 2) * 128 + (x & ~1) + 2].y;
										uint16_t newy;
										if (((x + 1) & 1) && !((y + 1) & 1))
											newy = bl + br;
										else if (!((x + 1) & 1) && ((y + 1) & 1))
											newy = tr + br;
										else if (((x + 1) & 1) && ((y + 1) & 1))
											newy = (tl + tr + bl + br) >> 1;
										reg_G3X_GXFIFO = ((x + 1) << GX_VEC_VTX10_X_SHIFT) | (newy << GX_VEC_VTX10_Y_SHIFT) | ((y + 1) << GX_VEC_VTX10_Z_SHIFT);
									}
									else
										reg_G3X_GXFIFO = ((x + 1) << GX_VEC_VTX10_X_SHIFT) | (mHMap[(y + 1) * 128 + (x + 1)].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | ((y + 1) << GX_VEC_VTX10_Z_SHIFT);
								}
							}
							else
							{
								//TODO: Include the loop in this function:
								render_tile(&mHMap[y * 128 + x], x, y);
							}
#ifdef DEBUG_TILE_COUNT
							count++;
#endif
						}
						//dist += camDir->x * FX32_ONE;
					}
					//distbase += camDir->z * FX32_ONE;
				}
			}
			else if (lodLevel == 1)
			{
				MtxFx44 texMtx =
				{
					FX32_ONE * FX32_ONE / 64 * 8 * 16, 0, 0, 0,
					0, 0, 0, 0,
					0, FX32_ONE * FX32_ONE / 64 * 4 * 16, 0, 0,
					0, 0, 0, 0
				};
				G3_MtxMode(GX_MTXMODE_TEXTURE);
				G3_LoadMtx44(&texMtx);
				G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
				MI_CpuClearFast(mLodLevels, 128 * 128);
				//int count = 0;
				G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_FOG | GX_POLYGON_ATTR_MISC_FAR_CLIPPING);
				fx32 xadd = FX32_ONE - camPos->x - 32 * FX32_ONE;
				fx32 zadd = FX32_ONE - camPos->z - 32 * FX32_ONE;
				//fx32 ything = camDir->y * ((mHMap[((zstart + zend) / 2) * 128 + ((xstart + xend) / 2)].y - Y_OFFSET) * Y_SCALE - camPos->y);
				for (int y = zstart & ~1; y < (zend | 1) && y + 2 <= 127; y += 2)
				{
					for (int x = xstart & ~1; x < (xend | 1) && x + 2 <= 127; x += 2)
					{
						fx32 top = 
							camDir->x * (x * FX32_ONE + xadd) + 
							camDir->y * ((mHMap[y * 128 + x].y - Y_OFFSET) * Y_SCALE - camPos->y) + 
							camDir->z * (y * FX32_ONE + zadd);//FX_Mul(camDir->x, diff.x) + FX_Mul(camDir->y, diff.y) + FX_Mul(camDir->z, diff.z);////VEC_DotProduct(camDir, &diff);

						if (top >= (8 * FX32_ONE * FX32_ONE) && top <= (/*35*/25 * FX32_ONE * FX32_ONE))
						{
							mLodLevels[y * 128 + (x + 1)] = 1;
							mLodLevels[(y + 1) * 128 + x] = 1;
							mLodLevels[(y + 1) * 128 + (x + 1)] = 1;
							mLodLevels[(y + 2) * 128 + (x + 1)] = 1;
							mLodLevels[(y + 1) * 128 + (x + 2)] = 1;

							uint32_t texOffset = mTerrainTextureManager8->GetTextureAddress(
								mHMap[y * 128 + x].tex,
								mHMap[y * 128 + x + 2].tex,
								mHMap[(y + 2) * 128 + x].tex,
								mHMap[(y + 2) * 128 + x + 2].tex,
								mHMap[y * 128 + x].texAddress << 3);
							mHMap[y * 128 + x].texAddress = texOffset >> 3;

							reg_G3_TEXIMAGE_PARAM = 0xDC100000 | (texOffset >> 3);
							render_tile2x2(&mHMap[y * 128 + x], x, y);
#ifdef DEBUG_TILE_COUNT
							count++;
#endif
						}
						else if (top > (/*35*/25 * FX32_ONE * FX32_ONE) && top <= (50 * FX32_ONE * FX32_ONE))
						{
							mLodLevels[y * 128 + (x + 1)] = 1;
							mLodLevels[(y + 1) * 128 + x] = 1;
							mLodLevels[(y + 1) * 128 + (x + 1)] = 1;
							mLodLevels[(y + 2) * 128 + (x + 1)] = 1;
							mLodLevels[(y + 1) * 128 + (x + 2)] = 1;
							reg_G3X_GXFIFO = GX_PACK_OP(G3OP_TEXIMAGE_PARAM, G3OP_BEGIN, G3OP_LIGHT_COLOR, G3OP_NORMAL);
							{
								reg_G3X_GXFIFO = 0;
								reg_G3X_GXFIFO = GX_PACK_BEGIN_PARAM(GX_BEGIN_TRIANGLE_STRIP);
								reg_G3X_GXFIFO = GX_PACK_LIGHTCOLOR_PARAM(0, mTerrainTextureManager16->mTextureDatas[mHMap[y * 128 + x].tex][0]);
								reg_G3X_GXFIFO = mHMap[y * 128 + x].normal;
							}
							reg_G3X_GXFIFO = GX_PACK_OP(G3OP_VTX_10, G3OP_LIGHT_COLOR, G3OP_NORMAL, G3OP_VTX_10);
							{
								reg_G3X_GXFIFO = (x << GX_VEC_VTX10_X_SHIFT) | (mHMap[y * 128 + x].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | (y << GX_VEC_VTX10_Z_SHIFT);//GX_PACK_VTX10_PARAM(x << 6, mVtx[y * 128 + x] << 6, y << 6);
								reg_G3X_GXFIFO = GX_PACK_LIGHTCOLOR_PARAM(0, mTerrainTextureManager16->mTextureDatas[mHMap[(y + 2) * 128 + x].tex][15 * 16]);
								reg_G3X_GXFIFO = mHMap[(y + 2) * 128 + x].normal;
								reg_G3X_GXFIFO = (x << GX_VEC_VTX10_X_SHIFT) | (mHMap[(y + 2) * 128 + x].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | ((y + 2) << GX_VEC_VTX10_Z_SHIFT);//GX_PACK_VTX10_PARAM(x << 6, mVtx[(y + 2) * 128 + x] << 6, (y << 6) + (2 << 6));					
							}
							reg_G3X_GXFIFO = GX_PACK_OP(G3OP_LIGHT_COLOR, G3OP_NORMAL, G3OP_VTX_10, G3OP_LIGHT_COLOR);
							{
								reg_G3X_GXFIFO = GX_PACK_LIGHTCOLOR_PARAM(0, mTerrainTextureManager16->mTextureDatas[mHMap[y * 128 + x + 2].tex][15]);
								reg_G3X_GXFIFO = mHMap[y * 128 + (x + 2)].normal;
								reg_G3X_GXFIFO = ((x + 2) << GX_VEC_VTX10_X_SHIFT) | (mHMap[y * 128 + (x + 2)].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | (y << GX_VEC_VTX10_Z_SHIFT);//GX_PACK_VTX10_PARAM((x << 6) + (2 << 6), mVtx[y * 128 + (x + 2)] << 6, y << 6);
								reg_G3X_GXFIFO = GX_PACK_LIGHTCOLOR_PARAM(0, mTerrainTextureManager16->mTextureDatas[mHMap[(y + 2) * 128 + x + 2].tex][15 * 16 + 15]);								
							}
							reg_G3X_GXFIFO = GX_PACK_OP(G3OP_NORMAL, G3OP_VTX_10, G3OP_END, G3OP_LIGHT_COLOR);
							{
								reg_G3X_GXFIFO = mHMap[(y + 2) * 128 + (x + 2)].normal;
								reg_G3X_GXFIFO = ((x + 2) << GX_VEC_VTX10_X_SHIFT) | (mHMap[(y + 2) * 128 + (x + 2)].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | ((y + 2) << GX_VEC_VTX10_Z_SHIFT);//GX_PACK_VTX10_PARAM((x << 6) + (2 << 6), mVtx[(y + 2) * 128 + (x + 2)] << 6, (y << 6) + (2 << 6));
								reg_G3X_GXFIFO = GX_PACK_LIGHTCOLOR_PARAM(0, 0x7FFF);
							}
#ifdef DEBUG_TILE_COUNT
							count++;
#endif
						}
					}
				}
			}
			G3_MtxMode(GX_MTXMODE_TEXTURE);
			G3_Identity();
			G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
		}
	}
	G3_PopMtx(1);
#ifdef DEBUG_TILE_COUNT
	OS_Printf("%d\n", count);
#endif
	if (!picking)
	{
		TrackPieceEx* trackPiece = NULL;
		while ((trackPiece = (TrackPieceEx*)NNS_FndGetNextListObject(&mTrackList, trackPiece)) != NULL)
		{
			//if (trackPiece->mPosition.x >= xstart && trackPiece->mPosition.x < xend &&
			//	trackPiece->mPosition.z >= zstart && trackPiece->mPosition.z < zend)
			//{
				if (picking) G3_MaterialColorSpecEmi(0, 0, FALSE);
				trackPiece->Render(mTerrainManager);
			//}
		}
		if (mGhostPiece != NULL)
		{
			if (picking) G3_MaterialColorSpecEmi(0, 0, FALSE);
			mGhostPiece->Render(mTerrainManager);
		}
		trackPiece = NULL;
		while ((trackPiece = (TrackPieceEx*)NNS_FndGetNextListObject(&mTrackList, trackPiece)) != NULL)
		{
			trackPiece->RenderMarkers(mTerrainManager);
		}
		if (mGhostPiece != NULL)
		{
			mGhostPiece->RenderMarkers(mTerrainManager);
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
	//TODO: move this elsewhere, since this doesn't update with sub 3d rendering
	mTerrainManager->mTrackMarkerRotation += FX32_CONST(2);
	if (mTerrainManager->mTrackMarkerRotation >= 360 * FX32_ONE)
		mTerrainManager->mTrackMarkerRotation -= 360 * FX32_ONE;

	mTerrainTextureManager8->UpdateResourceCounter();
	mTerrainTextureManager16->UpdateResourceCounter();
}