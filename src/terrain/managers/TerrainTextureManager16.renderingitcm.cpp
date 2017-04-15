#include <nitro.h>
#include <nnsys/fnd.h>
#include "core.h"
#include "util.h"
#include "TerrainTextureManager16.h"

extern "C" void gen_terrain_texture(u16* tl, u16* tr, u16* bl, u16* br, u16* dst);

asm uint32_t TerrainTextureManager16::GetTextureAddress(int tl, int tr, int bl, int br, uint32_t oldTexKey)
{
#define arg_br (4 * 9)
#define arg_oldTexKey (4 * 10)
	enum
	{
		offsetof_mResourceCounter = offsetof(TerrainTextureManager16, mResourceCounter),
		offsetof_mCacheBlocks = offsetof(TerrainTextureManager16, mCacheBlocks),
		offsetof_mTextureDatas = offsetof(TerrainTextureManager16, mTextureDatas),
		offsetof_mVramCTexData = offsetof(TerrainTextureManager16, mVramCTexData),
		offsetof_mMessageQueue = offsetof(TerrainTextureManager16, mMessageQueue)
	};
	stmfd sp!, { r4 - r11,lr }
	ldr r4, [sp, #arg_br]
	ldr r7, [r0, #offsetof_mResourceCounter]
	//create tag
	orr r6, r1, r2, lsl #8
	orr r6, r6, r3, lsl #16
	orr r6, r6, r4, lsl #24

	ldr r11, [sp, #arg_oldTexKey]
	subs r9, r11, #(128 * 1024)
		bmi notexkey
	add r9, r0, r9, lsr #6
	ldr r8, [r9, #offsetof_mCacheBlocks]!
	cmp r8, r6
		bne notexkey

	str r7, [r9, #4]
	mov r0, r11
	ldmfd sp!, { r4 - r11,pc }

notexkey:
	add r9, r0, #offsetof_mCacheBlocks
	add r8, r9, #(256 * 8)
	str r6, [r8], #8
loop:
	ldr r10, [r9], #8
	teq r10, r6
	bne loop

	cmp r9, r8
	bne tag_found

	mvn r5, #1
	mvn r8, #0x80000000
	add r9, r0, #(offsetof_mCacheBlocks + 4)
	mov r12, #256
loop2:
	ldr r10, [r9], #8
	cmp r10, r8
		movlt r8, r10
		movlt r5, r12
	subs r12, r12, #1
	bgt loop2
	rsb r5, r5, #256
	add r12, r0, #offsetof_mCacheBlocks
	add r12, r12, r5, lsl #3
	strd r6, [r12]

	ldr r1, = offsetof_mMessageQueue
	add r0, r0, r1
	mov r1, r5
	mov r2, #OS_MESSAGE_NOBLOCK
	bl OS_JamMessage
	mov r0, r5, lsl #9
	add r0, r0, #(128 * 1024)
	ldmfd sp!, { r4 - r11,pc }
	/*add r12, r0, r5, lsl #9

	add lr, r0, #offsetof_mTextureDatas
	ldr r0, [lr, r1, lsl #2]
	ldr r1, [lr, r2, lsl #2]
	ldr r2, [lr, r3, lsl #2]
	ldr r3, [lr, r4, lsl #2]

	add r4, r12, #offsetof_mVramCTexData

	bl gen_terrain_texture
	mov r0, r5, lsl #9
	add r0, r0, #(128 * 1024)
	ldmfd sp!, { r4 - r11,pc }*/
tag_found:
	sub r12, r8, r9
	rsb r12, r12, #(256 * 8)
	str r7, [r9, #-4]
	mov r12, r12, lsl #6
	add r0, r12, #(128 * 1024)
	ldmfd sp!, { r4 - r11,pc }
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
	}
}