#include <nitro.h>
#include <nnsys/fnd.h>
#include "core.h"
#include "util.h"
#include "TerrainTextureManager8.h"

extern "C" void gen_terrain_texture_8(u16* tl, u16* tr, u16* bl, u16* br, u16* dst);

uint32_t TerrainTextureManager8::GetTextureAddress(int tl, int tr, int bl, int br, uint32_t oldTexKey)
{
	uint32_t tag = tl | (tr << 8) | (bl << 16) | (br << 24);
	if (oldTexKey != 0)
	{
		int blocknr = (oldTexKey - (128 * 1024)) >> 8;
		if (mCacheBlocks[blocknr].tag == tag)
		{
			MoveToTail(blocknr);
			return oldTexKey;
		}
	}
	mCacheBlocks[512].tag = tag;
	texture_cache_block2_t* pBlock = &mCacheBlocks[0];
	while (pBlock->tag != tag)
		pBlock++;
	int blocknr;
	if (pBlock == &mCacheBlocks[512])
	{
		//not found
		blocknr = mReplaceListHead;
		gen_terrain_texture_8(mTextureDatas[tl], mTextureDatas[tr], mTextureDatas[bl], mTextureDatas[br], (u16*)&mVramCTexData[blocknr << 8]);
		mCacheBlocks[blocknr].tag = tag;
	}
	else
		blocknr = ((u32)pBlock - (u32)&mCacheBlocks[0]) >> 3;
	MoveToTail(blocknr);
	return (blocknr << 8) + (128 * 1024);
}

/*asm uint32_t TerrainTextureManager8::GetTextureAddress(int tl, int tr, int bl, int br, uint32_t oldTexKey)
{
#define arg_br (4 * 9)
#define arg_oldTexKey (4 * 10)
	enum
	{
		offsetof_mReplaceListHead = offsetof(TerrainTextureManager8, mReplaceListHead),
		offsetof_mReplaceListTail = offsetof(TerrainTextureManager8, mReplaceListTail),
		offsetof_mResourceCounter = offsetof(TerrainTextureManager8, mResourceCounter),
		offsetof_mCacheBlocks = offsetof(TerrainTextureManager8, mCacheBlocks),
		offsetof_mTextureDatas = offsetof(TerrainTextureManager8, mTextureDatas),
		offsetof_mVramCTexData = offsetof(TerrainTextureManager8, mVramCTexData)
	};
	stmfd sp!, { r4 - r11,lr }
	ldr r4, [sp, #arg_br]
	//create tag
	orr r6, r1, r2, lsl #8
	orr r6, r6, r3, lsl #16
	orr r6, r6, r4, lsl #24

	ldr r11, [sp, #arg_oldTexKey]
	cmp r11, #0
	beq notexkey
	sub r9, r11, #(128 * 1024)
	add r10, r0, r9, lsr #5 //#4
	ldr r8, [r10, #offsetof_mCacheBlocks]!
	cmp r8, r6
		moveq r4, r9, lsr #8 //cur block nr
		beq tag_found_old

notexkey:
	add r9, r0, #offsetof_mCacheBlocks
	add r8, r9, #(512 * 8) //#(1024 * 8)
	str r6, [r8], #8
loop:	
	ldr r10, [r9], #8
	teq r10, r6
		bne loop

	cmp r9, r8
		bne tag_found

/*	mvn r5, #1
	mvn r8, #0x80000000
	add r9, r0, #(offsetof_mCacheBlocks + 4)
	mov r12, #512 //#1024
loop2:
	ldr r10, [r9], #8
	cmp r10, r8
		movlt r8, r10
		movlt r5, r12
	subs r12, r12, #1
	bgt loop2
	rsb r5, r5, #512 //#1024
	add r12, r0, #offsetof_mCacheBlocks
	add r12, r12, r5, lsl #3
	strd r6, [r12]

	add r12, r0, r5, lsl #8 //#7

	ldr r11,= offsetof_mTextureDatas
	add lr, r0, r11
	ldr r0, [lr, r1, lsl #2]
	ldr r1, [lr, r2, lsl #2]
	ldr r2, [lr, r3, lsl #2]
	ldr r3, [lr, r4, lsl #2]

	ldr r11, = offsetof_mVramCTexData
	add r4, r12, r11//#offsetof_mVramCTexData

	bl gen_terrain_texture_8
	mov r0, r5, lsl #8 //#7
	add r0, r0, #(128 * 1024)
	ldmfd sp!, { r4 - r11,pc }/

	ldrh r5, [r0, #offsetof_mReplaceListHead]
	add r10, r0, #offsetof_mCacheBlocks
	add r10, r10, r5, lsl #3
	str r6, [r10] //update tag
	ldrh r6, [r10, #6] //next
	strh r6, [r0, #offsetof_mReplaceListHead] //set as new head

	add r12, r0, r5, lsl #8

	mov r6, r0 //save this

	ldr r11,= offsetof_mTextureDatas
	add lr, r0, r11
	ldr r0, [lr, r1, lsl #2]
	ldr r1, [lr, r2, lsl #2]
	ldr r2, [lr, r3, lsl #2]
	ldr r3, [lr, r4, lsl #2]

	ldr r11, = offsetof_mVramCTexData
	add r4, r12, r11//#offsetof_mVramCTexData

	bl gen_terrain_texture_8
	mov r11, r5, lsl #8
	add r11, r11, #(128 * 1024) //address to return
	mov r4, r5 //cur block nr
	mov r0, r6 //this
	b tag_found_old

	//ldmfd sp!, { r4 - r11,pc }


tag_found:
//	sub r12, r8, r9
//	rsb r12, r12, #(512 * 8) //#(1024 * 8)
//	str r7, [r9, #-4]
//	mov r12, r12, lsl #5 //#4
//	add r0, r12, #(128 * 1024)
//	ldmfd sp!, { r4 - r11,pc }
	sub r4, r8, r9
	rsb r4, r4, #(512 * 8)
	add r10, r9, #8 //cur block ptr
	mov r11, r4, lsl #5
	add r11, r11, #(128 * 1024)//address to return
	mov r4, r4, lsr #3 //cur block nr
	//ldmfd sp!, { r4 - r11,pc }

tag_found_old:
	//move block to end of replace list
	//first remove the block from its current spot and update the blocks around it
	//mov r4, r9, lsr #8 //block nr
	ldr r12,= 0xFFFF
	ldr r7, [r10, #4]
	mov r8, r7, lsl #16
	cmp r12, r8, lsr #16
		addne r9, r0, r8, lsr #13 //this + prev * 8
	mov r8, r7, lsr #16 //next
		strneh r8, [r9, #(offsetof_mCacheBlocks + 6)]
	cmp r12, r8
		addne r9, r0, r8, lsl #3 //this + next * 8
		strneh r7, [r9, #(offsetof_mCacheBlocks + 4)]
	//move the block to the tail
	ldrh r7, [r0, #offsetof_mReplaceListTail]
	orr r7, r7, r12, lsl #16
	str r7, [r10, #4] //prev is the current tail, next = 0xFFFF
	add r9, r0, r7, lsl #3
	strh r4, [r9, #(offsetof_mCacheBlocks + 6)]
	strh r4, [r0, #offsetof_mReplaceListTail]

	mov r0, r11
	ldmfd sp!, { r4 - r11,pc }
}*/