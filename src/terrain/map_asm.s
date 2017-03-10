.section .dtcm

.global gen_terrain_texture_coeftable
gen_terrain_texture_coeftable:
 .set y, 0
 .rept 16
	.set x, 0
	.rept 16
		.byte x * y
		.byte (15 - x) * y
		.byte (15 - x) * (15 - y)
		.byte x * (15 - y)
		.set x, x + 1
	.endr
	.set y, y + 1
 .endr