#ifndef __TERRAIN_H__
#define __TERRAIN_H__
#include <nnsys/gfd.h>

typedef struct 
{
	NNSGfdTexKey texKey;
	NNSGfdPlttKey plttKey;
	int nitroWidth;
	int nitroHeight;
	int nitroFormat;
} texture_t;

typedef struct
{
	uint16_t y;
	uint8_t groundType;
} tile_t;

void tile_render(tile_t* tile);

typedef struct
{
	tile_t tiles[16][16];//[y][x]
} sector_t;


#endif