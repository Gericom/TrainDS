#ifndef __LOOKATCAMERA_H__
#define __LOOKATCAMERA_H__

class LookAtCamera : public Camera
{
public:
	VecFx32 mPosition;
	VecFx32 mDestination;
	VecFx32 mUp;

	void Apply();
};

#endif