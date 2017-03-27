#include <nitro.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "util.h"
#include "TerrainManager.h"

TerrainManager::TerrainManager()
{
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