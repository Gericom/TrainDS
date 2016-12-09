#ifndef __CAMERA_H__
#define __CAMERA_H__

class Camera
{
public:
	virtual void Apply() = 0;
	virtual void GetLookDirection(VecFx32* lookDirection) = 0;
};

#endif