#ifndef __TERRAIN_MANAGER_H__
#define __TERRAIN_MANAGER_H__
#include "terrain.h"

class TerrainManager
{
private:
	texture_t* mTerrainTextures[2];
public:
	TerrainManager();
	texture_t* GetTerrainTexture(int terrainId);
};

#endif