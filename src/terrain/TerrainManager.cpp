#include <nitro.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "util.h"
#include "terrain.h"
#include "TerrainManager.h"

TerrainManager::TerrainManager()
{
	mTerrainTextures[0] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/grass.ntft", "/data/map/grass.ntfp", mTerrainTextures[0]->texKey, mTerrainTextures[0]->plttKey);
	mTerrainTextures[0]->nitroWidth = GX_TEXSIZE_S32;
	mTerrainTextures[0]->nitroHeight = GX_TEXSIZE_T32;
	mTerrainTextures[0]->nitroFormat = GX_TEXFMT_PLTT256;

	Util_LoadTextureFromCard("/data/map/track.ntft", "/data/map/track.ntfp", mTrackTexture.texKey, mTrackTexture.plttKey);
	mTrackTexture.nitroWidth = GX_TEXSIZE_S64;
	mTrackTexture.nitroHeight = GX_TEXSIZE_T64;
	mTrackTexture.nitroFormat = GX_TEXFMT_A3I5;
}

texture_t* TerrainManager::GetTerrainTexture(int terrainId)
{
	return mTerrainTextures[terrainId];
}