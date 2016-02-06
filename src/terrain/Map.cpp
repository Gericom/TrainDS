#include <nitro.h>
#include "core.h"
#include "terrain.h"
#include "TerrainManager.h"
#include "Map.h"

Map::Map()
{
	mTerrainManager = new TerrainManager();
}

Map::~Map()
{
	delete mTerrainManager;
}


