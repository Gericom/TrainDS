#ifndef __MAP_H__
#define __MAP_H__

class TerrainManager;
class TrackPiece;
class FlexTrack;
class SceneryObject;

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

class Map
{
public:
	uint8_t* mVtx;
	VecFx10* mNormals;
	//tile_t mTiles[64][64];
private:
	TerrainManager* mTerrainManager;
	NNSFndList mTrackList;
	FlexTrack* mGhostPiece;
	NNSFndList mSceneryList;
	bool mGridEnabled;

	void RecalculateNormals(int xstart, int xend, int zstart, int zend);
public:
	Map();
	~Map();

	void AddTrackPiece(FlexTrack* piece)
	{
		NNS_FndAppendListObject(&mTrackList, piece);
	}

	void BeginAddTrackPiece(FlexTrack* piece)
	{
		mGhostPiece = piece;
	}

	void FinishAddTrackPiece(FlexTrack* piece)
	{
		if (mGhostPiece == piece)
		{
			AddTrackPiece(mGhostPiece);
			mGhostPiece = NULL;
		}
	}

	void AddSceneryObject(SceneryObject* object)
	{
		NNS_FndAppendListObject(&mSceneryList, object);
	}

	void Render(int xstart, int xend, int zstart, int zend, bool picking, int selectedMapX, int selectedMapZ, VecFx32* camPos);

	bool GetGridEnabled()
	{
		return mGridEnabled;
	}

	void SetGridEnabled(bool enabled)
	{
		mGridEnabled = enabled;
	}

	bool ScreenPosToWorldPos(int screenX, int screenY, int mapX, int mapY, VecFx32* result);

	void TrySnapGhostTrack();
};

#endif
