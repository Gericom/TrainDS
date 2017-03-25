#ifndef __THIRDPERSONCAMERA_H__
#define __THIRDPERSONCAMERA_H__
#include "LookAtCamera.h"
#include "Quaternion.h"
#include "WorldObject.h"

#if 0

//A camera that looks at the locomotive of a train
class ThirdPersonCamera : public LookAtCamera
{
public:
	ThirdPersonCamera()
		: mRadius(2.5 * FX32_ONE), mTheta(0), mPhi(0), mObject(NULL)
	{
		//mRotation = Quaternion();
		//mRotation.Identity();
		//mDirection.x = 0;//FX32_ONE;
		//mDirection.y = FX32_ONE;
		//mDirection.z = 0;//FX32_ONE;
	}

	WorldObject* mObject;
	fx32 mRadius;
	fx32 mTheta;//inclination
	fx32 mPhi;//azimuth
	//VecFx32 mDirection;
	//Quaternion mRotation;

	/*void TranslateOnSphere(fx32 x, fx32 y)
	{
		VecFx32 perpendicular = {-mDirection.y, mDirection.x, 0};
        Quaternion verticalRotation(&perpendicular, y);
		//VecFx32 dirx = mDirection;
		//Quaternion invrot;
		//mRotation.Conjugate(&invrot);
		//invrot.MultiplyByVector(&dirx, &dirx);
		Quaternion horizontalRotation(&mDirection, x);
		horizontalRotation.MultiplyByQuaternion(&verticalRotation, &horizontalRotation);
		mRotation.MultiplyByQuaternion(&horizontalRotation, &mRotation);
	}*/

	void Apply();
};
#endif

#endif