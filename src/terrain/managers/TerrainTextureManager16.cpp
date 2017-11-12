#include "common.h"


#include "util.h"
#include "TerrainTextureManager16.h"

extern u8 gen_terrain_texture_coeftable[];

TerrainTextureManager16::TerrainTextureManager16()
	: TerrainTextureManager()
{
	int i = 0;
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/grass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/grass_b.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/grass_stripes.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/dry_grass_tufts.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/dry_longgrass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/dry_longgrass_b.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/dry_longgrass_tufts.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/gravel_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/gravel_b.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/concrete_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/concrete_overgrown.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/mud_overgrown.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/grass_a1.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/gravel_grass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/dry_goldgrass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/dry_longgrass_stripes.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/dry_greygrass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/grass_wlonggrass.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/gravel_c.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/mud_darkovergrown.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/grass_a3.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/graystone_crack.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/graystone.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/grass_lavender.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/grass_a2.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x16/grass_a_b.ntft");

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