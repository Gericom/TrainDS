#include <nitro.h>
#include <nnsys/fnd.h>
#include "core.h"
#include "util.h"
#include "TerrainTextureManager8.h"

extern "C" void gen_terrain_texture_8(u16* tl, u16* tr, u16* bl, u16* br, u16* dst);

asm uint32_t TerrainTextureManager8::GetTextureAddress(int tl, int tr, int bl, int br, uint32_t oldTexKey)
{
#define arg_br (4 * 9)
#define arg_oldTexKey (4 * 10)
	enum
	{
		offsetof_mResourceCounter = offsetof(TerrainTextureManager8, mResourceCounter),
		offsetof_mCacheBlocks = offsetof(TerrainTextureManager8, mCacheBlocks),
		offsetof_mTextureDatas = offsetof(TerrainTextureManager8, mTextureDatas),
		offsetof_mVramCTexData = offsetof(TerrainTextureManager8, mVramCTexData)
	};
	stmfd sp!, { r4 - r11,lr }
	ldr r4, [sp, #arg_br]
	ldr r7, [r0, #offsetof_mResourceCounter]
	add r7, r7, #1
	str r7, [r0, #offsetof_mResourceCounter]
	//create tag
	orr r6, r1, r2, lsl #8
	orr r6, r6, r3, lsl #16
	orr r6, r6, r4, lsl #24

	ldr r11, [sp, #arg_oldTexKey]
	cmp r11, #0
	beq notexkey
	sub r9, r11, #(128 * 1024)
	add r9, r0, r9, lsr #4
	ldr r8, [r9, #offsetof_mCacheBlocks]!
	cmp r8, r6
	beq tag_found_old

notexkey:
	add r9, r0, #offsetof_mCacheBlocks
	add r8, r9, #(1024 * 8)
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
	mov r12, #1024
loop2:
	ldr r10, [r9], #8
	cmp r10, r8
		movlt r8, r10
		movlt r5, r12
	subs r12, r12, #1
	bgt loop2
	rsb r5, r5, #1024
	add r12, r0, #offsetof_mCacheBlocks
	add r12, r12, r5, lsl #3
	strd r6, [r12]

	add r12, r0, r5, lsl #7

	ldr r11,= offsetof_mTextureDatas
	add lr, r0, r11
	ldr r0, [lr, r1, lsl #2]
	ldr r1, [lr, r2, lsl #2]
	ldr r2, [lr, r3, lsl #2]
	ldr r3, [lr, r4, lsl #2]

	ldr r11, = offsetof_mVramCTexData
	add r4, r12, r11//#offsetof_mVramCTexData

	bl gen_terrain_texture_8
	mov r0, r5, lsl #7
	add r0, r0, #(128 * 1024)
	ldmfd sp!, { r4 - r11,pc }
tag_found:
	sub r12, r8, r9
	rsb r12, r12, #(1024 * 8)
	str r7, [r9, #-4]
	mov r12, r12, lsl #4
	add r0, r12, #(128 * 1024)
	ldmfd sp!, { r4 - r11,pc }

tag_found_old:
	str r7, [r9, #4]
	mov r0, r11
	ldmfd sp!, { r4 - r11,pc }
}