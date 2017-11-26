#pragma once

#include "terrain\scenery\BillboardSceneryObject.h"

class RCT2Tree1 : public BillboardSceneryObject
{
public:
	RCT2Tree1(uint16_t x, uint8_t y, uint16_t z, int rot)
		: BillboardSceneryObject(x, y, z, rot)
	{ }

	virtual void Render(TerrainManager* terrainManager);
};