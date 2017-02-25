#ifndef __TERRAIN_TEXTURE_MANAGER_H__
#define __TERRAIN_TEXTURE_MANAGER_H__

class TerrainTextureManager
{
private:
	struct texture_cache_block_t
	{
		uint32_t tag;
		uint32_t last_accessed;
	};
	uint32_t mResourceCounter;
	texture_cache_block_t mCacheBlocks[256] ATTRIBUTE_ALIGN(32);

	uint16_t* mTextureDatas[26] ATTRIBUTE_ALIGN(32);

	void* mTexArcData;
	NNSFndArchive mTexArc;

	uint8_t mVramCTexData[128 * 1024] ATTRIBUTE_ALIGN(32);
public:
	TerrainTextureManager();
	uint32_t GetTextureAddress(int tl, int tr, int bl, int br);
	void UpdateVramC();
};

#endif