#ifndef __TERRAIN_TEXTURE_MANAGER_16_H__
#define __TERRAIN_TEXTURE_MANAGER_16_H__

#include "TerrainTextureManager.h"

class TerrainTextureManager16 : public TerrainTextureManager
{
private:
	texture_cache_block_t mCacheBlocks[257] ATTRIBUTE_ALIGN(32);

	uint16_t* mTextureDatas[26] ATTRIBUTE_ALIGN(32);

	void* mTexArcData;
	NNSFndArchive mTexArc;

	uint8_t mVramCTexData[128 * 1024] ATTRIBUTE_ALIGN(32);
public:
	TerrainTextureManager16();
	uint32_t GetTextureAddress(int tl, int tr, int bl, int br, uint32_t oldTexKey);
	void UpdateVramC();
};

#endif