#pragma once

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

#pragma options align=packed
	struct coefd_texture_sample_t
	{
		u16 r, g, b;
	};
#pragma options align=reset
public:
	TerrainTextureManager()
		: mResourceCounter(0)
	{ }
	virtual ~TerrainTextureManager() { }
	virtual uint32_t GetTextureAddress(int tl, int tr, int bl, int br, uint32_t oldTexKey) = 0;
	virtual void UpdateVramC() = 0;
	void UpdateResourceCounter()
	{
		if (mResourceCounter == 0x7FFFFFFF)
			mResourceCounter = 0;
		else
			mResourceCounter++;
	}
};