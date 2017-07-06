#pragma once

#include "WorldObject.h"
#include "terrain/Map.h"

class Water : public WorldObject
{
private:
	Map* mMap;
	VecFx32 mPosition;
	fx32 mWidth;
	fx32 mHeight;
	fx32 mDepth;
public:
	Water(VecFx32* position, fx32 width, fx32 height, fx32 depth)
		: mPosition(*position), mWidth(width), mHeight(height), mDepth(depth)
	{ }

	void Render();
	void Render2();
	void GetPosition(VecFx32* dst)
	{
		*dst = mPosition;
	}
	void GetBounds(box2d_t* box)
	{
		box->x1 = mPosition.x - mWidth / 2;
		box->y1 = mPosition.z - mHeight / 2;
		box->x2 = mPosition.x + mWidth / 2;
		box->y2 = mPosition.z + mHeight / 2;
	}
};