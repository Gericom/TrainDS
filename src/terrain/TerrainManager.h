#ifndef __TERRAIN_MANAGER_H__
#define __TERRAIN_MANAGER_H__
#include "terrain.h"

class TerrainManager
{
private:
	texture_t* mTerrainTextures[1];
	texture_t mTrackTexture;
	texture_t mRCT2TreeTexture;
public:
	TerrainManager();
	texture_t* GetTerrainTexture(int terrainId);
	texture_t* GetTrackTexture() { return &mTrackTexture; }
	texture_t* GetRCT2TreeTexture() { return &mRCT2TreeTexture; }
};

#endif