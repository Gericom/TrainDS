#include <nitro.h>
#include <nnsys/fnd.h>
#include "core.h"
#include "terrain.h"
#include "TerrainManager.h"
#include "terrain/track/TrackPiece.h"
#include "Map.h"

Map::Map()
{
	NNS_FND_INIT_LIST(&mTrackList, TrackPiece, mLink);
	mTerrainManager = new TerrainManager();
}

Map::~Map()
{
	delete mTerrainManager;
}


