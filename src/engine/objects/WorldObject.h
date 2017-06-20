#ifndef __WORLDOBJECT_H__
#define __WORLDOBJECT_H__

#include "box2d.h"

class WorldObject
{
public:
	virtual void Render() = 0;
	virtual void GetPosition(VecFx32* dst) = 0;
	virtual void GetBounds(box2d_t* box) = 0;
};

#endif