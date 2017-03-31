.arm

#define G3OP_NOP                0x00   /* Nop            */

#define G3OP_MTX_MODE           0x10   /* MatrixMode     */
#define G3OP_MTX_PUSH           0x11   /* PushMatrix     */
#define G3OP_MTX_POP            0x12   /* PopMatrix      */
#define G3OP_MTX_STORE          0x13   /* StoreMatrix    */
#define G3OP_MTX_RESTORE        0x14   /* RestoreMatrix  */
#define G3OP_MTX_IDENTITY       0x15   /* Identity       */
#define G3OP_MTX_LOAD_4x4       0x16   /* LoadMatrix44   */
#define G3OP_MTX_LOAD_4x3       0x17   /* LoadMatrix43   */
#define G3OP_MTX_MULT_4x4       0x18   /* MultMatrix44   */
#define G3OP_MTX_MULT_4x3       0x19   /* MultMatrix43   */
#define G3OP_MTX_MULT_3x3       0x1a   /* MultMatrix33   */
#define G3OP_MTX_SCALE          0x1b   /* Scale          */
#define G3OP_MTX_TRANS          0x1c   /* Translate      */

#define G3OP_COLOR              0x20   /* Color          */
#define G3OP_NORMAL             0x21   /* Normal         */
#define G3OP_TEXCOORD           0x22   /* TexCoord       */
#define G3OP_VTX_16             0x23   /* Vertex         */
#define G3OP_VTX_10             0x24   /* VertexShort    */
#define G3OP_VTX_XY             0x25   /* VertexXY       */
#define G3OP_VTX_XZ             0x26   /* VertexXZ       */
#define G3OP_VTX_YZ             0x27   /* VertexYZ       */
#define G3OP_VTX_DIFF           0x28   /* VertexDiff     */
#define G3OP_POLYGON_ATTR       0x29   /* PolygonAttr    */
#define G3OP_TEXIMAGE_PARAM     0x2a   /* TexImageParam  */
#define G3OP_TEXPLTT_BASE       0x2b   /* TexPlttBase    */

#define G3OP_DIF_AMB            0x30   /* MaterialColor0 */
#define G3OP_SPE_EMI            0x31   /* MaterialColor1 */
#define G3OP_LIGHT_VECTOR       0x32   /* LightVector    */
#define G3OP_LIGHT_COLOR        0x33   /* LightColor     */
#define G3OP_SHININESS          0x34   /* Shininess      */

#define G3OP_BEGIN              0x40   /* Begin          */
#define G3OP_END                0x41   /* End            */

#define G3OP_SWAP_BUFFERS       0x50   /* SwapBuffers    */

#define G3OP_VIEWPORT           0x60   /* Viewport       */

#define G3OP_BOX_TEST           0x70   /* BoxTest        */
#define G3OP_POS_TEST           0x71   /* PositionTest   */
#define G3OP_VEC_TEST           0x72   /* VectorTest     */

#define G3OP_DUMMY_COMMAND      0xFF   /* Dummy command */

//void render_tile(pVtx, x, y)
.global render_tile
render_tile:
	stmfd sp!, {r4-r11}
	orr r10, r1, r2, lsl #20
	ldr r1,= G3OP_BEGIN | (G3OP_TEXCOORD << 8) | (G3OP_NORMAL << 16) | (G3OP_VTX_10 << 24)
	mov r2, #2 //GX_BEGIN_TRIANGLE_STRIP
	mov r3, #0
	ldmia r0, {r4, r5, r6, r7}
	and r5, r5, #0xFF
	orr r5, r10, r5, lsl #10
	ldr r8,= G3OP_TEXCOORD | (G3OP_NORMAL << 8) | (G3OP_VTX_10 << 16) | (G3OP_TEXCOORD << 24)
	mov r9, #0x01000000

	ldr r12,= 0x04000400
	stmia r12, {r1-r5,r8,r9}

	add r2, r0, #(128 * 8)
	ldmia r2, {r2, r3, r9, r11}
	and r3, r3, #0xFF
	orr r3, r10, r3, lsl #10
	add r3, r3, #(1 << 20)

	mov r4, #0x00000100

	ldr r5,= G3OP_NORMAL | (G3OP_VTX_10 << 8) | (G3OP_TEXCOORD << 16) | (G3OP_NORMAL << 24)
	and r7, r7, #0xFF
	add r10, r10, #1
	orr r7, r10, r7, lsl #10

	orr r8, r4, #0x01000000		//=0x01000100

	and r11, r11, #0xFF
	orr r11, r10, r11, lsl #10
	add r11, r11, #(1 << 20)

	ldr r10,= G3OP_VTX_10 | (G3OP_END << 8) | (G3OP_NOP << 16) | (G3OP_NOP << 24)

	stmia r12, {r2-r11}

	ldmfd sp!, {r4-r11}
	bx lr

//void render_tile2x2(pVtx, x, y)
.global render_tile2x2
render_tile2x2:
	stmfd sp!, {r4-r11}
	orr r10, r1, r2, lsl #20
	ldr r1,= G3OP_BEGIN | (G3OP_TEXCOORD << 8) | (G3OP_NORMAL << 16) | (G3OP_VTX_10 << 24)
	mov r2, #2 //GX_BEGIN_TRIANGLE_STRIP
	mov r3, #0
	ldmia r0, {r4, r5}

	add r6, r0, #(2 * 8)
	ldmia r6, {r6, r7}

	and r5, r5, #0xFF
	orr r5, r10, r5, lsl #10
	ldr r8,= G3OP_TEXCOORD | (G3OP_NORMAL << 8) | (G3OP_VTX_10 << 16) | (G3OP_TEXCOORD << 24)
	mov r9, #0x00800000

	ldr r12,= 0x04000400
	stmia r12, {r1-r5,r8,r9}

	add r9, r0, #(2 * 128 * 8)
	ldmia r9, {r2, r3}

	add r9, r9, #(2 * 8)
	ldmia r9, {r9, r11}

	and r3, r3, #0xFF
	orr r3, r10, r3, lsl #10
	add r3, r3, #(2 << 20)

	mov r4, #0x00000100

	ldr r5,= G3OP_NORMAL | (G3OP_VTX_10 << 8) | (G3OP_TEXCOORD << 16) | (G3OP_NORMAL << 24)
	and r7, r7, #0xFF
	add r10, r10, #2
	orr r7, r10, r7, lsl #10

	orr r8, r4, #0x00800000		//=0x00800100

	and r11, r11, #0xFF
	orr r11, r10, r11, lsl #10
	add r11, r11, #(2 << 20)

	ldr r10,= G3OP_VTX_10 | (G3OP_END << 8) | (G3OP_NOP << 16) | (G3OP_NOP << 24)

	stmia r12, {r2-r11}

	ldmfd sp!, {r4-r11}
	bx lr


//((15 - x)*(15 - y))*tl + (x*(15 - y))*tr + (y*(15 - x))*bl + (x*y)*br

//void gen_terrain_texture(u16* tl, u16* tr, u16* bl, u16* br, u16* dst)
.global gen_terrain_texture
gen_terrain_texture:
arg_dst	= 4 * 9
	stmfd sp!, {r4-r11,lr}
	//ldr r4, [sp, #arg_dst]	//dst
	ldr r5,= gen_terrain_texture_coeftable
	mov r6, #256
	gen_terrain_texture_yloop:
	//{
		//gen_terrain_texture_xloop:
		.rept 16
		//{
			//(x*y)*br
			ldr lr, [r5], #4

			ldrh r8, [r3], #2

			and r7, lr, #0xFF

			and r12, r8, #0x1F
			smulbb r9, r12, r7

			mov r8, r8, lsr #5
			and r12, r8, #0x1F
			smulbb r10, r12, r7

			mov r8, r8, lsr #5
			smulbb r11, r8, r7

			//(y*(15 - x))*bl
			ldrh r8, [r2], #2

			mov r7, lr, lsr #8
			and r7, r7, #0xFF

			and r12, r8, #0x1F
			smlabb r9, r12, r7, r9

			mov r8, r8, lsr #5
			and r12, r8, #0x1F
			smlabb r10, r12, r7, r10

			mov r8, r8, lsr #5
			smlabb r11, r8, r7, r11

			//((15 - x)*(15 - y))*tl
			ldrh r8, [r0], #2

			and r7, lr, #0xFF0000

			and r12, r8, #0x1F
			smlabt r9, r12, r7, r9

			mov r8, r8, lsr #5
			and r12, r8, #0x1F
			smlabt r10, r12, r7, r10

			mov r8, r8, lsr #5
			smlabt r11, r8, r7, r11

			//(x*(15 - y))*tr
			ldrh r8, [r1], #2

			mov r7, lr, lsr #24

			and r12, r8, #0x1F
			smlabb r9, r12, r7, r9

			mov r8, r8, lsr #5
			and r12, r8, #0x1F
			smlabb r10, r12, r7, r10

			//prevent interlock
			ldr lr,= 291

			mov r8, r8, lsr #5
			smlabb r11, r8, r7, r11

			smulwb r9, r9, lr
			smulwb r10, r10, lr
			smulwb r11, r11, lr

			orr r9, r9, r10, lsl #5
			orr r9, r9, r11, lsl #10
			orr r9, r9, #0x8000
			strh r9, [r4], #2
		//}
		.endr
		subs r6, r6, #16
		bgt gen_terrain_texture_yloop
	//}
	ldmfd sp!, {r4-r11,pc}

.pool

//void gen_terrain_texture(u16* tl, u16* tr, u16* bl, u16* br, u16* dst)
.global gen_terrain_texture_8
gen_terrain_texture_8:
arg_dst	= 4 * 9
	stmfd sp!, {r4-r11,lr}
	//ldr r4, [sp, #arg_dst]	//dst
	ldr r5,= gen_terrain_texture_8_coeftable
	mov r6, #128
	gen_terrain_texture_8_yloop:
	//{
		//gen_terrain_texture_xloop:
		.rept 16
		//{
			//(x*y)*br
			ldr lr, [r5], #4

			ldrh r8, [r3], #2

			and r7, lr, #0xFF

			and r12, r8, #0x1F
			smulbb r9, r12, r7

			mov r8, r8, lsr #5
			and r12, r8, #0x1F
			smulbb r10, r12, r7

			mov r8, r8, lsr #5
			smulbb r11, r8, r7

			//(y*(15 - x))*bl
			ldrh r8, [r2], #2

			mov r7, lr, lsr #8
			and r7, r7, #0xFF

			and r12, r8, #0x1F
			smlabb r9, r12, r7, r9

			mov r8, r8, lsr #5
			and r12, r8, #0x1F
			smlabb r10, r12, r7, r10

			mov r8, r8, lsr #5
			smlabb r11, r8, r7, r11

			//((15 - x)*(15 - y))*tl
			ldrh r8, [r0], #2

			and r7, lr, #0xFF0000

			and r12, r8, #0x1F
			smlabt r9, r12, r7, r9

			mov r8, r8, lsr #5
			and r12, r8, #0x1F
			smlabt r10, r12, r7, r10

			mov r8, r8, lsr #5
			smlabt r11, r8, r7, r11

			//(x*(15 - y))*tr
			ldrh r8, [r1], #2

			mov r7, lr, lsr #24

			and r12, r8, #0x1F
			smlabb r9, r12, r7, r9

			mov r8, r8, lsr #5
			and r12, r8, #0x1F
			smlabb r10, r12, r7, r10

			//prevent interlock
			ldr lr,= 624 //1337

			mov r8, r8, lsr #5
			smlabb r11, r8, r7, r11

			smulwb r9, r9, lr
			smulwb r10, r10, lr
			smulwb r11, r11, lr

			orr r9, r9, r10, lsl #5
			orr r9, r9, r11, lsl #10
			orr r9, r9, #0x8000
			strh r9, [r4], #2
		//}
		.endr
		subs r6, r6, #16
		bgt gen_terrain_texture_8_yloop
	//}
	ldmfd sp!, {r4-r11,pc}