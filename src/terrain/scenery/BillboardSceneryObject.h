#ifndef __BILLBOARDSCENERYOBJECT_H__
#define __BILLBOARDSCENERYOBJECT_H__

#include "terrain\scenery\SceneryObject.h"

class BillboardSceneryObject : public SceneryObject
{
public:
	BillboardSceneryObject(uint16_t x, uint8_t y, uint16_t z, int rot)
		: SceneryObject(x, y, z, rot)
	{ }

	virtual void Render(TerrainManager* terrainManager) = 0;
};

#endif