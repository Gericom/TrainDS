#include <nitro.h>
#include <nnsys/fnd.h>
#include "core.h"
#include "util.h"
#include "TerrainTextureManager8.h"

static int ditherTable[] = 
{
	0 , 12  , 3,  15,
	8 ,  4 , 11,   7,
	2 , 14 ,  1 , 13,
	10 ,  6  , 9 ,  5
};

TerrainTextureManager8::TerrainTextureManager8()
{
	mTexArcData = Util_LoadLZ77FileToBuffer("/data/map/britain.carc", NULL, FALSE);
	NNS_FndMountArchive(&mTexArc, "mt2", mTexArcData);

	int i = 0;
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/grass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/grass_b.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/grass_stripes.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/dry_grass_tufts.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/dry_longgrass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/dry_longgrass_b.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/dry_longgrass_tufts.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/gravel_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/gravel_b.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/concrete_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/concrete_overgrown.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/mud_overgrown.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/grass_a1.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/gravel_grass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/dry_goldgrass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/dry_longgrass_stripes.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/dry_greygrass_a.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/grass_wlonggrass.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/gravel_c.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/mud_darkovergrown.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/grass_a3.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/graystone_crack.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/graystone.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/grass_lavender.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/grass_a2.ntft");
	mTextureDatas[i++] = (uint16_t*)NNS_FndGetArchiveFileByName("mt2:/grass_a_b.ntft");

	for (int j = 0; j < i; j++)
	{
		for (int k = 0; k < 16 * 16; k++)
		{
			mTextureDatas[j][k] &= 0x7FFF;
		}
		uint16_t newtex[128];//64];
		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 8; x++)
			{
				//create new color
				int r = 0, g = 0, b = 0;
				for (int y2 = 0; y2 < 4; y2++)
				{
					for (int x2 = 0; x2 < 2; x2++)
					{
						int color = mTextureDatas[j][(y * 4 + y2) * 16 + x * 2 + x2];
						r += color & 0x1F;
						g += (color >> 5) & 0x1F;
						b += (color >> 10) & 0x1F;
					}
				}
				int dither = 4;//ditherTable[y * 4 + x];
				r = (r + dither) >> 3;
				if (r > 31)
					r = 31;
				g = (g + dither) >> 3;
				if (g > 31)
					g = 31;
				b = (b + dither) >> 3;
				if (b > 31)
					b = 31;
				r = r * 31 / 25;
				g = g * 31 / 25;
				b = b * 31 / 25;
				int newcolor = GX_RGB(r, g, b);

				newtex[y * 16 + x] = newcolor;
				newtex[y * 16 + x + 8] = newcolor;
				newtex[(y + 4) * 16 + x] = newcolor;
				newtex[(y + 4) * 16 + x + 8] = newcolor;
			}
		}
		MI_CpuCopyFast(newtex, mTextureDatas[j], sizeof(newtex));
	}
	mReplaceListHead = 0;
	mReplaceListTail = 511;
	for (int j = 0; j < 512; j++)
	{
		mCacheBlocks[j].tag = TEXTURE_CACHE_BLOCK_TAG_EMPTY;
		if (j != 0)
			mCacheBlocks[j].prev = j - 1;
		else
			mCacheBlocks[j].prev = 0xFFFF;
		if (j != 511)
			mCacheBlocks[j].next = j + 1;
		else
			mCacheBlocks[j].next = 0xFFFF;
	}
}

static void OnVRAMCopyComplete(void* arg)
{
	GX_SetBankForTex(GX_VRAM_TEX_01_AC);
}

void TerrainTextureManager8::UpdateVramC()
{
	//maybe we should flush the cache here?!
	GX_SetBankForLCDC(GX_GetBankForLCDC() | GX_VRAM_LCDC_C);
	MI_DmaCopy32Async(0, &mVramCTexData, (void*)HW_LCDC_VRAM_C, 128 * 1024, OnVRAMCopyComplete, NULL);
}

void TerrainTextureManager8::MoveToTail(uint16_t block)
{
	if (block == 0xFFFF || mReplaceListTail == block)
		return;
	if (mCacheBlocks[block].prev != 0xFFFF)
		mCacheBlocks[mCacheBlocks[block].prev].next = mCacheBlocks[block].next;
	if (mCacheBlocks[block].next != 0xFFFF)
	{
		mCacheBlocks[mCacheBlocks[block].next].prev = mCacheBlocks[block].prev;
		if(mCacheBlocks[block].prev == 0xFFFF)//head
			mReplaceListHead = mCacheBlocks[block].next;
	}
	mCacheBlocks[mReplaceListTail].next = block;
	mCacheBlocks[block].prev = mReplaceListTail;
	mCacheBlocks[block].next = 0xFFFF;
	mReplaceListTail = block;
}