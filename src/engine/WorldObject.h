#ifndef __WORLDOBJECT_H__
#define __WORLDOBJECT_H__

class WorldObject
{
public:
	virtual void GetPosition(VecFx32* dst) = 0;
};

#endif