#ifndef __TERRAIN_TEXTURE_MANAGER_H__
#define __TERRAIN_TEXTURE_MANAGER_H__

#define TEXTURE_CACHE_BLOCK_TAG_EMPTY	0xFFFFFFFF

class TerrainTextureManager
{
protected:
	struct texture_cache_block_t
	{
		uint32_t tag;
		uint32_t last_accessed;
	};
	uint32_t mResourceCounter;
public:
	virtual uint32_t GetTextureAddress(int tl, int tr, int bl, int br, uint32_t oldTexKey) = 0;
	virtual void UpdateVramC() = 0;
};

#endif