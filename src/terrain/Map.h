#ifndef __MAP_H__
#define __MAP_H__

class TerrainManager;
class TrackPiece;
class SceneryObject;

typedef struct
{
	uint32_t x : 12;
	uint32_t y : 8;
	uint32_t z : 12;
} mapcoord_t;

typedef uint16_t picking_result_t;

#define PICKING_TYPE_MAP	0
#define PICKING_TYPE_TRAIN	1

#define PICKING_COLOR(type,idx) ((picking_result_t)(0x8000 | ((type) & 7) << 12) | ((idx) & 0xFFF))
#define PICKING_TYPE(result) (((result) >> 12) & 7)
#define PICKING_IDX(result) ((result) & 0xFFF)

class Map
{
public:
	tile_t mTiles[64][64];
private:
	TerrainManager* mTerrainManager;
	NNSFndList mTrackList;
	NNSFndList mSceneryList;
	BOOL mGridEnabled;
public:
	Map();
	~Map();

	void AddTrackPiece(TrackPiece* piece)
	{
		NNS_FndAppendListObject(&mTrackList, piece);
	}

	void AddSceneryObject(SceneryObject* object)
	{
		NNS_FndAppendListObject(&mSceneryList, object);
	}

	void Render(int xstart, int xend, int zstart, int zend, BOOL picking, int selectedMapX, int selectedMapZ);

	BOOL GetGridEnabled()
	{
		return mGridEnabled;
	}

	void SetGridEnabled(BOOL enabled)
	{
		mGridEnabled = enabled;
	}
};

#endif
