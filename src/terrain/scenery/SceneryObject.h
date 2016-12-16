#ifndef __SCENERYOBJECT_H__
#define __SCENERYOBJECT_H__

#include <nnsys/fnd.h>
#include "terrain\Map.h"

class SceneryObject
{
	friend class Map;
private:
	NNSFndLink mLink;
public:
	mapcoord_t mPosition;
	union
	{
		struct
		{
			uint32_t mRot : 2;
			uint32_t : 30;
		};
		uint32_t mFlags;
	};

public:
	SceneryObject(uint16_t x, uint8_t y, uint16_t z, int rot)
		: mRot(rot)
	{
		mPosition.x = x;
		mPosition.y = y;
		mPosition.z = z;
	}

	virtual void Render(TerrainManager* terrainManager) = 0;
};

#endif