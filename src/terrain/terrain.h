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

#define TILE_CORNER_FLAT	0
#define TILE_CORNER_UP		1
#define TILE_CORNER_DOWN	2

//#define TILE_HEIGHT			(FX32_HALF >> 1)
#define TILE_HEIGHT			(FX32_HALF - (FX32_HALF >> 2))

typedef struct
{
	uint16_t y;
	union
	{
		struct
		{
			uint8_t ltCorner : 2;
			uint8_t rtCorner : 2;
			uint8_t lbCorner : 2;
			uint8_t rbCorner : 2;
		};
		uint8_t corners;
	};
	uint8_t groundType;
} tile_t;

class TerrainManager;

void setup_normals();
void tile_render(tile_t* tile, TerrainManager* terrainManager);

#endif