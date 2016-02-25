#include <nitro.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "util.h"
#include "terrain.h"
#include "TerrainManager.h"

TerrainManager::TerrainManager()
{
	uint32_t size;
	void* buffer = Util_LoadFileToBuffer("/data/map/grass.ntft", &size, TRUE);
	DC_FlushRange(buffer, size);

	NNSGfdTexKey texKey = NNS_GfdAllocTexVram(size, FALSE, 0);
	Util_LoadTextureWithKey(texKey, buffer);
	NNS_FndFreeToExpHeap(mHeapHandle, buffer);

	buffer = Util_LoadFileToBuffer("/data/map/grass.ntfp", &size, TRUE);
	DC_FlushRange(buffer, size);

	NNSGfdPlttKey plttKey = NNS_GfdAllocPlttVram(size, FALSE, 0);
	Util_LoadPaletteWithKey(plttKey, buffer);
	NNS_FndFreeToExpHeap(mHeapHandle, buffer);

	mTerrainTextures[0] = (texture_t*)NNS_FndAllocFromExpHeapEx(mHeapHandle, sizeof(texture_t), 16);
	mTerrainTextures[0]->texKey = texKey;
	mTerrainTextures[0]->plttKey = plttKey;
	mTerrainTextures[0]->nitroWidth = GX_TEXSIZE_S32;
	mTerrainTextures[0]->nitroHeight = GX_TEXSIZE_T32;
	mTerrainTextures[0]->nitroFormat = GX_TEXFMT_PLTT256;

	buffer = Util_LoadFileToBuffer("/data/map/track.ntft", &size, TRUE);
	DC_FlushRange(buffer, size);

	texKey = NNS_GfdAllocTexVram(size, FALSE, 0);
	Util_LoadTextureWithKey(texKey, buffer);
	NNS_FndFreeToExpHeap(mHeapHandle, buffer);

	buffer = Util_LoadFileToBuffer("/data/map/track.ntfp", &size, TRUE);
	DC_FlushRange(buffer, size);

	plttKey = NNS_GfdAllocPlttVram(size, FALSE, 0);
	Util_LoadPaletteWithKey(plttKey, buffer);
	NNS_FndFreeToExpHeap(mHeapHandle, buffer);

	mTrackTexture.texKey = texKey;
	mTrackTexture.plttKey = plttKey;
	mTrackTexture.nitroWidth = GX_TEXSIZE_S64;
	mTrackTexture.nitroHeight = GX_TEXSIZE_T64;
	mTrackTexture.nitroFormat = GX_TEXFMT_A3I5;
}

texture_t* TerrainManager::GetTerrainTexture(int terrainId)
{
	return mTerrainTextures[terrainId];
}