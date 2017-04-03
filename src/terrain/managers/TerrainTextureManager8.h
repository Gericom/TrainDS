#ifndef __TERRAIN_TEXTURE_MANAGER_8_H__
#define __TERRAIN_TEXTURE_MANAGER_8_H__

#include "TerrainTextureManager.h"

class TerrainTextureManager8 : public TerrainTextureManager
{
private:
	texture_cache_block2_t mCacheBlocks[/*1024*//*1025*/513] ATTRIBUTE_ALIGN(32);

	uint8_t mVramCTexData[128 * 1024] ATTRIBUTE_ALIGN(32);

	uint16_t* mTextureDatas[26] ATTRIBUTE_ALIGN(32);

	void* mTexArcData;
	NNSFndArchive mTexArc;

	void MoveToTail(uint16_t block);
public:
	TerrainTextureManager8();
	~TerrainTextureManager8()
	{
		NNS_FndUnmountArchive(&mTexArc);
		NNS_FndFreeToExpHeap(gHeapHandle, mTexArcData);
	}
	uint32_t GetTextureAddress(int tl, int tr, int bl, int br, uint32_t oldTexKey);
	void UpdateVramC();
};

#endif