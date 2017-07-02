#ifndef __MAP_H__
#define __MAP_H__

#include "io/TerrainData.h"
#include "managers/TerrainTextureManager16.h"
#include "managers/TerrainTextureManager8.h"
#include "engine/QuadTree.h"

class TerrainManager;
class TrackPieceEx;
class SceneryObject;
class ObjectData;

/*typedef struct
{
	uint32_t x : 12;
	uint32_t y : 8;
	uint32_t z : 12;
} mapcoord_t;*/
typedef VecFx32 mapcoord_t;

typedef uint16_t picking_result_t;

#define PICKING_TYPE_MAP	0
#define PICKING_TYPE_TRAIN	1

#define PICKING_COLOR(type,idx) ((picking_result_t)(0x8000 | ((type) & 7) << 12) | ((idx) & 0xFFF))
#define PICKING_TYPE(result) (((result) >> 12) & 7)
#define PICKING_IDX(result) ((result) & 0xFFF)

#define Y_SCALE /*768*/512 //128

#define Y_OFFSET 128 //40 //100

#define MAP_BLOCK_WIDTH		132
#define MAP_BLOCK_HEIGHT	132

class Water;
class SimpleSceneryObject;

class Map
{
private:
	struct hmap_block_data_t
	{
		uint16_t x, y;
		uint32_t last_accessed : 31;
		uint32_t has_normals : 1;
	};
public:
	//hvtx_t* pHMap;
	TerrainData* mTerrainData;
	//4 blocks of 128x128
	hvtx_t mHeightMap[4][MAP_BLOCK_WIDTH * MAP_BLOCK_HEIGHT];
	hmap_block_data_t mHeightMapBlockData[4];
	//uint8_t* mVtx;
	//uint8_t* mTextures;
	//VecFx10* mNormals;
	//uint32_t* mTexAddresses;
	uint8_t* mLodLevels;
	u8* mLastLod;
	int mLastXStart;
	int mLastZStart;

	TerrainTextureManager16* mTerrainTextureManager16;
	TerrainTextureManager8* mTerrainTextureManager8;

	TerrainManager* mTerrainManager;

	Water* mWaterTest;

	ObjectData* mObjectData;
private:
	uint32_t mResourceCounter;
	NNSFndList mTrackList;
	TrackPieceEx* mGhostPiece;
	NNSFndList mSceneryList;
	bool mGridEnabled;

	QuadTree* mObjectTree;

	void* mTexArcData;
	NNSFndArchive mTexArc;

	void RecalculateNormals(int xstart, int xend, int zstart, int zend);
	void RecalculateNormals(hvtx_t* pHMap, int xstart, int xend, int zstart, int zend);
	hvtx_t* GetMapBlock(int x, int y, bool withNormals);
	void Render(hvtx_t* pHMap, int xstart, int xend, int zstart, int zend, bool picking, VecFx32* camPos, VecFx32* camDir, int lodLevel, u8* lodData);
public:
	Map();
	~Map();

	void AddTrackPiece(TrackPieceEx* piece)
	{
		NNS_FndAppendListObject(&mTrackList, piece);
		mObjectTree->Insert((WorldObject*)piece);
	}

	void BeginAddTrackPiece(TrackPieceEx* piece)
	{
		mGhostPiece = piece;
	}

	void FinishAddTrackPiece(TrackPieceEx* piece)
	{
		if (mGhostPiece == piece)
		{
			AddTrackPiece(mGhostPiece);
			mGhostPiece = NULL;
		}
	}

	void AddSceneryObject(SimpleSceneryObject* object)
	{
		NNS_FndAppendListObject(&mSceneryList, object);
	}

	void Render(int xstart, int xend, int zstart, int zend, int xstart2, int xend2, int zstart2, int zend2, bool picking, VecFx32* camPos, VecFx32* camDir, int lodLevel);

	bool GetGridEnabled()
	{
		return mGridEnabled;
	}

	void SetGridEnabled(bool enabled)
	{
		mGridEnabled = enabled;
	}

	bool ScreenPosToWorldPos(int screenX, int screenY, int mapX, int mapY, VecFx32* result);
	fx32 GetYOnMap(fx32 x, fx32 z);

	void TrySnapGhostTrack(int inPoint, TrackPieceEx* ignore = NULL);
	void TrySnapTrack(TrackPieceEx* track, int inPoint, TrackPieceEx* ignore = NULL);

	//tempoarly
	TrackPieceEx* GetFirstTrackPiece()
	{
		return (TrackPieceEx*)NNS_FndGetNextListObject(&mTrackList, NULL);
	}

	void UpdateResourceCounter()
	{
		if (mResourceCounter == 0x7FFFFFFF)
			mResourceCounter = 0;
		else
			mResourceCounter++;
		mTerrainTextureManager8->UpdateResourceCounter();
		mTerrainTextureManager16->UpdateResourceCounter();
	}
};

#endif
