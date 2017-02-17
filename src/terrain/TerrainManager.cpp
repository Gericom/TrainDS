#include <nitro.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "util.h"
#include "terrain.h"
#include "TerrainManager.h"

TerrainManager::TerrainManager()
{
	int i = 0;
	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/grass_a.ntft", "/data/map/grass_a.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/grass_b.ntft", "/data/map/grass_b.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/grass_stripes.ntft", "/data/map/grass_stripes.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/dry_grass_tufts.ntft", "/data/map/dry_grass_tufts.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/dry_longgrass_a.ntft", "/data/map/dry_longgrass_a.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/dry_longgrass_b.ntft", "/data/map/dry_longgrass_b.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/dry_longgrass_tufts.ntft", "/data/map/dry_longgrass_tufts.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/gravel_a.ntft", "/data/map/gravel_a.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/gravel_b.ntft", "/data/map/gravel_b.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/concrete_a.ntft", "/data/map/concrete_a.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/concrete_overgrown.ntft", "/data/map/concrete_overgrown.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;
	
	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/mud_overgrown.ntft", "/data/map/mud_overgrown.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/grass_a1.ntft", "/data/map/grass_a1.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/gravel_grass_a.ntft", "/data/map/gravel_grass_a.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/dry_goldgrass_a.ntft", "/data/map/dry_goldgrass_a.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/dry_longgrass_stripes.ntft", "/data/map/dry_longgrass_stripes.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;
	
	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/dry_greygrass_a.ntft", "/data/map/dry_greygrass_a.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/grass_wlonggrass.ntft", "/data/map/grass_wlonggrass.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/gravel_c.ntft", "/data/map/gravel_c.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/mud_darkovergrown.ntft", "/data/map/mud_darkovergrown.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/grass_a3.ntft", "/data/map/grass_a3.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/graystone_crack.ntft", "/data/map/graystone_crack.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/graystone.ntft", "/data/map/graystone.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/grass_lavender.ntft", "/data/map/grass_lavender.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/grass_a2.ntft", "/data/map/grass_a2.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	mTerrainTextures[i] = (texture_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(texture_t), 32);
	Util_LoadTextureFromCard("/data/map/grass_a_b.ntft", "/data/map/grass_a_b.ntfp", mTerrainTextures[i]->texKey, mTerrainTextures[i]->plttKey);
	mTerrainTextures[i]->nitroWidth = GX_TEXSIZE_S64;
	mTerrainTextures[i]->nitroHeight = GX_TEXSIZE_T64;
	mTerrainTextures[i]->nitroFormat = GX_TEXFMT_PLTT16;
	i++;

	Util_LoadTextureFromCard("/data/map/track.ntft", "/data/map/track.ntfp", mTrackTexture.texKey, mTrackTexture.plttKey);
	mTrackTexture.nitroWidth = GX_TEXSIZE_S64;
	mTrackTexture.nitroHeight = GX_TEXSIZE_T64;
	mTrackTexture.nitroFormat = GX_TEXFMT_A3I5;

	Util_LoadTexture4x4FromCard("/data/map/scenery/RCT2A.ntft", "/data/map/scenery/RCT2A.ntfi", "/data/map/scenery/RCT2A.ntfp", mRCT2TreeTexture.texKey, mRCT2TreeTexture.plttKey);
	mRCT2TreeTexture.nitroWidth = GX_TEXSIZE_S64;
	mRCT2TreeTexture.nitroHeight = GX_TEXSIZE_T256;
	mRCT2TreeTexture.nitroFormat = GX_TEXFMT_COMP4x4;

	Util_LoadTextureFromCard("/data/map/track_marker.ntft", "/data/map/track_marker.ntfp", mTrackMarkerTexture.texKey, mTrackMarkerTexture.plttKey);
	mTrackMarkerTexture.nitroWidth = GX_TEXSIZE_S32;
	mTrackMarkerTexture.nitroHeight = GX_TEXSIZE_T32;
	mTrackMarkerTexture.nitroFormat = GX_TEXFMT_A5I3;

	mTrackMarkerRotation = 0;
}

texture_t* TerrainManager::GetTerrainTexture(int terrainId)
{
	if (terrainId < 0 || terrainId >= (sizeof(mTerrainTextures) / sizeof(mTerrainTextures[0])))
		terrainId = 0;
	return mTerrainTextures[terrainId];
}