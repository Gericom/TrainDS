#ifndef __WORLDOBJECT_H__
#define __WORLDOBJECT_H__

#include "box2d.h"

class WorldObject
{
public:
	u32 mRenderFlag;

	WorldObject()
		: mRenderFlag(0)
	{ }

	virtual void Render() = 0;
	virtual void GetPosition(VecFx32* dst) = 0;
	virtual void GetBounds(box2d_t* box) = 0;
	virtual bool Intersects(box2d_t* box)
	{
		box2d_t bounds;
		GetBounds(&bounds);
		return bounds.Intersects(box);
	}
};

#endif