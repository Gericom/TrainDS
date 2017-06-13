#include <nitro.h>
#include <nnsys/fnd.h>
#include "core.h"
#include "util.h"
#include "TerrainTextureManager16.h"

extern "C" void gen_terrain_texture(u16* tl, u16* tr, u16* bl, u16* br, u16* dst);

asm uint32_t TerrainTextureManager16::GetTextureAddress(int tl, int tr, int bl, int br, uint32_t oldTexKey)
{
#define arg_br (4 * 0)
#define arg_oldTexKey (4 * 1)
	enum
	{
		offsetof_mResourceCounter = offsetof(TerrainTextureManager16, mResourceCounter),
		offsetof_mCacheBlocks = offsetof(TerrainTextureManager16, mCacheBlocks),
		offsetof_mTextureDatas = offsetof(TerrainTextureManager16, mTextureDatas),
		offsetof_mVramCTexData = offsetof(TerrainTextureManager16, mVramCTexData),
		offsetof_mMessageQueue = offsetof(TerrainTextureManager16, mMessageQueue)
	};
	orr r2, r1, r2, lsl #8
	ldr r1, [sp, #arg_br]
	orr r2, r2, r3, lsl #16
	orr r2, r2, r1, lsl #24
	ldr r1, [sp, #arg_oldTexKey]

	subs r12, r1, #(128 * 1024)
		bmi notexkey
	add r12, r0, r12, lsr #6
	ldr r3, [r12, #offsetof_mCacheBlocks]!
	cmp r3, r2
		bne notexkey

	ldr r3, [r0, #offsetof_mResourceCounter]
	str r3, [r12, #4]
	mov r0, r1
	bx lr

notexkey:
	add r12, r0, #offsetof_mCacheBlocks
	add r1, r12, #(256 * 8)
	str r2, [r1], #8
loop:
	ldr r3, [r12], #8
	teq r3, r2
		bne loop

	cmp r12, r1
		beq no_tag_found

	ldr r3, [r0, #offsetof_mResourceCounter]
	sub r1, r1, r12
	rsb r1, r1, #(256 * 8)
	str r3, [r12, #-4]
	mov r1, r1, lsl #6
	add r0, r1, #(128 * 1024)
	bx lr

no_tag_found:
	stmfd sp!, { r4, r5, lr }
	mvn r1, #1
	mvn r4, #0x80000000
	add r12, r0, #(offsetof_mCacheBlocks + 4)
	mov r5, #256
loop2:
	ldr r3, [r12], #8
	cmp r3, r4
		movlt r4, r3
		movlt r1, r5
	subs r5, r5, #1
		bgt loop2

	ldr r3, [r0, #offsetof_mResourceCounter]
	rsb r5, r1, #256
	add r12, r0, #offsetof_mCacheBlocks
	add r12, r12, r5, lsl #3
	strd r2, [r12]

	ldr r1,= offsetof_mMessageQueue
	add r0, r0, r1
	mov r1, r5
	mov r2, #OS_MESSAGE_NOBLOCK
	bl OS_JamMessage
	mov r0, r5, lsl #9
	add r0, r0, #(128 * 1024)
	ldmfd sp!, { r4, r5, pc }
}

void TerrainTextureManager16::WorkerThreadMain()
{
	OSMessage msg;
	while (1)
	{
		OS_ReceiveMessage(&mMessageQueue, &msg, OS_MESSAGE_BLOCK);
		int cacheBlock = (int)msg;
		texture_cache_block_t* block = &mCacheBlocks[cacheBlock];
		gen_terrain_texture(
			mTextureDatas[block->tag & 0xFF],
			mTextureDatas[(block->tag >> 8) & 0xFF],
			mTextureDatas[(block->tag >> 16) & 0xFF],
			mTextureDatas[(block->tag >> 24) & 0xFF],
			(u16*)&mVramCTexData[cacheBlock << 9]);
		DC_FlushRange(&mVramCTexData[cacheBlock << 9], 512);
	}
}