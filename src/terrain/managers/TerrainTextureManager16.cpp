#include <nitro.h>
#include <nnsys/fnd.h>
#include "core.h"
#include "util.h"
#include "TerrainTextureManager16.h"

extern u8 gen_terrain_texture_coeftable[];

TerrainTextureManager16::TerrainTextureManager16()
	: TerrainTextureManager()
{
	int i = 0;
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/00.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/01.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/02.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/03.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/04.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/05.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/06.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/07.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/08.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/09.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/10.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/11.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/12.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/13.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/14.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/15.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/16.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/17.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/18.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/19.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/20.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/21.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/22.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/23.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/24.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/25.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/26.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/27.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/28.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/29.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/30.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/31.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/32.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/33.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/34.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/35.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/36.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/37.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/38.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/39.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/40.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/41.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/42.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/43.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/44.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/45.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/46.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/47.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/48.ntft");
	for (; i < 100; i++)
		mTextureDatas[i] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/48.ntft");

	for (int j = 0; j < i; j++)
	{
		int rr = 0, gg = 0, bb = 0;
		for (int k = 0; k < 16 * 16; k++)
		{
			rr += mTextureDatas[j][k] & 0x1F;
			gg += (mTextureDatas[j][k] >> 5) & 0x1F;
			bb += (mTextureDatas[j][k] >> 10) & 0x1F;
			mTextureDatas[j][k] &= 0x7FFF;
		}
		mMeanColors[j] = GX_RGB(rr >> 8, gg >> 8, bb >> 8);
		for (int k = 0; k < 4; k++)
		{
			for (int l = 0; l < 16 * 16; l++)
			{
				int r = mTextureDatas[j][l] & 0x1F;
				int g = (mTextureDatas[j][l] >> 5) & 0x1F;
				int b = (mTextureDatas[j][l] >> 10) & 0x1F;
				int coef = gen_terrain_texture_coeftable[l * 4 + k];
				mCoefdTextureDatas[j][k][l].r = r * coef;
				mCoefdTextureDatas[j][k][l].g = g * coef;
				mCoefdTextureDatas[j][k][l].b = b * coef;
			}
		}
	}

	for (int j = 0; j < 256; j++)
	{
		mCacheBlocks[j].tag = TEXTURE_CACHE_BLOCK_TAG_EMPTY;
		mCacheBlocks[j].last_accessed = 0;
	}
	MI_CpuFillFast(&mVramCTexData[0], 0xFFFFFFFF, sizeof(mVramCTexData));
	OS_InitMessageQueue(&mMessageQueue, &mMessageQueueData[0], TEXTURE16_QUEUE_LENGTH);
	OS_CreateThread(&mWorkerThread, WorkerThreadMain, this, mWorkerThreadStack + TEXTURE16_WORKER_THREAD_STACK_SIZE / sizeof(u32), TEXTURE16_WORKER_THREAD_STACK_SIZE, TEXTURE16_WORKER_THREAD_PRIORITY);
	OS_WakeupThreadDirect(&mWorkerThread);
}

static void OnVRAMCopyComplete(void* arg)
{
	GX_SetBankForTex(GX_VRAM_TEX_012_ACD);
	//GX_SetBankForTex((GXVRamTex)(GX_GetBankForTex() | GX_VRAM_C));//
}

void TerrainTextureManager16::UpdateVramC()
{
	GX_SetBankForLCDC(GX_GetBankForLCDC() | GX_VRAM_LCDC_C);
	MI_DmaCopy32Async(0, &mVramCTexData, (void*)HW_LCDC_VRAM_C, 128 * 1024, OnVRAMCopyComplete, NULL);
}