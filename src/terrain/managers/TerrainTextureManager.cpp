#include <nitro.h>
#include <nnsys/fnd.h>
#include "core.h"
#include "util.h"
#include "TerrainTextureManager.h"

#define TEXTURE_CACHE_BLOCK_TAG_EMPTY	0xFFFFFFFF

TerrainTextureManager::TerrainTextureManager()
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
			mTextureDatas[j][k] &= 0x7FFF;
		}
	}

	for (int j = 0; j < 256; j++)
	{
		mCacheBlocks[j].tag = TEXTURE_CACHE_BLOCK_TAG_EMPTY;
		mCacheBlocks[j].last_accessed = 0;
	}
}

extern "C" void gen_terrain_texture(u16* tl, u16* tr, u16* bl, u16* br, u16* dst);

#include <nitro/itcm_begin.h>
/*uint32_t TerrainTextureManager::GetTextureAddress(int tl, int tr, int bl, int br)
{
	mResourceCounter++;
	uint32_t tag = tl | (tr << 8) | (bl << 16) | (br << 24);
	//is it in the cache?
	int oldest = -1;
	int oldestcnt = 0x7FFFFFFF;
	texture_cache_block_t* pBlock = &mCacheBlocks[0];
	int j = 0;
	while(true)
	{
		if (pBlock->tag == tag)
			goto found;
		if (pBlock->last_accessed < oldestcnt)
		{
			oldest = j;
			oldestcnt = pBlock->last_accessed;
		}
		j++;
		pBlock++;
		if (j == 256)
			break;
	}
	//not found
	gen_terrain_texture(
		mTextureDatas[tl],
		mTextureDatas[tr],
		mTextureDatas[bl],
		mTextureDatas[br],
		(uint16_t*)&mVramCTexData[oldest * 16 * 16 * 2]);
	mCacheBlocks[oldest].last_accessed = mResourceCounter;
	mCacheBlocks[oldest].tag = tag;
	return 256 * 1024 + oldest * 16 * 16 * 2;
found:
	pBlock->last_accessed = mResourceCounter;
	return 256 * 1024 + j * 16 * 16 * 2;
}*/

#pragma access_errors off
enum
{ 
	offsetof_mResourceCounter = offsetof(TerrainTextureManager, mResourceCounter),
	offsetof_mCacheBlocks = offsetof(TerrainTextureManager, mCacheBlocks),
	offsetof_mTextureDatas = offsetof(TerrainTextureManager, mTextureDatas),
	offsetof_mVramCTexData = offsetof(TerrainTextureManager, mVramCTexData)
};
#pragma access_errors reset

asm uint32_t TerrainTextureManager::GetTextureAddress(int tl, int tr, int bl, int br)
{
#define arg_br (4 * 9)
	stmfd sp!, { r4 - r11,lr }
	ldr r4, [sp, #arg_br]
	ldr r7, [r0, #offsetof_mResourceCounter]
	add r7, r7, #1
	str r7, [r0, #offsetof_mResourceCounter]
	//create tag
	orr r6, r1, r2, lsl #8
	orr r6, r6, r3, lsl #16
	orr r6, r6, r4, lsl #24

	add r9, r0, #offsetof_mCacheBlocks
	mov r12, #256
@loop:
	ldr r10, [r9], #8
	teq r10, r6
	subnes r12, r12, #1
	bgt @loop
	cmp r12, #0
	bne @tag_found

	mvn r5, #1
	mvn r8, #0x80000000
	add r9, r0, #(offsetof_mCacheBlocks + 4)
	mov r12, #256
@loop2:
	ldr r10, [r9], #8
	cmp r10, r8
		movlt r8, r10
		movlt r5, r12
	subs r12, r12, #1
	bgt @loop2
	rsb r5, r5, #256
	add r12, r0, #offsetof_mCacheBlocks
	add r12, r12, r5, lsl #3
	strd r6, [r12]

	add r12, r0, r5, lsl #9

	add lr, r0, #offsetof_mTextureDatas
	ldr r0, [lr, r1, lsl #2]
	ldr r1, [lr, r2, lsl #2]
	ldr r2, [lr, r3, lsl #2]
	ldr r3, [lr, r4, lsl #2]

	add r4, r12, #offsetof_mVramCTexData

	bl gen_terrain_texture
	mov r0, r5, lsl #9
	add r0, r0, #(256 * 1024)
	ldmfd sp!, { r4 - r11,pc }
@tag_found:
	rsb r12, r12, #256
	str r7, [r9, #-4]
	mov r12, r12, lsl #9
	add r0, r12, #(256 * 1024)
	ldmfd sp!, { r4 - r11,pc }
}
#include <nitro/itcm_end.h>

static void OnVRAMCopyComplete(void* arg)
{
	GX_SetBankForTex(GX_VRAM_TEX_012_ABC);
}

void TerrainTextureManager::UpdateVramC()
{
	//maybe we should flush the cache here?!
	GX_SetBankForLCDC(GX_VRAM_LCDC_C | GX_VRAM_LCDC_D);
	MI_DmaCopy32Async(0, &mVramCTexData, (void*)HW_LCDC_VRAM_C, 128 * 1024, OnVRAMCopyComplete, NULL);
}