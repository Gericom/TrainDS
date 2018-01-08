#include "common.h"
#include "util.h"
#include "TerrainManager.h"
#include "terrain/track/TrackPieceEx.h"
#include "terrain/track/FlexTrack.h"
#include "terrain/scenery/SimpleSceneryObject.h"
#include "terrain/scenery/RCT2Tree1.h"
#include "managers/TerrainTextureManager16.h"
#include "managers/TerrainTextureManager8.h"
#include "GameController.h"
#include "io/TerrainData.h"
#include "io/ObjectData.h"
#include "engine/objects/Water.h"
#include "box2d.h"
#include "engine/QuadTree.h"
#include "Map.h"

void Map::RecalculateNormals(hvtx_t* pHMap, int xstart, int xend, int zstart, int zend)
{
	pHMap += 2 * MAP_BLOCK_WIDTH + 2;
	for (int y = zstart; y < zend && y < 129; y++)
	{
		for (int x = xstart; x < xend && x < 129; x++)
		{
			int hl = pHMap[y * MAP_BLOCK_WIDTH + x - 1].y; //TERRAIN(t, x - 1, z);
			int hr = pHMap[y * MAP_BLOCK_WIDTH + x + 1].y; //TERRAIN(t, x + 1, z);
			int hd = pHMap[(y + 1) * MAP_BLOCK_WIDTH + x].y; //TERRAIN(t, x, z + 1); /* Terrain expands towards -Z /
			int hu = pHMap[(y - 1) * MAP_BLOCK_WIDTH + x].y; //TERRAIN(t, x, z - 1);
			VecFx32 norm = { hl - hr, 2 * FX32_ONE / Y_SCALE, hu - hd };

			VEC_Normalize(&norm, &norm);

			if (norm.x > GX_FX32_FX10_MAX) 
				norm.x = GX_FX32_FX10_MAX;
			if (norm.y > GX_FX32_FX10_MAX) 
				norm.y = GX_FX32_FX10_MAX;
			if (norm.z > GX_FX32_FX10_MAX) 
				norm.z = GX_FX32_FX10_MAX;

			pHMap[y * MAP_BLOCK_WIDTH + x].normal = GX_VECFX10(norm.x, norm.y, norm.z);
		}
	}
}

void Map::Render(int xstart, int xend, int zstart, int zend, int xstart2, int xend2, int zstart2, int zend2, bool picking, VecFx32* camPos, VecFx32* camDir, int lodLevel)
{
	if (!picking && lodLevel == 1)
	{
		MI_CpuClearFast(mLodLevels, 128 * 128);
		//center the stuff
		int lodx = (128 - (xend - xstart)) >> 1;
		int lody = (128 - (zend - zstart)) >> 1;
		mLastLod = &mLodLevels[lody * 128 + lodx];
		mLastXStart = xstart;
		mLastZStart = zstart;
	}
	G3_PushMtx();
	{
		for (int y = zstart >> 7; y < (zend + 127) >> 7; y++)
		{
			for (int x = xstart >> 7; x < (xend + 127) >> 7; x++)
			{
				G3_PushMtx();
				{
					G3_Translate(x * 128 * FX32_ONE, 0, y * 128 * FX32_ONE);
					int xstart2 = xstart - x * 128;
					xstart2 = MATH_CLAMP(xstart2, 0, 128);
					int xend2 = xend - x * 128;
					xend2 = MATH_CLAMP(xend2, 0, 128);
					int zstart2 = zstart - y * 128;
					zstart2 = MATH_CLAMP(zstart2, 0, 128);
					int zend2 = zend - y * 128;
					zend2 = MATH_CLAMP(zend2, 0, 128);

					VecFx32 cam2 = *camPos;
					cam2.x -= x * 128 * FX32_ONE;
					cam2.z -= y * 128 * FX32_ONE;

					hvtx_t* pMap = GetMapBlock(x, y, true);
					if (pMap)
						Render(pMap, xstart2, xend2, zstart2, zend2, picking, &cam2, camDir, lodLevel, &mLastLod[(y * 128 - mLastZStart) * 128 + x * 128 - mLastXStart]);
				}
				G3_PopMtx(1);
			}
		}
		if (!picking)
		{
			box2d_t frustumbox = { xstart2 * FX32_ONE, zstart2 * FX32_ONE, xend2 * FX32_ONE, zend2 * FX32_ONE };
			/*TrackPieceEx* trackPiece = NULL;
			while ((trackPiece = (TrackPieceEx*)NNS_FndGetNextListObject(&mTrackList, trackPiece)) != NULL)
			{
				box2d_t bounds;
				trackPiece->GetBounds(&bounds);
				if (!bounds.Intersects(&frustumbox))
					continue;
				//if (trackPiece->mPosition.x >= xstart && trackPiece->mPosition.x < xend &&
				//	trackPiece->mPosition.z >= zstart && trackPiece->mPosition.z < zend)
				//{
				if (picking) G3_MaterialColorSpecEmi(0, 0, FALSE);
				trackPiece->Render();
				//}
			}*/
			if (mGhostPiece != NULL)
			{
				if (picking) G3_MaterialColorSpecEmi(0, 0, FALSE);
				mGhostPiece->Render();
			}
			/*TrackPieceEx* trackPiece = NULL;
			while ((trackPiece = (TrackPieceEx*)NNS_FndGetNextListObject(&mTrackList, trackPiece)) != NULL)
			{
				box2d_t bounds;
				trackPiece->GetBounds(&bounds);
				if (!bounds.Intersects(&frustumbox))
					continue;
				trackPiece->RenderMarkers();
			}
			if (mGhostPiece != NULL)
			{
				mGhostPiece->RenderMarkers();
			}*/
			/*SimpleSceneryObject* sceneryObject = NULL;
			while ((sceneryObject = (SimpleSceneryObject*)NNS_FndGetNextListObject(&mSceneryList, sceneryObject)) != NULL)
			{
				if (!sceneryObject->Intersects(&frustumbox))
					continue;
				sceneryObject->Render();
			}*/
			//if (lodLevel == 0)
			{
				G3_PushMtx();
				{
					G3_Translate(0, (FX32_ONE / 32), 0);
					mObjectTree->Render(&frustumbox);
				}
				G3_PopMtx(1);
			}
			mWaterTest->Render();
			mWaterTest->Render2();
		}
	}
	G3_PopMtx(1);
	//TODO: move this elsewhere, since this doesn't update with sub 3d rendering
	mTerrainManager->mTrackMarkerRotation += FX32_CONST(2);
	if (mTerrainManager->mTrackMarkerRotation >= 360 * FX32_ONE)
		mTerrainManager->mTrackMarkerRotation -= 360 * FX32_ONE;
}

//extern "C" void render_tile(VecFx10* pNorm, uint8_t* pVtx, int x, int y);
extern "C" void render_tile(hvtx_t* vtx, int x, int y);
extern "C" void render_tile2x2(hvtx_t* vtx, int x, int y);

//#define DEBUG_TILE_COUNT

extern "C" void render_lod0(int xstart, int xend, int zstart, int zend, fx32 distbase, fx32 camdirx, fx32 camdirz, hvtx_t* pmap, fx32 ymul, TerrainTextureManager16* texturemanager);

void Map::RenderLod0(hvtx_t* pHMap, int xstart, int xend, int zstart, int zend, VecFx32* camPos, VecFx32* camDir, u8* lodData)
{
	//setup texture matrix for vertex transform mode
	static const MtxFx43 texMtx =
	{
		FX32_ONE * FX32_ONE / 64 * 16 * 16, 0, 0,
		0, 0, 0,
		0, FX32_ONE * FX32_ONE / 64 * 16 * 16, 0,
		0, 0, 0
	};
	G3_MtxMode(GX_MTXMODE_TEXTURE);
	G3_LoadMtx43(&texMtx);
	G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

	G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_FOG | GX_POLYGON_ATTR_MISC_FAR_CLIPPING);

	if (zend > 128)
		zend = 128;
	if (xend > 128)
		xend = 128;
	hvtx_t* pmap = pHMap + zstart * MAP_BLOCK_WIDTH + xstart;
	u8* plod = &lodData[zstart * 128 + xstart];
	fx32 ymul = camDir->y * Y_SCALE;
	fx32 distbase = 
		camDir->x * (xstart * FX32_ONE + FX32_HALF - camPos->x) +
		camDir->z * (zstart * FX32_ONE + FX32_HALF - camPos->z) -
		camDir->y * camPos->y - 
		ymul * Y_OFFSET;
	//render_lod0(xstart, xend, zstart, zend, distbase, camDir->x, camDir->z, pmap, ymul, mTerrainTextureManager16);
	fx32 xadd2 = camDir->x * FX32_ONE;
	fx32 zadd2 = camDir->z * FX32_ONE;
	for (int y = zstart; y < zend; y++)
	{
		hvtx_t* pmap2 = pmap;
		u8* plod2 = plod;
		fx32 distbase2 = distbase;
		for (int x = xstart; x < xend; x++)
		{
			fx32 top = distbase2 + ymul * pmap2[0].y;
			if (top <= 10 * FX32_ONE * FX32_ONE)
			{
				uint32_t texOffset = mTerrainTextureManager16->GetTextureAddress(
					pmap2[0].tex,
					pmap2[1].tex,
					pmap2[MAP_BLOCK_WIDTH].tex,
					pmap2[MAP_BLOCK_WIDTH + 1].tex,
					pmap2[0].texAddress << 3);
				texOffset >>= 3;
				pmap2[0].texAddress = texOffset;
				reg_G3_TEXIMAGE_PARAM = 0xDC900000 | texOffset;

				if (plod2[0] || plod2[1] || plod2[128] || plod2[128 + 1])
				{
					hvtx_t* pmap3 = pmap2;
					if (x & 1)
						pmap3--;
					if (y & 1)
						pmap3 -= MAP_BLOCK_WIDTH;
					reg_G3_BEGIN_VTXS = GX_PACK_BEGIN_PARAM(GX_BEGIN_TRIANGLE_STRIP);
					reg_G3_TEXCOORD = ~(x << 8) ^ (y << 24);
					reg_G3_NORMAL = pmap2[0].normal;

					uint8_t tl = pmap3[0].y;
					uint8_t tr = pmap3[2].y;
					uint8_t bl = pmap3[2 * MAP_BLOCK_WIDTH].y;
					uint8_t br = pmap3[2 * MAP_BLOCK_WIDTH + 2].y;
					uint16_t newy1, newy2, newy3, newy4;
					if (plod2[0])
					{
						if (!(y & 1))
							newy1 = tl + tr;
						else if (!(x & 1))
							newy1 = tl + bl;
						else
							newy1 = (tl + tr + bl + br) >> 1;
					}
					else
						newy1 = pmap2[0].y << 1;
					reg_G3_VTX_10 = (x << GX_VEC_VTX10_X_SHIFT) | (newy1 << GX_VEC_VTX10_Y_SHIFT) | (y << GX_VEC_VTX10_Z_SHIFT);
					reg_G3_NORMAL = pmap2[MAP_BLOCK_WIDTH].normal;
					if (plod2[128])
					{
						if (y & 1)
							newy2 = bl + br;
						else if (!(x & 1))
							newy2 = tl + bl;
						else
							newy2 = (tl + tr + bl + br) >> 1;
					}
					else
						newy2 = pmap2[MAP_BLOCK_WIDTH].y << 1;
					reg_G3_VTX_10 = (x << GX_VEC_VTX10_X_SHIFT) | (newy2 << GX_VEC_VTX10_Y_SHIFT) | ((y + 1) << GX_VEC_VTX10_Z_SHIFT);
					reg_G3_NORMAL = pmap2[1].normal;
					if (plod2[1])
					{
						if (!(y & 1))
							newy3 = tl + tr;
						else if (x & 1)
							newy3 = tr + br;
						else
							newy3 = (tl + tr + bl + br) >> 1;
					}
					else
						newy3 = pmap2[1].y << 1;
					reg_G3_VTX_10 = ((x + 1) << GX_VEC_VTX10_X_SHIFT) | (newy3 << GX_VEC_VTX10_Y_SHIFT) | (y << GX_VEC_VTX10_Z_SHIFT);
					reg_G3_NORMAL = pmap2[MAP_BLOCK_WIDTH + 1].normal;
					if (plod2[128 + 1])
					{
						if (y & 1)
							newy4 = bl + br;
						else if (x & 1)
							newy4 = tr + br;
						else
							newy4 = (tl + tr + bl + br) >> 1;
					}
					else
						newy4 = pmap2[MAP_BLOCK_WIDTH + 1].y << 1;
					reg_G3_VTX_10 = ((x + 1) << GX_VEC_VTX10_X_SHIFT) | (newy4 << GX_VEC_VTX10_Y_SHIFT) | ((y + 1) << GX_VEC_VTX10_Z_SHIFT);
					reg_G3_END_VTXS = 0;
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
			distbase2 += xadd2;
			pmap2++;
			plod2++;
		}
		distbase += zadd2;
		pmap += MAP_BLOCK_WIDTH;
		plod += 128;
	}
}

void Map::RenderLod1(hvtx_t* pHMap, int xstart, int xend, int zstart, int zend, VecFx32* camPos, VecFx32* camDir, u8* lodData)
{
	static const MtxFx43 texMtx =
	{
		FX32_ONE * FX32_ONE / 64 * 8 * 16, 0, 0,
		0, 0, 0,
		0, FX32_ONE * FX32_ONE / 64 * 4 * 16, 0,
		0, 0, 0
	};
	G3_MtxMode(GX_MTXMODE_TEXTURE);
	G3_LoadMtx43(&texMtx);
	G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
	G3_StoreMtx(31);
	//int count = 0;
	G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_FOG | GX_POLYGON_ATTR_MISC_FAR_CLIPPING);

	GXRgb clr = mGameController->mLightColor;

	zstart &= ~1;
	xstart &= ~1;
	zend |= 1;
	xend |= 1;
	if (zend > 127)
		zend = 127;
	if (xend > 127)
		xend = 127;
	hvtx_t* pmap = pHMap + zstart * MAP_BLOCK_WIDTH + xstart;
	u8* plod = &lodData[zstart * 128 + xstart];
	fx32 ymul = camDir->y * Y_SCALE;
	fx32 distbase = 
		camDir->x * (xstart * FX32_ONE + FX32_ONE - camPos->x) + 
		camDir->z * (zstart * FX32_ONE + FX32_ONE - camPos->z) - 
		camDir->y * camPos->y - 
		ymul * Y_OFFSET;
	fx32 xadd2 = camDir->x * 2 * FX32_ONE;
	fx32 zadd2 = camDir->z * 2 * FX32_ONE;
	//currently disabled, because of the top-left texture sampling
	//#define USE_FAR_DEF_TRISTRIP_RENDER
#define MAP_FAR_DRAWINGFLAG_DEF_STARTED	1
#define MAP_FAR_DRAWINGFLAG_EXT_STARTED	2
	u32 drawingFlags = 0;
#define MAP_FAR_DRAWING_START_DEF_STRIP() \
				do { \
					reg_G3_TEXCOORD = GX_PACK_TEXCOORD_PARAM(0, -y * 4 * FX32_ONE); \
					reg_G3_BEGIN_VTXS = GX_PACK_BEGIN_PARAM(GX_BEGIN_TRIANGLE_STRIP); \
					drawingFlags |= MAP_FAR_DRAWINGFLAG_DEF_STARTED; \
				} while(0)
#define MAP_FAR_DRAWING_END_DEF_STRIP() \
				do { \
					reg_G3_END_VTXS = 0; \
					drawingFlags &= ~MAP_FAR_DRAWINGFLAG_DEF_STARTED; \
				} while(0)
#define MAP_FAR_DRAWING_START_EXT_STRIP() \
				do { \
					reg_G3_TEXIMAGE_PARAM = 0; \
					reg_G3_BEGIN_VTXS = GX_PACK_BEGIN_PARAM(GX_BEGIN_TRIANGLE_STRIP); \
					drawingFlags |= MAP_FAR_DRAWINGFLAG_EXT_STARTED; \
				} while(0)
#define MAP_FAR_DRAWING_END_EXT_STRIP() \
				do { \
					reg_G3_END_VTXS = 0; \
					reg_G3_LIGHT_COLOR = GX_PACK_LIGHTCOLOR_PARAM(0, clr); \
					drawingFlags &= ~MAP_FAR_DRAWINGFLAG_EXT_STARTED; \
				} while(0)
	reg_G3_TEXCOORD = 0;
	for (int y = zstart; y < zend; y += 2)
	{
		hvtx_t* pmap2 = pmap;
		u8* plod2 = plod;
		fx32 distbase2 = distbase;
		for (int x = xstart; x < xend; x += 2)
		{
			fx32 top = distbase2 + ymul * pmap2[0].y;
			if (top >= 8 * FX32_ONE * FX32_ONE && top <= 52 * FX32_ONE * FX32_ONE)
			{
				plod2[1] = 1;
				plod2[128] = 1;
				plod2[128 + 1] = 1;
				plod2[2 * 128 + 1] = 1;
				plod2[128 + 2] = 1;
				if (top <= 25 * FX32_ONE * FX32_ONE)
				{
					if (drawingFlags & MAP_FAR_DRAWINGFLAG_EXT_STARTED)
					MAP_FAR_DRAWING_END_EXT_STRIP();

					uint32_t texOffset = mTerrainTextureManager8->GetTextureAddress(
						pmap2[0].tex,
						pmap2[2].tex,
						pmap2[2 * MAP_BLOCK_WIDTH].tex,
						pmap2[2 * MAP_BLOCK_WIDTH + 2].tex,
						(pmap2[0].texAddress << 3) - 128 * 1024) + 128 * 1024;
					texOffset >>= 3;
					pmap2[0].texAddress = texOffset;

#ifdef USE_FAR_DEF_TRISTRIP_RENDER
					if (!(drawingFlags & MAP_FAR_DRAWINGFLAG_DEF_STARTED))
					{
						reg_G3_TEXIMAGE_PARAM = 0xDC110000 | texOffset;
						MAP_FAR_DRAWING_START_DEF_STRIP();
						reg_G3_NORMAL = pmap2[0].normal;
						reg_G3_VTX_10 = (x << GX_VEC_VTX10_X_SHIFT) | (pmap2[0].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | (y << GX_VEC_VTX10_Z_SHIFT);
						reg_G3_NORMAL = pmap2[2 * MAP_BLOCK_WIDTH].normal;
						reg_G3_VTX_10 = (x << GX_VEC_VTX10_X_SHIFT) | (pmap2[2 * MAP_BLOCK_WIDTH].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | ((y + 2) << GX_VEC_VTX10_Z_SHIFT);
						reg_G3_NORMAL = pmap2[2].normal;
						reg_G3_VTX_10 = ((x + 2) << GX_VEC_VTX10_X_SHIFT) | (pmap2[2].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | (y << GX_VEC_VTX10_Z_SHIFT);
						reg_G3_NORMAL = pmap2[2 * MAP_BLOCK_WIDTH + 2].normal;
						reg_G3_VTX_10 = ((x + 2) << GX_VEC_VTX10_X_SHIFT) | (pmap2[2 * MAP_BLOCK_WIDTH + 2].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | ((y + 2) << GX_VEC_VTX10_Z_SHIFT);
					}
					else
					{
						//BUGFIX: Before the teximageparam command, 36 cycles have to be inserted to
						//		  ensure the geometry engine is done processing the latest triangle.
						//		  This is to prevent the command from changing the latest triangle's
						//		  texture. The MTX_RESTORE command is exactly 36 cycles.
						{
							reg_G3_MTX_RESTORE = GX_PACK_RESTOREMTX_PARAM(31);
							reg_G3_TEXIMAGE_PARAM = 0xDC110000 | texOffset;
						}
						reg_G3_NORMAL = pmap2[2].normal;
						reg_G3_VTX_10 = ((x + 2) << GX_VEC_VTX10_X_SHIFT) | (pmap2[2].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | (y << GX_VEC_VTX10_Z_SHIFT);
						reg_G3_NORMAL = pmap2[2 * MAP_BLOCK_WIDTH + 2].normal;
						reg_G3_VTX_10 = ((x + 2) << GX_VEC_VTX10_X_SHIFT) | (pmap2[2 * MAP_BLOCK_WIDTH + 2].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | ((y + 2) << GX_VEC_VTX10_Z_SHIFT);
					}
#else
					reg_G3_TEXIMAGE_PARAM = 0xDC100000 | texOffset;
					render_tile2x2(pmap2, x, y);
#endif
#ifdef DEBUG_TILE_COUNT
					count++;
#endif
				}
				else// if (top > 25 * FX32_ONE * FX32_ONE && top <= 52 * FX32_ONE * FX32_ONE)
				{
#ifdef USE_FAR_DEF_TRISTRIP_RENDER
					if (drawingFlags & MAP_FAR_DRAWINGFLAG_DEF_STARTED)
						MAP_FAR_DRAWING_END_DEF_STRIP();
#endif

					if (!(drawingFlags & MAP_FAR_DRAWINGFLAG_EXT_STARTED))
					{
						MAP_FAR_DRAWING_START_EXT_STRIP();
						reg_G3_LIGHT_COLOR = GX_PACK_LIGHTCOLOR_PARAM(0, mTerrainTextureManager16->mMeanColorsFixed[pmap2[0].tex]);
						reg_G3_NORMAL = pmap2[0].normal;
						reg_G3_VTX_10 = (x << GX_VEC_VTX10_X_SHIFT) | (pmap2[0].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | (y << GX_VEC_VTX10_Z_SHIFT);
						reg_G3_LIGHT_COLOR = GX_PACK_LIGHTCOLOR_PARAM(0, mTerrainTextureManager16->mMeanColorsFixed[pmap2[2 * MAP_BLOCK_WIDTH].tex]);
						reg_G3_NORMAL = pmap2[2 * MAP_BLOCK_WIDTH].normal;
						reg_G3_VTX_10 = (x << GX_VEC_VTX10_X_SHIFT) | (pmap2[2 * MAP_BLOCK_WIDTH].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | ((y + 2) << GX_VEC_VTX10_Z_SHIFT);
					}

					reg_G3_LIGHT_COLOR = GX_PACK_LIGHTCOLOR_PARAM(0, mTerrainTextureManager16->mMeanColorsFixed[pmap2[2].tex]);
					reg_G3_NORMAL = pmap2[2].normal;
					reg_G3_VTX_10 = ((x + 2) << GX_VEC_VTX10_X_SHIFT) | (pmap2[2].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | (y << GX_VEC_VTX10_Z_SHIFT);
					reg_G3_LIGHT_COLOR = GX_PACK_LIGHTCOLOR_PARAM(0, mTerrainTextureManager16->mMeanColorsFixed[pmap2[2 * MAP_BLOCK_WIDTH + 2].tex]);
					reg_G3_NORMAL = pmap2[2 * MAP_BLOCK_WIDTH + 2].normal;
					reg_G3_VTX_10 = ((x + 2) << GX_VEC_VTX10_X_SHIFT) | (pmap2[2 * MAP_BLOCK_WIDTH + 2].y << (GX_VEC_VTX10_Y_SHIFT + 1)) | ((y + 2) << GX_VEC_VTX10_Z_SHIFT);
#ifdef DEBUG_TILE_COUNT
					count++;
#endif
				}
			}
			distbase2 += xadd2;
			pmap2 += 2;
			plod2 += 2;
		}
#ifdef USE_FAR_DEF_TRISTRIP_RENDER
		if (drawingFlags & MAP_FAR_DRAWINGFLAG_DEF_STARTED)
			MAP_FAR_DRAWING_END_DEF_STRIP();
#endif
		if (drawingFlags & MAP_FAR_DRAWINGFLAG_EXT_STARTED)
		MAP_FAR_DRAWING_END_EXT_STRIP();
		distbase += zadd2;
		pmap += 2 * MAP_BLOCK_WIDTH;
		plod += 2 * 128;
	}
}

void Map::Render(hvtx_t* pHMap, int xstart, int xend, int zstart, int zend, bool picking, VecFx32* camPos, VecFx32* camDir, int lodLevel, u8* lodData)
{
	pHMap += 2 * MAP_BLOCK_WIDTH + 2;
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
			fx32 xadd = FX32_HALF - camPos->x;
			fx32 zadd = FX32_HALF - camPos->z;
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
				RenderLod0(pHMap, xstart, xend, zstart, zend, camPos, camDir, lodData);
			else if (lodLevel == 1)
				RenderLod1(pHMap, xstart, xend, zstart, zend, camPos, camDir, lodData);
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
