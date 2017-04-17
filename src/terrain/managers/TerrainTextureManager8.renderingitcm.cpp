#include <nitro.h>
#include <nnsys/fnd.h>
#include "core.h"
#include "util.h"
#include "TerrainTextureManager8.h"

extern "C" void gen_terrain_texture_8(u16* tl, u16* tr, u16* bl, u16* br, u16* dst);

asm uint32_t TerrainTextureManager8::GetTextureAddress(int tl, int tr, int bl, int br, uint32_t oldTexKey)
{
//#define arg_br (4 * 9)
//#define arg_oldTexKey (4 * 10)
#define arg_br (4 * 0)
#define arg_oldTexKey (4 * 1)
	enum
	{
		offsetof_mResourceCounter = offsetof(TerrainTextureManager8, mResourceCounter),
		offsetof_mCacheBlocks = offsetof(TerrainTextureManager8, mCacheBlocks),
		offsetof_mTextureDatas = offsetof(TerrainTextureManager8, mTextureDatas),
		offsetof_mVramCTexData = offsetof(TerrainTextureManager8, mVramCTexData),
		offsetof_mMessageQueue = offsetof(TerrainTextureManager8, mMessageQueue)
	};
	orr r2, r1, r2, lsl #8
	ldr r1, [sp, #arg_br]
	orr r2, r2, r3, lsl #16
	orr r2, r2, r1, lsl #24
	ldr r1, [sp, #arg_oldTexKey]

	subs r12, r1, #(128 * 1024)
		bmi notexkey
	add r12, r0, r12, lsr #5
	ldr r3, [r12, #offsetof_mCacheBlocks]!
	cmp r3, r2
		bne notexkey

	ldr r3, [r0, #offsetof_mResourceCounter]
	str r3, [r12, #4]
	mov r0, r1
	bx lr

notexkey:
	add r12, r0, #offsetof_mCacheBlocks
	add r1, r12, #(512 * 8)
	str r2, [r1], #8
loop:
	ldr r3, [r12], #8
	teq r3, r2
		bne loop

	cmp r12, r1
		beq no_tag_found

	ldr r3, [r0, #offsetof_mResourceCounter]
	sub r1, r1, r12
	rsb r1, r1, #(512 * 8)
	str r3, [r12, #-4]
	mov r1, r1, lsl #5
	add r0, r1, #(128 * 1024)
	bx lr

no_tag_found:
	stmfd sp!, { r4, r5, lr }
	mvn r1, #1
	mvn r4, #0x80000000
	add r12, r0, #(offsetof_mCacheBlocks + 4)
	mov r5, #512
loop2:
	ldr r3, [r12], #8
	cmp r3, r4
		movlt r4, r3
		movlt r1, r5
	subs r5, r5, #1
		bgt loop2

	ldr r3, [r0, #offsetof_mResourceCounter]
	rsb r5, r1, #512
	add r12, r0, #offsetof_mCacheBlocks
	add r12, r12, r5, lsl #3
	strd r2, [r12]

	ldr r1,= offsetof_mMessageQueue
	add r0, r0, r1
	mov r1, r5
	mov r2, #OS_MESSAGE_NOBLOCK
	bl OS_JamMessage
	mov r0, r5, lsl #8
	add r0, r0, #(128 * 1024)
	ldmfd sp!, { r4, r5, pc }
}

void TerrainTextureManager8::WorkerThreadMain()
{
	OSMessage msg;
	while (1)
	{
		OS_ReceiveMessage(&mMessageQueue, &msg, OS_MESSAGE_BLOCK);
		int cacheBlock = (int)msg;
		texture_cache_block_t* block = &mCacheBlocks[cacheBlock];
		gen_terrain_texture_8(
			mTextureDatas[block->tag & 0xFF], 
			mTextureDatas[(block->tag >> 8) & 0xFF], 
			mTextureDatas[(block->tag >> 16) & 0xFF], 
			mTextureDatas[(block->tag >> 24) & 0xFF], 
			(u16*)&mVramCTexData[cacheBlock << 8]);
	}
}