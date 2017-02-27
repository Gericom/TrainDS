//.section .itcm
.section .videoplayer_itcm

DITHER_COEF_OFFSET = 0

//DITHER_COEF_0 = (DITHER_COEF_OFFSET + 0)
//DITHER_COEF_1 = (DITHER_COEF_OFFSET + 4)
//DITHER_COEF_2 = (DITHER_COEF_OFFSET + 3)
//DITHER_COEF_3 = (DITHER_COEF_OFFSET + 7)
//DITHER_COEF_4 = (DITHER_COEF_OFFSET + 2)
//DITHER_COEF_5 = (DITHER_COEF_OFFSET + 6)
//DITHER_COEF_6 = (DITHER_COEF_OFFSET + 1)
//DITHER_COEF_7 = (DITHER_COEF_OFFSET + 5)

DITHER_COEF_0 = (DITHER_COEF_OFFSET + 0)
DITHER_COEF_1 = (DITHER_COEF_OFFSET + 4)
DITHER_COEF_2 = (DITHER_COEF_OFFSET + 6)
DITHER_COEF_3 = (DITHER_COEF_OFFSET + 2)
DITHER_COEF_4 = (DITHER_COEF_OFFSET + 1)
DITHER_COEF_5 = (DITHER_COEF_OFFSET + 5)
DITHER_COEF_6 = (DITHER_COEF_OFFSET + 7)
DITHER_COEF_7 = (DITHER_COEF_OFFSET + 3)

COEF_RV = 27525 //Add one V aswell //93061
COEF_GU = -11272 //Mul by 2 //-22544
COEF_GV = -23396 //Mul by 2
COEF_BU = 29033 //Mul by 4 //116130

.global yuv2rgb_new
yuv2rgb_new:
	stmfd sp!, {r4-r11, lr}
	ldr r10,= (YUV2RGB_ClampRangeBitTable + 256)
	ldr lr,= (192*128)//(144*128)//(192 * 128)
loop:
	ldrh r9, [r1, #128]
	ldrh r8, [r1], #2
	orr r8, r8, r9, lsl #16

	ldr r12,= (((COEF_RV & 0xFFFF) << 16) | (COEF_BU & 0xFFFF))
	ldr r11,= (((COEF_GV & 0xFFFF) << 16) | (COEF_GU & 0xFFFF))
	//Get U and V
	and r3, r8, #0xFF	//U
	and r4, r9, #0xFF	//V
	sub r3, r3, #128
	sub r4, r4, #128
	//Calculate Rbase (R - Y)
	smlawt r5, r4, r12, r4
	//Calculate Gbase (G - Y)
	smulbt r6, r4, r11
	smlabb r6, r3, r11, r6
	//Calculate Bbase (B - Y)
	smulbb r7, r3, r12

	ldr r3, [r0], #4
	and r4, r3, #0xFF
	add r12, r10, r4
	//add r12, r12, #DITHER_COEF_0
	ldrb r4, [r12, r5]
	ldrb r11, [r12, r6, asr #15]
	ldrb r12, [r12, r7, asr #14]
	orr r4, r4, r11, lsl #5
	orr r4, r4, r12, lsl #10
	orr r4, r4, #0x8000

	mov r11, r3, lsl #16
	add r12, r10, r11, lsr #24
	add r12, r12, #DITHER_COEF_1
	ldrb r9, [r12, r5]
	ldrb r11, [r12, r6, asr #15]
	ldrb r12, [r12, r7, asr #14]
	orr r9, r9, r11, lsl #5
	orr r9, r9, r12, lsl #10
	orr r9, r9, #0x8000
	orr r9, r4, r9, lsl #16
	str r9, [r2], #4
	//second line
	ldr r9, [r0, #252]
	and r4, r9, #0xFF
	add r4, r4, #DITHER_COEF_2
	add r12, r10, r4
	ldrb r4, [r12, r5]
	ldrb r11, [r12, r6, asr #15]
	ldrb r12, [r12, r7, asr #14]
	orr r4, r4, r11, lsl #5
	orr r4, r4, r12, lsl #10
	orr r4, r4, #0x8000

	mov r11, r9, lsl #16
	add r12, r10, r11, lsr #24
	add r12, r12, #DITHER_COEF_3
	ldrb r5, [r12, r5]
	ldrb r6, [r12, r6, asr #15]
	ldrb r7, [r12, r7, asr #14]
	orr r5, r5, r6, lsl #5
	orr r5, r5, r7, lsl #10
	orr r5, r5, #0x8000
	orr r5, r4, r5, lsl #16
	str r5, [r2, #508]

	//loop unrolling

	//Get U and V
	mov r4, r8, lsr #24	//V
	mov r8, r8, lsr #8
	and r8, r8, #0xFF	//U
	sub r8, r8, #128
	sub r4, r4, #128
	ldr r12,= (((COEF_RV & 0xFFFF) << 16) | (COEF_BU & 0xFFFF))
	ldr r11,= (((COEF_GV & 0xFFFF) << 16) | (COEF_GU & 0xFFFF))
	//Calculate Rbase (R - Y)
	smlawt r5, r4, r12, r4
	//Calculate Gbase (G - Y)
	smulbt r6, r4, r11
	smlabb r6, r8, r11, r6
	//Calculate Bbase (B - Y)
	smulbb r7, r8, r12

	mov r11, r3, lsl #8
	add r12, r10, r11, lsr #24
	add r12, r12, #DITHER_COEF_4
	ldrb r4, [r12, r5]
	ldrb r11, [r12, r6, asr #15]
	ldrb r12, [r12, r7, asr #14]
	orr r4, r4, r11, lsl #5
	orr r4, r4, r12, lsl #10
	orr r4, r4, #0x8000

	add r12, r10, r3, lsr #24
	add r12, r12, #DITHER_COEF_5
	ldrb r3, [r12, r5]
	ldrb r11, [r12, r6, asr #15]
	ldrb r12, [r12, r7, asr #14]
	orr r3, r3, r11, lsl #5
	orr r3, r3, r12, lsl #10
	orr r3, r3, #0x8000
	orr r3, r4, r3, lsl #16
	str r3, [r2], #4
	//second line
	mov r11, r9, lsl #8
	add r12, r10, r11, lsr #24
	add r12, r12, #DITHER_COEF_6
	ldrb r4, [r12, r5]
	ldrb r11, [r12, r6, asr #15]
	ldrb r12, [r12, r7, asr #14]
	orr r4, r4, r11, lsl #5
	orr r4, r4, r12, lsl #10
	orr r4, r4, #0x8000

	add r12, r10, r9, lsr #24
	add r12, r12, #DITHER_COEF_7
	ldrb r9, [r12, r5]
	ldrb r11, [r12, r6, asr #15]
	ldrb r12, [r12, r7, asr #14]
	orr r9, r9, r11, lsl #5
	orr r9, r9, r12, lsl #10
	orr r9, r9, #0x8000
	orr r9, r4, r9, lsl #16
	str r9, [r2, #508]

	subs lr, lr, #4
	ldmlefd sp!, {r4-r11, pc}
	tst lr, #0xFF
	bne loop

	add r0, r0, #256 //#(STRIDE + 256) //#256
	add r1, r1, #128 //#((STRIDE >> 1) + 128) //#128
	add r2, r2, #512

	b loop

.pool