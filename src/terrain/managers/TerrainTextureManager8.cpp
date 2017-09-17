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
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/00.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/01.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/02.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/03.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/04.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/05.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/06.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/07.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/08.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/09.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/10.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/11.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/12.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/13.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/14.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/15.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/16.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/17.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/18.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/19.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/20.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/21.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/22.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/23.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/24.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/25.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/26.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/27.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/28.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/29.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/30.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/31.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/32.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/33.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/34.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/35.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/36.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/37.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/38.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/39.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/40.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/41.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/42.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/43.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/44.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/45.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/46.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/47.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/48.ntft");
	for (; i < 100; i++)
		mTextureDatas[i] = (uint16_t*)NNS_FndGetArchiveFileByName("mtx:/16x8/48.ntft");

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