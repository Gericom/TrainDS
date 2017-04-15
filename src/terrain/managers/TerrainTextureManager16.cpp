#include <nitro.h>
#include <nnsys/fnd.h>
#include "core.h"
#include "util.h"
#include "TerrainTextureManager16.h"

TerrainTextureManager16::TerrainTextureManager16()
	: TerrainTextureManager()
{
	mTexArcData = Util_LoadLZ77FileToBuffer("/data/map/britain.carc", NULL, FALSE);
	NNS_FndMountArchive(&mTexArc, "mtx", mTexArcData);

	int i = 0;
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/grass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/grass_b.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/grass_stripes.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/dry_grass_tufts.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/dry_longgrass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/dry_longgrass_b.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/dry_longgrass_tufts.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/gravel_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/gravel_b.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/concrete_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/concrete_overgrown.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/mud_overgrown.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/grass_a1.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/gravel_grass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/dry_goldgrass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/dry_longgrass_stripes.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/dry_greygrass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/grass_wlonggrass.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/gravel_c.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/mud_darkovergrown.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/grass_a3.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/graystone_crack.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/graystone.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/grass_lavender.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/grass_a2.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/grass_a_b.ntft");

	for (int j = 0; j < i; j++)
	{
		for (int k = 0; k < 16 * 16; k++)
		{
			int r = mTextureDatas[j][k] & 0x1F;
			int g = (mTextureDatas[j][k] >> 5) & 0x1F;
			int b = (mTextureDatas[j][k] >> 10) & 0x1F;
			r = r * 31 / 20;//25;
			g = g * 31 / 20;// 25;
			b = b * 31 / 20;// 25;
			if (r > 31)
				r = 31;
			if (g > 31)
				g = 31;
			if (b > 31)
				b = 31;
			mTextureDatas[j][k] = GX_RGB(r, g, b);
			//mTextureDatas[j][k] &= 0x7FFF;
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
	GX_SetBankForTex(GX_VRAM_TEX_01_AC);
}

void TerrainTextureManager16::UpdateVramC()
{
	//maybe we should flush the cache here?!
	GX_SetBankForLCDC(GX_GetBankForLCDC() | GX_VRAM_LCDC_C);
	MI_DmaCopy32Async(0, &mVramCTexData, (void*)HW_LCDC_VRAM_C, 128 * 1024, OnVRAMCopyComplete, NULL);
}