#include <nitro.h>
#include <nnsys/fnd.h>
#include <nnsys/g3d.h>
#include "core.h"
#include "util.h"
#include "TerrainManager.h"
#include "terrain/track/TrackPieceEx.h"
#include "terrain/track/FlexTrack.h"
#include "terrain/scenery/SimpleSceneryObject.h"
#include "terrain/scenery/RCT2Tree1.h"
#include "managers/TerrainTextureManager16.h"
#include "managers/TerrainTextureManager8.h"
#include "io/TerrainData.h"
#include "io/ObjectData.h"
#include "engine/objects/Water.h"
#include "box2d.h"
#include "engine/QuadTree.h"
#include "Map.h"

Map::Map()
	: mGridEnabled(FALSE), mGhostPiece(NULL), mResourceCounter(0)
{
	NNS_FND_INIT_LIST(&mTrackList, TrackPieceEx, mLink);
	NNS_FND_INIT_LIST(&mSceneryList, SimpleSceneryObject, mLink);
	mTerrainManager = new TerrainManager();

	mTerrainData = new TerrainData("/data/map/britainmap.tdat");
	for (int i = 0; i < 4; i++)
	{
		mHeightMapBlockData[i].last_accessed = 0;
		mHeightMapBlockData[i].x = 0xFFFF;
		mHeightMapBlockData[i].y = 0xFFFF;
	}

	VecFx32 wpos = { 373680, -6 * FX32_ONE + (FX32_ONE >> 2), 1153947 };
	mWaterTest = new Water(&wpos, 50 * FX32_ONE, 50 * FX32_ONE, 4 * FX32_ONE);

	mObjectData = new ObjectData("/data/map/objects.objd");
	mObjectTree = new QuadTree(-32 * FX32_ONE, -32 * FX32_ONE, mTerrainData->GetWidth() * FX32_ONE, mTerrainData->GetHeight() * FX32_ONE, 6);
	for (int i = 0; i < mObjectData->mFileData->header.nr_sceneries; i++)
	{
		ObjectData::object_data_scenery_entry_t* s = &mObjectData->mSceneryEntries[i];
		bool billboard =
			STD_StrNCmp("tree_g", mObjectData->mFileData->object_ref_entries[s->object_type].string_offset, 6) &&
			!STD_StrNCmp("tree", mObjectData->mFileData->object_ref_entries[s->object_type].string_offset, 4);
		SimpleSceneryObject* ob = new SimpleSceneryObject(this, s->object_type, s->x - 32 * FX32_ONE, s->z - 32 * FX32_ONE, s->roty, billboard);
		ob->Invalidate();
		mObjectTree->Insert(ob);
		//AddSceneryObject(new SimpleSceneryObject(this, s->object_type, s->x - 32 * FX32_ONE, s->z - 32 * FX32_ONE));
	}

	//pHMap = new hvtx_t[128 * 128];
	/*hvtx_t* pHMap = mHeightMap[0];

	uint8_t* vtx = (uint8_t*)Util_LoadLHFileToBuffer("/data/map/terrain.hmap.diff.lh", NULL, true);
	MI_UnfilterDiff8(vtx, vtx);
	for (int y = 0; y < 128; y++)
	{
		for (int x = 0; x < 128; x++)
		{
			pHMap[y * 128 + x].y = vtx[y * 128 + x];
		}
	}
	NNS_FndFreeToExpHeap(gHeapHandle, vtx);

	uint8_t* tex = (uint8_t*)Util_LoadLHFileToBuffer("/data/map/terrain.tmap.lh", NULL, true);
	for (int y = 0; y < 128; y++)
	{
		for (int x = 0; x < 128; x++)
		{
			pHMap[y * 128 + x].tex = tex[y * 128 + x];
			pHMap[y * 128 + x].texAddress = 0;
		}
	}
	NNS_FndFreeToExpHeap(gHeapHandle, tex);*/

	//mount the tex data archive
	mTexArcData = Util_LoadLHFileToBuffer("/data/map/britain.narc.lh", NULL, FALSE);
	NNS_FndMountArchive(&mTexArc, "mtx", mTexArcData);

	//mVtx = (uint8_t*)Util_LoadFileToBuffer("/data/map/terrain.hmap", NULL, false);
	//mTextures = (uint8_t*)Util_LoadFileToBuffer("/data/map/terrain.tmap", NULL, false);

	mTerrainTextureManager16 = new TerrainTextureManager16();
	mTerrainTextureManager8 = new TerrainTextureManager8();
	//mTexAddresses = new uint32_t[128 * 128];
	//MI_CpuClearFast(mTexAddresses, 128 * 128 * 4);

	mLodLevels = new uint8_t[128 * 128];
	MI_CpuFillFast(mLodLevels, 0, 128 * 128);

	//mVtx = new uint8_t[128 * 128];
	//MI_CpuFillFast(mVtx, 0x80808080, 128 * 128);
	//mNormals = new VecFx10[128 * 128];
	//RecalculateNormals(0, 127, 0, 127);

	//load tracks
	u32 len;
	fx32* trackdata = (fx32*)Util_LoadFileToBuffer("/data/map/trackdata.bin", &len, true);
	fx32* curPtr = trackdata;
	for (int i = 0; i < len / 16; i++)
	{
		VecFx32 a = { *curPtr++ - 32 * FX32_ONE, 0, *curPtr++ - 32 * FX32_ONE };
		VecFx32 b = { *curPtr++ - 32 * FX32_ONE, 0, *curPtr++ - 32 * FX32_ONE };
		FlexTrack* track = new FlexTrack(this, &a, &b);
		AddTrackPiece(track);
	}
	TrackPieceEx* trackPiece = NULL;
	while ((trackPiece = (TrackPieceEx*)NNS_FndGetNextListObject(&mTrackList, trackPiece)) != NULL)
	{
		TrySnapTrack(trackPiece, 0);
		TrySnapTrack(trackPiece, 1);
	}

	/*SimpleSceneryObject* sceneryObject = NULL;
	while ((sceneryObject = (SimpleSceneryObject*)NNS_FndGetNextListObject(&mSceneryList, sceneryObject)) != NULL)
	{
		sceneryObject->Invalidate();
	}*/

	NNS_FndFreeToExpHeap(gHeapHandle, trackdata);
}

Map::~Map()
{
	NNS_FndUnmountArchive(&mTexArc);
	NNS_FndFreeToExpHeap(gHeapHandle, mTexArcData);

	delete mTerrainData;
	delete mTerrainManager;
	//NNS_FndFreeToExpHeap(gHeapHandle, mVtx);
	//NNS_FndFreeToExpHeap(gHeapHandle, mTextures);
	delete mTerrainTextureManager16;
	delete mTerrainTextureManager8;
	//delete mTexAddresses;
	//delete mNormals;
	//delete pHMap;
	delete mLodLevels;
}

hvtx_t* Map::GetMapBlock(int x, int y, bool withNormals)
{
	if (x < 0 || y < 0 || x >= mTerrainData->GetHBlockCount() || y >= mTerrainData->GetVBlockCount())
		return NULL;
	for (int i = 0; i < 4; i++)
	{
		if (mHeightMapBlockData[i].x == x && mHeightMapBlockData[i].y == y)
		{
			mHeightMapBlockData[i].last_accessed = mResourceCounter;
			if (withNormals && !mHeightMapBlockData[i].has_normals)
			{
				RecalculateNormals(&mHeightMap[i][0], 0, 129, 0, 129);
				mHeightMapBlockData[i].has_normals = 1;
			}
			return &mHeightMap[i][0];
		}
	}
	int oldest = -1;
	u32 oldestcounter = 0x7FFFFFFF;
	for (int i = 0; i < 4; i++)
	{
		if (mHeightMapBlockData[i].last_accessed < oldestcounter)
		{
			oldest = i;
			oldestcounter = mHeightMapBlockData[i].last_accessed;
		}
	}
	mTerrainData->GetBlock(x, y, &mHeightMap[oldest][0]);
	mHeightMapBlockData[oldest].x = x;
	mHeightMapBlockData[oldest].y = y;
	mHeightMapBlockData[oldest].last_accessed = mResourceCounter;

	if (withNormals)
	{
		RecalculateNormals(&mHeightMap[oldest][0], 0, 129, 0, 129);
		mHeightMapBlockData[oldest].has_normals = 1;
	}
	else
		mHeightMapBlockData[oldest].has_normals = 0;

	return &mHeightMap[oldest][0];
}

void Map::RecalculateNormals(int xstart, int xend, int zstart, int zend)
{
	for (int y = zstart >> 7; y < (zend + 127) >> 7; y++)
	{
		for (int x = xstart >> 7; x < (xend + 127) >> 7; x++)
		{
			int xstart2 = xstart - x * 128;
			if (xstart2 < 0)
				xstart2 = 0;
			if (xstart2 > 129)
				xstart2 = 129;
			int xend2 = xend - x * 128;
			if (xend2 < 0)
				xend2 = 0;
			if (xend2 > 129)
				xend2 = 129;
			int zstart2 = zstart - y * 128;
			if (zstart2 < 0)
				zstart2 = 0;
			if (zstart2 > 129)
				zstart2 = 129;
			int zend2 = zend - y * 128;
			if (zend2 < 0)
				zend2 = 0;
			if (zend2 > 129)
				zend2 = 129;

			hvtx_t* pMap = GetMapBlock(x, y, false);
			if (pMap)
				RecalculateNormals(pMap, xstart2, xend2, zstart2, zend2);
		}
	}
}

static inline fx32 sign(VecFx32* p1, VecFx32* p2, VecFx32* p3)
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
	//fix the camera matrix to get reliable results
	MtxFx44 mtx = *NNS_G3dGlbGetProjectionMtx();
	NNS_G3dGlbPerspectiveW(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 4096 >> 3, 35 * 4096, 40960 * 4);

	VecFx32 near;
	VecFx32 far;
	NNS_G3dScrPosToWorldLine(screenX, screenY, &near, &far);

	//and restore the old one again
	NNS_G3dGlbSetProjectionMtx(&mtx);

	VecFx32 nf;
	VEC_Subtract(&far, &near, &nf);

	hvtx_t* pHMap = mHeightMap[0];
	pHMap += 2 * MAP_BLOCK_WIDTH + 2;

	//get the plane of the map square
	VecFx32 a =
	{
		mapX * FX32_ONE - 32 * FX32_ONE,
		(pHMap[mapY * MAP_BLOCK_WIDTH + mapX].y - Y_OFFSET) * Y_SCALE,
		mapY * FX32_ONE - 32 * FX32_ONE
	};
	VecFx32 b =
	{
		mapX * FX32_ONE - 32 * FX32_ONE,
		(pHMap[(mapY + 1) * MAP_BLOCK_WIDTH + mapX].y - Y_OFFSET) * Y_SCALE,
		(mapY + 1) * FX32_ONE - 32 * FX32_ONE
	};
	VecFx32 c =
	{
		(mapX + 1) * FX32_ONE - 32 * FX32_ONE,
		(pHMap[mapY * MAP_BLOCK_WIDTH + mapX + 1].y - Y_OFFSET) * Y_SCALE,
		mapY * FX32_ONE - 32 * FX32_ONE
	};
	VecFx32 d =
	{
		(mapX + 1) * FX32_ONE - 32 * FX32_ONE,
		(pHMap[(mapY + 1) * MAP_BLOCK_WIDTH + mapX + 1].y - Y_OFFSET) * Y_SCALE,
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
	int mapX = (x + 32 * FX32_ONE) >> FX32_SHIFT;
	int mapY = (z + 32 * FX32_ONE) >> FX32_SHIFT;

	int xPart = mapX / 128;
	int yPart = mapY / 128;

	hvtx_t* pHMap = GetMapBlock(xPart, yPart, false); //mHeightMap[0];
	pHMap += 2 * MAP_BLOCK_WIDTH + 2;
	mapX -= xPart * 128;
	mapY -= yPart * 128;

	x -= xPart * 128 * FX32_ONE;
	z -= yPart * 128 * FX32_ONE;

	VecFx32 near = { x, 256 * FX32_ONE, z };
	VecFx32 far = { x, -256 * FX32_ONE, z };



	//if (mapX < 0 || mapY < 0 || mapX >= 126 || mapY >= 126)
	//	return 0;



	//get the plane of the map square
	VecFx32 a =
	{
		mapX * FX32_ONE - 32 * FX32_ONE,
		(pHMap[mapY * MAP_BLOCK_WIDTH + mapX].y - Y_OFFSET) * Y_SCALE,
		mapY * FX32_ONE - 32 * FX32_ONE
	};
	VecFx32 b =
	{
		mapX * FX32_ONE - 32 * FX32_ONE,
		(pHMap[(mapY + 1) * MAP_BLOCK_WIDTH + mapX].y - Y_OFFSET) * Y_SCALE,
		(mapY + 1) * FX32_ONE - 32 * FX32_ONE
	};
	VecFx32 c =
	{
		(mapX + 1) * FX32_ONE - 32 * FX32_ONE,
		(pHMap[mapY * MAP_BLOCK_WIDTH + mapX + 1].y - Y_OFFSET) * Y_SCALE,
		mapY * FX32_ONE - 32 * FX32_ONE
	};
	VecFx32 d =
	{
		(mapX + 1) * FX32_ONE - 32 * FX32_ONE,
		(pHMap[(mapY + 1) * MAP_BLOCK_WIDTH + mapX + 1].y - Y_OFFSET) * Y_SCALE,
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
	TrySnapTrack(mGhostPiece, inPoint, ignore);
}

void Map::TrySnapTrack(TrackPieceEx* track, int inPoint, TrackPieceEx* ignore)
{
	if (track == NULL)
		return;
	VecFx32 ghostEnd;
	track->GetConnectionPoint(inPoint, &ghostEnd);
	TrackPieceEx* trackPiece = NULL;
	while ((trackPiece = (TrackPieceEx*)NNS_FndGetNextListObject(&mTrackList, trackPiece)) != NULL)
	{
		if (trackPiece == ignore || trackPiece == track)
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
				track->Connect(inPoint, trackPiece, i, true);
				return;
			}
		}
	}
	track->Disconnect(inPoint);
}