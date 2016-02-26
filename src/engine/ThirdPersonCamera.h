#ifndef __THIRDPERSONCAMERA_H__
#define __THIRDPERSONCAMERA_H__
#include "LookAtCamera.h"
#include "vehicles/train.h"

//A camera that looks at the locomotive of a train
class ThirdPersonCamera : public LookAtCamera
{
public:
	ThirdPersonCamera()
		: mRadius(2.5 * FX32_ONE), mTheta(0), mPhi(0), mTrain(NULL)
	{
		mUp.x = 0;
		mUp.y = FX32_ONE;
		mUp.z = 0;
	}

	train_t* mTrain;
	fx32 mRadius;
	fx32 mTheta;//inclination
	fx32 mPhi;//azimuth

	void Apply();
};

#endif