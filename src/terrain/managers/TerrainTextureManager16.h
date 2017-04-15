#ifndef __TERRAIN_TEXTURE_MANAGER_16_H__
#define __TERRAIN_TEXTURE_MANAGER_16_H__

#include "TerrainTextureManager.h"

#define TEXTURE16_QUEUE_LENGTH	1024
#define TEXTURE16_WORKER_THREAD_STACK_SIZE	1024
#define TEXTURE16_WORKER_THREAD_PRIORITY   (OS_THREAD_LAUNCHER_PRIORITY + 8)

class TerrainTextureManager16 : public TerrainTextureManager
{
private:
	texture_cache_block_t mCacheBlocks[257] ATTRIBUTE_ALIGN(32);

public:
	uint16_t* mTextureDatas[26] ATTRIBUTE_ALIGN(32);

private:
	void* mTexArcData;
	NNSFndArchive mTexArc;

	uint8_t mVramCTexData[128 * 1024] ATTRIBUTE_ALIGN(32);

	void WorkerThreadMain();

	OSMessage mMessageQueueData[TEXTURE16_QUEUE_LENGTH];
	OSMessageQueue mMessageQueue;

	OSThread mWorkerThread;
	u32 mWorkerThreadStack[TEXTURE16_WORKER_THREAD_STACK_SIZE / sizeof(u32)];
public:
	TerrainTextureManager16();
	~TerrainTextureManager16()
	{
		OS_DestroyThread(&mWorkerThread);
		NNS_FndUnmountArchive(&mTexArc);
		NNS_FndFreeToExpHeap(gHeapHandle, mTexArcData);
	}
	uint32_t GetTextureAddress(int tl, int tr, int bl, int br, uint32_t oldTexKey);
	void UpdateVramC();

	static void WorkerThreadMain(void* arg)
	{
		((TerrainTextureManager16*)arg)->WorkerThreadMain();
	}
};

#endif