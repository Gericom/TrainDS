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

Map::Map()
	: mGridEnabled(FALSE), mGhostPiece(NULL)
{
	NNS_FND_INIT_LIST(&mTrackList, TrackPieceEx, mLink);
	NNS_FND_INIT_LIST(&mSceneryList, SceneryObject, mLink);
	mTerrainManager = new TerrainManager();

	mHMap = new hvtx_t[128 * 128];

	uint8_t* vtx = (uint8_t*)Util_LoadFileToBuffer("/data/map/terrain.hmap", NULL, true);
	for (int y = 0; y < 128; y++)
	{
		for (int x = 0; x < 128; x++)
		{
			mHMap[y * 128 + x].y = vtx[y * 128 + x];
		}
	}
	NNS_FndFreeToExpHeap(gHeapHandle, vtx);

	uint8_t* tex = (uint8_t*)Util_LoadFileToBuffer("/data/map/terrain.tmap", NULL, true);
	for (int y = 0; y < 128; y++)
	{
		for (int x = 0; x < 128; x++)
		{
			mHMap[y * 128 + x].tex = tex[y * 128 + x];
			mHMap[y * 128 + x].texAddress = 0;
		}
	}
	NNS_FndFreeToExpHeap(gHeapHandle, tex);

	//mount the tex data archive
	mTexArcData = Util_LoadLZ77FileToBuffer("/data/map/britain.carc", NULL, FALSE);
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
	RecalculateNormals(0, 127, 0, 127);

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
	NNS_FndFreeToExpHeap(gHeapHandle, trackdata);
}

Map::~Map()
{
	NNS_FndUnmountArchive(&mTexArc);
	NNS_FndFreeToExpHeap(gHeapHandle, mTexArcData);

	delete mTerrainManager;
	//NNS_FndFreeToExpHeap(gHeapHandle, mVtx);
	//NNS_FndFreeToExpHeap(gHeapHandle, mTextures);
	delete mTerrainTextureManager16;
	delete mTerrainTextureManager8;
	//delete mTexAddresses;
	//delete mNormals;
	delete mHMap;
	delete mLodLevels;
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

	//get the plane of the map square
	VecFx32 a =
	{
		mapX * FX32_ONE - 32 * FX32_ONE,
		(mHMap[mapY * 128 + mapX].y - Y_OFFSET) * Y_SCALE,
		mapY * FX32_ONE - 32 * FX32_ONE
	};
	VecFx32 b =
	{
		mapX * FX32_ONE - 32 * FX32_ONE,
		(mHMap[(mapY + 1) * 128 + mapX].y - Y_OFFSET) * Y_SCALE,
		(mapY + 1) * FX32_ONE - 32 * FX32_ONE
	};
	VecFx32 c =
	{
		(mapX + 1) * FX32_ONE - 32 * FX32_ONE,
		(mHMap[mapY * 128 + mapX + 1].y - Y_OFFSET) * Y_SCALE,
		mapY * FX32_ONE - 32 * FX32_ONE
	};
	VecFx32 d =
	{
		(mapX + 1) * FX32_ONE - 32 * FX32_ONE,
		(mHMap[(mapY + 1) * 128 + mapX + 1].y - Y_OFFSET) * Y_SCALE,
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

	//if (mapX < 0 || mapY < 0 || mapX >= 126 || mapY >= 126)
	//	return 0;

	//get the plane of the map square
	VecFx32 a =
	{
		mapX * FX32_ONE - 32 * FX32_ONE,
		(mHMap[mapY * 128 + mapX].y - Y_OFFSET) * Y_SCALE,
		mapY * FX32_ONE - 32 * FX32_ONE
	};
	VecFx32 b =
	{
		mapX * FX32_ONE - 32 * FX32_ONE,
		(mHMap[(mapY + 1) * 128 + mapX].y - Y_OFFSET) * Y_SCALE,
		(mapY + 1) * FX32_ONE - 32 * FX32_ONE
	};
	VecFx32 c =
	{
		(mapX + 1) * FX32_ONE - 32 * FX32_ONE,
		(mHMap[mapY * 128 + mapX + 1].y - Y_OFFSET) * Y_SCALE,
		mapY * FX32_ONE - 32 * FX32_ONE
	};
	VecFx32 d =
	{
		(mapX + 1) * FX32_ONE - 32 * FX32_ONE,
		(mHMap[(mapY + 1) * 128 + mapX + 1].y - Y_OFFSET) * Y_SCALE,
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