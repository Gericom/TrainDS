#ifndef __TILE_H__
#define __TILE_H__

typedef struct
{
	uint16_t y;
} tile_t;

void tile_render(tile_t* tile);

#endif