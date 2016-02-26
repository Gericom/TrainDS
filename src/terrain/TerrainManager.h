#ifndef __TERRAIN_MANAGER_H__
#define __TERRAIN_MANAGER_H__
#include "terrain.h"

class TerrainManager
{
private:
	texture_t* mTerrainTextures[1];
	texture_t mTrackTexture;
public:
	TerrainManager();
	texture_t* GetTerrainTexture(int terrainId);
	texture_t* GetTrackTexture() { return &mTrackTexture; }
};

#endif