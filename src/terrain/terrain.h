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
	uint8_t ltCorner : 2;
	uint8_t rtCorner : 2;
	uint8_t lbCorner : 2;
	uint8_t rbCorner : 2;
	uint8_t groundType;
} tile_t;

#define TRACKPIECE_KIND_FLAT					0
#define TRACKPIECE_KIND_FLAT_SMALL_CURVED_LEFT	1	//	_/	2x2

#define TRACKPIECE_ROT_0						0	// ->
#define TRACKPIECE_ROT_90						1	// ^
#define TRACKPIECE_ROT_180						2	// <-
#define TRACKPIECE_ROT_270						3	// V

struct trackpiece_t
{
	uint8_t kind : 6;
	uint8_t rot : 2;
	uint16_t x;
	uint16_t y;
	uint16_t z;
	trackpiece_t* prev[4];
	trackpiece_t* next[4];
};

class TerrainManager;

void tile_render(tile_t* tile, TerrainManager* terrainManager);
void trackpiece_render(trackpiece_t* piece, TerrainManager* terrainManager);


#endif