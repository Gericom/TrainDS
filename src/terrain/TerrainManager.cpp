#include <nitro.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "util.h"
#include "terrain.h"
#include "TerrainManager.h"

TerrainManager::TerrainManager()
{
	uint32_t size;
	void* buffer = Util_LoadFileToBuffer("/data/map/grass.ntft", &size);
	DC_FlushRange(buffer, size);

	NNSGfdTexKey texKey = NNS_GfdAllocTexVram(size, FALSE, 0);
	GX_BeginLoadTex();
	{
		GX_LoadTex(buffer, NNS_GfdGetTexKeyAddr(texKey), size);
	}
	GX_EndLoadTex();
	NNS_FndFreeToExpHeap(mHeapHandle, buffer);

	buffer = Util_LoadFileToBuffer("/data/map/grass.ntfp", &size);
	DC_FlushRange(buffer, size);

	NNSGfdPlttKey plttKey = NNS_GfdAllocPlttVram(size, FALSE, 0);
	GX_BeginLoadTexPltt();
	{
		GX_LoadTexPltt(buffer, NNS_GfdGetPlttKeyAddr(plttKey), size);
	}
	GX_EndLoadTexPltt();
	NNS_FndFreeToExpHeap(mHeapHandle, buffer);

	mTerrainTextures[0] = (texture_t*)NNS_FndAllocFromExpHeapEx(mHeapHandle, sizeof(texture_t), 16);
	mTerrainTextures[0]->texKey = texKey;
	mTerrainTextures[0]->plttKey = plttKey;
	mTerrainTextures[0]->nitroWidth = GX_TEXSIZE_S32;
	mTerrainTextures[0]->nitroHeight = GX_TEXSIZE_T32;
	mTerrainTextures[0]->nitroFormat = GX_TEXFMT_PLTT256;

	buffer = Util_LoadFileToBuffer("/data/map/grass_track_horizontal.ntft", &size);
	DC_FlushRange(buffer, size);

	texKey = NNS_GfdAllocTexVram(size, FALSE, 0);
	GX_BeginLoadTex();
	{
		GX_LoadTex(buffer, NNS_GfdGetTexKeyAddr(texKey), size);
	}
	GX_EndLoadTex();
	NNS_FndFreeToExpHeap(mHeapHandle, buffer);

	buffer = Util_LoadFileToBuffer("/data/map/grass_track_horizontal.ntfp", &size);
	DC_FlushRange(buffer, size);

	plttKey = NNS_GfdAllocPlttVram(size, FALSE, 0);
	GX_BeginLoadTexPltt();
	{
		GX_LoadTexPltt(buffer, NNS_GfdGetPlttKeyAddr(plttKey), size);
	}
	GX_EndLoadTexPltt();
	NNS_FndFreeToExpHeap(mHeapHandle, buffer);

	mTerrainTextures[1] = (texture_t*)NNS_FndAllocFromExpHeapEx(mHeapHandle, sizeof(texture_t), 16);
	mTerrainTextures[1]->texKey = texKey;
	mTerrainTextures[1]->plttKey = plttKey;
	mTerrainTextures[1]->nitroWidth = GX_TEXSIZE_S32;
	mTerrainTextures[1]->nitroHeight = GX_TEXSIZE_T32;
	mTerrainTextures[1]->nitroFormat = GX_TEXFMT_PLTT256;
}

texture_t* TerrainManager::GetTerrainTexture(int terrainId)
{
	return mTerrainTextures[terrainId];
}