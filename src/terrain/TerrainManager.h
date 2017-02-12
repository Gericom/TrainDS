#ifndef __TERRAIN_MANAGER_H__
#define __TERRAIN_MANAGER_H__
#include "terrain.h"

class TerrainManager
{
private:
	texture_t* mTerrainTextures[1];
	texture_t mTrackTexture;
	texture_t mRCT2TreeTexture;
	texture_t mTrackMarkerTexture;
public:
	fx32 mTrackMarkerRotation;
	TerrainManager();
	texture_t* GetTerrainTexture(int terrainId);
	texture_t* GetTrackTexture() { return &mTrackTexture; }
	texture_t* GetRCT2TreeTexture() { return &mRCT2TreeTexture; }
	texture_t* GetTrackMarkerTexture() { return &mTrackMarkerTexture; }
};

#endif