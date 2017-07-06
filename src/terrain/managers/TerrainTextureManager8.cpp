#include <nitro.h>
#include <nnsys/fnd.h>
#include "core.h"
#include "util.h"
#include "TerrainTextureManager8.h"

extern u8 gen_terrain_texture_8_coeftable[];

TerrainTextureManager8::TerrainTextureManager8()
	: TerrainTextureManager()
{
	int i = 0;
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/grass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/grass_b.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/grass_stripes.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/dry_grass_tufts.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/dry_longgrass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/dry_longgrass_b.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/dry_longgrass_tufts.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/gravel_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/gravel_b.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/concrete_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/concrete_overgrown.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/mud_overgrown.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/grass_a1.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/gravel_grass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/dry_goldgrass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/dry_longgrass_stripes.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/dry_greygrass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/grass_wlonggrass.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/gravel_c.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/mud_darkovergrown.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/grass_a3.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/graystone_crack.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/graystone.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/grass_lavender.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/grass_a2.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/grass_a_b.ntft");

	static int tmpFix[] = { 3, 2, 0, 1 };

	for (int j = 0; j < i; j++)
	{
		for (int k = 0; k < 16 * 8; k++)
		{
			mTextureDatas[j][k] &= 0x7FFF;
		}
		uint16_t newtex[128];
		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 8; x++)
			{
				//create new color
				int r = 0, g = 0, b = 0;
				for (int y2 = 0; y2 < 2; y2++)
				{
					for (int x2 = 0; x2 < 2; x2++)
					{
						int color = mTextureDatas[j][(y * 2 + y2) * 16 + x * 2 + x2];
						r += color & 0x1F;
						g += (color >> 5) & 0x1F;
						b += (color >> 10) & 0x1F;
					}
				}
				int dither = 2;
				r = (r + dither) >> 2;
				g = (g + dither) >> 2;
				b = (b + dither) >> 2;

				int newcolor = GX_RGB(r, g, b);

				newtex[y * 16 + x] = newcolor;
				newtex[y * 16 + x + 8] = newcolor;
				newtex[(y + 4) * 16 + x] = newcolor;
				newtex[(y + 4) * 16 + x + 8] = newcolor;
			}
		}
		MI_CpuCopyFast(newtex, mTextureDatas[j], sizeof(newtex));
		for (int k = 0; k < 4; k++)
		{
			for (int l = 0; l < 16 * 8; l++)
			{
				int r = mTextureDatas[j][l] & 0x1F;
				int g = (mTextureDatas[j][l] >> 5) & 0x1F;
				int b = (mTextureDatas[j][l] >> 10) & 0x1F;
				int coef = gen_terrain_texture_8_coeftable[l * 4 + k];
				mCoefdTextureDatas[j][k][l].r = r * coef;
				mCoefdTextureDatas[j][k][l].g = g * coef;
				mCoefdTextureDatas[j][k][l].b = b * coef;
			}
		}
	}

	for (int j = 0; j < 512; j++)
	{
		mCacheBlocks[j].tag = TEXTURE_CACHE_BLOCK_TAG_EMPTY;
		mCacheBlocks[j].last_accessed = 0;
	}
	MI_CpuFillFast(&mVramCTexData[0], 0xFFFFFFFF, sizeof(mVramCTexData));
	OS_InitMessageQueue(&mMessageQueue, &mMessageQueueData[0], TEXTURE8_QUEUE_LENGTH);
	OS_CreateThread(&mWorkerThread, WorkerThreadMain, this, mWorkerThreadStack + TEXTURE8_WORKER_THREAD_STACK_SIZE / sizeof(u32), TEXTURE8_WORKER_THREAD_STACK_SIZE, TEXTURE8_WORKER_THREAD_PRIORITY);
	OS_WakeupThreadDirect(&mWorkerThread);
}

static void OnVRAMCopyComplete(void* arg)
{
	GX_SetBankForTex(GX_VRAM_TEX_01_AC);
	//GX_SetBankForTex((GXVRamTex)(GX_GetBankForTex() | GX_VRAM_C));// GX_VRAM_TEX_01_AC);
}

void TerrainTextureManager8::UpdateVramC()
{
	GX_SetBankForLCDC(GX_GetBankForLCDC() | GX_VRAM_LCDC_C);
	MI_DmaCopy32Async(0, &mVramCTexData, (void*)HW_LCDC_VRAM_C, 128 * 1024, OnVRAMCopyComplete, NULL);
}