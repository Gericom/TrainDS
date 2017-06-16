.global gen_terrain_texture_coeftable
gen_terrain_texture_coeftable:
 .set y, 0
 .rept 16
	.set x, 0
	.rept 16
		.byte (15 - x) * (15 - y)
		.byte x * (15 - y)
		.byte (15 - x) * y
		.byte x * y
		.set x, x + 1
	.endr
	.set y, y + 1
 .endr

 .global gen_terrain_texture_8_coeftable
gen_terrain_texture_8_coeftable:
 .set y, 0
 .rept 8
	.set x, 0
	.rept 16
		.byte (15 - x) * (7 - y)
		.byte x * (7 - y)
		.byte (15 - x) * y
		.byte x * y
		.set x, x + 1
	.endr
	.set y, y + 1
 .endr