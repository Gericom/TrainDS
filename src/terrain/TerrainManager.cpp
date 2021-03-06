#include "common.h"


#include "util.h"
#include "TerrainManager.h"

TerrainManager::TerrainManager()
{
	Util_LoadLZ77TextureFromCard("/data/map/track.ntft.lz", "/data/map/track.ntfp", mTrackTexture.texKey, mTrackTexture.plttKey);
	mTrackTexture.nitroWidth = GX_TEXSIZE_S64;
	mTrackTexture.nitroHeight = GX_TEXSIZE_T64;
	mTrackTexture.nitroFormat = GX_TEXFMT_A3I5;

	Util_LoadLZ77Texture4x4FromCard("/data/map/scenery/RCT2A.ntft.lz", "/data/map/scenery/RCT2A.ntfi.lz", "/data/map/scenery/RCT2A.ntfp.lz", mRCT2TreeTexture.texKey, mRCT2TreeTexture.plttKey);
	mRCT2TreeTexture.nitroWidth = GX_TEXSIZE_S64;
	mRCT2TreeTexture.nitroHeight = GX_TEXSIZE_T256;
	mRCT2TreeTexture.nitroFormat = GX_TEXFMT_COMP4x4;

	Util_LoadLZ77TextureFromCard("/data/map/track_marker.ntft.lz", "/data/map/track_marker.ntfp", mTrackMarkerTexture.texKey, mTrackMarkerTexture.plttKey);
	mTrackMarkerTexture.nitroWidth = GX_TEXSIZE_S32;
	mTrackMarkerTexture.nitroHeight = GX_TEXSIZE_T32;
	mTrackMarkerTexture.nitroFormat = GX_TEXFMT_A5I3;

	Util_LoadLZ77TextureFromCard("/data/map/sun.ntft.lz", "/data/map/sun.ntfp", mSunTexture.texKey, mSunTexture.plttKey);
	mSunTexture.nitroWidth = GX_TEXSIZE_S32;
	mSunTexture.nitroHeight = GX_TEXSIZE_T32;
	mSunTexture.nitroFormat = GX_TEXFMT_A5I3;

	Util_LoadLZ77TextureFromCard("/data/map/mk_lensfx02_ds.ntft.lz", "/data/map/sun.ntfp", mFlareTexture.texKey, mFlareTexture.plttKey);
	mFlareTexture.nitroWidth = GX_TEXSIZE_S32;
	mFlareTexture.nitroHeight = GX_TEXSIZE_T32;
	mFlareTexture.nitroFormat = GX_TEXFMT_A5I3;

	mTrackMarkerRotation = 0;
}