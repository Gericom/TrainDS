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

void Map::RecalculateNormals(hvtx_t* pHMap, int xstart, int xend, int zstart, int zend)
{
	pHMap += 2 * MAP_BLOCK_WIDTH + 2;
	for (int y = zstart; y < zend && y < 129; y++)
	{
		for (int x = xstart; x < xend && x < 129; x++)
		{
			fx32 hl = (pHMap[y * MAP_BLOCK_WIDTH + x - 1].y - Y_OFFSET) * Y_SCALE; //TERRAIN(t, x - 1, z);
			fx32 hr = (pHMap[y * MAP_BLOCK_WIDTH + x + 1].y - Y_OFFSET) * Y_SCALE; //TERRAIN(t, x + 1, z);
			fx32 hd = (pHMap[(y + 1) * MAP_BLOCK_WIDTH + x].y - Y_OFFSET) * Y_SCALE; //TERRAIN(t, x, z + 1); /* Terrain expands towards -Z /
			fx32 hu = (pHMap[(y - 1) * MAP_BLOCK_WIDTH + x].y - Y_OFFSET) * Y_SCALE; //TERRAIN(t, x, z - 1);
			VecFx32 norm = { hl - hr, 2 * FX32_ONE, hu - hd };

			VEC_Normalize(&norm, &norm);

			if (norm.x > GX_FX32_FX10_MAX) norm.x = GX_FX32_FX10_MAX;
			else if (norm.x < GX_FX32_FX10_MIN) norm.x = GX_FX32_FX10_MIN;
			if (norm.y > GX_FX32_FX10_MAX) norm.y = GX_FX32_FX10_MAX;
			else if (norm.y < GX_FX32_FX10_MIN) norm.y = GX_FX32_FX10_MIN;
			if (norm.z > GX_FX32_FX10_MAX) norm.z = GX_FX32_FX10_MAX;
			else if (norm.z < GX_FX32_FX10_MIN) norm.z = GX_FX32_FX10_MIN;

			pHMap[y * MAP_BLOCK_WIDTH + x].normal = GX_VECFX10(norm.x, norm.y, norm.z);
		}
	}
}

//extern "C" void render_tile(VecFx10* pNorm, uint8_t* pVtx, int x, int y);
extern "C" void render_tile(hvtx_t* vtx, int x, int y);
extern "C" void render_tile2x2(hvtx_t* vtx, int x, int y);

//#define DEBUG_TILE_COUNT

//TODO: Fix mLodLevels
void Map::Render(hvtx_t* pHMap, int xstart, int xend, int zstart, int zend, bool picking, VecFx32* camPos, VecFx32* camDir, int lodLevel, u8* lodData, fx32 xshift, fx32 zshift)
{
	pHMap += 2 * MAP_BLOCK_WIDTH + 2;
	//fx32 d = -(FX_Mul(camDir->x, camPos->x) + FX_Mul(camDir->y, camPos->y) + FX_Mul(camDir->z, camPos->z));
#ifdef DEBUG_TILE_COUNT
	int count = 0;
#endif
	G3_PushMtx();
	{
		G3_Scale(FX32_ONE / 64 * FX32_ONE, Y_SCALE * FX32_ONE / 128, FX32_ONE / 64 * FX32_ONE);
		G3_Translate(0, -Y_OFFSET * 128, 0);
		if (picking)
		{
			int i = 0;
			fx32 xadd = FX32_HALF - camPos->x - 32 * FX32_ONE;
			fx32 zadd = FX32_HALF - camPos->z - 32 * FX32_ONE;
			hvtx_t* pmap = pHMap + zstart * MAP_BLOCK_WIDTH + xstart;
			for (int y = zstart; y < zend && y + 1 <= 127; y++)
			{
				hvtx_t* pmap2 = pmap;
				for (int x = xstart; x < xend && x + 1 <= 127; x++)
				{
					fx32 top = camDir->x * (x * FX32_ONE + xadd) + camDir->y * ((pmap2[0].y - Y_OFFSET) * Y_SCALE - camPos->y) + camDir->z * (y * FX32_ONE + zadd);
					if (top <= (20 * FX32_ONE * FX32_ONE))
					{
						reg_G3X_GXFIFO = GX_PACK_OP(G3OP_BEGIN, G3OP_COLOR, G3OP_VTX_10, G3OP_VTX_10);
						{
							reg_G3X_GXFIFO = GX_PACK_BEGIN_PARAM(GX_BEGIN_TRIANGLE_STRIP);
							reg_G3X_GXFIFO = GX_PACK_COLOR_PARAM(PICKING_COLOR(PICKING_TYPE_MAP, i + 1));
							reg_G3X_GXFIFO = (x << GX_VEC_VTX10_X_SHIFT) | (pmap2[0].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | (y << GX_VEC_VTX10_Z_SHIFT);
							reg_G3X_GXFIFO = (x << GX_VEC_VTX10_X_SHIFT) | (pmap2[MAP_BLOCK_WIDTH].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | ((y + 1) << GX_VEC_VTX10_Z_SHIFT);
						}
						reg_G3X_GXFIFO = GX_PACK_OP(G3OP_VTX_10, G3OP_VTX_10, G3OP_END, G3OP_NOP);
						{
							reg_G3X_GXFIFO = ((x + 1) << GX_VEC_VTX10_X_SHIFT) | (pmap2[1].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | (y << GX_VEC_VTX10_Z_SHIFT);
							reg_G3X_GXFIFO = ((x + 1) << GX_VEC_VTX10_X_SHIFT) | (pmap2[MAP_BLOCK_WIDTH + 1].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | ((y + 1) << GX_VEC_VTX10_Z_SHIFT);
						}
#ifdef DEBUG_TILE_COUNT
						count++;
#endif
					}
					i++;
					pmap2++;
				}
				pmap++;
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

				G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_FOG | GX_POLYGON_ATTR_MISC_FAR_CLIPPING);

				fx32 xadd = FX32_HALF - camPos->x - 32 * FX32_ONE;
				fx32 zadd = FX32_HALF - camPos->z - 32 * FX32_ONE;
				//fx32 ything = camDir->y * ((pHMap[((zstart + zend) / 2) * 128 + ((xstart + xend) / 2)].y - Y_OFFSET) * Y_SCALE - camPos->y);
				//fx32 dist = camDir->x * (xstart * FX32_ONE + xadd) + camDir->z * (zstart * FX32_ONE + zadd);
				//fx32 distbase = camDir->x * (xstart * FX32_ONE + xadd) + camDir->z * (zstart * FX32_ONE + zadd) - camDir->y * camPos->y - camDir->y * Y_OFFSET * Y_SCALE;
				if (zend > 128)
					zend = 128;
				if (xend > 128)
					xend = 128;
				hvtx_t* pmap = pHMap + zstart * MAP_BLOCK_WIDTH + xstart;
				for (int y = zstart; y < zend; y++)
				{
					//fx32 dist = distbase;
					hvtx_t* pmap2 = pmap;
					for (int x = xstart; x < xend; x++)
					{
						fx32 top = 
							camDir->x * (x * FX32_ONE + xadd) + 
							camDir->y * ((pmap2[0].y - Y_OFFSET) * Y_SCALE - camPos->y) +
							camDir->z * (y * FX32_ONE + zadd); //VEC_DotProduct(camDir, &diff);
						//fx32 top = dist + camDir->y * pHMap[y * 128 + x].y * Y_SCALE;
						if (top <= (10 * FX32_ONE * FX32_ONE))
						{
							if (mGridEnabled)
								G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, ((x & 1) ^ (y & 1)) << 1, 31, GX_POLYGON_ATTR_MISC_FOG | GX_POLYGON_ATTR_MISC_FAR_CLIPPING);

							uint32_t texOffset = mTerrainTextureManager16->GetTextureAddress(
								pmap2[0].tex,
								pmap2[1].tex,
								pmap2[MAP_BLOCK_WIDTH].tex,
								pmap2[MAP_BLOCK_WIDTH + 1].tex,
								pmap2[0].texAddress << 3);
							pmap2[0].texAddress = texOffset >> 3;
							//reg_G3_TEXIMAGE_PARAM = 0x1C900000 | (texOffset >> 3);
							reg_G3_TEXIMAGE_PARAM = 0xDC900000 | (texOffset >> 3);

							if (lodData[y * 128 + x] || lodData[y * 128 + (x + 1)] || lodData[(y + 1) * 128 + x] || lodData[(y + 1) * 128 + (x + 1)])
							{
								hvtx_t* pmap3 = pmap2;
								if (x & 1)
									pmap3--;
								if (y & 1)
									pmap3 -= MAP_BLOCK_WIDTH;
								reg_G3X_GXFIFO = GX_PACK_OP(G3OP_BEGIN, G3OP_TEXCOORD, G3OP_NORMAL, G3OP_VTX_10);
								{
									reg_G3X_GXFIFO = GX_PACK_BEGIN_PARAM(GX_BEGIN_TRIANGLE_STRIP);
									reg_G3X_GXFIFO = ~(x << 8) ^ (y << 24);
									reg_G3X_GXFIFO = pmap2[0].normal;

									if (lodData[y * 128 + x])
									{
										uint8_t tl = pmap3[0].y;
										uint8_t tr = pmap3[2].y;
										uint8_t bl = pmap3[2 * MAP_BLOCK_WIDTH].y;
										uint8_t br = pmap3[2 * MAP_BLOCK_WIDTH + 2].y;
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
										reg_G3X_GXFIFO = (x << GX_VEC_VTX10_X_SHIFT) | (pmap2[0].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | (y << GX_VEC_VTX10_Z_SHIFT);
								}
								reg_G3X_GXFIFO = GX_PACK_OP(G3OP_NORMAL, G3OP_VTX_10, G3OP_NORMAL, G3OP_VTX_10);
								{
									reg_G3X_GXFIFO = pmap2[MAP_BLOCK_WIDTH].normal;
									if (lodData[(y + 1) * 128 + x])
									{
										uint8_t tl = pmap3[0].y;
										uint8_t tr = pmap3[2].y;
										uint8_t bl = pmap3[2 * MAP_BLOCK_WIDTH].y;
										uint8_t br = pmap3[2 * MAP_BLOCK_WIDTH + 2].y;
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
										reg_G3X_GXFIFO = (x << GX_VEC_VTX10_X_SHIFT) | (pmap2[MAP_BLOCK_WIDTH].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | ((y + 1) << GX_VEC_VTX10_Z_SHIFT);
									reg_G3X_GXFIFO = pmap2[1].normal;
									if (lodData[y * 128 + (x + 1)])
									{
										uint8_t tl = pmap3[0].y;
										uint8_t tr = pmap3[2].y;
										uint8_t bl = pmap3[2 * MAP_BLOCK_WIDTH].y;
										uint8_t br = pmap3[2 * MAP_BLOCK_WIDTH + 2].y;
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
										reg_G3X_GXFIFO = ((x + 1) << GX_VEC_VTX10_X_SHIFT) | (pmap2[1].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | (y << GX_VEC_VTX10_Z_SHIFT);
								}
								reg_G3X_GXFIFO = GX_PACK_OP(G3OP_NORMAL, G3OP_VTX_10, G3OP_END, G3OP_NOP);
								{	
									reg_G3X_GXFIFO = pmap2[MAP_BLOCK_WIDTH + 1].normal;
									if (lodData[(y + 1) * 128 + (x + 1)])
									{
										uint8_t tl = pmap3[0].y;
										uint8_t tr = pmap3[2].y;
										uint8_t bl = pmap3[2 * MAP_BLOCK_WIDTH].y;
										uint8_t br = pmap3[2 * MAP_BLOCK_WIDTH + 2].y;
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
										reg_G3X_GXFIFO = ((x + 1) << GX_VEC_VTX10_X_SHIFT) | (pmap2[MAP_BLOCK_WIDTH + 1].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | ((y + 1) << GX_VEC_VTX10_Z_SHIFT);
								}
							}
							else
							{
								//TODO: Include the loop in this function:
								render_tile(pmap2, x, y);
							}
#ifdef DEBUG_TILE_COUNT
							count++;
#endif
						}
						pmap2++;
						//dist += camDir->x * FX32_ONE;
					}
					pmap += MAP_BLOCK_WIDTH;
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
				//int count = 0;
				G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_FOG | GX_POLYGON_ATTR_MISC_FAR_CLIPPING);

				fx32 xadd = FX32_ONE - camPos->x - 32 * FX32_ONE;
				fx32 zadd = FX32_ONE - camPos->z - 32 * FX32_ONE;
				//fx32 ything = camDir->y * ((pHMap[((zstart + zend) / 2) * 128 + ((xstart + xend) / 2)].y - Y_OFFSET) * Y_SCALE - camPos->y);
				zstart &= ~1;
				xstart &= ~1;
				zend |= 1;
				xend |= 1;
				if (zend > 127)
					zend = 127;
				if (xend > 127)
					xend = 127;
				hvtx_t* pmap = pHMap + zstart * MAP_BLOCK_WIDTH + xstart;
				for (int y = zstart; y < zend; y += 2)
				{
					hvtx_t* pmap2 = pmap;
					for (int x = xstart; x < xend; x += 2)
					{
						fx32 top = 
							camDir->x * (x * FX32_ONE + xadd) + 
							camDir->y * ((pmap2[0].y - Y_OFFSET) * Y_SCALE - camPos->y) + 
							camDir->z * (y * FX32_ONE + zadd);//FX_Mul(camDir->x, diff.x) + FX_Mul(camDir->y, diff.y) + FX_Mul(camDir->z, diff.z);////VEC_DotProduct(camDir, &diff);

						if (top >= (8 * FX32_ONE * FX32_ONE) && top <= (/*35*/25 * FX32_ONE * FX32_ONE))
						{
							lodData[y * 128 + (x + 1)] = 1;
							lodData[(y + 1) * 128 + x] = 1;
							lodData[(y + 1) * 128 + (x + 1)] = 1;
							lodData[(y + 2) * 128 + (x + 1)] = 1;
							lodData[(y + 1) * 128 + (x + 2)] = 1;

							uint32_t texOffset = mTerrainTextureManager8->GetTextureAddress(
								pmap2[0].tex,
								pmap2[2].tex,
								pmap2[2 * MAP_BLOCK_WIDTH].tex,
								pmap2[2 * MAP_BLOCK_WIDTH + 2].tex,
								pmap2[0].texAddress << 3);
							pmap2[0].texAddress = texOffset >> 3;

							reg_G3_TEXIMAGE_PARAM = 0xDC100000 | (texOffset >> 3);
							render_tile2x2(pmap2, x, y);
#ifdef DEBUG_TILE_COUNT
							count++;
#endif
						}
						else if (top > (/*35*/25 * FX32_ONE * FX32_ONE) && top <= (52 * FX32_ONE * FX32_ONE))
						{
							lodData[y * 128 + (x + 1)] = 1;
							lodData[(y + 1) * 128 + x] = 1;
							lodData[(y + 1) * 128 + (x + 1)] = 1;
							lodData[(y + 2) * 128 + (x + 1)] = 1;
							lodData[(y + 1) * 128 + (x + 2)] = 1;

							reg_G3X_GXFIFO = GX_PACK_OP(G3OP_TEXIMAGE_PARAM, G3OP_BEGIN, G3OP_LIGHT_COLOR, G3OP_NORMAL);
							{
								reg_G3X_GXFIFO = 0;
								reg_G3X_GXFIFO = GX_PACK_BEGIN_PARAM(GX_BEGIN_TRIANGLE_STRIP);
								reg_G3X_GXFIFO = GX_PACK_LIGHTCOLOR_PARAM(0, mTerrainTextureManager16->mTextureDatas[pmap2[0].tex][0]);
								reg_G3X_GXFIFO = pmap2[0].normal;
							}
							reg_G3X_GXFIFO = GX_PACK_OP(G3OP_VTX_10, G3OP_LIGHT_COLOR, G3OP_NORMAL, G3OP_VTX_10);
							{
								reg_G3X_GXFIFO = (x << GX_VEC_VTX10_X_SHIFT) | (pmap2[0].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | (y << GX_VEC_VTX10_Z_SHIFT);//GX_PACK_VTX10_PARAM(x << 6, mVtx[y * 128 + x] << 6, y << 6);
								reg_G3X_GXFIFO = GX_PACK_LIGHTCOLOR_PARAM(0, mTerrainTextureManager16->mTextureDatas[pmap2[2 * MAP_BLOCK_WIDTH].tex][15 * 16]);
								reg_G3X_GXFIFO = pmap2[2 * MAP_BLOCK_WIDTH].normal;
								reg_G3X_GXFIFO = (x << GX_VEC_VTX10_X_SHIFT) | (pmap2[2 * MAP_BLOCK_WIDTH].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | ((y + 2) << GX_VEC_VTX10_Z_SHIFT);//GX_PACK_VTX10_PARAM(x << 6, mVtx[(y + 2) * 128 + x] << 6, (y << 6) + (2 << 6));					
							}
							reg_G3X_GXFIFO = GX_PACK_OP(G3OP_LIGHT_COLOR, G3OP_NORMAL, G3OP_VTX_10, G3OP_LIGHT_COLOR);
							{
								reg_G3X_GXFIFO = GX_PACK_LIGHTCOLOR_PARAM(0, mTerrainTextureManager16->mTextureDatas[pmap2[2].tex][15]);
								reg_G3X_GXFIFO = pmap2[2].normal;
								reg_G3X_GXFIFO = ((x + 2) << GX_VEC_VTX10_X_SHIFT) | (pmap2[2].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | (y << GX_VEC_VTX10_Z_SHIFT);//GX_PACK_VTX10_PARAM((x << 6) + (2 << 6), mVtx[y * 128 + (x + 2)] << 6, y << 6);
								reg_G3X_GXFIFO = GX_PACK_LIGHTCOLOR_PARAM(0, mTerrainTextureManager16->mTextureDatas[pmap2[2 * MAP_BLOCK_WIDTH + 2].tex][15 * 16 + 15]);
							}
							reg_G3X_GXFIFO = GX_PACK_OP(G3OP_NORMAL, G3OP_VTX_10, G3OP_END, G3OP_LIGHT_COLOR);
							{
								reg_G3X_GXFIFO = pmap2[2 * MAP_BLOCK_WIDTH + 2].normal;
								reg_G3X_GXFIFO = ((x + 2) << GX_VEC_VTX10_X_SHIFT) | (pmap2[2 * MAP_BLOCK_WIDTH + 2].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | ((y + 2) << GX_VEC_VTX10_Z_SHIFT);//GX_PACK_VTX10_PARAM((x << 6) + (2 << 6), mVtx[(y + 2) * 128 + (x + 2)] << 6, (y << 6) + (2 << 6));
								reg_G3X_GXFIFO = GX_PACK_LIGHTCOLOR_PARAM(0, 0x7FFF);
							}
#ifdef DEBUG_TILE_COUNT
							count++;
#endif
						}
						pmap2 += 2;
					}
					pmap += 2 * MAP_BLOCK_WIDTH;
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
}