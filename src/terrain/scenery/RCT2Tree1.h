#ifndef __RCT2TREE1_H__
#define __RCT2TREE1_H__

#include "terrain\scenery\BillboardSceneryObject.h"

class RCT2Tree1 : public BillboardSceneryObject
{
public:
	RCT2Tree1(uint16_t x, uint8_t y, uint16_t z, int rot)
		: BillboardSceneryObject(x, y, z, rot)
	{ }

	virtual void Render(TerrainManager* terrainManager);
};

#endif