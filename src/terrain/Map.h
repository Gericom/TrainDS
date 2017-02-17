#ifndef __MAP_H__
#define __MAP_H__

class TerrainManager;
class TrackPieceEx;
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
	uint8_t* mTextures;
	VecFx10* mNormals;
private:
	TerrainManager* mTerrainManager;
	NNSFndList mTrackList;
	TrackPieceEx* mGhostPiece;
	NNSFndList mSceneryList;
	bool mGridEnabled;

	void RecalculateNormals(int xstart, int xend, int zstart, int zend);
public:
	Map();
	~Map();

	void AddTrackPiece(TrackPieceEx* piece)
	{
		NNS_FndAppendListObject(&mTrackList, piece);
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
	fx32 GetYOnMap(fx32 x, fx32 z);

	void TrySnapGhostTrack(int inPoint, TrackPieceEx* ignore = NULL);

	//tempoarly
	TrackPieceEx* GetFirstTrackPiece()
	{
		return (TrackPieceEx*)NNS_FndGetNextListObject(&mTrackList, NULL);
	}
};

#endif
