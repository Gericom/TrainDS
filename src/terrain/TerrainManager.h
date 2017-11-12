#ifndef __TERRAIN_MANAGER_H__
#define __TERRAIN_MANAGER_H__


typedef struct
{
	NNSGfdTexKey texKey;
	NNSGfdPlttKey plttKey;
	int nitroWidth;
	int nitroHeight;
	int nitroFormat;
} texture_t;

//obselete and should be replaced by something else, most likely the TextureManager
class TerrainManager
{
private:
	texture_t mTrackTexture;
	texture_t mRCT2TreeTexture;
	texture_t mTrackMarkerTexture;
	texture_t mSunTexture;
	texture_t mFlareTexture;
public:
	fx32 mTrackMarkerRotation;
	TerrainManager();
	texture_t* GetTrackTexture() { return &mTrackTexture; }
	texture_t* GetRCT2TreeTexture() { return &mRCT2TreeTexture; }
	texture_t* GetTrackMarkerTexture() { return &mTrackMarkerTexture; }
	texture_t* GetSunTexture() { return &mSunTexture; }
	texture_t* GetFlareTexture() { return &mFlareTexture; }
};

#endif