#pragma once

#include <nnsys/g3d.h>
#include "engine/objects/WorldObject.h"

class Map;

class SimpleSceneryObject : public WorldObject
{
	friend class Map;
private:
	NNSFndLink mLink;
	u32 mObjectType;
	NNSG3dRenderObj mRenderObj;
	Map* mMap;
	VecFx32 mPosition;
	u16 mRotY;
	bool mUse1Mat1ShpRendering;

public:
	SimpleSceneryObject(Map* map, u32 type, fx32 x, fx32 z, fx32 rotY);

	void Render();

	void GetPosition(VecFx32* dst)
	{
		*dst = mPosition;
	}

	void GetBounds(box2d_t* box)
	{
		box->x1 = mPosition.x - 2 / 2;
		box->y1 = mPosition.z - 2 / 2;
		box->x2 = mPosition.x + 2 / 2;
		box->y2 = mPosition.z + 2 / 2;
	}

	bool Intersects(box2d_t* box)
	{
		return ((mPosition.x - 2 / 2) <= box->x2) && ((mPosition.x + 2 / 2) >= box->x1) &&
			((mPosition.z - 2 / 2) <= box->y2) && ((mPosition.z + 2 / 2) >= box->y1);
		//return bounds.Intersects(box);
	}

	void Invalidate();
};