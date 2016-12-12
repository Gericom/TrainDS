#ifndef __MAP_H__
#define __MAP_H__

class TerrainManager;

class Map
{
private:
	TerrainManager* mTerrainManager;
	tile_t mTiles[128][128];
	NNSFndList mTrackList;
public:
	Map();
	~Map();

	void AddTrackPiece(TrackPiece* piece)
	{
		NNS_FndAppendListObject(&mTrackList, piece);
	}
};

#endif
