#pragma once

#include "TerrainTextureManager.h"

#define TEXTURE8_QUEUE_LENGTH	1024
#define TEXTURE8_WORKER_THREAD_STACK_SIZE	1024
#define TEXTURE8_WORKER_THREAD_PRIORITY   (OS_THREAD_LAUNCHER_PRIORITY + 12)

class TerrainTextureManager8 : public TerrainTextureManager
{
private:
	texture_cache_block_t mCacheBlocks[/*1024*//*1025*/513] ATTRIBUTE_ALIGN(32);

	uint8_t mVramCTexData[128 * 1024] ATTRIBUTE_ALIGN(32);
	uint32_t mVramCacheChanged[512 / 32];

	uint16_t* mTextureDatas[26] ATTRIBUTE_ALIGN(32);
	coefd_texture_sample_t mCoefdTextureDatas[26][4][16 * 8];

	void WorkerThreadMain();

	OSMessage mMessageQueueData[TEXTURE8_QUEUE_LENGTH];
	OSMessageQueue mMessageQueue;

	OSThread mWorkerThread;
	u32 mWorkerThreadStack[TEXTURE8_WORKER_THREAD_STACK_SIZE / sizeof(u32)];
public:
	TerrainTextureManager8();
	~TerrainTextureManager8()
	{
		OS_DestroyThread(&mWorkerThread);
	}
	uint32_t GetTextureAddress(int tl, int tr, int bl, int br, uint32_t oldTexKey);
	void UpdateVramC();

	static void WorkerThreadMain(void* arg)
	{
		((TerrainTextureManager8*)arg)->WorkerThreadMain();
	}
};