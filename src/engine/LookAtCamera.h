#ifndef __LOOKATCAMERA_H__
#define __LOOKATCAMERA_H__

class LookAtCamera : public Camera
{
public:
	LookAtCamera()
	{
		mUp.x = 0;
		mUp.y = FX32_ONE;
		mUp.z = 0;
	}

	VecFx32 mPosition;
	VecFx32 mDestination;
	VecFx32 mUp;

	void Apply();

	virtual void GetLookDirection(VecFx32* lookDirection)
	{
		VEC_Subtract(&mDestination, &mPosition, lookDirection);
		VEC_Normalize(lookDirection, lookDirection);
	}
};

#endif